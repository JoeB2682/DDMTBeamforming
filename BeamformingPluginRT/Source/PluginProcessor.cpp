//==============================================================================
#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
// Constructor
BeamformingRTPluginAudioProcessor::BeamformingRTPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth // MODIFY CHANNEL LAYOUT DEPENDING UPON ARRAY!!!
                       .withInput  ("Input",  juce::AudioChannelSet::discreteChannels(8), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::discreteChannels(8), true)
                     #endif
                       ) 
#endif
{
    // Instanciate and Connect Motion Tracker
    motiontracker = std::make_unique<MotionTrackerHandler>();

    if(motiontracker)
        motiontracker->connect();
}

BeamformingRTPluginAudioProcessor::~BeamformingRTPluginAudioProcessor()
{
}

//==============================================================================
// Setup
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
    // Instanciating in prepare to play is often better than constructor due to bus configurations 
    // already being specified...
    {
        // Debug messages to ensure that I/O is configured properly
        DBG("Inputs  = " << getTotalNumInputChannels());
        DBG("Outputs = " << getTotalNumOutputChannels());

        DBG("Main input bus  = " << getMainBusNumInputChannels());
        DBG("Main output bus = " << getMainBusNumOutputChannels());
    }

    DBG("Main Buffer Size = " << samplesPerBlock);

    // FFT Processor
    fftprocessor = std::make_shared<FFTProcessor>(11, getSampleRate());

    // Instanciate Beamformers
    DelayandSumBeamformer = std::make_shared<DAS>(getSampleRate(), 8, ArrayRadius, getTotalNumOutputChannels());
    FilterandSumBeamformer = std::make_unique<FAS>(DelayandSumBeamformer.get(), 64, samplesPerBlock, 833.33f, 666.67f, 1000.f, true, true, fftprocessor);
}

void BeamformingRTPluginAudioProcessor::releaseResources()
{
    fftprocessor->releaseResources();
}

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
//==============================================================================
// SNR Functions



//==============================================================================
// Main Processing
void BeamformingRTPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    getChainSettings(chainsettings);

    // assign parameters to variables
    float gain = chainsettings.Gain;
    bool bypass = chainsettings.bypass;
    int Channel = chainsettings.Channel - 1;
    bool Outputtype = chainsettings.outtype;
    float brightX = chainsettings.brightx;
    float brightY = chainsettings.brighty;
    float thresh = chainsettings.Thresh;
    float inpgain = chainsettings.inpgain;
    bool MTrack = chainsettings.MTrack;

    //DBG("isBypass = " << (bypass ? "true" : "false"));

    // Safety check stops out of bounds indexing
    if (Channel < 0 || Channel >= buffer.getNumChannels()) return;

    int numSamples = buffer.getNumSamples();

    // Convert normalized position in metres
    brightX *= ArrayRadius;
    brightY *= ArrayRadius;

    //DBG("BrightPointX: " << brightX << " BrightPointY: " << brightY);

    // Get mocap coordinates
    position = motiontracker->getPosition();

    //DBG(position.x << position.y << position.z);
    //DBG("MTrack " << (MTrack ? "true" : "false"));

    // map motion track data
    float MtrackXMapped = juce::jmap(position.x, -1.24f, 0.551f, -ArrayRadius, ArrayRadius);
    float MtrackYMapped = juce::jmap(position.x, -0.f, 0.52f, -ArrayRadius, ArrayRadius);

    // if toggle use motion tracking
    if (MTrack) {
        brightX = MtrackXMapped;
        brightY = MtrackYMapped;
    }
       
    // Allows bypassing button
    if (bypass) return;

    // Get copy of buffer before writing
    juce::AudioBuffer<float> micBuffer;

    micBuffer.makeCopyOf(buffer);

    // Apply and adjust Input Gain accordingly depending upon receiver used
    micBuffer.applyGain(inpgain);

    // Toggle to switch between speaker test or beamformer
    if (!Outputtype)
    {
        // Generate beamformer output (BE CAREFUL IF RADIUS IS WRONG GAIN WILL SPIKE!!!!!!)
        //DelayandSumBeamformer->processcircularDAS(buffer, brightX, brightY, 1000.f, 0.5f, gain);

        FilterandSumBeamformer->processcircularFAS(buffer, micBuffer, brightX, brightY, 0.5f, gain, thresh);

        //DBG("Raw BrightX: " << brightX);
        //DBG("Raw BrightY: " << brightY);

        // Add mic data to FFT FIFO
        for (int channel = 0; channel < micBuffer.getNumChannels(); ++channel)
        {
            auto* samples = micBuffer.getReadPointer(channel);

            for (int sample = 0; sample < micBuffer.getNumSamples(); ++sample)
                fftprocessor->pushNextSampleIntoFifo(samples[sample]);
        }

        // For Analysis and effects
        fftprocessor->processFFT();
    }
    else
    {
        // Sample Loop 
        for (int sample = 0; sample < numSamples; sample++)
        {
            // Generate test tone on selected channel only
            float testsig = DelayandSumBeamformer->generateTestTone(1000.0f, 0.5f, 0.0f);

            // Assign to output 
            float output = testsig * gain;

            // Output on selected channel (for testing source outputs)
            buffer.getWritePointer(Channel)[sample] = output;
        }
    }

    // Get input level after processing (threshold)
    miclevel = micBuffer.getMagnitude(0, 0, micBuffer.getNumSamples());
    //DBG(miclevel);
}

//==============================================================================
// Editor handling
bool BeamformingRTPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* BeamformingRTPluginAudioProcessor::createEditor()
{ return new BeamformingRTPluginAudioProcessorEditor (*this); }
//==============================================================================
// Parameter and State 
void BeamformingRTPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData){}
void BeamformingRTPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes){}

void BeamformingRTPluginAudioProcessor::getChainSettings(ChainSettings& settings)
{
    // Load Parameters into chainsettings
    settings.Gain = apvts.getRawParameterValue("Gain")->load();
    settings.bypass = apvts.getRawParameterValue("Bypass")->load();
    settings.Channel = apvts.getRawParameterValue("Channel")->load();
    settings.outtype = apvts.getRawParameterValue("Outtype")->load();
    settings.brightx = apvts.getRawParameterValue("BrightX")->load();
    settings.brighty = apvts.getRawParameterValue("BrightY")->load();
    settings.Thresh = apvts.getRawParameterValue("thresh")->load();
    settings.inpgain = apvts.getRawParameterValue("inpgain")->load();
    settings.MTrack = apvts.getRawParameterValue("MTrack")->load();
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
//==============================================================================
