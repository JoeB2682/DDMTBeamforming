//===============================================================================
//
// MVDR.h
// 
// Minimum-Variance-Distortionless-Response algorithm to go on receiver array.
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include "DAS.h"
#include "FFTProcessor.h"

//===============================================================================
class MVDR : public FFTProcessor
{
public:

	MVDR(DAS* dasPtr, int N, int Fs);
	~MVDR() = default;
	
	void calculateSteeringVector(std::vector<float> receivertau, int N);
	void processCircularMVDR(juce::AudioBuffer<float>& micbuffer, float bright_x, float bright_y);
	
	// Inherit and Process Frequency Domain Data
	void processFreqDomain(std::vector<float>& magnvect,
						   std::vector<float>& phasevec,
						   std::vector<float>& freqvec,
						   std::vector<float>& rawFFTData) override;

public:
	
	std::vector<std::vector<float>> R;

private:

	std::vector<float> Out, w, v;

public:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MVDR)
};
//===============================================================================
