//==============================================================================
#include "LookandFeel.h"

//==============================================================================
//Main UI Look (Draw Background Elemenets Here)
void MainUIlookandFeel::drawmainUI(juce::Graphics& g, juce::Component& c)
{
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(15.0f));

    // Define the gradient
    juce::ColourGradient gradient(
        juce::Colours::black, 0.0f, 0.0f,
        juce::Colours::darkgrey, (float)c.getWidth(), (float)c.getHeight(), false);

    g.setGradientFill(gradient);
    g.fillRect(c.getLocalBounds());

    // Parameters for outlines and corners
    float outlinethickness = 3.0f;
    float cornerRadius = 0.0f;

    // Draw main rounded rectangle outline
    juce::Rectangle<float> bounds = c.getLocalBounds().toFloat().reduced(outlinethickness * 0.5f);
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(bounds, cornerRadius, outlinethickness);

    float lineY = cornerRadius + outlinethickness * 0.5f;

    g.fillRect(0.0f, lineY - (outlinethickness / 2.0f), (float)c.getWidth(), outlinethickness);
}
//==============================================================================
// Slider Look
SliderLookAndFeel::SliderLookAndFeel()
{
    RingColour = juce::Colours::white;
}

void SliderLookAndFeel::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    const float radius = juce::jmin(width, height) / 2.0f - 2.0f;

    const float centreX = x + width / 2.0f;
    const float centreY = y + height / 2.0f;

    const float rx = centreX - radius;
    const float ry = centreY - radius;

    const float rw = radius * 2.0f;

    const float angle =
        rotaryStartAngle +
        sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Background
    g.setColour(juce::Colours::black);
    g.fillEllipse(rx, ry, rw, rw);

    // Base ring
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawEllipse(rx, ry, rw, rw, 1.5f);

    // Value arc
    juce::Path valueArc;

    valueArc.addArc(
        rx,
        ry,
        rw,
        rw,
        rotaryStartAngle,
        angle,
        true
    );

    g.setColour(juce::Colours::white);

    g.strokePath(
        valueArc,
        juce::PathStrokeType(2.0f)
    );
}

juce::Slider::SliderLayout SliderLookAndFeel::getSliderLayout(
    juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;

    auto bounds = slider.getLocalBounds();

    const int textBoxHeight = 20;
    const int textBoxOffset = 6;

    auto sliderArea = bounds.reduced(2);

    sliderArea.removeFromBottom(
        textBoxHeight + textBoxOffset
    );

    layout.sliderBounds = sliderArea;

    layout.textBoxBounds =
    {
        (bounds.getWidth() - 60) / 2,
        sliderArea.getBottom() + textBoxOffset, 60, textBoxHeight
    };

    return layout;
}
//==============================================================================
// Button Look
void ButtonLookandFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& /*backgroundColour*/,
    bool isMouseOverButton,
    bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);

    // Fill green background if pressed
    if (isButtonDown)
        g.setColour(juce::Colours::green);
    else
        g.setColour(juce::Colours::transparentBlack);

    g.fillRect(bounds);


    // Hover overlay
    if (isMouseOverButton)
    {
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.fillRect(bounds);
    }


    // Border
    g.setColour(juce::Colours::white);
    g.drawRect(bounds, 3.0f);
}


void ButtonLookandFeel::drawButtonText(juce::Graphics& g,
    juce::TextButton& button,
    bool /*isMouseOverButton*/,
    bool /*isButtonDown*/)
{
    auto bounds = button.getLocalBounds();

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    g.drawFittedText(button.getButtonText(),
        bounds,
        juce::Justification::centred,
        1);
}
//==============================================================================
// Rounded Button Look (builds off of other button class)
void RoundedButtonLookandFeel::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& /*backgroundColour*/,
    bool isMouseOverButton,
    bool isButtonDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(1.5f);

    float cornerRadius = 10.0f;


    // Pressed state
    if (isButtonDown)
        g.setColour(juce::Colours::green);
    else
        g.setColour(juce::Colours::transparentBlack);

    g.fillRoundedRectangle(bounds, cornerRadius);


    // Hover state
    if (isMouseOverButton)
    {
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, cornerRadius);
    }


    // Border
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(bounds,
        cornerRadius,
        3.0f);
}
//==============================================================================
