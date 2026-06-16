#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BeamformingRTPluginAudioProcessorEditor::BeamformingRTPluginAudioProcessorEditor (BeamformingRTPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Attach Params and make visible
    addAndMakeVisible(gainSlider);
    gainAttachment = EditorParameterHelper::createSliderAttachment
        (audioProcessor.apvts, "Gain", gainSlider, &sliderlookandfeel);
    gainSlider.setLookAndFeel(&sliderlookandfeel);
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

    setSize (800, 500);
}

BeamformingRTPluginAudioProcessorEditor::~BeamformingRTPluginAudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);

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

    gainSlider.setBounds(bounds.getCentreX() - 50, bounds.getCentreY() - 50, 100, 100);

}
//==============================================================================