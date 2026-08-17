//===============================================================================
//
// TrajectoryWorker.h
// 
// Moves trajectory data population onto seperate worker thread for more efficient 
// processing.
// 
// Gotta make full use of that Intel Core i7 ey!
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>
#include "MotionTrackerHandler.h"
#include "TrajectoryLogger.h"

//===============================================================================
class trajectoryWorker : public juce::Thread 
{
public:

    trajectoryWorker(std::shared_ptr<MotionTrackerHandler> mtrackhandler);
    ~trajectoryWorker() override;

    std::vector<float> getTrajectory();
    void getLogbool(bool logbool) { dataLogBool = logbool; }
    void setLoggingEnabled(bool enabled);

    void run() override;

public:

    std::vector<float> xBuff, yBuff, zBuff;
    std::vector<float> vxBuff, vyBuff, vzBuff;
    std::vector<float> trajectory;

    float vx, vy, vz;
    float mx, my, mz;
    float mvx, mvy, mvz;
    float sx, sy, sz;
    float svx, svy, svz;

    const float updaterate;

    std::atomic<bool> dataLogBool{ false };

private:

    const int frameSize;
    std::shared_ptr<MotionTrackerHandler> trackerhandler;
    std::unique_ptr<TrajectoryLogger> trajectoryLogger;
};
//===============================================================================