#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include "../DSP/ToneStack.h"
#include "../DSP/Cabinet.h"
#include "IronStackLookAndFeel.h"
#include <vector>

namespace ironstack
{

class ToneVisualizerComponent : public juce::Component
{
public:
    ToneVisualizerComponent()
    {
        setInterceptsMouseClicks(false, false);
    }

    void updateCurves(const ToneStack& ts, const Cabinet& cab)
    {
        // Sample 120 points from 20 Hz to 20 kHz log-spaced
        toneCurve.clear();
        cabCurve.clear();
        combinedCurve.clear();

        const int numPoints = 120;
        toneCurve.reserve(numPoints);
        cabCurve.reserve(numPoints);
        combinedCurve.reserve(numPoints);

        const double logMin = std::log10(20.0);
        const double logMax = std::log10(20000.0);

        for (int i = 0; i < numPoints; ++i)
        {
            double f = std::pow(10.0, logMin + (logMax - logMin) * (i / (double)(numPoints - 1)));
            
            // Tone stack transfer
            std::complex<double> h_ts = ts.evaluateAnalogResponse(f);
            float ts_db = static_cast<float>(20.0 * std::log10(std::max(std::abs(h_ts), 1e-4)));
            
            // Cabinet transfer
            float cab_db = cab.evaluateMagnitudeDb(static_cast<float>(f));

            toneCurve.push_back({ static_cast<float>(f), ts_db });
            cabCurve.push_back({ static_cast<float>(f), cab_db });
            combinedCurve.push_back({ static_cast<float>(f), ts_db + cab_db });
        }

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Recessed Dark Screen Background
        g.setColour(juce::Colour(0xff101113));
        g.fillRoundedRectangle(bounds, 5.0f);

        // Precision Grid Lines
        g.setColour(juce::Colour(0xff22252a));
        float fGrid[] = { 50.0f, 100.0f, 250.0f, 500.0f, 1000.0f, 2500.0f, 5000.0f, 10000.0f };
        for (float f : fGrid)
        {
            float normX = (std::log10(f) - std::log10(20.0f)) / (std::log10(20000.0f) - std::log10(20.0f));
            float x = bounds.getX() + normX * bounds.getWidth();
            g.drawVerticalLine(juce::roundToInt(x), bounds.getY(), bounds.getBottom());
        }

        float dbGrid[] = { 10.0f, 0.0f, -10.0f, -20.0f, -30.0f };
        for (float db : dbGrid)
        {
            float normY = 1.0f - (db - minDb) / (maxDb - minDb);
            float y = bounds.getY() + normY * bounds.getHeight();
            g.drawHorizontalLine(juce::roundToInt(y), bounds.getX(), bounds.getRight());
        }

        // 0 dB Baseline (subtle gold)
        float zeroY = bounds.getY() + (1.0f - (0.0f - minDb) / (maxDb - minDb)) * bounds.getHeight();
        g.setColour(IronStackLookAndFeel::goldAccent.withAlpha(0.25f));
        g.drawHorizontalLine(juce::roundToInt(zeroY), bounds.getX(), bounds.getRight());

        // Draw Curves
        if (!combinedCurve.empty())
        {
            auto toPoint = [&](const std::pair<float, float>& pt) -> juce::Point<float> {
                float normX = (std::log10(pt.first) - std::log10(20.0f)) / (std::log10(20000.0f) - std::log10(20.0f));
                float normY = 1.0f - (pt.second - minDb) / (maxDb - minDb);
                return { bounds.getX() + normX * bounds.getWidth(),
                         bounds.getY() + juce::jlimit(0.0f, bounds.getHeight(), normY * bounds.getHeight()) };
            };

            // 1. Tone Stack Curve (Dotted / Thin Tweed)
            juce::Path pTone;
            pTone.startNewSubPath(toPoint(toneCurve[0]));
            for (size_t i = 1; i < toneCurve.size(); ++i)
                pTone.lineTo(toPoint(toneCurve[i]));
            g.setColour(IronStackLookAndFeel::tweedGold.withAlpha(0.40f));
            g.strokePath(pTone, juce::PathStrokeType(1.2f));

            // 2. Cabinet Curve (Thin Charcoal Blue)
            juce::Path pCab;
            pCab.startNewSubPath(toPoint(cabCurve[0]));
            for (size_t i = 1; i < cabCurve.size(); ++i)
                pCab.lineTo(toPoint(cabCurve[i]));
            g.setColour(juce::Colour(0xff4a90e2).withAlpha(0.35f));
            g.strokePath(pCab, juce::PathStrokeType(1.2f));

            // 3. Master Combined Transfer Function (Luminous Gold Glow)
            juce::Path pCombined;
            pCombined.startNewSubPath(toPoint(combinedCurve[0]));
            for (size_t i = 1; i < combinedCurve.size(); ++i)
                pCombined.lineTo(toPoint(combinedCurve[i]));

            // Underglow gradient fill
            juce::Path fillPath = pCombined;
            fillPath.lineTo(bounds.getRight(), bounds.getBottom());
            fillPath.lineTo(bounds.getX(), bounds.getBottom());
            fillPath.closeSubPath();
            juce::ColourGradient fillGrad(IronStackLookAndFeel::goldAccent.withAlpha(0.12f), bounds.getCentreX(), bounds.getY(),
                                          juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getBottom(), false);
            g.setGradientFill(fillGrad);
            g.fillPath(fillPath);

            // Glow Stroke
            g.setColour(IronStackLookAndFeel::goldHighlight);
            g.strokePath(pCombined, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // Border & Glass Reflection
        g.setColour(IronStackLookAndFeel::goldAccent.withAlpha(0.35f));
        g.drawRoundedRectangle(bounds, 5.0f, 1.0f);

        // Legend Badge
        auto legendBox = juce::Rectangle<float>(bounds.getRight() - 146.0f, bounds.getY() + 8.0f, 138.0f, 18.0f);
        g.setColour(IronStackLookAndFeel::carbonDark.withAlpha(0.75f));
        g.fillRoundedRectangle(legendBox, 3.0f);
        g.setFont(juce::Font("Georgia", 9.5f, juce::Font::bold));
        g.setColour(IronStackLookAndFeel::goldHighlight.withAlpha(0.9f));
        g.drawText("OUTPUT RESPONSE H(w)", legendBox.toNearestInt(), juce::Justification::centred, false);
    }

private:
    std::vector<std::pair<float, float>> toneCurve;
    std::vector<std::pair<float, float>> cabCurve;
    std::vector<std::pair<float, float>> combinedCurve;

    const float minDb = -35.0f;
    const float maxDb = 15.0f;
};

} // namespace ironstack
