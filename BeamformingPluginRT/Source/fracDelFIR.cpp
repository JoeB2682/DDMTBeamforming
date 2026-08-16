//===============================================================================
#include "fracDelFIR.h"
//===============================================================================
fracDelFIR::fracDelFIR(int ntaps, int ArraySize, int srate, float freq, bool iswideband, std::shared_ptr<FrequencyBand> band) :
	N(ntaps),
	windowgenerationflag(false),
	freqband(std::move(band))
{
	n.resize(N, 0.0f);

	// Generate n (tap vector)
	for (int i = 0; i < N; i++)
		n[i] = i - (N - 1) / 2.0f;
	

	b.resize(N, 0.0f);
	z.resize(N, 0.0f);
	win.resize(N, 0.0f);
	h.resize(N, 0.0f);
	nnCorrection.resize(N, 0.0f);
	correctedB.resize(N, 0.0f);

	Out.resize(ArraySize, 0.0f);

	getSampleRate(srate);

	//fc = Fs / 2; //Nyquist

	// Set bandwidth depending on whether signal is narrow or wideband
	fc = (iswideband && freqband != nullptr) ? freqband->bandwidth() : freq;

	wc = 2.0f * juce::MathConstants<float>::pi * fc / Fs;
}
//===============================================================================
// Return Coefficients
const std::vector<float> fracDelFIR::getCoeficients() const
{
	return b;
}
//===============================================================================
// Apply NN Correction to Coefficients
void fracDelFIR::setCorrection(const std::vector<float>& correction)
{
	if (correction.size() != nnCorrection.size())
		return;

	nnCorrection = correction;
}
//===============================================================================
// Calculate fractional delay (interpolation)

/*
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
*/

float fracDelFIR::calculateCurrentU(float tau, float tauMax)
{
	return (tauMax - tau) * Fs;
}
//===============================================================================
// Hanning Window Generator
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
// SIMD inner product optimisation(NOT MY FUNCTION!!!!!!)
// Taken from https://ccrma.stanford.edu/~jatin/Notebooks/FIRBenchmarks.html

inline float fracDelFIR::simdInnerProduct(float* in, float* kernel, int numSamples, float y) 
{
	constexpr size_t simdN = juce::dsp::SIMDRegister<float>::SIMDNumElements;

	// compute SIMD products
	int idx = 0;
	for (; idx <= numSamples - simdN; idx += simdN)
	{
		auto simdIn = juce::dsp::SIMDRegister<float>::fromRawArray(in + idx);
		auto simdKernel = juce::dsp::SIMDRegister<float>::fromRawArray(kernel + idx);
		y += (simdIn * simdKernel).sum();
	}

	// compute leftover samples
	y = std::inner_product(in + idx, in + numSamples, kernel + idx, y);

	return y;
}
//===============================================================================
float fracDelFIR::process(float tau, float tauMax, float x)
{
	// Get toi vector from beamformer
	//getTau(tauvec);

	// Get current fractional delay amount
	float u = calculateCurrentU(tau, tauMax);

	// prevents division by 0
	if (std::fmod(u, 1.0f) == 0.0f)
		u += std::numeric_limits<float>::epsilon();

	// Generate Hanning window once
	if (!windowgenerationflag) {
		win = GenerateHanning(N);
		windowgenerationflag = true;
	}

	// Only change FIR on delay change
	if (u != currentU)
	{
		currentU = u;

		// Get ideal LPF IR
		for (int i = 0; i < n.size(); i++) {
			h[i] = std::sin(wc * (n[i] - u)) /
				(juce::MathConstants<float>::pi * (n[i] - u));
		}

		// Apply Window
		for (int i = 0; i < h.size(); i++) {
			b[i] = h[i] * win[i];
		}

		// sums all values in b vector
		float bsum = std::accumulate(b.begin(), b.end(), 0.0f);

		// normalises coeffs in b vector
		for (auto& coeff : b) {
			coeff /= bsum;
		}
	}

	// Apply NN correctiom if bool is triggered
	if (applyNN)
	{
		for (size_t i = 0; i < b.size(); ++i)
			correctedB[i] = b[i] + nnCorrection[i];
	}
	else
	{
		correctedB = b;
	}

	// Shift samples through FIR delay line
	for (int i = z.size() - 1; i > 0; i--) {
		z[i] = z[i - 1];
	}

	// Insert current sample
	z[0] = x;

	// Apply filter to sample using SIMD function
	float y = simdInnerProduct(z.data(), correctedB.data(), N);

	//DBG(y);
	//DBG("ApplyNN = " << (applyNN ? "true" : "false"));
	// Return output sample
	return y;
}
//===============================================================================

