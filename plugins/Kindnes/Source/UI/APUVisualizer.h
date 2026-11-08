#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <atomic>
#include <mutex>

namespace KindnesUI
{

/**
 * APUVisualizer: Real-time APU oscilloscope and 1-bit DPCM staircase display.
 */
class APUVisualizer : public juce::Component, public juce::Timer
{
public:
    APUVisualizer()
    {
        scopeBuffer_.resize(512, 0.0f);
        displayBuffer_.resize(512, 0.0f);
        startTimerHz(30);
    }

    ~APUVisualizer() override
    {
        stopTimer();
    }

    void pushSamples(const float* samples, int numSamples)
    {
        if (numSamples <= 0 || samples == nullptr)
            return;

        for (int i = 0; i < numSamples; ++i)
        {
            scopeBuffer_[writeIndex_] = samples[i];
            writeIndex_ = (writeIndex_ + 1) % scopeBuffer_.size();
        }
    }

    void setDpcmPreviews(const std::vector<float>& original, const std::vector<float>& reconstructed)
    {
        std::lock_guard<std::mutex> lock(dpcmMutex_);
        dpcmOriginal_ = original;
        dpcmReconstructed_ = reconstructed;
    }

    void timerCallback() override
    {
        // Copy circular buffer to display buffer with trigger sync
        int triggerIdx = 0;
        for (size_t i = 1; i < scopeBuffer_.size() / 2; ++i)
        {
            if (scopeBuffer_[i - 1] < 0.0f && scopeBuffer_[i] >= 0.0f)
            {
                triggerIdx = static_cast<int>(i);
                break;
            }
        }

        for (size_t i = 0; i < displayBuffer_.size(); ++i)
        {
            size_t src = (triggerIdx + i) % scopeBuffer_.size();
            displayBuffer_[i] = scopeBuffer_[src];
        }

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // 1. Dark CRT Screen Background
        g.setColour(juce::Colour(0xff0D0E11));
        g.fillRoundedRectangle(bounds, 6.0f);

        // 2. Bezel Border
        g.setColour(juce::Colour(0xff2A2D36));
        g.drawRoundedRectangle(bounds, 6.0f, 1.5f);

        // Split into Left (Master APU Oscilloscope) and Right (1-Bit DPCM Staircase Analyzer)
        auto scopeBounds = bounds.removeFromLeft(bounds.getWidth() * 0.58f);
        auto dpcmBounds = bounds;

        // Grid lines
        g.setColour(juce::Colour(0x15E5B869));
        g.drawLine(scopeBounds.getX(), scopeBounds.getCentreY(), scopeBounds.getRight(), scopeBounds.getCentreY(), 1.0f);
        g.drawLine(scopeBounds.getCentreX(), scopeBounds.getY(), scopeBounds.getCentreX(), scopeBounds.getBottom(), 1.0f);

        // --- 1. APU Master Oscilloscope Trace ---
        juce::Path p;
        float xStep = scopeBounds.getWidth() / static_cast<float>(displayBuffer_.size());
        float halfH = scopeBounds.getHeight() * 0.40f;
        float midY = scopeBounds.getCentreY();

        for (size_t i = 0; i < displayBuffer_.size(); ++i)
        {
            float x = scopeBounds.getX() + static_cast<float>(i) * xStep;
            float y = midY - displayBuffer_[i] * halfH;
            y = std::clamp(y, scopeBounds.getY() + 3.0f, scopeBounds.getBottom() - 3.0f);

            if (i == 0) p.startNewSubPath(x, y);
            else p.lineTo(x, y);
        }

        g.setColour(juce::Colour(0x33E5B869));
        g.strokePath(p, juce::PathStrokeType(4.0f));
        g.setColour(juce::Colour(0xffFFD700)); // Gold Phosphor
        g.strokePath(p, juce::PathStrokeType(1.5f));

        g.setFont(juce::Font(9.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xa0E5B869));
        g.drawText("APU OSCILLOSCOPE | 1.789 MHz", scopeBounds.reduced(8.0f), juce::Justification::topLeft, false);

        // --- 2. 1-Bit DPCM Delta Staircase Analyzer ---
        g.setColour(juce::Colour(0xff1C1F26));
        g.drawLine(dpcmBounds.getX(), dpcmBounds.getY(), dpcmBounds.getX(), dpcmBounds.getBottom(), 1.5f);

        g.setColour(juce::Colour(0xa000E5FF));
        g.drawText("1-BIT DPCM DELTA STAIRCASE", dpcmBounds.reduced(8.0f), juce::Justification::topLeft, false);

        std::lock_guard<std::mutex> lock(dpcmMutex_);
        if (!dpcmReconstructed_.empty())
        {
            float dStep = dpcmBounds.getWidth() / static_cast<float>(dpcmReconstructed_.size());
            float dMidY = dpcmBounds.getCentreY();
            float dHalfH = dpcmBounds.getHeight() * 0.38f;

            // Reconstructed 1-bit stepped staircase (Cyan)
            juce::Path dpcmPath;
            for (size_t i = 0; i < dpcmReconstructed_.size(); ++i)
            {
                float x = dpcmBounds.getX() + static_cast<float>(i) * dStep;
                float y = dMidY - dpcmReconstructed_[i] * dHalfH;
                y = std::clamp(y, dpcmBounds.getY() + 3.0f, dpcmBounds.getBottom() - 3.0f);

                if (i == 0) dpcmPath.startNewSubPath(x, y);
                else dpcmPath.lineTo(x, y);
            }

            g.setColour(juce::Colour(0x3000E5FF));
            g.strokePath(dpcmPath, juce::PathStrokeType(3.0f));
            g.setColour(juce::Colour(0xff00E5FF)); // Stepped cyan
            g.strokePath(dpcmPath, juce::PathStrokeType(1.2f));

            g.setFont(juce::Font(9.0f, juce::Font::plain));
            g.setColour(juce::Colour(0x90D0D2D6));
            g.drawText("SLOPE OVERLOAD & DELTA STEPS", dpcmBounds.reduced(8.0f), juce::Justification::bottomLeft, false);
        }
        else
        {
            g.setFont(juce::Font(10.0f, juce::Font::italic));
            g.setColour(juce::Colour(0x60D0D2D6));
            g.drawText("No sample loaded", dpcmBounds, juce::Justification::centred, false);
        }
    }

private:
    std::vector<float> scopeBuffer_;
    std::vector<float> displayBuffer_;
    size_t writeIndex_{ 0 };

    std::mutex dpcmMutex_;
    std::vector<float> dpcmOriginal_;
    std::vector<float> dpcmReconstructed_;
};

} // namespace KindnesUI
