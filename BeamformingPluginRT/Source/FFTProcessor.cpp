//===============================================================================
#include "FFTProcessor.h"
//===============================================================================
// Constructor
FFTProcessor::FFTProcessor(int fftorder, int samplerate) : fftOrder(fftorder), 
														   FFT(fftorder),
														   sampleRate(samplerate)
{
	// Bit shift for size
	fftSize = 1 << fftOrder;

	fifo.resize(fftSize);
	fftData.resize(fftSize * 2);
	magnitude.resize(fftSize / 2);
	phase.resize(fftSize / 2);
	frequencies.resize(fftSize / 2);

	window = std::make_unique<juce::dsp::WindowingFunction<float>>(
		fftSize,
		juce::dsp::WindowingFunction<float>::hamming
	);
}
//===============================================================================
// Destructor
FFTProcessor::~FFTProcessor() 
{
	releaseResources();
}
//===============================================================================
// Release Resources
void FFTProcessor::releaseResources() 
{
	// Clear FIFO and FFT data buffers
	std::fill(fifo.begin(), fifo.end(), 0.0f);
	std::fill(magnitude.begin(), magnitude.end(), 0.0f);
	std::fill(phase.begin(), phase.end(), 0.0f);
	std::fill(fftData.begin(), fftData.end(), 0.0f);

	// Reset Index
	fifoIndex = 0;

	// Set Flags
	fftReadyForInverse = false;
	nextFFTBlockReady = false;
}
//===============================================================================
// First-In-First-Out sample-by-sample queue 
void FFTProcessor::pushNextSampleIntoFifo(float sample) 
{
	if (fifoIndex == fftSize)
	{
		if (!nextFFTBlockReady)
		{
			std::fill(fftData.begin(), fftData.end(), 0.0f);
			std::copy(fifo.begin(), fifo.end(), fftData.begin());
			nextFFTBlockReady = true;
		}
		fifoIndex = 0;
	}
	fifo[(size_t)fifoIndex++] = sample;
}
//===============================================================================
// Fill the data buffers with current fft block data
void FFTProcessor::populateDataVectors(std::vector<float>& magnvect,
									   std::vector<float>& phasevec,
									   std::vector<float>& freqvec,
									   std::vector<float>& rawFFTData) 
{
	for (int k = 0; k < fftSize / 2; k++)
	{
		// JUCE Interleaves data with realonly transform
		float real = rawFFTData[2 * k];
		float imag = rawFFTData[2 * k + 1];

		// Pythagorean theorem for magnitude
		magnvect[k] = std::sqrt(real * real + imag * imag);

		// arctan function for phase
		phasevec[k] = std::atan2(imag, real);

		// Extract frequencies individually
		freqvec[k] = (float)k * sampleRate / fftSize;
	}
}
//===============================================================================
// Process and perform FFT (data already acquired from FIFO Function)
void FFTProcessor::processFFT() 
{
	// Only populate data if next block is ready
	if (nextFFTBlockReady)
	{
		// Window Data
		window->multiplyWithWindowingTable(fftData.data(), fftSize);

		// Perform FFT on current data block
		FFT.performRealOnlyForwardTransform(fftData.data());

		populateDataVectors(magnitude, phase, frequencies, fftData);

		// Modify Frequency Domain
		processFreqDomain(magnitude, phase, frequencies, fftData);
	
		// Repopulate after modification
		populateDataVectors(magnitude, phase, frequencies, fftData);

		// Set flags
		fftReadyForInverse = true;
		nextFFTBlockReady = false;
	}
}
//===============================================================================
// Process and perform IFFT
void FFTProcessor::processIFFT()
{
	if (fftReadyForInverse)
	{
		// Perform IFFT on current data block
		FFT.performRealOnlyInverseTransform(fftData.data());
		fftReadyForInverse = false;
	}
}
//===============================================================================
// OVERRIDE THIS! Modify and process frequency domain data, allows 
void FFTProcessor::processFreqDomain(std::vector<float>& magnvect,
									 std::vector<float>& phasevec,
									 std::vector<float>& freqvec,
									 std::vector<float>& rawFFTData) 
{
	// Modify in Derived Class
	// e.g. rawFFTData[100] = 0.0f; // remove frequency bin
}
//===============================================================================
