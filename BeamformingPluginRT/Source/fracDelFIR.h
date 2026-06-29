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

//===============================================================================
class fracDelFIR
{
public:

    fracDelFIR(int ntaps, int ArraySize, int srate);
    ~fracDelFIR() = default;

    inline void getSampleRate(int samplerate) { Fs = samplerate; }
    inline void getTau(std::vector<float> Tau) { tau = Tau; }

    float calculateCurrentU(std::vector<float> tau);

    std::vector<float> GenerateHanning(int WindowLength);

    std::vector<float> process(std::vector<float> tauvec, 
                               std::vector<float> x, 
                               std::vector<float> z);

private:

    int Fs, ntaps, N, ArraySize;
    float fc, wc;
    bool windowgenerationflag;
    std::vector<float> tau, n, b, Out, z, win, h;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fracDelFIR)
};
//===============================================================================
