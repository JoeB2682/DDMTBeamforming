//==================================================================================

#pragma once

#include <JuceHeader.h>
#include "ParameterHelper.h"
#include "DAS.h"
#include "FAS.h"
#include "FFTProcessor.h"
#include "MotionTrackerHandler.h"

//==================================================================================
// Chainsettings keeps track of parameter states 
struct ChainSettings {

    float Gain{ 1.0f };
    bool bypass{ }, outtype{ }, MTrack;
    int Channel{ 1 };
    float brightx{  }, brighty{  };
    float Thresh{  };
    float inpgain{ 400.f };
    float f0{ 483.33f }, f1{ 1050.0f }, f2{ 1616.67f };
    float Length{  }, Width, Height{  };
    float MaxOrder{  };
    float Absorption{  };
    float rt60{  };
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

    juce::AudioParameterFloat *brightX, *brightY;

public:

    // Beamformers
    std::shared_ptr<DAS> DelayandSumBeamformer;
    std::unique_ptr<FAS> FilterandSumBeamformer;

    // FFT Processor
    std::shared_ptr<FFTProcessor> fftprocessor;

    float ArrayRadius = 1.5; // meters
    float miclevel;

    // Motion Tracker
    std::shared_ptr<MotionTrackerHandler> motiontracker;
    Position position;
    float x, y, z;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BeamformingRTPluginAudioProcessor)
};
//==================================================================================
