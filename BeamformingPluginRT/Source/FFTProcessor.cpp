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
	multifftData.resize(fftSize * 2);
	magnitude.resize(fftSize / 2);
	phase.resize(fftSize / 2);
	frequencies.resize(fftSize / 2);

	window = std::make_unique<juce::dsp::WindowingFunction<float>>(
		fftSize,
		juce::dsp::WindowingFunction<float>::hamming
	);

	// Overlap Add
	hopSize = fftSize / 2; // 50% Overlap
	overlapBuffer.resize(fftSize - hopSize, 0.0f);
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
	std::fill(multifftData.begin(), multifftData.end(), 0.0f);

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
// Process and perform FFT across entire buffer for multiple channels (array processing)
void FFTProcessor::processMultiChannelFFT(juce::AudioBuffer<float>& buffer,
	std::vector<std::vector<std::complex<float>>>& spectra)
{
	/*
	DBG("Channels: " << buffer.getNumChannels());
	DBG("Samples: " << buffer.getNumSamples());
	DBG("FFT size: " << fftSize);
	*/

	// Get number of channels in specified buffer
	int channels = buffer.getNumChannels();

	// Check buffer validity
	if (channels == 0 || buffer.getNumSamples() == 0) return;

	// resize output matrix
	spectra.resize(channels);

	// Iterate each channel
	for (int channel = 0; channel < channels; channel++)
	{
		// Resize for output for num channels and fftsize
		spectra[channel].resize(fftSize / 2);

		// reset data buffer
		std::fill(multifftData.begin(), multifftData.end(), 0.0f);

		// Get buffer pointer 
		auto* samples = buffer.getReadPointer(channel);

		// copy samples into data buffer up to fftsize
		int copySize = juce::jmin(fftSize, buffer.getNumSamples());

		std::copy(samples, samples + copySize, multifftData.begin());

		// window (I <3 a good Hamm)
		window->multiplyWithWindowingTable(multifftData.data(), fftSize);

		// Perform real only FFT
		FFT.performRealOnlyForwardTransform(multifftData.data());

		// Iterate frequency domain adding interleaved data to output matrix
		for (int k = 0; k < fftSize / 2; k++)
			spectra[channel][k] = std::complex<float>(multifftData[2 * k], multifftData[2 * k + 1]);
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
//Process and perform IFFT across entire buffer for multiple channels
void FFTProcessor::processMultiChannelIFFT(std::vector<std::complex<float>>& spectrum,
	juce::AudioBuffer<float>& outputBuffer)
{
	//DBG("IFFT size: " << fftSize);
	//DBG("Output samples: " << outputBuffer.getNumSamples());

	// Reset FFT data
	std::fill(multifftData.begin(), multifftData.end(), 0.0f);

	// Reconstruct interleaved FFT format
	for (int k = 0; k < fftSize / 2; k++)
	{
		multifftData[2 * k] = spectrum[k].real();
		multifftData[2 * k + 1] = spectrum[k].imag();
	}

	// Perform IFFT
	FFT.performRealOnlyInverseTransform(multifftData.data());

	// Scale
	float scale = 1.0f / fftSize;

	// Copy into output buffer
	for (int i = 0; i < fftSize; i++)
		outputBuffer.setSample(0, i, multifftData[i] * scale);
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
// Overlap add for STFT processing
void FFTProcessor::overlapAdd(juce::AudioBuffer<float>& buffer)
{
	// Add previous overlap to beginning of current frame
	for (int i = 0; i < overlapBuffer.size(); i++)
		buffer.setSample(0, i, buffer.getSample(0, i) + overlapBuffer[i]);
	
	// Store end of frame for next overlap
	for (int i = 0; i < overlapBuffer.size(); i++)
		overlapBuffer[i] = buffer.getSample(0,hopSize + i);
}
//===============================================================================
