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

    DAS(int samplerate, int N, float r);
    ~DAS() = default;

    float generateNarrowband(float freq, float amplitude);

    inline void setbrightPoint(Point2D& b, float x, float y);
    void setsourcePositions(float& radius, std::vector<Point2D>& speakers);
    void calcsourceTOI(std::vector<float>& tau, std::vector<Point2D>& speakers);
    void processcircularDAS(juce::AudioBuffer<float>& buffer, float bright_x, float bright_y);

private:

    const int speedofSound = 343; // sos in m/s
    int sampleRate, N, Nt;
    float dx, dy, r;
    bool setPosflag;

    Point2D b;

    std::vector<Point2D> speakers;
    std::vector<float> tau;

    std::unique_ptr<Oscillator> osc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAS)
};
//===============================================================================
