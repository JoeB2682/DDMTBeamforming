#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookandFeel.h"
#include "MoveableXY.h"
#include "BeamVisualiser.h"
#include "LevelMeter.h"

//==================================================================================
class BeamformingRTPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    BeamformingRTPluginAudioProcessorEditor(BeamformingRTPluginAudioProcessor&);
    ~BeamformingRTPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
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
    WhiteSliderLookAndFeel   whitesliderlookandfeel;

    //Toggle Buttons 
    TextButton bypassbutton, outputtypebutton, mtrackbutton;

    // Sliders
    juce::Slider gainSlider, channelSlider, threshslider, inpgainslider, f0slider, f1slider, f2slider;

    // Labels
    Label gainsliderlabel, channelsliderlabel, bypassbuttonlabel, outputtypebuttonlabel,
        threshsliderlabel, inputmeterlabel, inpgainsliderlabel, mtrackbuttonlabel, f0sliderlabel, f1sliderlabel, f2sliderlabel;

    // Parameter Attachments
    std::unique_ptr<EditorParameterHelper::SliderAttachment> gainAttachment, channelsliderattachment, 
                                                             threshsliderattachment, inpgainsliderattachment,
                                                             f0sliderattachment, f1sliderattachment, f2sliderattachment;

    std::unique_ptr<EditorParameterHelper::ButtonAttachment> bypassAttachment, outputtypeattachment, mtrackbuttonattachment;

    // XY Bright Point Mover
    BrightPointPlot brightpointplot;

    // Beamvisualiser
    BeamVisualizer beamvisualiser;

    // Input Meter
    LevelMeter inputMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeamformingRTPluginAudioProcessorEditor)
};
//===================================================================================