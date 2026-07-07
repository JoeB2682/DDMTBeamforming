//==============================================================================
#include "LevelMeter.h"
//==============================================================================
void LevelMeter::setLevel(float newLevel)
{
    float attackFactor = 0.2f;
    float releaseFactor = 1.0f;

    if (newLevel > level)
        level = level + attackFactor * (newLevel - level);
    else
        level = level + releaseFactor * (newLevel - level);

    // Allows safe access from the audio thread to GUI thread (avoid if possible)
    juce::MessageManager::callAsync([this] { repaint(); });
}

//==============================================================================
void LevelMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    g.fillAll(juce::Colours::black);

    float meterWidth = juce::jmap(level, 0.0f, 1.0f, 0.0f, (float)bounds.getWidth());

    g.setColour(bgCol);
    g.fillRect(juce::Rectangle<float>(
        (float)bounds.getX(),
        (float)(bounds.getCentreY() - 5),
        (float)bounds.getWidth(),
        10.0f));

    g.setColour(barCol);
    g.fillRect(juce::Rectangle<float>(
        (float)bounds.getX(),
        (float)(bounds.getCentreY() - 5),
        meterWidth,
        10.0f));
}
//==============================================================================
void LevelMeter::applyDecayFactor() 
{
    level *= decayFactor;
    repaint();
}
//==============================================================================
