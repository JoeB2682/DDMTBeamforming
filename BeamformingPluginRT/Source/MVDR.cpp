//===============================================================================
#include "MVDR.h"
//===============================================================================
MVDR::MVDR(DAS* dasPtr, int N, int Fs, std::shared_ptr<FFTProcessor> FFTprocessor) :
	fftprocessor(FFTprocessor), sampleRate(Fs), N(N)
{
    outputSpectrum.resize(fftprocessor->fftSize / 2);
}
//===============================================================================
// Calculate Steering Vector
void MVDR::calculateSteeringVector(std::vector<float> receivertau, float freq)
{
	/*
	for (int i = 0; i < N; i++) {
		v[i] = exp(-1*2*)
	}
	*/
}
//===============================================================================
// Calculate Weights
void MVDR::calculateWeights()
{

}
//===============================================================================
// Calculate Spatial Covariance Matrix
void MVDR::calculateSCM(std::vector<std::vector<std::complex<float>>>& spectra, int bin) 
{

}
//===============================================================================
// Apply MVDR Weights
std::complex<float> MVDR::applyWeights(std::vector<std::vector<std::complex<float>>>& spectra, int bin) 
{
    return 0;
}
//===============================================================================
// Main Process Function
void MVDR::processCircularMVDR(juce::AudioBuffer<float>& micbuffer, 
                               std::vector<float> receivertau)
{
    // Frequency Domain Matrix
    std::vector<std::vector<std::complex<float>>> spectra;

    // Convert microphone signals into frequency domain using FFTProcessor
    fftprocessor->processMultiChannelFFT(micbuffer, spectra);

    // Iterate frequency bins
    for (int k = 0; k < fftprocessor->fftSize / 2; k++)
    {
        // Calculate Current bin in Hz
        float frequency = (float)k * sampleRate / fftprocessor->fftSize;

        // Calculate steering vector for this frequency
        calculateSteeringVector(receivertau, frequency);

        // Calculate SCM
        calculateSCM(spectra, k);

        // Calculate MVDR weights
        calculateWeights();

        // Beamform output
        //outputSpectrum[k] = applyWeights(spectra, k);
    }
}
//===============================================================================
