//===============================================================================
//
// DAS.h
// 
// Delay and Sum beamformer class, generates and handles both narrow and wideband 
// signals. Allows for both linear and nonlinear arrays.
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"

// Class to handle all delay and sum beamforming...

//===============================================================================
class DAS 
{
public:

    // Used to store 2D coordinates
    struct Point2D
    {
        float x;
        float y;
    };

public:

    DAS(int samplerate, int N, float r, int totalNoOutputChannels);
    ~DAS() = default;

    // Generator functions
    float generateTestTone(float freq, float amplitude, float Phi);
    void generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank, juce::AudioBuffer<float>& buffer,
        float freq, float amplitude, std::vector<float>& tau, float gain);

    // Beamforming functions
    inline void setbrightPoint(Point2D& b, float x, float y);
    void setsourcePositions(float& radius, std::vector<Point2D>& speakers);
    void calcsourceTOI(std::vector<float>& tau, std::vector<Point2D>& speakers, Point2D& b);
    void processcircularDAS(juce::AudioBuffer<float>& buffer, float bright_x, float bright_y, float gain);

private:

    int totalNoOutputChannels;
    const int speedofSound = 343; // sos in m/s
    int sampleRate, N, Nt;
    float dx, dy, r;
    bool setPosflag, setoscbankflag;

    Point2D b;

    std::vector<Point2D> speakers;
    std::vector<float> tau;
    std::vector<std::unique_ptr<Oscillator>> oscbank;

    std::unique_ptr<Oscillator> testosc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAS)
};
//===============================================================================
