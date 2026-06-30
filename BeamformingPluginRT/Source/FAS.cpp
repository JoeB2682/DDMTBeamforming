//===============================================================================
#include "FAS.h"
//===============================================================================
FAS::FAS(DAS* dasObject, int numtaps, int bufferlen, float freq)
    : das(dasObject), numTaps(numtaps)
{
	// Initialise Filter bank
	filterBank.resize(dasObject->N);

	for (int i = 0; i < dasObject->N; i++)
	{
		filterBank[i] = std::make_unique<fracDelFIR>(numtaps, bufferlen, das->sampleRate, freq);
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
void FAS::processcircularFAS(juce::AudioBuffer<float>& buffer,
	float bright_x, float bright_y, float freq, float amplitude, float gain) 
{
	if (!das->setPosflag)
	{
		das->setsourcePositions(das->r, das->speakers);
		das->setPosflag = true;
	}

	das->setbrightPoint(das->b, bright_x, bright_y);
	das->calcsourceTOI(das->tau, das->speakers, das->b);

	// Uses own generate function
	generateNarrowband(das->oscbank, filterBank, buffer, freq, 0.5f, das->tau, gain);
}
//===============================================================================
