/*
  ==============================================================================

    FrequencyBand.h
    Created: 1 Jul 2026 11:40:26am
    Author:  josep

  ==============================================================================
*/

#pragma once

//===============================================================================
struct FrequencyBand
{
	double low, high;

	double lowerBandwidth() { return centreFrequency() - low; }
	double upperBandwidth() { return high - centreFrequency(); }
	double centreFrequency() { return (low + high) * 0.5; }
	double bandwidth() { return high - low; }
};
//===============================================================================
