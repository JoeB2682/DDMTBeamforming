//===============================================================================
//
// FAS.h
// 
// Filter and Sum beamformer class, generates and handles both narrow and wideband 
// signals. Allows for both linear and nonlinear arrays.
// 
// Primarilly uses DAS class functions...
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once

#include <JuceHeader.h>
#include "DAS.h"
#include "fracDelFIR.h"

//===============================================================================
class FAS
{
public:

	FAS(DAS* dasPtr, int numtaps, int bufferlen, float freq, float bandlow, float bandhigh, bool iswideband);
	~FAS() = default;

	float estimateMicTOA(juce::AudioBuffer<float>& micbuffer, int srate, float thresh);

	void generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
							std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							juce::AudioBuffer<float>& buffer,
							float freq,
							float amplitude,
							std::vector<float>& tau,
							float gain);

	void generateWideband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
							std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							juce::AudioBuffer<float>& buffer,
							float freq,
							float amplitude,
							std::vector<float>& tau,
							float gain,
							const std::shared_ptr<FrequencyBand>& frequencyband);

	void processcircularFAS(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& micbuffer,
							float bright_x, float bright_y, float amplitude, float gain, float thresh);

public:

	int numTaps;
	float freq;
	bool wideband;

	DAS* das;
	std::vector<std::unique_ptr<fracDelFIR>> filterBank;
	std::shared_ptr<FrequencyBand> band;

	std::vector<std::unique_ptr<Oscillator>> lowoscbank, highoscbank;

	
};
//===============================================================================
