//===============================================================================
//
// LevelMeter.h
// 
// Level meter to be used for visualisation of receiver input
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>

//===============================================================================
class LevelMeter : public juce::Component
{
public:

    LevelMeter() = default;
    ~LevelMeter() = default;

    inline void setDecayFactor(float newDecayFactor) { decayFactor = newDecayFactor; }
    inline void setBGCol(juce::Colour Col) { bgCol = Col; }
    inline void setBarCol(juce::Colour Col) { barCol = Col; }

    void setLevel(float newLevel);
    void applyDecayFactor();
    void paint(juce::Graphics& g) override;

protected:

    juce::Colour bgCol, barCol;

    float level = 0.0f;
    float decayFactor = 0.95f;
};
//===============================================================================
