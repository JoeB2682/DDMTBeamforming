//===============================================================================
#include "NNWorker.h"
//===============================================================================
// Constructor starts the thread
NNWorker::NNWorker()
    : juce::Thread("NNWorker")
{
    // Preallocate NN input tensors
    room.resize(7, 0.0f);
    trajectory.resize(35, 0.0f);
    fircoeffs.resize(1 * 1 * 100 * 8 * 64, 0.0f);
    beam.resize(1 * 1 * 72 * 100, 0.0f);
    filteredBeam.resize(1 * 1 * 72 * 100, 0.0f);

    // Load ONNX NN model usiong network handler
    networkhandler.loadModel(3);

    // Start worker thread
    startThread();
}
//===============================================================================
// Stop the thread on destruction
NNWorker::~NNWorker()
{
    stopThread(1000);
}
//===============================================================================
// Main Running Function for Thread
void NNWorker::run()
{
    while (!threadShouldExit())
    {
        bool shouldProcess = false;

        std::vector<float> localRoom;
        std::vector<float> localTrajectory;
        std::vector<float> localFir;
        std::vector<float> localBeam;
        std::vector<float> localFilteredBeam;

        {
            const juce::ScopedLock lock(locker);

            if (jobReady)
            {
                // Copy the complete job while locked
                localRoom = room;
                localTrajectory = trajectory;
                localFir = fircoeffs;
                localBeam = beam;
                localFilteredBeam = filteredBeam;

                jobReady = false;
                shouldProcess = true;
            }
        }

        if (shouldProcess)
        {
            // ============================================================
            // Run model outside lock

            //DBG("NN inference starting...");

            auto result = networkhandler.runModel(
                localRoom,
                localTrajectory,
                localFir,
                localBeam,
                localFilteredBeam
            );

            //DBG("NN inference finished. Output size: " << result.size());

            // ============================================================
            // Store result
            {
                const juce::ScopedLock lock(locker);

                correction = std::move(result);
                correctionReady = true;
            }
        }
        else
        {
            wait(1);
        }
    }
}
//===============================================================================
// Submits new job to worker thread
void NNWorker::submit(
    const std::vector<float>& newRoom,
    const std::vector<float>& newTrajectory,
    const std::vector<float>& newFir,
    const std::vector<float>& newBeam,
    const std::vector<float>& newFilteredBeam)
{
    const juce::ScopedLock lock(locker);

    // Copy the latest inputs into the worker
    room = newRoom;
    trajectory = newTrajectory;
    fircoeffs = newFir;
    beam = newBeam;
    filteredBeam = newFilteredBeam;

    // Tell worker that a job is available
    jobReady = true;
}
//===============================================================================
// Gets the latest FIR filter correction
bool NNWorker::getLatestCorrection(std::vector<float>& output)
{
    const juce::ScopedLock lock(locker);

    if (!correctionReady)
        return false;

    output = correction;

    correctionReady = false;

    return true;
}
//===============================================================================