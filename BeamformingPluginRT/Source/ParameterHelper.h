/*
  ==============================================================================

    ParameterHelper.h
    Created: 16 Jun 2026 3:48:34pm
    Author:  josep

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class ParameterHelper
{
public:

    static void addParameters(
        juce::AudioProcessorValueTreeState::ParameterLayout& layout
    );

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterHelper)
};
//==============================================================================
class EditorParameterHelper
{
public:

    using SliderAttachment =
        juce::AudioProcessorValueTreeState::SliderAttachment;


    static std::unique_ptr<SliderAttachment> createSliderAttachment(
        juce::AudioProcessorValueTreeState& apvts,
        const juce::String& parameterID,
        juce::Slider& slider,
        juce::LookAndFeel* lookAndFeel = nullptr
    );

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorParameterHelper)
};
//==============================================================================
