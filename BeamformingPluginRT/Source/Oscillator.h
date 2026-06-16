/*
  ==============================================================================

    Oscillator.h
    Created: 31 Jul 2025 11:06:39am
    Author:  josep

    Basic Oscillator class for future expansion larger projects.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <random>

enum oscillator {
    SINE,
    HarmSAW,
    Square,
    Triangle,
    Noise,
    WhiteNoise,
    Formant
};

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
    inline void setPhase(float newPhase) { phase = newPhase; }

protected:

    const double PI = juce::MathConstants<double>::pi;

    double Amplitude, frequency;
    int buffersize, samplerate;
    double Phi, phase;
    double PhaseInc;
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

class ActualNoise : public Oscillator, protected NoiseOSC {
    float incrementSample();
};


