#include "ScopeComponent.h"
#include "../PluginProcessor.h"
#include "LookAndFeel.h"

namespace FriedTofuUI
{

ScopeComponent::ScopeComponent(FriedTofuMBAudioProcessor& p)
    : audioProcessor(p)
{
    startTimerHz(30);
}

ScopeComponent::~ScopeComponent()
{
    stopTimer();
}

void ScopeComponent::timerCallback()
{
    audioProcessor.getScopeSamples(displayBuffer.data(), ScopePoints);
    repaint();
}

void ScopeComponent::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // Dark screen background
    g.setColour(juce::Colour(0xff0d1117));
    g.fillRoundedRectangle(bounds, 6.0f);

    // Subtle grid lines
    g.setColour(juce::Colour(0x2230363d));
    g.drawHorizontalLine(static_cast<int>(bounds.getCentreY()), bounds.getX(), bounds.getRight());
    g.drawVerticalLine(static_cast<int>(bounds.getCentreX()), bounds.getY(), bounds.getBottom());

    // Phosphor green waveform
    g.setColour(FriedTofuLookAndFeel::phosphorGreen);
    juce::Path wavePath;

    const float midY = bounds.getCentreY();
    const float scaleY = bounds.getHeight() * 0.45f;
    const float stepX = bounds.getWidth() / static_cast<float>(ScopePoints);

    bool first = true;
    for (int i = 0; i < ScopePoints; ++i)
    {
        const float x = bounds.getX() + i * stepX;
        const float y = midY - displayBuffer[static_cast<size_t>(i)] * scaleY;

        if (first)
        {
            wavePath.startNewSubPath(x, y);
            first = false;
        }
        else
        {
            wavePath.lineTo(x, y);
        }
    }

    g.strokePath(wavePath, juce::PathStrokeType(1.6f));

    // Outer phosphor border
    g.setColour(juce::Colour(0x442ea043));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void ScopeComponent::resized()
{
}

} // namespace FriedTofuUI
