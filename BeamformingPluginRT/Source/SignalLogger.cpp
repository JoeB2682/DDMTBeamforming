//===============================================================================
#include "SignalLogger.h"
//===============================================================================
SignalLogger::SignalLogger()
    : juce::Thread("SignalLogger")
{
    for (auto& buffer : bufferFifo)
        buffer.setSize(numChannels, maxBlockSize);

    startThread();
}
//===============================================================================
SignalLogger::~SignalLogger()
{
    signalThreadShouldExit();
    stopThread(1000);
}
//===============================================================================
void SignalLogger::run()
{
    while (!threadShouldExit())
    {
        if (!enabled)
        {
            wait(10);
            continue;
        }

        if (outputStream == nullptr || !outputStream->openedOk())
        {
            wait(10);
            continue;
        }

        if (numReadyBuffers.load() > 0)
        {
            auto& buffer = bufferFifo[fifoReadPosition];

            writeBuffer(buffer);

            fifoReadPosition = (fifoReadPosition + 1) % fifoSize;

            numReadyBuffers.fetch_sub(1);
        }
        else
        {
            wait(1);
        }
    }
}
//===============================================================================
// Called from the audio thread
void SignalLogger::logBuffer(juce::AudioBuffer<float>& buffer)
{
    if (!enabled) return;

    const int numSamples = buffer.getNumSamples();

    if (numSamples > maxBlockSize)
        return;

    if (numReadyBuffers.load() >= fifoSize)
        return;

    auto& destination = bufferFifo[fifoWritePosition];

    for (int channel = 0; channel < numChannels; ++channel)
    {
        destination.copyFrom(
            channel,
            0,
            buffer,
            channel,
            0,
            numSamples);
    }

    destination.setSize(
        numChannels,
        numSamples,
        false,
        false,
        true);

    fifoWritePosition = (fifoWritePosition + 1) % fifoSize;

    numReadyBuffers.fetch_add(1);
}
//===============================================================================
// Called from the logger thread
void SignalLogger::writeBuffer(
    const juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        for (int channel = 0; channel < numChannels; ++channel)
        {
            outputStream->writeString(
                juce::String(buffer.getSample(channel, sample), 6));

            if (channel < numChannels - 1)
                outputStream->writeString(",");
        }
        outputStream->writeString("\r\n");
    }
    outputStream->flush();
}
//===============================================================================
// Enable or disable logging
void SignalLogger::setEnabled(bool shouldEnable)
{
    enabled = shouldEnable;
}
//===============================================================================
// Create new file and write header
void SignalLogger::createNewFile()
{
    auto outputFolder =
        juce::File("C:/Users/josep/Desktop/SignalCSVs");

    if (!outputFolder.exists())
        outputFolder.createDirectory();

    recordingNumber = 1;

    while (outputFolder.getChildFile(
        "timedomain_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv").existsAsFile())
    {
        ++recordingNumber;
    }

    auto file = outputFolder.getChildFile(
        "timedomain_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv");

    DBG("CSV path: " << file.getFullPathName());

    outputStream =
        std::make_unique<juce::FileOutputStream>(file);

    if (outputStream->openedOk())
    {
        DBG("CSV OPENED");

        outputStream->writeString(
            "channel_1,channel_2,channel_3,channel_4,"
            "channel_5,channel_6,channel_7,channel_8\r\n");

        outputStream->flush();
    }
    else
    {
        DBG("SIGNAL CSV FAILED TO OPEN");
    }
}
//===============================================================================