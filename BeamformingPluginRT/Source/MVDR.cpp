//===============================================================================
#include "MVDR.h"
//===============================================================================
MVDR::MVDR(DAS* dasPtr, int N, int Fs, std::shared_ptr<FFTProcessor> FFTprocessor) :
    fftprocessor(FFTprocessor), sampleRate(Fs), N(N)
{
    outputSpectrum.resize(fftprocessor->fftSize / 2);

    v.resize(N);
    w.resize(N);
    Rv.resize(N);

    R.resize(N, N);

    outputBuffer.setSize(1, fftprocessor->fftSize);
}
//===============================================================================
// Calculate Steering Vector
void MVDR::calculateSteeringVector(std::vector<float>& receivertau, float freq)
{
    for (int i = 0; i < N; i++)
    {
        // Phase term (MATLAB Equivalent: v(i) = exp(-1i*2*pi*f*tau(i));)
        float phase = -2.0f * juce::MathConstants<float>::pi * freq * receivertau[i];

        // Adds to std::complex vector
        v(i) = std::polar(1.0f, phase);
    }
}
//===============================================================================
// Calculate Weights (MMATLAB equivalent, w = R_v / (v' * R_v);)
void MVDR::calculateWeights()
{
    w = Rv / (v.adjoint() * Rv);
}
//===============================================================================
// Calculate Spatial Covariance Matrix...
// MATLAB Equivalant,  R = (mic_signals * mic_signals') / size(mic_signals,2);
void MVDR::calculateSCM(std::vector<std::vector<std::complex<float>>>& spectra, int bin)
{
    // Set coeffs to 0
    R.setZero();

    // Iterate Matrix
    for (int i = 0; i < N; i++)
    {
        // Create SCM
        for (int j = 0; j < N; j++)
            R(i, j) = spectra[i][bin] * std::conj(spectra[j][bin]);
    }

    // Normalise
    R /= N;
}
//===============================================================================
// Apply MVDR Weights (MATLAB, output = w' * mic_signals;)
std::complex<float> MVDR::applyWeights(std::vector<std::vector<std::complex<float>>>& spectra, int bin)
{
    // CReate another vector for out (vector for matrix infomation)
    Eigen::VectorXcf x(N);

    // Assign values in vector to x from spectra 
    for (int i = 0; i < N; i++)
        x(i) = spectra[i][bin];

    // Apply weights
    return w.adjoint() * x;
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

    // Single microphone adaptive filtering (as MVDR not possible woithout proper mic array)
    if (N == 1)
    {
        for (int k = 0; k < fftprocessor->fftSize / 2; k++)
        {
            // Remove DC Offset component
            if (k == 0) { outputSpectrum[k] = 0; continue; }

            // Microphone spectrum
            std::complex<float> X = spectra[0][k];

            // Calculate signal power
            float power = std::norm(X);

            // Diagonal loading / noise floor
            float lambda = 0.001f;

            // Single channel MVDR/Wiener gain
            float gain = power / (power + lambda);

            // Apply filtering
            outputSpectrum[k] = X * gain;
        }
    }

    // Multi microphone MVDR beamforming
    else
    {
        // Iterate frequency bins
        for (int k = 0; k < fftprocessor->fftSize / 2; k++)
        {
            // Remove DC Offset component
            if (k == 0) { outputSpectrum[k] = 0; continue; }

            // Calculate Current bin in Hz (needed for wideband)
            float frequency = (float)k * sampleRate / fftprocessor->fftSize;

            // Calculate steering vector for this frequency
            calculateSteeringVector(receivertau, frequency);

            // Calculate SCM
            calculateSCM(spectra, k);

            // Diagonal Loading
            Eigen::MatrixXcf Rloaded = R + 0.001f * Eigen::MatrixXcf::Identity(N, N);

            // Invert SCM (MATLAB, R_v = R\v;)
            Rv = Rloaded.ldlt().solve(v);

            // Calculate MVDR weights
            calculateWeights();

            // Beamform output
            outputSpectrum[k] = applyWeights(spectra, k);
        }
    }

    // Convert back to time domain and output
    fftprocessor->processMultiChannelIFFT(outputSpectrum, outputBuffer);

    // Apply Overlap Add
    fftprocessor->overlapAdd(outputBuffer);

    int samplesToCopy = juce::jmin(
        micbuffer.getNumSamples(),
        fftprocessor->fftSize
    );

    // Copy result back into input buffer
    for (int i = 0; i < samplesToCopy; i++)
    {
        micbuffer.setSample(0, i, outputBuffer.getSample(0, i));
    }
}
//===============================================================================