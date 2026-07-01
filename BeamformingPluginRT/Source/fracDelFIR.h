//===============================================================================
//
// fracDelFIR.h
// 
// Fractional Delay FIR filter for Filter and Sum Beamformer implementation.
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <limits>
#include <memory>
#include "FrequencyBand.h"

//===============================================================================
class fracDelFIR
{
public:

    fracDelFIR(int ntaps, int ArraySize, int srate, float freq, bool iswideband, std::shared_ptr<FrequencyBand> band);
    ~fracDelFIR() = default;

    inline void getSampleRate(int samplerate) { Fs = samplerate; }
    inline void getTau(std::vector<float> Tau) { tau = Tau; }

    //float calculateCurrentU(std::vector<float> tau);
    float calculateCurrentU(float tau, float tauMax);

    std::vector<float> GenerateHanning(int WindowLength);

    //float process(std::vector<float> tauvec, float x);
    float process(float tau, float tauMax, float x);

    inline float simdInnerProduct(float* in, float* kernel, int numSamples, float y = 0.0f);

public:

    int N;
    std::vector<float> n;

    std::shared_ptr<FrequencyBand> freqband;

private:

    float currentU = -1.0f;
    int Fs, ntaps, ArraySize;
    float fc, wc;
    bool windowgenerationflag, iswideband;
    std::vector<float> tau, b, Out, z, win, h;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fracDelFIR)
};
//===============================================================================
