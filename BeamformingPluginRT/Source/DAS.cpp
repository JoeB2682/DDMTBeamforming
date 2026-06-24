//===============================================================================
#include "DAS.h"
//===============================================================================
// Constructor
DAS::DAS(int samplerate, int N, float r, int totalNoOutputChannels) :
	sampleRate(48000),
	N(N),
	r(r),
	setPosflag(false),
	setoscbankflag(false)
{
	// Overwrites default value with actual sr
	sampleRate = samplerate;

	testosc = std::make_unique<Oscillator>();

	// change from NULL to main size if processing osc as buffer 
	testosc->setupOscillator(sampleRate, NULL, false);

	// Initialise vectors
	speakers.resize(N);
	tau.resize(N, 0.0f);

	// Just give arbritrary vals upon construction
	setbrightPoint(b, r + 1, r + 2);

	// Initialise oscillator bank
	oscbank.resize(N);

	for (int i = 0; i < N; i++)
	{
		oscbank[i] = std::make_unique<Oscillator>();
	}
}

//===============================================================================
// Generates single test tone for output config testing 
float DAS::generateTestTone(float freq, float amplitude, float Phi)
{
	// Generates test tone using made oscillator class
	testosc->setFrequency(freq);

	testosc->setAmplitude(amplitude);
	testosc->setTargetAmplitude(amplitude);
	testosc->setPhaseOffset(Phi);

	testosc->updateIncrement();

	return testosc->incrementSample();
}

//===============================================================================
// Generates full narrowband signal for each source (calc correct offset prior)
void DAS::generateNarrowband(std::vector<std::unique_ptr<Oscillator>>& oscbank,
	juce::AudioBuffer<float>& buffer,
	float freq,
	float amplitude,
	std::vector<float>& tau,
	float gain)
{
	// Calculate max of time of arrival array
	float tauMax = *std::max_element(tau.begin(), tau.end());

	// Use flag to setup oscillators once
	if (!setoscbankflag)
	{
		for (int i = 0; i < oscbank.size(); i++)
		{
			oscbank[i]->setupOscillator(sampleRate,
				buffer.getNumSamples(),
				false);
		}
		setoscbankflag = true;
	}

	// Calculate relative delays for each speaker and apply to each channel
	for (int speaker = 0; speaker < N; speaker++)
	{
		float relativeDelay = tauMax - tau[speaker];
		float phaseOffset = -2.0f * juce::MathConstants<float>::pi * freq * relativeDelay;

		oscbank[speaker]->setFrequency(freq);
		oscbank[speaker]->setTargetAmplitude(amplitude);
		oscbank[speaker]->setPhaseOffset(phaseOffset);

		auto* channel = buffer.getWritePointer(speaker);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{
			channel[sample] = oscbank[speaker]->incrementSample() * gain;
		}
	}
}

//===============================================================================
// Sets source positions vector based on specified radius
void DAS::setsourcePositions(float& radius, std::vector<Point2D>& speakers) 
{
	speakers.clear();

	for (int i = 0; i < N; ++i)
	{
		float angle = juce::MathConstants<float>::twoPi * i / 8.0f;

		speakers.push_back(
			{
				radius * std::cos(angle),
				radius * std::sin(angle)
			});
	}
}

//===============================================================================
// Sets bright point based on specified x, y coordinates
inline void DAS::setbrightPoint(Point2D& b, float x, float y) 
{
	b.x = x; b.y = y;
}

//===============================================================================
// Calculates tau vector for phase offset
void DAS::calcsourceTOI(std::vector<float>& tau, std::vector<Point2D>& speakers, Point2D& b)
{
	for (int i = 0; i < N; i++)
	{
		float dx = speakers[i].x - b.x;
		float dy = speakers[i].y - b.y;

		float distance = std::sqrt(dx * dx + dy * dy);

		tau[i] = distance / speedofSound;
	}
}

//===============================================================================
// Calls necessary functions to process circular array
void DAS::processcircularDAS(juce::AudioBuffer<float>& buffer,
	float bright_x, float bright_y, float gain)
{
	if (!setPosflag)
	{
		setsourcePositions(r, speakers);
		setPosflag = true;
	}

	setbrightPoint(b, bright_x, bright_y);
	calcsourceTOI(tau, speakers, b);
	generateNarrowband(oscbank, buffer, 1000.0f, 0.5f, tau, gain);
}
//===============================================================================