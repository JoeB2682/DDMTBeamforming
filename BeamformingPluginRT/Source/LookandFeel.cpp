//==============================================================================
#include "LookandFeel.h"

//==============================================================================
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
        sliderArea.getBottom() + textBoxOffset,
        60,
        textBoxHeight
    };

    return layout;
}
//==============================================================================
