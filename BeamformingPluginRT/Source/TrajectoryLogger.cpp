//===============================================================================
#include "TrajectoryLogger.h"
//===============================================================================
void TrajectoryLogger::setEnabled(bool shouldEnable)
{
    // Start recording
    if (shouldEnable && !enabled)
    {
        createNewFile();
    }

    // Stop recording
    if (!shouldEnable && enabled)
    {
        if (outputStream != nullptr)
            outputStream->flush();

        outputStream.reset();
    }

    enabled = shouldEnable;
}
//===============================================================================
void TrajectoryLogger::createNewFile()
{
    // Get the directory containing TrajectoryLogger.cpp
    auto outputFolder = juce::File(
        "C:/Users/josep/Desktop/TrajectoryCSVs");

    DBG("Output folder: " << outputFolder.getFullPathName());

    // Find next available recording number
    recordingNumber = 1;

    while (outputFolder.getChildFile(
        "trajectory_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv").existsAsFile())
    {
        ++recordingNumber;
    }

    auto file = outputFolder.getChildFile(
        "trajectory_" +
        juce::String(recordingNumber).paddedLeft('0', 3) +
        ".csv");

    DBG("CSV path: " << file.getFullPathName());

    outputStream = std::make_unique<juce::FileOutputStream>(file);

    if (outputStream->openedOk())
    {
        DBG("CSV OPENED");

        outputStream->writeString(
            "start_x,start_y,start_z,"
            "mean_x,mean_y,mean_z,"
            "std_x,std_y,std_z,"
            "mean_vx,mean_vy,mean_vz,"
            "std_vx,std_vy,std_vz\n");

        outputStream->flush();
    }
    else
    {
        DBG("CSV FAILED TO OPEN");
    }
}
//===============================================================================
void TrajectoryLogger::logTrajectory(
    const std::vector<float>& trajectory)
{
    if (!enabled)
        return;

    if (outputStream == nullptr || !outputStream->openedOk())
        return;

    if (trajectory.size() < 15)
        return;

    juce::String row;

    for (int i = 0; i < 15; ++i)
    {
        row += juce::String(trajectory[i], 6);

        if (i < 14)
            row += ",";
    }

    row += "\n";

    outputStream->writeString(row);
    outputStream->flush();
}
//===============================================================================
