
#include "BeamVisualiser.h"

void BeamVisualizer::setSpeakers(const std::vector<DAS::Point2D>& newSpeakers) { speakers = newSpeakers; repaint(); }
void BeamVisualizer::setBrightPoint(float x, float y) { brightPoint.x = x; brightPoint.y = y; repaint(); }
void BeamVisualizer::setTau(const std::vector<float>& newTau){ tau = newTau; repaint(); }
void BeamVisualizer::setFrequency(float freq) { frequency = freq; repaint(); }

float BeamVisualizer::calculateBeam(float x, float y)
{
    if (speakers.empty() || tau.size() != speakers.size())
        return 0.0f;

    float real = 0.0f;
    float imag = 0.0f;

    float tauMax = *std::max_element(tau.begin(), tau.end());

    for (int i = 0; i < speakers.size(); i++)
    {
        // distance from array element to test point
        float dx = x - speakers[i].x;
        float dy = y - speakers[i].y;

        float distance = std::sqrt(dx * dx + dy * dy);

        // arrival time from this direction
        float testTau = distance / 343.0f;
        float steeringPhase = -2.0f * juce::MathConstants<float>::pi * frequency * (tauMax - tau[i]);

        // phase of incoming wave at this point
        float arrivalPhase = 2.0f * juce::MathConstants<float>::pi * frequency * testTau;

        float phase = arrivalPhase + steeringPhase;

        real += std::cos(phase);
        imag += std::sin(phase);
    }

    float magnitude = std::sqrt(real * real + imag * imag);

    return magnitude / speakers.size();
}

void BeamVisualizer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    auto area = getLocalBounds().toFloat();
    float scale = juce::jmin(area.getWidth(), area.getHeight()) / 4.0f;

    auto centre = area.getCentre();

    for (int ix = 0; ix < resolution; ix++)
    {
        for (int iy = 0; iy < resolution; iy++)
        {
            float x = ((float)ix / resolution - 0.5f) * 4.0f;
            float y = ((float)iy / resolution - 0.5f) * 4.0f;

            float level = calculateBeam(x, y);

            level = std::pow(level, 2.0f);
            level = juce::jlimit(0.0f, 1.0f, level);

            g.setColour(juce::Colour::fromHSV((1.0f - level) * 0.65f, 1.0f, level, 1.0f));
            g.fillRect(centre.x + x * scale, centre.y - y * scale, scale * 4 / resolution, scale * 4 / resolution);
        }
    }

    // Speakers
    g.setColour(juce::Colours::white);

    for (auto& s : speakers)
        g.fillEllipse(centre.x + s.x * scale - 5, centre.y - s.y * scale - 5, 10, 10);
    
    // Bright point
    g.setColour(juce::Colours::red);
    g.fillEllipse(centre.x + brightPoint.x * scale - 7, centre.y - brightPoint.y * scale - 7, 14, 14);

    // Component outline
    g.setColour(juce::Colours::white);
    g.drawRect(getLocalBounds().toFloat(), 3.0f);
}
