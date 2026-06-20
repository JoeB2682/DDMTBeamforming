/*
  ==============================================================================

    ParameterHelper.cpp
    Created: 16 Jun 2026 3:48:34pm
    Author:  josep

  ==============================================================================
*/

#include "ParameterHelper.h"

//==============================================================================
// Parameter Layout 
void ParameterHelper::addParameters(
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
)
{
    // Sliders
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.02f
    ));

    layout.add(std::make_unique<juce::AudioParameterInt>("Channel", "Channel", 0, 8, 1));

    // Buttons
    layout.add(std::make_unique<juce::AudioParameterBool>("Bypass", "Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Outtype", "Outtype", false));

}
//==============================================================================
// Creates Slider Attachment
std::unique_ptr<EditorParameterHelper::SliderAttachment>
EditorParameterHelper::createSliderAttachment(
    juce::AudioProcessorValueTreeState& apvts,
    const juce::String& parameterID,
    juce::Slider& slider,
    juce::LookAndFeel* lookAndFeel
)
{
    return std::make_unique<SliderAttachment>(
        apvts,
        parameterID,
        slider
    );
}

//==============================================================================
// Creates Button Attachment
std::unique_ptr<EditorParameterHelper::ButtonAttachment>
EditorParameterHelper::createButtonAttachment(
    juce::AudioProcessorValueTreeState& apvts,
    const juce::String& parameterID,
    juce::Button& button,
    juce::LookAndFeel* lookAndFeel)
{
    if (lookAndFeel != nullptr)
        button.setLookAndFeel(lookAndFeel);

    return std::make_unique<ButtonAttachment>(
        apvts,
        parameterID,
        button);
}
//==============================================================================
