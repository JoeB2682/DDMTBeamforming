//===============================================================================
//
// MVDR.h
// 
// Minimum-Variance-Distortionless-Response algorithm to go on receiver array.
// 
// Uses Eigen library 5.0.0 to make linear algebra operations easier.
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include "DAS.h"
#include "FFTProcessor.h"
#include <Eigen/Dense>

//===============================================================================
class MVDR 
{
public:

	MVDR(DAS* dasPtr, int N, int Fs, std::shared_ptr<FFTProcessor> FFTprocessor);
	~MVDR() = default;
	
	void calculateSteeringVector(std::vector<float>& receivertau, float freq);
	void calculateSCM( std::vector<std::vector<std::complex<float>>>& spectra, int bin);
	void calculateWeights();
	std::complex<float> applyWeights(std::vector<std::vector<std::complex<float>>>& spectra, int bin);
	void processCircularMVDR(juce::AudioBuffer<float>& micbuffer, std::vector<float> receivertau);

public:
	
	int sampleRate, N;
	std::shared_ptr<FFTProcessor> fftprocessor;
	std::vector<std::complex<float>> outputSpectrum;
	juce::AudioBuffer<float> outputBuffer;

private:

	Eigen::VectorXcf v, w, Rv;
	Eigen::MatrixXcf R;

public:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MVDR)
};
//===============================================================================
