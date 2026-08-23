//===============================================================================
//
// CoefficientLogger.h
//
// Logs Filter Coefficients and their corrections...
// Same logic as other loggers.
//
// created by Joseph Bozzo
//
//===============================================================================
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
#include "TrajectoryLogger.h"

//===============================================================================

class CoefficientLogger : public juce::Thread, public TrajectoryLogger
{
public:
    CoefficientLogger();
    ~CoefficientLogger() override;

    void run() override;

public:

    void logCoefficients(int driverChannel, const std::vector<float>& nnCorrection,
        const std::vector<float>& CorrecteddB,
        const std::vector<float>& b);

    void setEnabled(bool shouldEnable);

private:

    struct CoefficientData
    {
        int driverChannel = 0;
        std::vector<float> nnCorrection;
        std::vector<float> CorrecteddB;
        std::vector<float> b;
    };

    static constexpr int fifoSize = 128;

    std::array<CoefficientData, fifoSize> coefficientFifo;

    juce::AbstractFifo fifo{ fifoSize };

    void writeCoefficients(const CoefficientData& data);

    void createNewFile() override;

    std::unique_ptr<juce::FileOutputStream> outputStream;

    bool enabled = false;
    bool fileCreated = false;

    int recordingNumber = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoefficientLogger)
};

//===============================================================================