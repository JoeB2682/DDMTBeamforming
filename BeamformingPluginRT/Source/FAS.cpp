//===============================================================================
#include "FAS.h"
//===============================================================================
FAS::FAS(DAS* dasObject, int numtaps, int bufferlen, float freq, float bandlow, 
								 float bandhigh, bool iswideband, bool useMVDR, 
								 std::shared_ptr<FFTProcessor> fftProcessor,
								 std::shared_ptr<MotionTrackerHandler> motionTrackerHandler)

    : das(dasObject), numTaps(numtaps), freq(freq), wideband(iswideband), 
	  isMVDR(useMVDR), fftprocessor(fftProcessor), motiontrackerhandler(motionTrackerHandler)
{
	// Initialise wideband signal
	band = std::make_shared<FrequencyBand>();

	band->low = bandlow;
	band->high = bandhigh;

	// Initialise Filter bank
	filterBank.resize(dasObject->N);

	for (int i = 0; i < dasObject->N; i++)
	{
		filterBank[i] = std::make_unique<fracDelFIR>(
			numtaps,
			bufferlen,
			das->sampleRate,
			freq,
			wideband,
			band
		);
	}

	// Initialise other oscillator banks
	lowoscbank.resize(dasObject->N);
	highoscbank.resize(dasObject->N);

	for (int i = 0; i < dasObject->N; i++)
	{
		lowoscbank[i] = std::make_unique<Oscillator>();
		highoscbank[i] = std::make_unique<Oscillator>();
	}

	// FOR NOW, use N = 1, MVDR not technically possible without mic array this atm is
	// just an optimised distortion filter.
	mvdr = std::make_unique<MVDR>(dasObject, 1, dasObject->sampleRate, fftprocessor);
	//mvdr = std::make_unique<MVDR>(dasObject, ADD NO MICS WHEN ARRAY EXTENDED!, dasObject->sampleRate, fftprocessor);

	// Preallocate Neural Network tensors
	room.resize(7, 0.0f);
	trajectory.resize(35, 0.0f);
	fircoeffs.resize(1 * 1 * 100 * 8 * 64, 0.0f);
	beam.resize(1 * 1 * 72 * 100, 0.0f);
	filteredBeam.resize(1 * 1 * 72 * 100, 0.0f);

	// Make NN Worker thread instance
	nnWorker = std::make_unique<NNWorker>();

	// Make Trajectory Worker Thread Instance
	trajectoryworker = std::make_unique<trajectoryWorker>(motiontrackerhandler);

	// Instanciate logger
	coefficientlogger = std::make_unique<CoefficientLogger>();
}
//===============================================================================
// Sets the band frequencies using parameter vals
void FAS::setBandFreqs(const std::shared_ptr<FrequencyBand>& frequencyband, float f0, float f1, float f2)
{
	frequencyband->low = f0;
	frequencyband->mid = f1;
	frequencyband->high = f2;
}
//===============================================================================
// Generates full narrowband signal for each source (calc correct offset prior)
void FAS::generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
						     std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							 juce::AudioBuffer<float>& buffer,
							 float freq,
							 float amplitude,
							 std::vector<float>& tau,
							 float gain,
							 bool ApplyNN)
{
	// Calculate max of time of arrival array
	float tauMax = *std::max_element(tau.begin(), tau.end());

	// Use flag to setup oscillators once
	if (!das->setoscbankflag)
	{
		for (int i = 0; i < oscbank.size(); i++)
		{
			oscbank[i]->setupOscillator(das->sampleRate, buffer.getNumSamples(), false);
		}
		das->setoscbankflag = true;
	}

	// Clamp to prevent blowup
	//gain = juce::jlimit(0.0f, 0.01f, gain);

	// Calculate relative delays for each speaker and to be use in FIR filters
	for (int speaker = 0; speaker < das->N; speaker++)
	{
		oscbank[speaker]->setFrequency(freq);
		oscbank[speaker]->setTargetAmplitude(amplitude);

		auto* channel = buffer.getWritePointer(speaker);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{
			// Normalise to prevent insane gain levels
			filterBank[speaker]->getNNBool(ApplyNN);
			channel[sample] = filterbank[speaker]->process(tau[speaker], tauMax, das->oscbank[speaker]->incrementSample() * gain / das->N);
			//DBG(channel[sample]);
		}
	}
}
//===============================================================================
// Generates wideband chord signal
void FAS::generateWideband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
	std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
	juce::AudioBuffer<float>& buffer,
	float freq,
	float amplitude,
	std::vector<float>& tau,
	float gain,
	const std::shared_ptr<FrequencyBand>& frequencyband,
	float f0,
	float f1,
	float f2,
	bool ApplyNN)
{
	setBandFreqs(frequencyband, f0, f1, f2);

	// Calculate max of time of arrival array
	float tauMax = *std::max_element(tau.begin(), tau.end());

	// Use flag to setup oscillators once
	if (!das->setoscbankflag)
	{
		for (int i = 0; i < oscbank.size(); i++)
		{
			oscbank[i]->setupOscillator(das->sampleRate,
				buffer.getNumSamples(),
				false);

			lowoscbank[i]->setupOscillator(das->sampleRate,
				buffer.getNumSamples(),
				false);

			highoscbank[i]->setupOscillator(das->sampleRate,
				buffer.getNumSamples(),
				false);
		}
		das->setoscbankflag = true;
	}

	// Clamp to prevent blowup
	//gain = juce::jlimit(0.0f, 0.01f, gain);
	gain /= 10;

	// Calculate relative delays for each speaker and to be use in FIR filters
	for (int speaker = 0; speaker < das->N; speaker++)
	{
		if (frequencyband != nullptr) {

			// mid bank low end of the band
			oscbank[speaker]->setFrequency(frequencyband->mid);
			oscbank[speaker]->setTargetAmplitude(amplitude);

			// lower bank low end of the band
			lowoscbank[speaker]->setFrequency(frequencyband->low);
			lowoscbank[speaker]->setTargetAmplitude(amplitude);

			// higher bank high end of the band
			highoscbank[speaker]->setFrequency(frequencyband->high);
			highoscbank[speaker]->setTargetAmplitude(amplitude);
		}
		
		auto* channel = buffer.getWritePointer(speaker);

		// Near field compensation gamma coefficient
		float gamma = std::pow(4.0f * juce::MathConstants<float>::pi * das->vDistance[speaker], 2.0f / das->N);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{
			// Sum oscillators to generate chord smaple 
			float input = ((oscbank[speaker]->incrementSample() +
							lowoscbank[speaker]->incrementSample()   +
							highoscbank[speaker]->incrementSample()) / 3.0f) * gain;

			filterBank[speaker]->getNNBool(ApplyNN);

			/*
			DBG("FAS speaker " << speaker
				<< " tau=" << tau[speaker]
				<< " tauMax=" << tauMax
				<< " delay=" << (tauMax - tau[speaker]));
			*/

			channel[sample] = gamma * filterbank[speaker]->process(tau[speaker], tauMax, input);
		}

		// Log Data For current filter
		coefficientlogger->setEnabled(log);

		if (log)
		{
			coefficientlogger->logCoefficients(
				speaker,
				filterBank[speaker]->getnncorrection(),
				filterBank[speaker]->getcorrecteddB(),
				filterBank[speaker]->getb());
		}

		wasLoggingEnabled = log;
	}
}
//===============================================================================
// Just an onset detector really
float FAS::estimateMicTOA(juce::AudioBuffer<float>& micbuffer, int srate, float thresh)
{
	// Eventually loop for all receivers
	const float* x = micbuffer.getReadPointer(0);
	int N = micbuffer.getNumSamples();
	//float maxLevel = 0.0f;
	for (int i = 0; i < N; i++)
	{
		//DBG(x[i]);
		if (std::abs(x[i]) > thresh)
			return (float)i / srate;
	}

	/*
	DBG("NO TOA: maxLevel = " << maxLevel
		<< " threshold = " << thresh);
	*/
	return -1.0f;
}
//===============================================================================
void FAS::processcircularFAS(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& micbuffer,
							 float bright_x, float bright_y, float amplitude, float gain, float thresh, 
						     float f0, float f1, float f2, float Length, float Width, float Height, float Absorption, 
							 float MaxOrder, float rt60, int NumSpeakers, bool ApplyNN, bool log) 
{
	//===========================================================================
	// IMPORTANT!!!!!!!!!!!
	//===========================================================================
	//
	// This needs changing depending on whether using a circular or Horizontal 
	// ULA I am not rewriting big processing functions at this stage just comment
	// or uncomment the required block!

	/*
	if (!das->setPosflag)
	{
		das->setsourcePositions(das->r, das->speakers);
		das->setreceiverPositions(das->r, das->receivers);
		das->setPosflag = true;
	}
	*/
	
	if (!das->setPosflag)
	{
		float spacing = 0.15f;
		float yOffset = 0.0f;

		das->setninearhSourcePositions(
			spacing,
			yOffset,
			Length,
			Width,
			das->speakers);

		das->setreceiverPositions(das->r, das->receivers);
		das->setPosflag = true;

		/*
		for (int i = 0; i < das->N; ++i)
		{
			DBG("Speaker " << i
				<< " x=" << das->speakers[i].x
				<< " y=" << das->speakers[i].y);
		}
		*/
	}
	
	//===========================================================================
	das->setbrightPoint(das->b, bright_x, bright_y);
	das->calcsourceTOI(das->tau, das->speakers, das->b);

	/*
	DBG("Bright point: x=" << das->b.x << " y=" << das->b.y);

	for (int i = 0; i < das->N; ++i)
	{
		DBG("Speaker " << i
			<< " x=" << das->speakers[i].x
			<< " y=" << das->speakers[i].y
			<< " distance=" << das->vDistance[i]
			<< " tau=" << das->tau[i]);
	}
	*/
	das->calcReceiverTOI(das->tau_rx, das->speakers, das->receivers);

	// Calculates tau using estimate + receiver arrival
	float predictedMicArrival = 0.0f;

	// Sum Speaker delay calculated from bright point only with the time of arrival to the mic 
	for (int i = 0; i < das->N; i++) predictedMicArrival = std::max(predictedMicArrival, das->tau[i] + das->tau_rx[i]);

	// measured mic arrival 
	//float measuredMicArrival = estimateMicTOA(micbuffer, das->sampleRate, thresh);
	float measuredMicArrival = estimateMicTOA(micbuffer, das->sampleRate, thresh);
	float delta = measuredMicArrival - predictedMicArrival;

	std::vector<float> tauRXCorrected = das->tau_rx;

	// apply correction to source TOI
	for (int i = 0; i < das->N; i++) {
		das->tau_Corrected[i] = delta;
		//DBG(das->tau_Corrected[i]);
		tauRXCorrected[i] = delta;
	}

	
	//DBG("predicted " << predictedMicArrival);
	//DBG("measured " << measuredMicArrival);
	

	//DBG("delta " << delta);

	// ================================================================

	// Uses own generate functions
	if (wideband)
		generateWideband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau_Corrected, gain, band, f0, f1, f2, ApplyNN);
		//generateWideband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau, gain, band, f0, f1, f2, ApplyNN);
	else
		generateNarrowband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau_Corrected, gain, ApplyNN);
	
	if (isMVDR)
	{
		//DBG("MVDR enabled: " << (isMVDR ? "true" : "false"));

		// Process MVDR Receiver Algorithm
		mvdr->processCircularMVDR(micbuffer, tauRXCorrected);
	}

	// ================================================================
	// Get NN Output
	std::vector<float> newCorrection;

	if (nnWorker->getLatestCorrection(newCorrection))
	{
		latestCorrection = std::move(newCorrection);

		//DBG("NN correction received: " << latestCorrection.size());

		// Apply FIR Correction 
		for (int speaker = 0; speaker < das->N; ++speaker)
		{
			filterBank[speaker]->setCorrection(latestCorrection);
		}
	}
	// ================================================================
	// Prep NN Input

	// populate room vector from parameters
	std::vector<float> room =
	{
		Length,
		Width,
		Height,
		Absorption,
		rt60,
		MaxOrder,
		static_cast<float>(NumSpeakers)
	};

	// Get Trajectory from worker thread
	trajectory = trajectoryworker->getTrajectory();

	// Populate fircoeffs with b from current fir
	for (int speaker = 0; speaker < das->N; speaker++)
	{
		const auto& coeffs = filterBank[speaker]->getCoeficients();

		for (int tap = 0; tap < numTaps; tap++)
		{
			fircoeffs[speaker * numTaps + tap] = coeffs[tap];
		}
	}

	// ================================================================
	// Submit NN job
	nnWorker->submit(
		room,
		trajectory,
		fircoeffs,
		beam,
		filteredBeam
	);
}
//===============================================================================
// Another stupid bool passing function
void FAS::setloggingEnabled(bool enabled)
{
	trajectoryworker->setLoggingEnabled(enabled);
}
//===============================================================================