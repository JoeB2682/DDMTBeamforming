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

//===============================================================================
class MVDR 
{
public:

	MVDR(DAS* dasPtr, int N, int Fs);
	~MVDR() = default;

	void processCircularMVDR(juce::AudioBuffer<float>& micbuffer, float bright_x, float bright_y);

public:
	
	std::vector<std::vector<float>> R;

private:

	std::vector<float> Out, w, v, tau;

};
//===============================================================================
