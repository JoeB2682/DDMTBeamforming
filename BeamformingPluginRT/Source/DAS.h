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

    int sampleRate, oscbuffersize;

    std::unique_ptr<Oscillator> osc;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DAS)
};
//===============================================================================
