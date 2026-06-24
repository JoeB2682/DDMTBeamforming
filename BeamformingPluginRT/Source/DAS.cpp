//===============================================================================
#include "DAS.h"
//===============================================================================
// Constructor
DAS::DAS(int samplerate, int N, float r) :
	sampleRate(48000),
	N(N),
	r(r),
	setPosflag(false)
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
float DAS::generateNarrowband(float freq, float amplitude, float Phi)
{

}

//===============================================================================
// Sets source positions vector based on specified radius
void DAS::setsourcePositions(float& radius, std::vector<Point2D>& speakers) 
{
	for (int i = 0; i < speakers.size(); ++i)
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
void DAS::calcsourceTOI(std::vector<float>& tau, std::vector<Point2D>& speakers)
{

}

//===============================================================================
// Calls necessary functions to process circular array
void DAS::processcircularDAS(juce::AudioBuffer<float>& buffer, float bright_x, float bright_y)
{
	// Sets speaker positions if not allready set
	if (setPosflag == false) {
		setsourcePositions(r, speakers);
		setPosflag == true;
	}

	// Update bright point based on parameters
	setbrightPoint(b, bright_x, bright_y);
	
	// Calculate time of arrival vector
	calcsourceTOI(tau, speakers);


}
//===============================================================================