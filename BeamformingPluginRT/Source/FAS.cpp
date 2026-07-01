//===============================================================================
#include "FAS.h"
//===============================================================================
FAS::FAS(DAS* dasObject, int numtaps, int bufferlen, float freq, float bandlow, float bandhigh, bool iswideband)
    : das(dasObject), numTaps(numtaps), freq(freq), wideband(iswideband)
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
	gain = juce::jlimit(0.0f, 0.01f, gain);

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
	gain = juce::jlimit(0.0f, 0.01f, gain);

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
void FAS::processcircularFAS(juce::AudioBuffer<float>& buffer,
	float bright_x, float bright_y, float amplitude, float gain) 
{
	if (!das->setPosflag)
	{
		das->setsourcePositions(das->r, das->speakers);
		das->setPosflag = true;
	}

	das->setbrightPoint(das->b, bright_x, bright_y);
	das->calcsourceTOI(das->tau, das->speakers, das->b);

	// Uses own generate functions
	if (wideband) {
		generateWideband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau, gain, band);
	}
	else {
		generateNarrowband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau, gain);
	}
}
//===============================================================================
