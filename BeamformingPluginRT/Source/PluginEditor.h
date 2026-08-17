#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookandFeel.h"
#include "MoveableXY.h"
#include "BeamVisualiser.h"
#include "LevelMeter.h"

//==================================================================================
class BeamformingRTPluginAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer,
                                                public juce::KeyListener
{
public:
    BeamformingRTPluginAudioProcessorEditor(BeamformingRTPluginAudioProcessor&);
    ~BeamformingRTPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    // Timer Callbacks for Updates
    void timerCallback() override;
    //void visibilityChanged() override;

    // Key press listener
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    bool loggingEnabled = false;

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
    TextButton bypassbutton, outputtypebutton, mtrackbutton, applyNNbutton;

    // Sliders
    juce::Slider gainSlider, channelSlider, threshslider, inpgainslider,
        f0slider, f1slider, f2slider,
        lengthslider, widthslider, heightslider, maxorderslider, absorptionslider,
        rt60slider;

    // Labels
    Label gainsliderlabel, channelsliderlabel, bypassbuttonlabel, outputtypebuttonlabel,
        threshsliderlabel, inputmeterlabel, inpgainsliderlabel, mtrackbuttonlabel,
        f0sliderlabel, f1sliderlabel, f2sliderlabel,
        lengthsliderlabel, widthsliderlabel, heightsliderlabel,
        maxordersliderlabel, absorptionsliderlabel,
        rt60sliderlabel, applyNNbuttonlabel;

    // Parameter Attachments
    std::unique_ptr<EditorParameterHelper::SliderAttachment>
        gainAttachment, channelsliderattachment,
        threshsliderattachment, inpgainsliderattachment,
        f0sliderattachment, f1sliderattachment, f2sliderattachment,
        lengthsliderattachment, widthsliderattachment,
        heightsliderattachment, maxordersliderattachment,
        absorptionsliderattachment, rt60sliderattachment;

    std::unique_ptr<EditorParameterHelper::ButtonAttachment> bypassAttachment, outputtypeattachment, mtrackbuttonattachment, applyNNbuttonattachment;

    // XY Bright Point Mover
    BrightPointPlot brightpointplot;

    // Beamvisualiser
    BeamVisualizer beamvisualiser;

    // Input Meter
    LevelMeter inputMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeamformingRTPluginAudioProcessorEditor)
};
//===================================================================================