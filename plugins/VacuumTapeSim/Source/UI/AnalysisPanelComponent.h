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
        
        // LCD Panel Background (Dark Matte Glass with Gold Border)
        g.setColour(juce::Colour(0xff121416));
        g.fillRoundedRectangle(bounds, 8.0f);
        
        // Brushed panel bezel
        g.setColour(juce::Colour(0xffd4af37).withAlpha(0.6f));
        g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
        
        // Inner Fine Grid (Oscilloscope style)
        g.setColour(juce::Colour(0xffd4af37).withAlpha(0.06f));
        for (int i = 1; i < 20; ++i) {
            float x = bounds.getWidth() * (i / 20.0f);
            g.drawVerticalLine(juce::roundToInt(x), 0.0f, bounds.getHeight());
        }
        for (int i = 1; i < 8; ++i) {
            float y = bounds.getHeight() * (i / 8.0f);
            g.drawHorizontalLine(juce::roundToInt(y), 0.0f, bounds.getWidth());
        }
        
        auto innerArea = bounds.reduced(10.0f);
        
        // 4 Columns:
        // Col 1: Frequency & Head Bump Curve (36%)
        // Col 2: Saturation Transfer Curve & Dynamic Operating Point (28%)
        // Col 3: Tape Physics & Wow Modulation Orbit (18%)
        // Col 4: Live Audio Telemetry Meters (18%)
        auto freqArea = innerArea.removeFromLeft(innerArea.getWidth() * 0.36f).reduced(4.0f);
        auto transferArea = innerArea.removeFromLeft(innerArea.getWidth() * 0.44f).reduced(4.0f);
        auto orbitArea = innerArea.removeFromLeft(innerArea.getWidth() * 0.50f).reduced(4.0f);
        auto meterArea = innerArea.reduced(4.0f);

        drawFrequencyResponse(g, freqArea);
        drawTransferCurve(g, transferArea);
        drawPhysicsOrbit(g, orbitArea);
        drawLiveTelemetryMeters(g, meterArea);
    }

