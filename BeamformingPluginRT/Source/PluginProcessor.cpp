#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BeamformingRTPluginAudioProcessor::BeamformingRTPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

BeamformingRTPluginAudioProcessor::~BeamformingRTPluginAudioProcessor()
{
}

//==============================================================================
const juce::String BeamformingRTPluginAudioProcessor::getName() const { 
    return JucePlugin_Name; 
}
bool BeamformingRTPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}
bool BeamformingRTPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}
bool BeamformingRTPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}
double BeamformingRTPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int BeamformingRTPluginAudioProcessor::getNumPrograms() { return 1; }
int BeamformingRTPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}
void BeamformingRTPluginAudioProcessor::setCurrentProgram (int index)
{
}
const juce::String BeamformingRTPluginAudioProcessor::getProgramName (int index)
{
    return {};
}
void BeamformingRTPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BeamformingRTPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

}

void BeamformingRTPluginAudioProcessor::releaseResources(){}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BeamformingRTPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BeamformingRTPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    getChainSettings(chainsettings);

    float gain = chainsettings.Gain;
   
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

    }
}

//==============================================================================
bool BeamformingRTPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* BeamformingRTPluginAudioProcessor::createEditor()
{
    return new BeamformingRTPluginAudioProcessorEditor (*this);
}
//==============================================================================
void BeamformingRTPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData){}
void BeamformingRTPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes){}

void BeamformingRTPluginAudioProcessor::getChainSettings(ChainSettings& settings)
{
    // Load Parameters into chainsettings
    settings.Gain = apvts.getRawParameterValue("Gain")->load();
}

juce::AudioProcessorValueTreeState::ParameterLayout
BeamformingRTPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    ParameterHelper::addParameters(layout);

    return layout;
}
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new BeamformingRTPluginAudioProcessor(); }
