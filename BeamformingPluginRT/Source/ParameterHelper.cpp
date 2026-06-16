/*
  ==============================================================================

    ParameterHelper.cpp
    Created: 16 Jun 2026 3:48:34pm
    Author:  josep

  ==============================================================================
*/

#include "ParameterHelper.h"

//==============================================================================
void ParameterHelper::addParameters(
    juce::AudioProcessorValueTreeState::ParameterLayout& layout
)
{
    layout.add(std::make_unique<juce::AudioParameterFloat>("Gain","Gain", 
                                juce::NormalisableRange<float>(0.0f, 1.0f),0.5f));


}
//==============================================================================
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
