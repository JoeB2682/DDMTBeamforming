/*
  ==============================================================================

    BeamVisualiser.h
    Created: 24 Jun 2026 6:30:32pm
    Author:  josep

    AI Assisted (assisted in development due to time constraints) JUCE component 
    to visualise beams from circular array, built off of DAS Beamformer class 
    using ChatGPT.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DAS.h"


class BeamVisualizer : public juce::Component
{

public:

    BeamVisualizer() {};

    void setSpeakers(const std::vector<DAS::Point2D>& newSpeakers);
    void setTau(const std::vector<float>& newTau);
    void setBrightPoint(float x, float y);
    void setFrequency(float freq);
    void paint(juce::Graphics& g) override;

    void changeResolution(int Resolution) { resolution = Resolution; }

private:

    int resolution = 100;

    std::vector<DAS::Point2D> speakers;
    std::vector<float> tau;

    DAS::Point2D brightPoint{ 0.0f,0.0f };

    float frequency = 1000.0f;

    float calculateBeam(float x, float y);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeamVisualizer)
};