//==============================================================================
#pragma once

#include <JuceHeader.h>

using namespace juce;
//==============================================================================
class MainUIlookandFeel : public juce::LookAndFeel_V4
{

public:

    MainUIlookandFeel() = default;
    ~MainUIlookandFeel() = default;

    void drawmainUI(juce::Graphics& g, juce::Component& c);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainUIlookandFeel)
};
//==============================================================================
class SliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SliderLookAndFeel();

    void drawRotarySlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

    inline void setRingCol(juce::Colour newcolour)
    {
        RingColour = newcolour;
    }

private:

    juce::Colour RingColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderLookAndFeel)
};
//==============================================================================

