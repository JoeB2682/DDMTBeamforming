#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookandFeel.h"
#include "MoveableXY.h"
#include "BeamVisualiser.h"

//==================================================================================
class BeamformingRTPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    BeamformingRTPluginAudioProcessorEditor (BeamformingRTPluginAudioProcessor&);
    ~BeamformingRTPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    // Timer Callbacks for Updates
    void timerCallback() override;

private:
    //==============================================================================
    //Instanciation Station
    
    BeamformingRTPluginAudioProcessor& audioProcessor;

    // Custom UI look and feel classes
    MainUIlookandFeel        mainuilookandfeel;
    SliderLookAndFeel        sliderlookandfeel;
    ButtonLookandFeel        buttonlookandfeel;
    RoundedButtonLookandFeel roundedbuttonlookandfeel;

    //Toggle Buttons 
    TextButton bypassbutton, outputtypebutton;

    // Sliders
    juce::Slider gainSlider, channelSlider;

    // Labels
    Label gainsliderlabel, channelsliderlabel, bypassbuttonlabel, outputtypebuttonlabel;

    // Parameter Attachments
    std::unique_ptr<EditorParameterHelper::SliderAttachment> gainAttachment, channelsliderattachment;
    std::unique_ptr<EditorParameterHelper::ButtonAttachment> bypassAttachment, outputtypeattachment;

    // XY Bright Point Mover
    BrightPointPlot brightpointplot;

    // Beamvisualiser
    BeamVisualizer beamvisualiser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeamformingRTPluginAudioProcessorEditor)
};
//===================================================================================