//===============================================================================
//
// TrajectoryLogger.h
// 
// AI assisted the development of this due to time and the pain of file handling.
// 
// Press 'i' key to log data from the trajectory worker thread...
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once

#include <JuceHeader.h>
#include <vector>
#include <memory>
//===============================================================================
class TrajectoryLogger
{
public:
    TrajectoryLogger() {};

    void setEnabled(bool enabled);
    void logTrajectory(const std::vector<float>& trajectory);

private:
    void createNewFile();

    std::unique_ptr<juce::FileOutputStream> outputStream;

    bool enabled = false;
    int recordingNumber = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrajectoryLogger)
};
//===============================================================================
