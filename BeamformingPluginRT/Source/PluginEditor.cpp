#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BeamformingRTPluginAudioProcessorEditor::BeamformingRTPluginAudioProcessorEditor (BeamformingRTPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Size of plugin
    setSize(800, 500);

    // Attach Params and make visible
    addAndMakeVisible(gainSlider);
    gainAttachment = EditorParameterHelper::createSliderAttachment
        (audioProcessor.apvts, "Gain", gainSlider, &sliderlookandfeel);
    gainSlider.setLookAndFeel(&sliderlookandfeel);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setRange(0.0f, 1.0f, 0.01f);
    gainSlider.setNumDecimalPlacesToDisplay(2);

    gainsliderlabel.setText("Gain", juce::dontSendNotification);
    gainsliderlabel.setJustificationType(juce::Justification::centred);
    gainsliderlabel.attachToComponent(&gainSlider, false);


    addAndMakeVisible(channelSlider);
    channelsliderattachment = EditorParameterHelper::createSliderAttachment
        (audioProcessor.apvts, "Channel", channelSlider, &sliderlookandfeel);
    channelSlider.setLookAndFeel(&sliderlookandfeel);
    channelSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    channelSlider.setRange(0, 8, 1);
    channelSlider.setNumDecimalPlacesToDisplay(0);

    channelsliderlabel.setText("ChannelOut", juce::dontSendNotification);
    channelsliderlabel.setJustificationType(juce::Justification::centred);
    channelsliderlabel.attachToComponent(&channelSlider, false);

    {
        // Buttons and toggles (ENSURE TO CONTROL BUTTONS THROUGH TOGGLE STATE NOT A SEPERATE BOOLEAN!)

        bypassbutton.setClickingTogglesState(true);
        bypassAttachment = EditorParameterHelper::createButtonAttachment
            (audioProcessor.apvts, "Bypass", bypassbutton,&roundedbuttonlookandfeel);

        bypassbutton.setLookAndFeel(&roundedbuttonlookandfeel);
        bypassbutton.setButtonText("OFF");

        bypassbuttonlabel.setText("Bypass", juce::dontSendNotification);
        bypassbuttonlabel.setJustificationType(juce::Justification::centred);
        bypassbuttonlabel.attachToComponent(&bypassbutton, false);

        bypassbutton.onClick = [this]
            { 
                bypassbutton.setButtonText(bypassbutton.getToggleState() ? "ON" : "OFF"); 
            };

        addAndMakeVisible(bypassbutton);

        outputtypebutton.setClickingTogglesState(true);
        outputtypeattachment = EditorParameterHelper::createButtonAttachment
        (audioProcessor.apvts, "Outtype", outputtypebutton, &roundedbuttonlookandfeel);

        outputtypebutton.setLookAndFeel(&roundedbuttonlookandfeel);
        outputtypebutton.setButtonText("SCHAN");

        outputtypebuttonlabel.setText("Output", juce::dontSendNotification);
        outputtypebuttonlabel.setJustificationType(juce::Justification::centred);
        outputtypebuttonlabel.attachToComponent(&outputtypebutton, false);

        outputtypebutton.onClick = [this]
            {
                outputtypebutton.setButtonText(outputtypebutton.getToggleState() ? "SCHAN" : "ALL");
            };

        addAndMakeVisible(outputtypebutton);



    }





}

BeamformingRTPluginAudioProcessorEditor::~BeamformingRTPluginAudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
    bypassbutton.setLookAndFeel(nullptr);
    channelSlider.setLookAndFeel(nullptr);

    setLookAndFeel(nullptr);
}

//==============================================================================
void BeamformingRTPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); 

    mainuilookandfeel.drawmainUI(g, *this);

}

void BeamformingRTPluginAudioProcessorEditor::resized()
{
    // Resize and postion components
    auto bounds = getLocalBounds();

    // Sliders
    gainSlider.setBounds(bounds.getCentreX() + 150, bounds.getCentreY() - 100, 100, 100);
    channelSlider.setBounds(bounds.getCentreX() + 250, bounds.getCentreY() - 100, 100, 100);

    // Buttons 
    bypassbutton.setBounds(gainSlider.getX() + (gainSlider.getWidth() - 50) / 2, gainSlider.getBottom() + 30, 50, 50);
    outputtypebutton.setBounds(channelSlider.getX() + (channelSlider.getWidth() - 80) / 2, channelSlider.getBottom() + 30, 80, 50);
}
//==============================================================================