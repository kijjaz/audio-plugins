#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/TapeDSP.h"

namespace vts
{

class AnalysisPanelComponent : public juce::Component
{
public:
    AnalysisPanelComponent(TapeDSP* dspPtr) : dsp(dspPtr) {}

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // LCD Background
        g.setColour(juce::Colour(0xff1a1c1e));
        g.fillRoundedRectangle(bounds, 10.0f);
        g.setColour(juce::Colour(0xffd4af37));
        g.drawRoundedRectangle(bounds, 10.0f, 2.0f);
        
        // Inner Grid
        g.setColour(juce::Colour(0xffd4af37).withAlpha(0.1f));
        for (int i = 1; i < 10; ++i) {
            float x = bounds.getWidth() * (i / 10.0f);
            float y = bounds.getHeight() * (i / 10.0f);
            g.drawVerticalLine(juce::roundToInt(x), 0.0f, bounds.getHeight());
            g.drawHorizontalLine(juce::roundToInt(y), 0.0f, bounds.getWidth());
        }
        
        auto freqArea = bounds.removeFromLeft(bounds.getWidth() * 0.45f).reduced(15.0f);
        auto transferArea = bounds.removeFromLeft(bounds.getWidth() * 0.7f).reduced(15.0f);
        auto meterArea = bounds.reduced(15.0f);

        drawFrequencyResponse(g, freqArea);
        drawTransferCurve(g, transferArea);
        drawMeters(g, meterArea);
    }

private:
    TapeDSP* dsp;

    void drawFrequencyResponse(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText("FREQUENCY RESPONSE (Linear magnitude approx)", area.removeFromTop(20.0f), juce::Justification::centredTop);

        if (dsp == nullptr) return;

        juce::Path p;
        bool started = false;
        
        float minFreq = 20.0f;
        float maxFreq = 20000.0f;
        
        for (int x = 0; x < area.getWidth(); x += 2)
        {
            float normX = x / area.getWidth();
            // Log scale for frequency
            float freq = minFreq * std::pow(maxFreq / minFreq, normX);
            
            float mag = dsp->getMagnitudeAtFrequency(freq);
            float db = juce::Decibels::gainToDecibels(mag);
            
            // Map +/- 15 dB to height
            float yNorm = juce::jmap(db, -15.0f, 15.0f, 1.0f, 0.0f);
            float y = area.getY() + yNorm * area.getHeight();
            y = juce::jlimit(area.getY(), area.getBottom(), y);
            
            if (!started) {
                p.startNewSubPath(area.getX() + x, y);
                started = true;
            } else {
                p.lineTo(area.getX() + x, y);
            }
        }
        
        // Zero dB line
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        float zeroY = area.getY() + juce::jmap(0.0f, -15.0f, 15.0f, 1.0f, 0.0f) * area.getHeight();
        g.drawHorizontalLine(juce::roundToInt(zeroY), area.getX(), area.getRight());

        g.setColour(juce::Colour(0xffd4af37)); // Gold
        g.strokePath(p, juce::PathStrokeType(2.0f));
    }

    void drawTransferCurve(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText("TRANSFER CURVE (Harmonic Generation)", area.removeFromTop(20.0f), juce::Justification::centredTop);

        if (dsp == nullptr) return;

        juce::Path p;
        bool started = false;
        
        for (int x = 0; x < area.getWidth(); x += 2)
        {
            float normX = (x / area.getWidth()) * 2.0f - 1.0f; // -1 to 1
            float yVal = dsp->getTransferFunction(normX);
            
            float yNorm = juce::jmap(yVal, -1.2f, 1.2f, 1.0f, 0.0f);
            float y = area.getY() + yNorm * area.getHeight();
            y = juce::jlimit(area.getY(), area.getBottom(), y);
            
            if (!started) {
                p.startNewSubPath(area.getX() + x, y);
                started = true;
            } else {
                p.lineTo(area.getX() + x, y);
            }
        }
        
        // Draw axes
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawHorizontalLine(juce::roundToInt(area.getCentreY()), area.getX(), area.getRight());
        g.drawVerticalLine(juce::roundToInt(area.getCentreX()), area.getY(), area.getBottom());
        
        g.setColour(juce::Colour(0xfff3e5ab)); // Light Gold
        g.strokePath(p, juce::PathStrokeType(2.0f));
    }
    
    void drawMeters(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText("SAG G.R.", area.removeFromTop(20.0f), juce::Justification::centredTop);

        if (dsp == nullptr) return;
        
        float gr = dsp->getCurrentSagGR(); // e.g. 0 to -20
        float fillHeight = juce::jmap(gr, -20.0f, 0.0f, area.getHeight(), 0.0f);
        fillHeight = juce::jlimit(0.0f, area.getHeight(), fillHeight);
        
        juce::Rectangle<float> meterBar = area.withTrimmedTop(fillHeight);
        
        // Gradient from green to red for GR
        juce::ColourGradient grad(juce::Colours::red, area.getX(), area.getY(),
                                  juce::Colours::green, area.getX(), area.getBottom(), false);
        g.setGradientFill(grad);
        g.fillRect(meterBar);
        
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.drawRect(area, 1.0f);
        
        g.setFont(14.0f);
        g.setColour(juce::Colours::white);
        g.drawText(juce::String(gr, 1) + " dB", area.withTrimmedTop(area.getHeight() + 5.0f).withHeight(20.0f), juce::Justification::centred);
    }
};

} // namespace vts
