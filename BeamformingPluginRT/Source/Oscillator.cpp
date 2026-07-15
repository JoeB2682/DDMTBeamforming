//===============================================================================
#include "Oscillator.h"
//===============================================================================
Oscillator::Oscillator() :

    Amplitude(0.0f), frequency(0.0f),
    buffersize(NULL), samplerate(44100),
    Phi(0.0f), phase(0.0f), Offset(false),
    sample(0.0)
{
    updateIncrement();
}

void Oscillator::setupOscillator(int sampleRate, int BufferSize, bool offset)
{
    samplerate = sampleRate; buffersize = BufferSize; Offset = offset;

    //Phi = (offset) ? PI / 2.0 : 0.0;
    Phi = 0.0;
}

void Oscillator::updateIncrement() { PhaseInc = (2 * PI * frequency) / samplerate; }
void Oscillator::setFrequency(float Frequency) { frequency = Frequency; updateIncrement(); }
void Oscillator::setAmplitude(float amplitude) { Amplitude = amplitude; }

float Oscillator::incrementSample()
{
    // smooth amplitude each sample (exponential ramp)
    currentAmplitude += ampSmoothingCoeff * (targetAmplitude - currentAmplitude);

    sample = juce::dsp::FastMathApproximations::sin(phase + Phi);

    phase += PhaseInc;

    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    return sample * currentAmplitude;
}

float Oscillator::randomFloat(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

//===============================================================================
HarmonicOsc::HarmonicOsc() :
    Oscillator(),
    numberofHarmonics(1),
    time(0),
    harmonicfreq(frequency) {
}

float HarmonicOsc::incrementSample()
{
    float result = 0.0f;

    currentAmplitude += ampSmoothingCoeff * (targetAmplitude - currentAmplitude);

    int maxHarmonics = (int)(samplerate / 2 / frequency);
    int numHarmonicsToUse = juce::jmin(numberofHarmonics, maxHarmonics);

    for (int k = 1; k <= numHarmonicsToUse; k++) {
        result += (currentAmplitude / k) * juce::dsp::FastMathApproximations::sin(phase * k + Phi);
    }

    phase += PhaseInc;

    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    return result;
}
//===============================================================================
SquareOsc::SquareOsc() :
    Oscillator(),
    numberofHarmonics(10),
    time(0),
    harmonicfreq(frequency) {
}

float SquareOsc::incrementSample()
{
    float result = 0.0f;

    currentAmplitude += ampSmoothingCoeff * (targetAmplitude - currentAmplitude);

    int maxHarmonics = (int)(samplerate / 2 / frequency);
    int numHarmonicsToUse = juce::jmin(numberofHarmonics, maxHarmonics);

    for (int k = 1; k <= numHarmonicsToUse; k += 2)
    {
        harmonicfreq = frequency * k;

        result += (currentAmplitude / k) * juce::dsp::FastMathApproximations::sin(phase * k + Phi);
    }

    phase += PhaseInc;

    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    return result;
}
//===============================================================================
TriangleOsc::TriangleOsc() :
    Oscillator(),
    numberofHarmonics(10),
    time(0),
    harmonicfreq(frequency)
{
    updateIncrement();
}


float TriangleOsc::incrementSample()
{
    float result = 0.0f;

    currentAmplitude += ampSmoothingCoeff * (targetAmplitude - currentAmplitude);

    int maxHarmonics = (int)(samplerate / 2 / frequency);
    int numHarmonicsToUse = juce::jmin(numberofHarmonics, maxHarmonics);

    for (int k = 1; k <= numHarmonicsToUse; k += 2)
    {
        harmonicfreq = frequency * k;

        float sign = std::pow(-1.0f, (k - 1) / 2.0f);
        result += sign * ((8 / pow(PI, 2)) * currentAmplitude / (k * k)) * juce::dsp::FastMathApproximations::sin(phase * k + Phi);
    }

    phase += PhaseInc;

    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    return result;
}
//===============================================================================
NoiseOSC::NoiseOSC() :
    Oscillator(),
    numberofHarmonics(50),
    time(0),
    harmonicfreq(frequency),
    ampUpdateCounter(0),
    ampUpdateRate(103)
{
    harmonicAmps.resize(numberofHarmonics);
    updateIncrement();
}

void NoiseOSC::updateIncrement()
{
    float jitteredFreq = frequency * randomFloat(0.95f, 1.05f);
    PhaseInc = (2.0f * juce::MathConstants<float>::pi * jitteredFreq) / samplerate;
}

float NoiseOSC::incrementSample()
{
    if (ampUpdateCounter++ >= ampUpdateRate)
    {
        ampUpdateCounter = 0;
        for (int k = 0; k < numberofHarmonics; k++)
            harmonicAmps[k] = randomFloat(0.00f, 1.00f);
    }

    float result = 0.0f;
    int maxHarmonics = juce::jmin(numberofHarmonics, (int)(samplerate / (2.0f * frequency)));

    for (int k = 1; k <= maxHarmonics; ++k)
    {
        result += (harmonicAmps[k - 1] / sqrt(k)) *
            juce::dsp::FastMathApproximations::sin(phase * k + Phi);
    }

    //phase = phase + PhaseInc, 2.0f * juce::MathConstants<float>::pi;

    phase += PhaseInc;

    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    return result;
}
//===============================================================================
float ActualNoise::incrementSample()
{
    return randomFloat(-1.0f, 1.0f);
}
//===============================================================================
