//===============================================================================
#include "DAS.h"
//===============================================================================
// Constructor
DAS::DAS(int samplerate, int N, float r, int totalNoOutputChannels) :
	sampleRate(48000),
	N(N),
	r(r),
	setPosflag(false),
	setoscbankflag(false),
	totalNoOutputChannels(8)
{
	// Overwrites default value with actual sr
	sampleRate = samplerate;

	testosc = std::make_unique<Oscillator>();

	// change from NULL to main size if processing osc as buffer 
	testosc->setupOscillator(sampleRate, NULL, false);

	// Initialise vectors
	speakers.resize(N);
	receivers.resize(N);

	tau.resize(N, 0.0f);
	tau_rx.resize(N, 0.0f);
	tau_total.resize(N, 0.0f);
	tau_Corrected.resize(N, 0.0f);
	vDistance.resize(N, 0.0f);

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

	// Clamp to prevent blowup
	gain = juce::jlimit(0.0f, 0.01f, gain);

	// Calculate relative delays for each speaker and apply to each channel
	for (int speaker = 0; speaker < N; speaker++)
	{
		float relativeDelay = tauMax - tau[speaker];
		float phaseOffset = -2.0f * juce::MathConstants<float>::pi * freq * relativeDelay;
		float distance = tau[speaker] * speedofSound;

		distance = juce::jmax(distance, 0.1f); 
		float distanceWeight = 1.0f / distance;

		float compensatedAmplitude = amplitude; //* distanceWeight;

		oscbank[speaker]->setFrequency(freq);
		oscbank[speaker]->setTargetAmplitude(compensatedAmplitude);
		oscbank[speaker]->setPhaseOffset(phaseOffset);

		auto* channel = buffer.getWritePointer(speaker);

		for (int sample = 0; sample < buffer.getNumSamples(); sample++)
		{
			// Normalise to prevent insane gain levels
			channel[sample] = oscbank[speaker]->incrementSample() * gain / N;
		}
	}

	/*
	for (int i = 0; i < N; i++)
	{
		DBG("tau " << i << ": " << tau[i]);
	}
	*/
}
//===============================================================================
// Sets source positions vector based on specified radius
void DAS::setsourcePositions(float& radius, std::vector<Point2D>& speakers) 
{
	speakers.clear();

	for (int i = 0; i < N; ++i)
	{

		// Change this depending upon speaker channel config 
		//float angle = juce::MathConstants<float>::twoPi * i / N;
		float angle = juce::MathConstants<float>::twoPi * i / N + juce::MathConstants<float>::halfPi; // This is for lab room array

		speakers.push_back(
			{
				radius * std::cos(angle),
				radius * std::sin(angle)
			});
	}
}
//===============================================================================
// Sets receiver positions vector based on smaller radius
void DAS::setreceiverPositions(float& radius, std::vector<Point2D>& receivers)
{
	receivers.clear();

	// Currently only using 1 mic at centre
	receivers.push_back({ 0.0f, 0.0f });

	/*
	for (int i = 0; i < N; ++i)
	{

		// Change this depending upon microphone channel config 
		//float angle = juce::MathConstants<float>::twoPi * i / N;
		float angle = juce::MathConstants<float>::twoPi * i / N + juce::MathConstants<float>::halfPi; // This is for lab room array

		receivers.push_back(
			{
				radius * std::cos(angle),
				radius * std::sin(angle)
			});
	}
	*/
}
//===============================================================================
// Sets bright point based on specified x, y coordinates
inline void DAS::setbrightPoint(Point2D& b, float x, float y) 
{
	b.x = x; b.y = y;
}

//===============================================================================
// Calculates tau vector for source phase offset
void DAS::calcsourceTOI(std::vector<float>& tau, std::vector<Point2D>& speakers, Point2D& b)
{
	for (int i = 0; i < N; i++)
	{
		dx = b.x - speakers[i].x;
		dy = b.y - speakers[i].y;

		float distance = std::sqrt(dx * dx + dy * dy);

		vDistance[i] = distance;
		tau[i] = distance / speedofSound;
	}
}
//===============================================================================
// Calculates tau vector for receiver phase offset
void DAS::calcReceiverTOI(std::vector<float>& tau_rx, std::vector<Point2D>& speakers, std::vector<Point2D>& receivers)
{
	tau_rx.resize(speakers.size());

	for (int i = 0; i < speakers.size(); i++)
	{
		float dx = speakers[i].x - receivers[0].x;
		float dy = speakers[i].y - receivers[0].y;

		float distance = std::sqrt(dx * dx + dy * dy);

		tau_rx[i] = distance / speedofSound; 
	}
}
//===============================================================================
// Calls necessary functions to process circular array
void DAS::processcircularDAS(juce::AudioBuffer<float>& buffer,
	float bright_x, float bright_y, float freq, float amplitude, float gain)
{
	if (!setPosflag)
	{
		setsourcePositions(r, speakers);
		setPosflag = true;
	}

	setbrightPoint(b, bright_x, bright_y);
	calcsourceTOI(tau, speakers, b);
	generateNarrowband(oscbank, buffer, freq, 0.5f, tau, gain);
}
//===============================================================================