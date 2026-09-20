#include "ScopeComponent.h"
#include "../PluginProcessor.h"
#include "LookAndFeel.h"

namespace FriedTofuUI
{

ScopeComponent::ScopeComponent(FriedTofuAudioProcessor& processor)
    : audioProcessor(processor)
{
    displayBuffer.fill(0.0f);
    startTimerHz(30); // 30 FPS oscilloscope refresh
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
    auto bounds = getLocalBounds().toFloat();

    // Dark screen background
    g.setColour(juce::Colour::fromRGB(0x0e, 0x12, 0x13));
    g.fillRoundedRectangle(bounds, 2.0f);

    // Border
    g.setColour(FriedTofuLookAndFeel::lineBorder);
    g.drawRoundedRectangle(bounds, 2.0f, 1.0f);

    // Horizontal grid lines
    const float h = bounds.getHeight();
    const float w = bounds.getWidth();
    g.setColour(FriedTofuLookAndFeel::phosphorGreen.withAlpha(0.08f));
    for (int i = 1; i < 4; ++i)
    {
        float y = h * static_cast<float>(i) / 4.0f;
        g.drawHorizontalLine(static_cast<int>(y), 0.0f, w);
    }

    // Oscilloscope waveform trace
    juce::Path p;
    const float centerY = h * 0.5f;
    const float scaleY  = h * 0.42f;

    for (int i = 0; i < ScopePoints; ++i)
    {
        float x = w * static_cast<float>(i) / static_cast<float>(ScopePoints - 1);
        float sampleVal = std::clamp(displayBuffer[static_cast<size_t>(i)], -1.5f, 1.5f);
        float y = centerY - sampleVal * scaleY;

        if (i == 0)
            p.startNewSubPath(x, y);
        else
            p.lineTo(x, y);
    }

    // Phosphor glow
    g.setColour(FriedTofuLookAndFeel::phosphorGreen.withAlpha(0.2f));
    g.strokePath(p, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Core sharp trace
    g.setColour(FriedTofuLookAndFeel::phosphorGreen);
    g.strokePath(p, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void ScopeComponent::resized()
{
}

} // namespace FriedTofuUI
