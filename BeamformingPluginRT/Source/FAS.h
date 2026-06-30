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

	FAS(DAS* dasPtr, int numtaps, int bufferlen, float freq);
	~FAS() = default;

	void generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
							std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							juce::AudioBuffer<float>& buffer,
							float freq,
							float amplitude,
							std::vector<float>& tau,
							float gain);

	void processcircularFAS(juce::AudioBuffer<float>& buffer, float bright_x, float bright_y, float freq, float amplitude, float gain);

public:

	int numTaps;
	float freq;

	DAS* das;
	std::vector<std::unique_ptr<fracDelFIR>> filterBank;
};
//===============================================================================
