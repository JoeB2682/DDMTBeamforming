//===============================================================================
//
// SignalLogger.h
// 
// Used the trajectory logger to log the main buffer to .csv for analysis...
// 
// I used AI for the FIFO and multithreading due to time constraints.
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once

#include <JuceHeader.h>
//===============================================================================
class SignalLogger : public juce::Thread
{
public:
    SignalLogger();
    ~SignalLogger();

    //===============================================================================
    void logBuffer(juce::AudioBuffer<float>& buffer);
    void createNewFile();

    void setEnabled(bool shouldEnable);

private:

    //===============================================================================
    static constexpr int numChannels = 8;
    static constexpr int maxBlockSize = 1024;
    static constexpr int fifoSize = 128;

    juce::AudioBuffer<float> bufferFifo[fifoSize];

    int fifoWritePosition = 0;
    int fifoReadPosition = 0;

    std::atomic<int> numReadyBuffers{ 0 };

    std::unique_ptr<juce::FileOutputStream> outputStream;

    bool enabled = false;
    int recordingNumber = 1;

    //===============================================================================
    void run() override;
    void writeBuffer(const juce::AudioBuffer<float>& buffer);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalLogger)
};
//===============================================================================