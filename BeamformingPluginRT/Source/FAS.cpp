//===============================================================================
#include "FAS.h"
//===============================================================================
FAS::FAS(DAS* dasObject, int numtaps, int bufferlen, float freq, float bandlow, 
								 float bandhigh, bool iswideband, bool useMVDR, 
								 std::shared_ptr<FFTProcessor> fftProcessor)

    : das(dasObject), numTaps(numtaps), freq(freq), wideband(iswideband), 
	  isMVDR(useMVDR), fftprocessor(fftProcessor)
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
}
//===============================================================================
// Generates full narrowband signal for each source (calc correct offset prior)
void FAS::generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
						     std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							 juce::AudioBuffer<float>& buffer,
							 float freq,
							 float amplitude,
							 std::vector<float>& tau,
							 float gain)
{
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
	const std::shared_ptr<FrequencyBand>& frequencyband)
{
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
		// Main Osc bank from DAS set at narrowband
		oscbank[speaker]->setFrequency(freq);
		oscbank[speaker]->setTargetAmplitude(amplitude);

		if (frequencyband != nullptr) {
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
			float input = ((das->oscbank[speaker]->incrementSample() +
							lowoscbank[speaker]->incrementSample()   +
							highoscbank[speaker]->incrementSample()) / 3.0f) * gain;

			channel[sample] = gamma * filterbank[speaker]->process(tau[speaker], tauMax, input);
		}
	}
}
//===============================================================================
// Just an onset detector really
float FAS::estimateMicTOA(juce::AudioBuffer<float>& micbuffer, int srate, float thresh)
{
	// Eventually loop for all receivers
	const float* x = micbuffer.getReadPointer(0);
	int N = micbuffer.getNumSamples();

	for (int i = 0; i < N; i++)
	{
		//DBG(x[i]);
		if (std::abs(x[i]) > thresh)
			return (float)i / srate;
	}
	return -1.0f;
}
//===============================================================================
void FAS::processcircularFAS(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& micbuffer,
							 float bright_x, float bright_y, float amplitude, float gain, float thresh) 
{
	if (!das->setPosflag)
	{
		das->setsourcePositions(das->r, das->speakers);
		das->setreceiverPositions(das->r, das->receivers);
		das->setPosflag = true;
	}

	das->setbrightPoint(das->b, bright_x, bright_y);
	das->calcsourceTOI(das->tau, das->speakers, das->b);
	das->calcReceiverTOI(das->tau_rx, das->speakers, das->receivers);

	// Calculates tau using estimate + receiver arrival
	float predictedMicArrival = 0.0f;

	// Sum Speaker delay calculated from bright point only with the time of arrival to the mic 
	for (int i = 0; i < das->N; i++) predictedMicArrival = std::max(predictedMicArrival, das->tau[i] + das->tau_rx[i]);

	// measured mic arrival 
	float measuredMicArrival = estimateMicTOA(micbuffer, das->sampleRate, thresh);
	float delta = measuredMicArrival - predictedMicArrival;

	std::vector<float> tauRXCorrected = das->tau_rx;

	// apply correction to source TOI
	for (int i = 0; i < das->N; i++) {
		das->tau_Corrected[i] = delta;
		//DBG(das->tau_Corrected[i]);
		tauRXCorrected[i] = delta;
	}

	/*
	DBG("predicted " << predictedMicArrival);
	DBG("measured " << measuredMicArrival);
	*/

	//DBG("delta " << delta);
	// Uses own generate functions
	if (wideband)
		generateWideband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau_Corrected, gain, band);
	else
		generateNarrowband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau_Corrected, gain);
	
	if (isMVDR)
	{
		//DBG("MVDR enabled: " << (isMVDR ? "true" : "false"));

		// Process MVDR Receiver Algorithm
		mvdr->processCircularMVDR(micbuffer, tauRXCorrected);
	}
}
//===============================================================================
