//===============================================================================

#pragma once

#include "fracDelFIR.h"

//===============================================================================
fracDelFIR::fracDelFIR(int ntaps, int ArraySize, int srate) :
	N(ntaps),
	windowgenerationflag(false)
{
	n.resize(N, 0.0f);

	// Generate n (tap vector)
	for (int i = 0; i < N; i++)
		n[i] = i - (N - 1) / 2.0f;
	

	b.resize(N, 0.0f);
	z.resize(N-1, 0.0f);
	win.resize(N, 0.0f);
	h.resize(N, 0.0f);

	Out.resize(ArraySize, 0.0f);

	getSampleRate(srate);
	fc = Fs / 2;

	wc = 2.0f * juce::MathConstants<float>::pi * fc / Fs;
}
//===============================================================================
float fracDelFIR::calculateCurrentU(std::vector<float> tau) 
{
	float u = 0.0f;

	// Calculate tauMax (again)
	float tauMax = *std::max_element(tau.begin(), tau.end());

	// calculate current fractional delay amount
	for (int i = 0; i < tau.size(); i++)
		u = std::max(u, (tauMax - tau[i]) * Fs);
	
	return u;
}
//===============================================================================
//Hanning Window Generator
std::vector<float> fracDelFIR::GenerateHanning(int windowLength)
{
	std::vector<float> hanningWindow(windowLength);

	for (int i = 0; i < windowLength; ++i)
	{
		hanningWindow[i] = 0.5f * (1.0f - std::cos((2.0f * juce::MathConstants<float>::pi * i) / (windowLength - 1)));
	}
	return hanningWindow;
}
//===============================================================================
std::vector<float> fracDelFIR::process(std::vector<float> tauvec, 
		 							   std::vector<float> x, 
		 							   std::vector<float> z)
{
	// Get toi vector from beamformer
	getTau(tauvec);
	
	// Get current fractional delay amount
	float u = calculateCurrentU(tau);

	// prevents division by 0
	if (std::fmod(u, 1.0f) == 0.0f)
		u += std::numeric_limits<float>::epsilon();
	
	// Generate Hanning window once
	if (!windowgenerationflag) {
		win = GenerateHanning(N);
		windowgenerationflag = true;
	}

	for (int i = 0; i < n.size(); i++) {
		// Generate Ideal LPF IR
		h[i] = std::sin(wc * (n[i] - u)) / (juce::MathConstants<float>::pi * (n[i] - u));
	}

	// Apply Window
	for (int i = 0; i < h.size(); i++) {
		b[i] = h[i] * win[i];
	}

	// sums all values in b vector
	float bsum = std::accumulate(b.begin(), b.end(), 0.0f);

	// normalises coeffs in b vector
	for (auto& coeff : b)
	{
		coeff /= bsum;
	}

	// ensures corrcet size of memory vector
	if (z.size() != b.size() - 1)
	{
		z.assign(b.size() - 1, 0.0f);
	}

	// ASSIGN OUT AND DO FIR CONVOLUTION BLOCK!!!!
	/*
	.
	.
	.
	.
	.
	.
	.
	*/

	return Out;
}
//===============================================================================

