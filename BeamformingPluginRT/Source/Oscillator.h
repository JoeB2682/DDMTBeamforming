//===============================================================================
//
// Oscillator.h
// 
// Oscillator classes for the generation of pure, complex tones and noise.
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>
#include <random>

//===============================================================================
// Sine wave generator (pure tone, narrowband)
class Oscillator
{
public:

    Oscillator();
    virtual ~Oscillator() = default;

    void setupOscillator(int sampleRate, int BufferSize, bool offset);
    virtual void updateIncrement();
    void setFrequency(float Frequency);
    void setAmplitude(float Amplitude);
    virtual float incrementSample();
    virtual inline void resetPhase() { phase = 0.0f; }
    static float randomFloat(float min, float max);

    inline void resetPhaseRandom() { phase = randomFloat(0.0f, 2.0f * juce::MathConstants<float>::pi); }
    inline void setTargetAmplitude(float newAmp) { targetAmplitude = newAmp; }
    inline void setPhaseOffset(float newOffset) { Phi = newOffset; }

protected:

    const double PI = juce::MathConstants<double>::pi;

    double Amplitude, frequency;
    int buffersize, samplerate;
    double Phi, phase;
    double PhaseInc, PhaseOffset;
    bool Offset;
    double sample;

    double targetAmplitude = 0.0;
    double ampSmoothingCoeff = 0.001;

public:

    double currentAmplitude = 0.0;

    float samplePeriod = samplerate / frequency;
    inline float getSamplePeriod() { return samplePeriod; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator)
};

//===============================================================================
// Saw wave generator
class HarmonicOsc : public Oscillator
{
public:

    HarmonicOsc();
    ~HarmonicOsc() override = default;

    float incrementSample();
    void getSliderVal(int noHarmonics) { numberofHarmonics = noHarmonics; }

protected:

    int maxHarmonics = std::min(numberofHarmonics, int(samplerate / (2 * frequency)));

    int numberofHarmonics;
    double time, harmonicfreq;
    float harmonicPhaseInc = 2.0f * PI * harmonicfreq / samplerate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HarmonicOsc)
};

//===============================================================================
// Square wave generator
class SquareOsc : public Oscillator
{
public:

    SquareOsc();
    ~SquareOsc() override = default;

    float incrementSample();
    void getSliderVal(int noHarmonics) { numberofHarmonics = noHarmonics; }

protected:

    int maxHarmonics = std::min(numberofHarmonics, int(samplerate / (2 * frequency)));

    int numberofHarmonics;
    double time, harmonicfreq;
    float harmonicPhaseInc = 2.0f * PI * harmonicfreq / samplerate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SquareOsc)
};
//===============================================================================
// Triangle wave generator
class TriangleOsc : public Oscillator
{
public:

    TriangleOsc();
    ~TriangleOsc() override = default;

    float incrementSample();
    void getSliderVal(int noHarmonics) { numberofHarmonics = noHarmonics; }

protected:

    int maxHarmonics = std::min(numberofHarmonics, int(samplerate / (2 * frequency)));

    int numberofHarmonics;
    double time, harmonicfreq;
    float harmonicPhaseInc = 2.0f * PI * harmonicfreq / samplerate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TriangleOsc)
};

//===============================================================================
// Mixed noise and harmonics
class NoiseOSC : public Oscillator
{
public:
    NoiseOSC();
    ~NoiseOSC() override = default;

    float incrementSample() override;
    void updateIncrement() override;
    void getSliderVal(int noHarmonics) { numberofHarmonics = noHarmonics; }

protected:

    int numberofHarmonics;
    float time;
    float harmonicfreq;
    std::vector<float> harmonicAmps; // stored amplitudes for harmonics
    int ampUpdateCounter;
    int ampUpdateRate; // how many samples between amplitude randomisation

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseOSC)
};
//===============================================================================
// Generates white noise
class ActualNoise : public Oscillator, protected NoiseOSC {
    float incrementSample();
};
//===============================================================================


