#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookandFeel.h"

//==================================================================================
class BeamformingRTPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BeamformingRTPluginAudioProcessorEditor (BeamformingRTPluginAudioProcessor&);
    ~BeamformingRTPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    BeamformingRTPluginAudioProcessor& audioProcessor;

    MainUIlookandFeel mainuilookandfeel;
    SliderLookAndFeel sliderlookandfeel;

    // Sliders
    juce::Slider gainSlider;

    // Attachments
    std::unique_ptr<EditorParameterHelper::SliderAttachment> gainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeamformingRTPluginAudioProcessorEditor)
};
//===================================================================================