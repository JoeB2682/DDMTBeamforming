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
#include "MVDR.h"
#include "NeuralNetworkHandler.h"
//===============================================================================
class FAS
{
public:

	FAS(DAS* dasPtr, int numtaps, int bufferlen, float freq, float bandlow, 
					 float bandhigh, bool iswideband, bool useMVDR, 
					 std::shared_ptr<FFTProcessor> fftProcessor);

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
							const std::shared_ptr<FrequencyBand>& frequencyband, 
							float f0,
						    float f1,
							float f2);

	void processcircularFAS(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& micbuffer,
							float bright_x, float bright_y, float amplitude, float gain, float thresh, 
							float f0, float f1, float f2, float Length, float Width, float Height,
							float Absorption, float MaxOrder, float rt60, int NumSpeakers);

	void setBandFreqs(const std::shared_ptr<FrequencyBand>& frequencyband, float f0, float f1, float f2);

public:

	int numTaps;
	float freq;
	bool wideband, isMVDR;

	// Objects
	DAS* das;
	std::vector<std::unique_ptr<fracDelFIR>> filterBank;
	std::shared_ptr<FrequencyBand> band;
	std::unique_ptr<MVDR> mvdr;
	std::shared_ptr<FFTProcessor> fftprocessor;
	std::vector<std::unique_ptr<Oscillator>> lowoscbank, highoscbank;	

	// Neural Network Handler
	std::unique_ptr<NeuralNetworkHandler> networkhandler;
};
//===============================================================================
