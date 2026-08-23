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

    // These are Really setters but I cannot be bothered to rename
    inline void getSampleRate(int samplerate) { Fs = samplerate; }
    inline void getTau(std::vector<float> Tau) { tau = Tau; }

    // Actual Getters and not your daft naming
    inline std::vector<float> getb() { return b; }
    inline std::vector<float> getcorrecteddB() { return correctedB; }
    inline std::vector<float> getnncorrection() { return nnCorrection; }

    //float calculateCurrentU(std::vector<float> tau);
    float calculateCurrentU(float tau, float tauMax);

    std::vector<float> GenerateHanning(int WindowLength);

    //float process(std::vector<float> tauvec, float x);
    float process(float tau, float tauMax, float x);
    inline float simdInnerProduct(float* in, float* kernel, int numSamples, float y = 0.0f);

    const std::vector<float> getCoeficients() const;
    void getNNBool(bool ApplyNN) { applyNN = ApplyNN; }
    void setCorrection(const std::vector<float>& correction);

public:

    int N;
    std::vector<float> n;

    std::shared_ptr<FrequencyBand> freqband;

private:

    float currentU = -1.0f;
    int Fs, ntaps, ArraySize;
    float fc, wc;
    bool windowgenerationflag, iswideband, applyNN = false;
    std::vector<float> tau, b, Out, z, win, h, nnCorrection, correctedB;;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(fracDelFIR)
};
//===============================================================================
