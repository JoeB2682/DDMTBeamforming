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
#include "NNWorker.h"
#include "MotionTrackerHandler.h"
#include "TrajectoryWorker.h"
//===============================================================================
class FAS
{
public:

	FAS(DAS* dasPtr, int numtaps, int bufferlen, float freq, float bandlow, 
					 float bandhigh, bool iswideband, bool useMVDR, 
					 std::shared_ptr<FFTProcessor> fftProcessor,
					 std::shared_ptr<MotionTrackerHandler> motionTrackerHandler);

	~FAS() = default;

	float estimateMicTOA(juce::AudioBuffer<float>& micbuffer, int srate, float thresh);

	void generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
							std::vector<std::unique_ptr<fracDelFIR>>& filterbank,
							juce::AudioBuffer<float>& buffer,
							float freq,
							float amplitude,
							std::vector<float>& tau,
							float gain,
							bool ApplyNN);

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
							float f2, 
							bool ApplyNN);

	void processcircularFAS(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& micbuffer,
							float bright_x, float bright_y, float amplitude, float gain, float thresh, 
							float f0, float f1, float f2, float Length, float Width, float Height,
							float Absorption, float MaxOrder, float rt60, int NumSpeakers, bool ApplyNN);

	void setBandFreqs(const std::shared_ptr<FrequencyBand>& frequencyband, float f0, float f1, float f2);

public:

	int numTaps;
	float freq;
	bool wideband, isMVDR;

	// Neural Network input tensors
	std::vector<float> room;
	std::vector<float> trajectory;
	std::vector<float> fircoeffs;
	std::vector<float> beam;
	std::vector<float> filteredBeam;

	// Objects
	DAS* das;
	std::vector<std::unique_ptr<fracDelFIR>> filterBank;
	std::shared_ptr<FrequencyBand> band;
	std::unique_ptr<MVDR> mvdr;
	std::shared_ptr<FFTProcessor> fftprocessor;
	std::shared_ptr<MotionTrackerHandler> motiontrackerhandler;
	std::vector<std::unique_ptr<Oscillator>> lowoscbank, highoscbank;	

	// Neural Network Handler
	std::unique_ptr<NNWorker> nnWorker;
	std::vector<float> latestCorrection;

	// Trajectory Worker Thread
	std::unique_ptr<trajectoryWorker> trajectoryworker;
};
//===============================================================================
