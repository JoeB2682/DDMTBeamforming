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

    // Draw line across full width
    float liney = 85.f;
    g.fillRect(0.0f, liney, (float)c.getWidth(), outlinethickness);

    // More stupid UI lines
    g.fillRect(455.0f, liney, outlinethickness, (float)c.getHeight() - liney);
    g.fillRect(75.0f, lineY, outlinethickness, (float)c.getHeight() - 415);
    g.fillRect(455.0f, 410.0f, 380.0f, outlinethickness);

    // Title Box
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions("Cascadia Code", 28.0f, juce::Font::bold));

    g.drawText(
        "Neural Beamforming Optimiser",
        0,
        17,
        c.getWidth(),
        50,
        juce::Justification::centred,
        false
    );
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
// Slider Look and Feel (not rotary dial)
WhiteSliderLookAndFeel::WhiteSliderLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::trackColourId, juce::Colours::white);
}

void WhiteSliderLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float /*minSliderPos*/,
    float /*maxSliderPos*/,
    const juce::Slider::SliderStyle /*style*/,
    juce::Slider& /*slider*/)
{
    // --- Track ---
    float trackY = y + height * 0.4f;
    float trackHeight = height * 0.2f;

    juce::Rectangle<float> trackBounds(
        (float)x,
        trackY,
        (float)width,
        trackHeight
    );

    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.fillRect(trackBounds);

    // --- Filled Track ---
    juce::Rectangle<float> fill(trackBounds);

    fill.setWidth(sliderPos - (float)x);

    g.setColour(juce::Colours::white);
    g.fillRect(fill);

    // --- Thumb ---
    const float maxThumbRadius = 12.0f;
    const float thumbRadius = juce::jmin(height * 0.3f, maxThumbRadius);

    const float cx = juce::jlimit(
        x + thumbRadius,
        x + width - thumbRadius,
        sliderPos
    );

    const float cy = y + height * 0.5f;

    // Outer white circle
    g.setColour(juce::Colours::white);

    g.fillEllipse(
        cx - thumbRadius,
        cy - thumbRadius,
        thumbRadius * 2.0f,
        thumbRadius * 2.0f
    );

    // Inner black circle
    const float innerRadius = thumbRadius * 0.9f;

    g.setColour(juce::Colours::black);

    g.fillEllipse(
        cx - innerRadius,
        cy - innerRadius,
        innerRadius * 2.0f,
        innerRadius * 2.0f
    );
}

int WhiteSliderLookAndFeel::getSliderThumbRadius(juce::Slider& /*slider*/)
{
    return 12;
}
//==============================================================================