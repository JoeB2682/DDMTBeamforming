//==============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
BeamformingRTPluginAudioProcessor::BeamformingRTPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::discreteChannels(8), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::discreteChannels(8), true)
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
int BeamformingRTPluginAudioProcessor::getCurrentProgram(){ return 0; }
void BeamformingRTPluginAudioProcessor::setCurrentProgram (int index){}
const juce::String BeamformingRTPluginAudioProcessor::getProgramName (int index){ return {};}
void BeamformingRTPluginAudioProcessor::changeProgramName (int index, const juce::String& newName){}
//==============================================================================
void BeamformingRTPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    {
        // Debug messages to ensure that I/O is configured properly
        DBG("Inputs  = " << getTotalNumInputChannels());
        DBG("Outputs = " << getTotalNumOutputChannels());

        DBG("Main input bus  = " << getMainBusNumInputChannels());
        DBG("Main output bus = " << getMainBusNumOutputChannels());
    }

    DelayandSumBeamformer = std::make_unique<DAS>(getSampleRate());
}

void BeamformingRTPluginAudioProcessor::releaseResources(){}

// Modify layout to support more channels (as specified in constructor)
bool BeamformingRTPluginAudioProcessor::isBusesLayoutSupported
(const BusesLayout& layouts) const
{
    auto outputLayout = layouts.getMainOutputChannelSet();

    if (outputLayout != juce::AudioChannelSet::discreteChannels(8))
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainInputChannelSet() != outputLayout)
        return false;
#endif
    return true;
}

void BeamformingRTPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    getChainSettings(chainsettings);

    // assign parameters to variables
    float gain = chainsettings.Gain;
    bool bypass = chainsettings.bypass;
    int Channel = chainsettings.Channel - 1;

    //DBG("isBypass = " << (bypass ? "true" : "false"));

    // Safety check stops out of bounds indexing
    if (Channel < 0 || Channel >= buffer.getNumChannels()) return;

    float numSamples = buffer.getNumSamples();

    // Allows bypassusing button
    if (bypass) return;

    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        auto* channelData = buffer.getWritePointer (Channel);

        for (int sample = 0; sample < numSamples; sample++) {

            channelData[sample] = DelayandSumBeamformer->generateNarrowband(1000, 0.5f) * (float)gain;
        }
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
    settings.bypass = apvts.getRawParameterValue("Bypass")->load();
    settings.Channel = apvts.getRawParameterValue("Channel")->load();
}

// Abstracted parameter crap into helper file
juce::AudioProcessorValueTreeState::ParameterLayout
BeamformingRTPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    ParameterHelper::addParameters(layout);
    return layout;
}
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new BeamformingRTPluginAudioProcessor(); }