private:
    TapeDSP* dsp;

    void drawFrequencyResponse(juce::Graphics& g, juce::Rectangle<float> area)
    {
        // Panel Header
        g.setColour(juce::Colour(0xffd4af37));
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("FREQUENCY RESPONSE", area.removeFromTop(18.0f), juce::Justification::centredLeft);

        if (dsp == nullptr) return;

        juce::Path p;
        juce::Path fillP;
        bool started = false;
        
        float minFreq = 20.0f;
        float maxFreq = 20000.0f;
        float zeroY = area.getY() + juce::jmap(0.0f, -15.0f, 15.0f, 1.0f, 0.0f) * area.getHeight();
        
        for (int x = 0; x < area.getWidth(); x += 2)
        {
            float normX = x / area.getWidth();
            float freq = minFreq * std::pow(maxFreq / minFreq, normX);
            
            float mag = dsp->getMagnitudeAtFrequency(freq);
            float db = juce::Decibels::gainToDecibels(mag);
            
            float yNorm = juce::jmap(db, -15.0f, 15.0f, 1.0f, 0.0f);
            float y = area.getY() + yNorm * area.getHeight();
            y = juce::jlimit(area.getY(), area.getBottom(), y);
            
            if (!started) {
                p.startNewSubPath(area.getX() + x, y);
                fillP.startNewSubPath(area.getX() + x, zeroY);
                fillP.lineTo(area.getX() + x, y);
                started = true;
            } else {
                p.lineTo(area.getX() + x, y);
                fillP.lineTo(area.getX() + x, y);
            }
        }

        fillP.lineTo(area.getRight(), zeroY);
        fillP.closeSubPath();
        
        // Subtle gold glow gradient under frequency curve
        juce::ColourGradient fillGrad(juce::Colour(0xffd4af37).withAlpha(0.18f), area.getCentreX(), area.getY(),
                                      juce::Colour(0xffd4af37).withAlpha(0.01f), area.getCentreX(), zeroY, false);
        g.setGradientFill(fillGrad);
        g.fillPath(fillP);

        // 0 dB reference line
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawHorizontalLine(juce::roundToInt(zeroY), area.getX(), area.getRight());

        // Response curve stroke
        g.setColour(juce::Colour(0xfff3e5ab)); // Light Gold
        g.strokePath(p, juce::PathStrokeType(2.0f));

        // Frequency calibration markers
        g.setFont(9.0f);
        g.setColour(juce::Colours::white.withAlpha(0.4f));
        g.drawText("100Hz", area.getX() + 15, area.getBottom() - 14, 40, 12, juce::Justification::left);
        g.drawText("1kHz", area.getCentreX() - 15, area.getBottom() - 14, 40, 12, juce::Justification::centred);
        g.drawText("10kHz", area.getRight() - 40, area.getBottom() - 14, 40, 12, juce::Justification::right);
    }

    void drawTransferCurve(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colour(0xffd4af37));
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("LANGEVIN TRANSFER CURVE", area.removeFromTop(18.0f), juce::Justification::centredLeft);

        if (dsp == nullptr) return;

        // Axes crosshair
        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawHorizontalLine(juce::roundToInt(area.getCentreY()), area.getX(), area.getRight());
        g.drawVerticalLine(juce::roundToInt(area.getCentreX()), area.getY(), area.getBottom());

        // Linear unity reference line (diagonal)
        g.setColour(juce::Colours::grey.withAlpha(0.3f));
        g.drawLine(area.getX(), area.getBottom(), area.getRight(), area.getY(), 1.0f);

        juce::Path p;
        bool started = false;
        
        for (int x = 0; x < area.getWidth(); x += 2)
        {
            float normX = (x / area.getWidth()) * 2.0f - 1.0f; // -1 to +1
            float yVal = dsp->getTransferFunction(normX);
            
            float yNorm = juce::jmap(yVal, -1.3f, 1.3f, 1.0f, 0.0f);
            float y = area.getY() + yNorm * area.getHeight();
            y = juce::jlimit(area.getY(), area.getBottom(), y);
            
            if (!started) {
                p.startNewSubPath(area.getX() + x, y);
                started = true;
            } else {
                p.lineTo(area.getX() + x, y);
            }
        }
        
        g.setColour(juce::Colour(0xffe6c666));
        g.strokePath(p, juce::PathStrokeType(2.0f));

        // Live Audio Operating Point dot on the curve!
        float liveIn = juce::jlimit(-1.0f, 1.0f, dsp->getLiveInputPeak());
        float liveOut = dsp->getTransferFunction(liveIn);
        float dotX = area.getCentreX() + liveIn * (area.getWidth() * 0.45f);
        float dotY = area.getY() + juce::jmap(liveOut, -1.3f, 1.3f, 1.0f, 0.0f) * area.getHeight();
        dotY = juce::jlimit(area.getY(), area.getBottom(), dotY);

        // Glowing live indicator
        g.setColour(juce::Colours::cyan.withAlpha(0.3f));
        g.fillEllipse(dotX - 7.0f, dotY - 7.0f, 14.0f, 14.0f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(dotX - 3.5f, dotY - 3.5f, 7.0f, 7.0f);

        // Indicator label
        g.setFont(9.0f);
        g.setColour(juce::Colours::cyan);
        g.drawText("OP POINT", area.getX() + 4, area.getBottom() - 14, 60, 12, juce::Justification::left);
    }
    
    void drawPhysicsOrbit(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colour(0xffd4af37));
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("TAPE DISPLACEMENT", area.removeFromTop(18.0f), juce::Justification::centredLeft);

        if (dsp == nullptr) return;

        auto center = area.getCentre();
        float radius = std::min(area.getWidth(), area.getHeight()) * 0.42f;

        // Concentric guides
        g.setColour(juce::Colour(0xffd4af37).withAlpha(0.12f));
        g.drawEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);
        g.drawEllipse(center.x - radius * 0.5f, center.y - radius * 0.5f, radius, radius, 1.0f);

        // Crosshairs
        g.drawLine(center.x - radius, center.y, center.x + radius, center.y, 1.0f);
        g.drawLine(center.x, center.y - radius, center.x, center.y + radius, 1.0f);

        // Live Wow displacement orbit
        float wow = dsp->getLiveWow();
        float angle = juce::Time::getMillisecondCounter() * 0.003f;
        float orbitR = radius * (0.3f + juce::jlimit(0.0f, 0.65f, std::abs(wow) * 8.0f));
        
        float orbX = center.x + orbitR * std::cos(angle);
        float orbY = center.y + orbitR * std::sin(angle);

        // Orbit tail
        g.setColour(juce::Colour(0xffd4af37).withAlpha(0.25f));
        g.drawLine(center.x, center.y, orbX, orbY, 1.5f);

        // Magnetic displacement marker
        g.setColour(juce::Colour(0xffff9900));
        g.fillEllipse(orbX - 5.0f, orbY - 5.0f, 10.0f, 10.0f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(orbX - 2.0f, orbY - 2.0f, 4.0f, 4.0f);

        g.setFont(9.0f);
        g.setColour(juce::Colours::white.withAlpha(0.6f));
        g.drawText("PITCH FLUTTER", area.getX(), area.getBottom() - 14, area.getWidth(), 12, juce::Justification::centred);
    }

    void drawLiveTelemetryMeters(juce::Graphics& g, juce::Rectangle<float> area)
    {
        g.setColour(juce::Colour(0xffd4af37));
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.drawText("LIVE DYNAMICS", area.removeFromTop(18.0f), juce::Justification::centredLeft);

        if (dsp == nullptr) return;

        // 3 Vertical Bars: IN, SAG GR, THD
        float colWidth = (area.getWidth() - 16.0f) / 3.0f;
        float barHeight = area.getHeight() - 28.0f;

        auto drawBar = [&](float x, float normVal, juce::Colour colTop, juce::Colour colBot, const juce::String& label, const juce::String& valStr)
        {
            juce::Rectangle<float> barRect(x, area.getY() + 4.0f, colWidth, barHeight);
            
            // Frame
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillRect(barRect);
            g.setColour(juce::Colours::white.withAlpha(0.15f));
            g.drawRect(barRect, 1.0f);

            // Level fill
            float clamped = juce::jlimit(0.0f, 1.0f, normVal);
            float fillH = barRect.getHeight() * clamped;
            juce::Rectangle<float> fillRect(barRect.getX(), barRect.getBottom() - fillH, barRect.getWidth(), fillH);

            juce::ColourGradient grad(colTop, fillRect.getX(), fillRect.getY(),
                                      colBot, fillRect.getX(), fillRect.getBottom(), false);
            g.setGradientFill(grad);
            g.fillRect(fillRect);

            // Labels
            g.setFont(9.0f);
            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.drawText(label, x - 5.0f, area.getBottom() - 20.0f, colWidth + 10.0f, 10.0f, juce::Justification::centred);
            g.setFont(juce::Font(8.0f, juce::Font::bold));
            g.drawText(valStr, x - 8.0f, area.getBottom() - 10.0f, colWidth + 16.0f, 10.0f, juce::Justification::centred);
        };

        // 1. Input Level (0 to 1.0+)
        float inPeak = dsp->getLiveInputPeak();
        float inDb = juce::Decibels::gainToDecibels(std::max(0.0001f, inPeak));
        float inNorm = juce::jmap(inDb, -40.0f, 3.0f, 0.0f, 1.0f);
        drawBar(area.getX(), inNorm, juce::Colours::gold, juce::Colours::darkgoldenrod, "IN", juce::String(inDb, 0) + "dB");

        // 2. Sag GR (0 to -15dB)
        float gr = dsp->getCurrentSagGR();
        float grNorm = juce::jmap(gr, 0.0f, -15.0f, 0.0f, 1.0f);
        drawBar(area.getX() + colWidth + 8.0f, grNorm, juce::Colours::crimson, juce::Colours::orange, "SAG", juce::String(gr, 1) + "dB");

        // 3. Nonlinear Saturation (THD proxy)
        float thd = dsp->getLiveTHD();
        float thdNorm = juce::jmap(thd, 0.0f, 0.4f, 0.0f, 1.0f);
        drawBar(area.getX() + 2 * (colWidth + 8.0f), thdNorm, juce::Colours::cyan, juce::Colours::deepskyblue, "SAT", juce::String(juce::roundToInt(thdNorm * 100)) + "%");
    }
};

} // namespace vts
