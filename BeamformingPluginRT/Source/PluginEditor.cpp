//==============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BeamformingRTPluginAudioProcessorEditor::BeamformingRTPluginAudioProcessorEditor(BeamformingRTPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Size of plugin
    setSize(800, 500);

    // Start Callcback
    startTimerHz(30);

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

    addAndMakeVisible(inpgainslider);
    inpgainsliderattachment = EditorParameterHelper::createSliderAttachment
    (audioProcessor.apvts, "inpgain", inpgainslider, &sliderlookandfeel);
    inpgainslider.setLookAndFeel(&sliderlookandfeel);
    inpgainslider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inpgainslider.setRange(0.0f, 1000.f, 0.1f);
    inpgainslider.setNumDecimalPlacesToDisplay(1);

    inpgainsliderlabel.setText("InpGain", juce::dontSendNotification);
    inpgainsliderlabel.setJustificationType(juce::Justification::centred);
    inpgainsliderlabel.attachToComponent(&inpgainslider, false);

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

    addAndMakeVisible(threshslider);
    threshsliderattachment = EditorParameterHelper::createSliderAttachment
    (audioProcessor.apvts, "thresh", threshslider, &sliderlookandfeel);
    threshslider.setLookAndFeel(&sliderlookandfeel);
    threshslider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    threshslider.setRange(0.0f, 1.0f, 0.000001f);
    threshslider.setNumDecimalPlacesToDisplay(2);

    threshsliderlabel.setText("thresh", juce::dontSendNotification);
    threshsliderlabel.setJustificationType(juce::Justification::centred);
    threshsliderlabel.attachToComponent(&threshslider, false);

    {
        // Buttons and toggles (ENSURE TO CONTROL BUTTONS THROUGH TOGGLE STATE NOT A SEPERATE BOOLEAN!)

        bypassbutton.setClickingTogglesState(true);
        bypassAttachment = EditorParameterHelper::createButtonAttachment
        (audioProcessor.apvts, "Bypass", bypassbutton, &roundedbuttonlookandfeel);

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

    // Bright Point Plot
    addAndMakeVisible(brightpointplot);

    brightpointplot.onPointMoved = [this](float x, float y)
        {
            audioProcessor.apvts.getParameter("BrightX")
                ->setValueNotifyingHost((x + 1.0f) * 0.5f);

            audioProcessor.apvts.getParameter("BrightY")
                ->setValueNotifyingHost((y + 1.0f) * 0.5f);
        };

    // Beamvisualiser
    addAndMakeVisible(beamvisualiser);
    beamvisualiser.setSpeakers(audioProcessor.DelayandSumBeamformer->speakers);
    beamvisualiser.setTau(audioProcessor.DelayandSumBeamformer->tau_Corrected);

    // Input Meter
    addAndMakeVisible(inputMeter);
    inputMeter.setBarCol(juce::Colours::green);
    inputMeter.setBGCol(juce::Colours::black);
    inputmeterlabel.setText("RecInput", juce::dontSendNotification);
    inputmeterlabel.setJustificationType(juce::Justification::centred);
    inputmeterlabel.attachToComponent(&inputMeter, false);
}

BeamformingRTPluginAudioProcessorEditor::~BeamformingRTPluginAudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
    bypassbutton.setLookAndFeel(nullptr);
    channelSlider.setLookAndFeel(nullptr);
    threshslider.setLookAndFeel(nullptr);

    setLookAndFeel(nullptr);
}

//==============================================================================
void BeamformingRTPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    mainuilookandfeel.drawmainUI(g, *this);
}
//==============================================================================
void BeamformingRTPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Bright Point Plot
    brightpointplot.setBounds(20, 20, 200, 200);

    // Beam Plot
    beamvisualiser.setBounds(brightpointplot.getRight() + 20, brightpointplot.getY(), brightpointplot.getWidth(), brightpointplot.getHeight());

    // Sliders
    int startX = beamvisualiser.getRight() + 20;
    int endX = getRight() - 20;

    std::vector<juce::Component*> s = { &gainSlider, &channelSlider, &threshslider };

    float step = (float)(endX - startX - 100) / (s.size() - 1);

    for (int i = 0; i < s.size(); i++)
        s[i]->setBounds(startX + (int)(i * step), beamvisualiser.getY() + 20, 100, 100);

    // Buttons
    bypassbutton.setBounds(gainSlider.getX() + (gainSlider.getWidth() - 50) / 2, beamvisualiser.getBottom() - 50, 50, 50);
    outputtypebutton.setBounds(channelSlider.getX() + (channelSlider.getWidth() - 80) / 2, beamvisualiser.getBottom() - 50, 80, 50);

    inputMeter.setBounds(threshslider.getX(), threshslider.getBottom() + 43, threshslider.getWidth(), 20);

    // More Sliders
    inpgainslider.setBounds(inputMeter.getX(), inputMeter.getBottom() + 40, inputMeter.getWidth(), 100);
}
//==============================================================================
// Timer Callback 
void BeamformingRTPluginAudioProcessorEditor::timerCallback()
{
    auto x = audioProcessor.apvts.getRawParameterValue("BrightX")->load();
    auto y = audioProcessor.apvts.getRawParameterValue("BrightY")->load();

    x *= audioProcessor.ArrayRadius;
    y *= audioProcessor.ArrayRadius;

    beamvisualiser.setBrightPoint(x, y);
    beamvisualiser.setTau(audioProcessor.DelayandSumBeamformer->tau);

    inputMeter.setLevel(audioProcessor.miclevel);
}
//==============================================================================