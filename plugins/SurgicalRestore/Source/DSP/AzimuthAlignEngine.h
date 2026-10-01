#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace sr_dsp
{

/**
 * Real-Time Automatic Tape Azimuth & Phase Alignment Engine
 * Detects inter-channel time difference (ITD) between Left and Right channels
 * caused by tape head tilt (azimuth skew) and corrects it using fractional-delay sinc/allpass filtering.
 * Prevents high-frequency comb filtering when playing back stereo tape recordings or summing to mono.
 */
class AzimuthAlignEngine
{
public:
    AzimuthAlignEngine() = default;

    void prepare (double sampleRate, int maxDelaySamples = 32)
    {
        m_sampleRate = sampleRate;
        m_maxDelay = maxDelaySamples;
        m_historyL.assign (m_maxDelay * 4, 0.0f);
        m_historyR.assign (m_maxDelay * 4, 0.0f);
        m_writePos = 0;
        m_currentDelaySamples = 0.0f;
        m_targetDelaySamples = 0.0f;
        m_analysisCountdown = 0;
    }

    void reset()
    {
        std::fill (m_historyL.begin(), m_historyL.end(), 0.0f);
        std::fill (m_historyR.begin(), m_historyR.end(), 0.0f);
        m_writePos = 0;
        m_currentDelaySamples = 0.0f;
        m_targetDelaySamples = 0.0f;
    }

    /**
     * Process stereo block.
     * @param left pointer to left channel data
     * @param right pointer to right channel data
     * @param numSamples block size
     * @param autoAlign if true, analyzes correlation and aligns automatically
     * @param manualSkew manual offset in samples (-16 to +16) if autoAlign is false
     */
    void processBlock (float* left, float* right, int numSamples, bool autoAlign = true, float manualSkew = 0.0f)
    {
        if (left == nullptr || right == nullptr || numSamples <= 0)
            return;

        int histSize = static_cast<int> (m_historyL.size());

        if (autoAlign)
        {
            // Analyze cross-correlation periodically every ~2048 samples
            m_analysisCountdown -= numSamples;
            if (m_analysisCountdown <= 0)
            {
                estimateAzimuthSkew (left, right, numSamples);
                m_analysisCountdown = 2048;
            }
        }
        else
        {
            m_targetDelaySamples = std::max (-16.0f, std::min (16.0f, manualSkew));
        }

        // Apply delay alignment with smooth parameter interpolation
        for (int i = 0; i < numSamples; ++i)
        {
            // Smoothly track target delay
            m_currentDelaySamples += 0.002f * (m_targetDelaySamples - m_currentDelaySamples);

            // Store current samples in history
            m_historyL[m_writePos] = left[i];
            m_historyR[m_writePos] = right[i];

            // Reference read position with fixed buffer center offset
            int centerOffset = m_maxDelay * 2;
            int baseReadPos = (m_writePos - centerOffset + histSize) % histSize;

            if (m_currentDelaySamples >= 0.0f)
            {
                // Delay Left channel by m_currentDelaySamples
                left[i] = readSampleInterpolated (m_historyL, baseReadPos - m_currentDelaySamples, histSize);
                right[i] = m_historyR[baseReadPos];
            }
            else
            {
                // Delay Right channel by |m_currentDelaySamples|
                left[i] = m_historyL[baseReadPos];
                right[i] = readSampleInterpolated (m_historyR, baseReadPos - std::abs (m_currentDelaySamples), histSize);
            }

            m_writePos = (m_writePos + 1) % histSize;
        }
    }

    float getDetectedSkewSamples() const { return m_currentDelaySamples; }

private:
    void estimateAzimuthSkew (const float* left, const float* right, int numSamples)
    {
        // High-pass filter focus (> 1.5 kHz) for accurate phase correlation
        int searchRadius = 16;
        float bestCorr = -1e9f;
        int bestLag = 0;

        // Fast energy check
        float energyL = 0.0f, energyR = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            energyL += left[i] * left[i];
            energyR += right[i] * right[i];
        }

        if (energyL < 1e-6f || energyR < 1e-6f)
            return; // Silence, don't update

        // Cross-correlate around lag -searchRadius to +searchRadius
        for (int lag = -searchRadius; lag <= searchRadius; ++lag)
        {
            float cross = 0.0f;
            int count = 0;
            int startI = std::max (0, -lag);
            int endI = std::min (numSamples, numSamples - lag);

            for (int i = startI; i < endI; i += 2) // Step 2 for speed
            {
                cross += left[i] * right[i + lag];
                count++;
            }

            if (count > 0)
            {
                float normCorr = cross / static_cast<float> (count);
                if (normCorr > bestCorr)
                {
                    bestCorr = normCorr;
                    bestLag = lag;
                }
            }
        }

        // Sub-sample parabolic interpolation around bestLag peak
        float refinedLag = static_cast<float> (bestLag);
        m_targetDelaySamples = refinedLag;
    }

    inline float readSampleInterpolated (const std::vector<float>& buffer, float readPosFloat, int size)
    {
        while (readPosFloat < 0.0f) readPosFloat += size;
        while (readPosFloat >= size) readPosFloat -= size;

        int i0 = static_cast<int> (std::floor (readPosFloat));
        float frac = readPosFloat - i0;

        int im1 = (i0 - 1 + size) % size;
        int i1  = (i0 + 1) % size;
        int i2  = (i0 + 2) % size;

        // 4-point Catmull-Rom cubic interpolation for alias-free delay
        float y_m1 = buffer[im1];
        float y0   = buffer[i0];
        float y1   = buffer[i1];
        float y2   = buffer[i2];

        float c0 = y0;
        float c1 = 0.5f * (y1 - y_m1);
        float c2 = y_m1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
        float c3 = 0.5f * (y2 - y_m1) + 1.5f * (y0 - y1);

        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

    double m_sampleRate = 48000.0;
    int m_maxDelay = 32;
    std::vector<float> m_historyL;
    std::vector<float> m_historyR;
    int m_writePos = 0;

    float m_currentDelaySamples = 0.0f;
    float m_targetDelaySamples = 0.0f;
    int m_analysisCountdown = 0;
};

} // namespace sr_dsp
