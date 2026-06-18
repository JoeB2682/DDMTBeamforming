#pragma once

#include <JuceHeader.h>
#include "ParameterHelper.h"
#include "DAS.h"

//==================================================================================
// Chainsettings keeps track of parameter states 
struct ChainSettings {

    float Gain{ 1.0f };
    bool bypass{ };
    int Channel{ 1 };
};
//==================================================================================
class BeamformingRTPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    BeamformingRTPluginAudioProcessor();
    ~BeamformingRTPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void getChainSettings(ChainSettings& settings);

    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParameterLayout();
    juce::AudioProcessorValueTreeState apvts
        {*this, nullptr, "Parameters", createParameterLayout()};

    ChainSettings chainsettings;

private:
    //==============================================================================
    //Instanciation Station

    std::unique_ptr<DAS> DelayandSumBeamformer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeamformingRTPluginAudioProcessor)
};
//==================================================================================
