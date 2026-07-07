//===============================================================================
//
// FFTProcessor.h
// 
// Abstracts FFT Functions from main processor. Also allows for the sharing of 
// buffers between objects easier.
// 
// For manipulation of phase and magnitudes, inherit this class and override the 
// processFreqDomain funcition. An example of its use has been given...
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once
#include <juceheader.h>
//===============================================================================
class FFTProcessor 
{
public:

    FFTProcessor(int fftorder, int samplerate);
    ~FFTProcessor();

    void releaseResources();
    void pushNextSampleIntoFifo(float sample);
    bool getNextFFTBlockReady() const { return nextFFTBlockReady; }
    void setNextFFTBlockReady(bool ready) { nextFFTBlockReady = ready; }

    void populateDataVectors(std::vector<float>& magnvect, 
                             std::vector<float>& phasevec, 
                             std::vector<float>& freqvec,
                             std::vector<float>& rawFFTData);

    const float* getrawFFTData() const { return fftData.data(); }
    const float* getMagnitudes() const { return magnitude.data(); }
    const float* getPhases() const { return phase.data(); }
    const float* getFrequencies() const { return frequencies.data(); }

    void processFFT();
    void processIFFT();

    virtual void processFreqDomain(std::vector<float>& magnvect,
                                   std::vector<float>& phasevec,
                                   std::vector<float>& freqvec,
                                   std::vector<float>& rawFFTData);

public:

    int fftOrder, fftSize, sampleRate;
   
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    juce::dsp::FFT FFT;
    std::vector<float> fifo, fftData, magnitude, phase, frequencies;

    int fifoIndex = 0;
    bool nextFFTBlockReady = false;
    bool fftReadyForInverse = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTProcessor)
};
//===============================================================================
