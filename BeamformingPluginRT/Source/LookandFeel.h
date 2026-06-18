//==============================================================================
#pragma once

#include <JuceHeader.h>

using namespace juce;
//==============================================================================
//Main UI Look (Draw Background Elemenets Here)
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
// Slider Look
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
// Button Look
class ButtonLookandFeel : public juce::LookAndFeel_V4
{
public:
    ButtonLookandFeel() = default;
    ~ButtonLookandFeel() override = default;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool isMouseOverButton, bool isButtonDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonLookandFeel)
};
//==============================================================================
// Rounded Button Look (builds off of other button class)
class RoundedButtonLookandFeel : public ButtonLookandFeel
{
public:
    RoundedButtonLookandFeel() = default;
    ~RoundedButtonLookandFeel() override = default;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
        const juce::Colour& backgroundColour,
        bool isMouseOverButton, bool isButtonDown) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoundedButtonLookandFeel)
};
//==============================================================================
