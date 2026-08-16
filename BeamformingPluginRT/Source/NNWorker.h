//===============================================================================
//
// NNWorker.h
// 
// Due to the NN handling being significantly too much for the audio thread I had
// to separate it onto a new one...
// 
// created by Joseph Bozzo
// 
//===============================================================================

#pragma once

#include <JuceHeader.h>
#include "NeuralNetworkHandler.h"

//===============================================================================
class NNWorker : public juce::Thread
{
public:

    NNWorker();
    ~NNWorker() override;

    void run() override;

    void submit(
        const std::vector<float>& newRoom,
        const std::vector<float>& newTrajectory,
        const std::vector<float>& newFir,
        const std::vector<float>& newBeam,
        const std::vector<float>& newFilteredBeam);
   
    bool getLatestCorrection(std::vector<float>& output);

private:

    juce::CriticalSection locker;

    NeuralNetworkHandler networkhandler;

    std::vector<float> room;
    std::vector<float> trajectory;
    std::vector<float> fircoeffs;
    std::vector<float> beam;
    std::vector<float> filteredBeam;

    std::vector<float> correction;

    bool jobReady = false;
    bool correctionReady = false;
};
//===============================================================================