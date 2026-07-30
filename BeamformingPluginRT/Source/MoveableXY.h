//===============================================================================
//
// MoveableXY.h
// 
// XY Grid to move the bright point within the array
// 
// Modified class I made for a prior project...
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>

//===============================================================================
class BrightPointPlot : public juce::Component
{
public:

    BrightPointPlot();
    ~BrightPointPlot() override = default;

    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    void setBrightPoint(float x, float y);

    juce::Point<float> getBrightPoint();
    std::function<void(float, float)> onPointMoved;

    juce::Point<float> getNormalizedPosition(int x, int y);
    juce::Point<float> brightPoint;

    bool dragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrightPointPlot)
};
//===============================================================================