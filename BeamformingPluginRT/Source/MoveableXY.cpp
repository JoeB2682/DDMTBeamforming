//==============================================================================
#include "MoveableXY.h"
//==============================================================================
BrightPointPlot::BrightPointPlot()
{
    setSize(400, 400);

    // Start bright point in centre
    brightPoint = { 0.0f, 0.0f };
}

//==============================================================================
void BrightPointPlot::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);


    float circleRadius = std::min(getWidth(), getHeight()) / 2.0f;
    float centerX = getWidth() / 2.0f;
    float centerY = getHeight() / 2.0f;

    // Draw grid
    g.setColour(juce::Colours::darkgreen);

    for (int i = 0; i < getWidth(); i += 20)
    {
        g.drawLine(i, 0, i, getHeight());
        g.drawLine(0, i, getWidth(), i);
    }

    // Draw unit circle
    g.setColour(juce::Colours::white);

    g.drawEllipse(
        centerX - circleRadius,
        centerY - circleRadius,
        circleRadius * 2,
        circleRadius * 2,
        2.0f
    );

    // Draw centre axes
    g.setColour(juce::Colours::grey);
    g.drawLine(centerX, 0, centerX, getHeight());
    g.drawLine(0, centerY, getWidth(), centerY);

    // Draw bright point
    float pointX = centerX + brightPoint.x * circleRadius;
    float pointY = centerY - brightPoint.y * circleRadius;

    g.setColour(juce::Colours::red);

    g.fillEllipse(
        pointX - 8,
        pointY - 8,
        16,
        16
    );

    // Draw outline
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds(), 3);
}
//==============================================================================
void BrightPointPlot::mouseDown(const juce::MouseEvent& event)
{
    dragging = true;
}
//==============================================================================
void BrightPointPlot::mouseDrag(const juce::MouseEvent& event)
{
    if (dragging)
    {
        brightPoint = getNormalizedPosition(event.x, event.y);

        // Updates if moved
        if (onPointMoved) onPointMoved(brightPoint.x, brightPoint.y);

        // Keep point inside unit circle (source array area)
        float radius = std::sqrt(
            brightPoint.x * brightPoint.x +
            brightPoint.y * brightPoint.y
        );

        if (radius > 1.0f)
        {
            brightPoint.x /= radius;
            brightPoint.y /= radius;
        }
        repaint();
    }
}
//==============================================================================
void BrightPointPlot::mouseUp(const juce::MouseEvent& event)
{
    dragging = false;
}
//==============================================================================
juce::Point<float> BrightPointPlot::getNormalizedPosition(int x, int y)
{
    float normalizedX = (x - getWidth() / 2.0f) / (getWidth() / 2.0f);
    float normalizedY = (getHeight() / 2.0f - y) / (getHeight() / 2.0f);

    return { normalizedX, normalizedY };
}
//==============================================================================
juce::Point<float> BrightPointPlot::getBrightPoint()
{
    return brightPoint;
}
//==============================================================================
void BrightPointPlot::setBrightPoint(float x, float y) 
{
    brightPoint.x = x;
    brightPoint.y = y;

    repaint();
}
//==============================================================================