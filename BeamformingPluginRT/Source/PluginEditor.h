#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeamformingRTPluginAudioProcessorEditor)
};
//===================================================================================