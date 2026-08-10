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

        mtrackbutton.setClickingTogglesState(true);
        mtrackbuttonattachment = EditorParameterHelper::createButtonAttachment
        (audioProcessor.apvts, "MTrack", mtrackbutton, &roundedbuttonlookandfeel);

        mtrackbutton.setLookAndFeel(&roundedbuttonlookandfeel);
        mtrackbutton.setButtonText("OFF");

        mtrackbuttonlabel.setText("Motion Tracking", juce::dontSendNotification);
        mtrackbuttonlabel.setJustificationType(juce::Justification::centred);
        mtrackbuttonlabel.attachToComponent(&mtrackbutton, false);

        mtrackbutton.onClick = [this]
            {
                mtrackbutton.setButtonText(mtrackbutton.getToggleState() ? "OFF" : "ON");
            };

        addAndMakeVisible(mtrackbutton);
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

    {
        // Frequency Sliders
        addAndMakeVisible(f0slider);
        f0sliderattachment = EditorParameterHelper::createSliderAttachment
        (audioProcessor.apvts, "F0", f0slider, &whitesliderlookandfeel);
        f0slider.setLookAndFeel(&whitesliderlookandfeel);
        f0slider.setSliderStyle(juce::Slider::LinearHorizontal);
        f0slider.setRange(200.0f, 766.67f, 0.01f);
        f0slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        f0slider.setNumDecimalPlacesToDisplay(2);

        f0sliderlabel.setText("F0", juce::dontSendNotification);
        f0sliderlabel.setJustificationType(juce::Justification::left);
        f0sliderlabel.attachToComponent(&f0slider, true);

        addAndMakeVisible(f1slider);
        f1sliderattachment = EditorParameterHelper::createSliderAttachment(
        audioProcessor.apvts, "F1", f1slider, &whitesliderlookandfeel);
        f1slider.setLookAndFeel(&whitesliderlookandfeel);
        f1slider.setSliderStyle(juce::Slider::LinearHorizontal);
        f1slider.setRange(766.67f, 1333.33f, 0.01f);
        f1slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        f1slider.setNumDecimalPlacesToDisplay(2);

        f1sliderlabel.setText("F1", juce::dontSendNotification);
        f1sliderlabel.setJustificationType(juce::Justification::left);
        f1sliderlabel.attachToComponent(&f1slider, true);

        addAndMakeVisible(f2slider);
        f2sliderattachment = EditorParameterHelper::createSliderAttachment(
        audioProcessor.apvts, "F2", f2slider, &whitesliderlookandfeel);
        f2slider.setLookAndFeel(&whitesliderlookandfeel);
        f2slider.setSliderStyle(juce::Slider::LinearHorizontal);
        f2slider.setRange(1333.33f, 1900.0f, 0.01f);
        f2slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        f2slider.setNumDecimalPlacesToDisplay(2);

        f2sliderlabel.setText("F2", juce::dontSendNotification);
        f2sliderlabel.setJustificationType(juce::Justification::left);
        f2sliderlabel.attachToComponent(&f2slider, true);
    }

    {
        // Room Params
        addAndMakeVisible(lengthslider);
        lengthsliderattachment = EditorParameterHelper::createSliderAttachment(
            audioProcessor.apvts, "Length", lengthslider, &whitesliderlookandfeel);
        lengthslider.setLookAndFeel(&whitesliderlookandfeel);
        lengthslider.setSliderStyle(juce::Slider::LinearHorizontal);
        lengthslider.setRange(4.0f, 8.0f, 0.01f);
        lengthslider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        lengthslider.setNumDecimalPlacesToDisplay(2);

        lengthsliderlabel.setText("L", juce::dontSendNotification);
        lengthsliderlabel.setJustificationType(juce::Justification::left);
        lengthsliderlabel.attachToComponent(&lengthslider, true);

        addAndMakeVisible(widthslider);
        widthsliderattachment = EditorParameterHelper::createSliderAttachment(
            audioProcessor.apvts, "Width", widthslider, &whitesliderlookandfeel);
        widthslider.setLookAndFeel(&whitesliderlookandfeel);
        widthslider.setSliderStyle(juce::Slider::LinearHorizontal);
        widthslider.setRange(4.0f, 8.0f, 0.01f);
        widthslider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        widthslider.setNumDecimalPlacesToDisplay(2);

        widthsliderlabel.setText("W", juce::dontSendNotification);
        widthsliderlabel.setJustificationType(juce::Justification::left);
        widthsliderlabel.attachToComponent(&widthslider, true);

        addAndMakeVisible(heightslider);
        heightsliderattachment = EditorParameterHelper::createSliderAttachment(
            audioProcessor.apvts, "Height", heightslider, &whitesliderlookandfeel);
        heightslider.setLookAndFeel(&whitesliderlookandfeel);
        heightslider.setSliderStyle(juce::Slider::LinearHorizontal);
        heightslider.setRange(2.5f, 3.5f, 0.01f);
        heightslider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        heightslider.setNumDecimalPlacesToDisplay(2);

        heightsliderlabel.setText("H", juce::dontSendNotification);
        heightsliderlabel.setJustificationType(juce::Justification::left);
        heightsliderlabel.attachToComponent(&heightslider, true);

        addAndMakeVisible(maxorderslider);
        maxordersliderattachment = EditorParameterHelper::createSliderAttachment(
            audioProcessor.apvts, "MaxOrder", maxorderslider, &whitesliderlookandfeel);
        maxorderslider.setLookAndFeel(&whitesliderlookandfeel);
        maxorderslider.setSliderStyle(juce::Slider::LinearHorizontal);
        maxorderslider.setRange(5.0f, 19.08f, 0.01f);
        maxorderslider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        maxorderslider.setNumDecimalPlacesToDisplay(2);

        maxordersliderlabel.setText("MO.", juce::dontSendNotification);
        maxordersliderlabel.setJustificationType(juce::Justification::left);
        maxordersliderlabel.attachToComponent(&maxorderslider, true);

        addAndMakeVisible(absorptionslider);
        absorptionsliderattachment = EditorParameterHelper::createSliderAttachment(
            audioProcessor.apvts, "Absorption", absorptionslider, &whitesliderlookandfeel);
        absorptionslider.setLookAndFeel(&whitesliderlookandfeel);
        absorptionslider.setSliderStyle(juce::Slider::LinearHorizontal);
        absorptionslider.setRange(0.2f, 0.8f, 0.01f);
        absorptionslider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 30);
        absorptionslider.setNumDecimalPlacesToDisplay(2);

        absorptionsliderlabel.setText("A", juce::dontSendNotification);
        absorptionsliderlabel.setJustificationType(juce::Justification::left);
        absorptionsliderlabel.attachToComponent(&absorptionslider, true);
    }
}

