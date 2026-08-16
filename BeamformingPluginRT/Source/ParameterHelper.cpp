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
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain", "Gain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.02f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("thresh", "thresh", juce::NormalisableRange<float>(0.0f, 1.0f, 0.000001f), 0.00002f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("inpgain", "inpgain", juce::NormalisableRange<float>(0.0f, 1000.f, 0.1f), 400.f));

    layout.add(std::make_unique<juce::AudioParameterInt>("Channel", "Channel", 0, 8, 1));

    // Buttons
    layout.add(std::make_unique<juce::AudioParameterBool>("Bypass", "Bypass", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("Outtype", "Outtype", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("MTrack", "MTrack", false));
    layout.add(std::make_unique<juce::AudioParameterBool>("ApplyNN", "ApplyNN", false));

    // XY Grid
    layout.add(std::make_unique<juce::AudioParameterFloat>("BrightX","BrightX", -1.0f, 1.0f, 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>( "BrightY", "BrightY", -1.0f, 1.0f, 0.0f));

    // Tone Freqs
    layout.add(std::make_unique<juce::AudioParameterFloat>("F0", "F0", 200.0f, 766.67f, 483.33f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("F1", "F1", 766.67f, 1333.33f, 1050.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("F2", "F2", 1333.33f, 1900.0f, 1616.67f));

    // Room Params
    layout.add(std::make_unique<juce::AudioParameterFloat>("Length", "Length", 4.0f, 8.0f, 4.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Width", "Width", 4.0f, 8.0f, 4.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Height", "Height", 2.5f, 3.5f, 2.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("MaxOrder", "MaxOrder", 5.0f, 19.08f, 8.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Absorption", "Absorption", 0.2f, 0.8f, 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("RT60", "RT60", 0.1f, 2.0f, 0.5f));
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
