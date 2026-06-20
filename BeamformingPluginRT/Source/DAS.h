//===============================================================================
//
// DAS.h
// 
// Delay and Sum beamformer class, generates and handles both narrow and wideband 
// signals. Allows for both linear and nonlinear arrays.
// 
// created by Joseph Bozzo
// 
//===============================================================================
#pragma once

#include <JuceHeader.h>
#include "Oscillator.h"

// Class to handle all delay and sum beamforming...

//===============================================================================
class DAS 
{
public:

    DAS(int samplerate);
    ~DAS() = default;

    float generateNarrowband(float freq, float amplitude);

private:

    int sampleRate;

    std::unique_ptr<Oscillator> osc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAS)
};
//===============================================================================
