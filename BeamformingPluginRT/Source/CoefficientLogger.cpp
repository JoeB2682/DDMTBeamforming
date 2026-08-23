//===============================================================================
#include "CoefficientLogger.h"
//===============================================================================
CoefficientLogger::CoefficientLogger()
    : juce::Thread("CoefficientLogger")
{
    startThread();
}
//===============================================================================
CoefficientLogger::~CoefficientLogger()
{
    stopThread(1000);
}
//===============================================================================
void CoefficientLogger::run()
{
    while (!threadShouldExit())
    {
        // Create file when logging is enabled
        if (enabled && !fileCreated)
        {
            createNewFile();
        }

        // Close file when logging is disabled
        if (!enabled && fileCreated)
        {
            if (outputStream != nullptr)
                outputStream->flush();

            outputStream.reset();

            fileCreated = false;
        }

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

        int start1 = 0;
        int size1 = 0;
        int start2 = 0;
        int size2 = 0;

        fifo.prepareToRead(
            1,
            start1,
            size1,
            start2,
            size2);

        if (size1 > 0)
        {
            for (int i = 0; i < size1; ++i)
            {
                writeCoefficients(coefficientFifo[start1 + i]);
            }

            for (int i = 0; i < size2; ++i)
            {
                writeCoefficients(coefficientFifo[start2 + i]);
            }

            fifo.finishedRead(size1 + size2);
        }
        else
        {
            wait(1);
        }
    }
}
//===============================================================================
// Enable or disable logging
void CoefficientLogger::setEnabled(bool shouldEnable)
{
    enabled = shouldEnable;
}
//===============================================================================
// Called from the processing thread
void CoefficientLogger::logCoefficients(
    int driverChannel,
    const std::vector<float>& nnCorrection,
    const std::vector<float>& CorrecteddB,
    const std::vector<float>& b)
{
    if (!enabled) return;

    int start1 = 0;
    int size1 = 0;
    int start2 = 0;
    int size2 = 0;

    fifo.prepareToWrite(
        1,
        start1,
        size1,
        start2,
        size2);

    if (size1 > 0)
    {
        auto& data = coefficientFifo[start1];

        data.driverChannel = driverChannel;
        data.nnCorrection = nnCorrection;
        data.CorrecteddB = CorrecteddB;
        data.b = b;

        fifo.finishedWrite(1);
    }
}
//===============================================================================
// Called from logger thread
void CoefficientLogger::writeCoefficients(
    const CoefficientData& data)
{
    outputStream->writeString(
        juce::String(data.driverChannel));

    outputStream->writeString(",");

    for (size_t i = 0; i < data.nnCorrection.size(); ++i)
    {
        outputStream->writeString(
            juce::String(data.nnCorrection[i], 6));

        outputStream->writeString(",");
    }

    for (size_t i = 0; i < data.CorrecteddB.size(); ++i)
    {
        outputStream->writeString(
            juce::String(data.CorrecteddB[i], 6));

        outputStream->writeString(",");
    }

    for (size_t i = 0; i < data.b.size(); ++i)
    {
        outputStream->writeString(
            juce::String(data.b[i], 6));

        if (i < data.b.size() - 1)
            outputStream->writeString(",");
    }

    outputStream->writeString("\r\n");

    outputStream->flush();
}
//===============================================================================
// Create new file from logger thread
void CoefficientLogger::createNewFile()
{
    auto outputFolder =
        juce::File("C:/Users/josep/Desktop/CoefficientCSVs");

    if (!outputFolder.exists())
        outputFolder.createDirectory();

    recordingNumber = 1;

    while (outputFolder.getChildFile(
        "coefficients_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv").existsAsFile())
    {
        ++recordingNumber;
    }

    auto file = outputFolder.getChildFile(
        "coefficients_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv");

    DBG("Coefficient CSV path: "
        << file.getFullPathName());

    outputStream =
        std::make_unique<juce::FileOutputStream>(file);

    if (outputStream->openedOk())
    {
        DBG("COEFFICIENT CSV OPENED");

        outputStream->writeString(
            "driver_channel,"
            "nn_correction,"
            "corrected_dB,"
            "coefficient\r\n");

        outputStream->flush();

        fileCreated = true;
    }
    else
    {
        DBG("COEFFICIENT CSV FAILED TO OPEN");

        fileCreated = false;
    }
}
//===============================================================================