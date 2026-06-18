//===============================================================================
#include "DAS.h"
//===============================================================================

DAS::DAS(int samplerate) :
	sampleRate(48000)
{
	// Overwrites default value with actual sr
	sampleRate = samplerate;

	osc = std::make_unique<Oscillator>();

	// change from NULL to main size if processing osc as buffer 
	osc->setupOscillator(sampleRate, NULL, false);
}

float DAS::generateNarrowband(float freq, float amplitude) 
{
	osc->setFrequency(freq); 

	// I genuinely can't remember which one is needed so use both functions
	osc->setAmplitude(1.f);
	osc->setTargetAmplitude(1.f);

	return osc->incrementSample();
}