BeamformingRTPluginAudioProcessorEditor::~BeamformingRTPluginAudioProcessorEditor()
{
    gainSlider.setLookAndFeel(nullptr);
    bypassbutton.setLookAndFeel(nullptr);
    channelSlider.setLookAndFeel(nullptr);
    threshslider.setLookAndFeel(nullptr);
    outputtypebutton.setLookAndFeel(nullptr);
    mtrackbutton.setLookAndFeel(nullptr);

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

    float plotsyoffset = 220;
    float slideryoffset = 60;

    brightpointplot.setBounds(20, 20 + plotsyoffset, 200, 200);
    beamvisualiser.setBounds(240, 20 + plotsyoffset, 200, 200);

    gainSlider.setBounds(20, 60 + slideryoffset, 100, 100);
    inpgainslider.setBounds(120, 60 + slideryoffset, 100, 100);
    threshslider.setBounds(240, 60 + slideryoffset, 100, 100);
    channelSlider.setBounds(340, 60 + slideryoffset, 100, 100);

    float xf0offset = 52;
    float yf0offset = -40;

    f0sliderlabel.setBounds(415 + xf0offset, 80 + slideryoffset + yf0offset, 25, 30);
    f0slider.setBounds(440 + xf0offset, 75 + slideryoffset + yf0offset, 300, 30);

    f1sliderlabel.setBounds(415 + xf0offset, 120 + slideryoffset + yf0offset, 25, 30);
    f1slider.setBounds(440 + xf0offset, 115 + slideryoffset + yf0offset, 300, 30);

    f2sliderlabel.setBounds(415 + xf0offset, 160 + slideryoffset + yf0offset, 25, 30);
    f2slider.setBounds(440 + xf0offset, 155 + slideryoffset + yf0offset, 300, 30);

    lengthsliderlabel.setBounds(415 + xf0offset, 200 + slideryoffset + yf0offset, 25, 30);
    lengthslider.setBounds(440 + xf0offset, 195 + slideryoffset + yf0offset, 300, 30);

    widthsliderlabel.setBounds(415 + xf0offset, 240 + slideryoffset + yf0offset, 25, 30);
    widthslider.setBounds(440 + xf0offset, 235 + slideryoffset + yf0offset, 300, 30);

    heightsliderlabel.setBounds(415 + xf0offset, 280 + slideryoffset + yf0offset, 25, 30);
    heightslider.setBounds(440 + xf0offset, 275 + slideryoffset + yf0offset, 300, 30);

    maxordersliderlabel.setBounds(415 + xf0offset, 320 + slideryoffset + yf0offset, 25, 30);
    maxorderslider.setBounds(440 + xf0offset, 315 + slideryoffset + yf0offset, 300, 30);

    absorptionsliderlabel.setBounds(415 + xf0offset, 360 + slideryoffset + yf0offset, 25, 30);
    absorptionslider.setBounds(440 + xf0offset, 355 + slideryoffset + yf0offset, 300, 30);

    bypassbutton.setBounds(13, 25, 50, 50);
    outputtypebutton.setBounds(465, 440, 80, 50);
    mtrackbutton.setBounds(555, 440, 80, 50);

    inputMeter.setBounds(20, 470, 420, 20);
}
//==============================================================================
// Timer Callback 
void BeamformingRTPluginAudioProcessorEditor::timerCallback()
{
    auto x = audioProcessor.apvts.getRawParameterValue("BrightX")->load();
    auto y = audioProcessor.apvts.getRawParameterValue("BrightY")->load();
    
    bool MTrack = audioProcessor.apvts.getRawParameterValue("MTrack")->load();

    if (MTrack) {
        beamvisualiser.setBrightPoint(audioProcessor.chainsettings.brightx, audioProcessor.chainsettings.brighty);
        brightpointplot.setBrightPoint(audioProcessor.chainsettings.brightx, audioProcessor.chainsettings.brighty);
    }
    else
    {
        x *= audioProcessor.ArrayRadius;
        y *= audioProcessor.ArrayRadius;

        beamvisualiser.setBrightPoint(x, y);
    }

    beamvisualiser.setTau(audioProcessor.DelayandSumBeamformer->tau);

    inputMeter.setLevel(audioProcessor.miclevel);
}
//==============================================================================