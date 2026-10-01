#pragma once
#include <juce_core/juce_core.h>
#include <cmath>
#include <algorithm>

namespace sr_dsp
{

/**
 * 3rd-order (18 dB/oct) Butterworth High-Pass Filter
 * Designed specifically for vinyl warp and turntable motor rumble suppression (< 25 Hz).
 */
class RumbleFilter
{
public:
    RumbleFilter() = default;

    void prepare (double sampleRate, float cutoffHz = 25.0f)
    {
        m_sampleRate = sampleRate;
        setCutoff (cutoffHz);
        reset();
    }

    void setCutoff (float cutoffHz)
    {
        m_cutoff = std::max (10.0f, std::min (cutoffHz, 80.0f));
        calculateCoefficients();
    }

    void reset()
    {
        m_x1 = m_x2 = m_x3 = 0.0f;
        m_y1 = m_y2 = m_y3 = 0.0f;
    }

    inline float processSample (float in)
    {
        // 1st order stage: y1 = b0_1 * x + b1_1 * x1 - a1_1 * y1
        float out1 = m_b0_1 * in + m_b1_1 * m_x1 - m_a1_1 * m_y1;
        m_x1 = in;
        m_y1 = out1;

        // 2nd order stage (Biquad HPF): y2 = b0_2 * out1 + b1_2 * x2 + b2_2 * x3 - a1_2 * y2 - a2_2 * y3
        float out2 = m_b0_2 * out1 + m_b1_2 * m_x2 + m_b2_2 * m_x3 - m_a1_2 * m_y2 - m_a2_2 * m_y3;
        m_x3 = m_x2;
        m_x2 = out1;
        m_y3 = m_y2;
        m_y2 = out2;

        return out2;
    }

private:
    void calculateCoefficients()
    {
        double omega = 2.0 * juce::MathConstants<double>::pi * m_cutoff / m_sampleRate;
        double tanW = std::tan (omega * 0.5);

        // 1st order HPF Butterworth: s / (s + 1)
        double a0_1 = 1.0 + tanW;
        m_b0_1 = static_cast<float> (1.0 / a0_1);
        m_b1_1 = static_cast<float> (-1.0 / a0_1);
        m_a1_1 = static_cast<float> ((tanW - 1.0) / a0_1);

        // 2nd order HPF Butterworth (Q = 1.0 for overall 3rd-order Butterworth alignment)
        double tanW2 = tanW * tanW;
        double a0_2 = 1.0 + std::sqrt (2.0) * tanW + tanW2;
        m_b0_2 = static_cast<float> (1.0 / a0_2);
        m_b1_2 = static_cast<float> (-2.0 / a0_2);
        m_b2_2 = static_cast<float> (1.0 / a0_2);
        m_a1_2 = static_cast<float> (2.0 * (tanW2 - 1.0) / a0_2);
        m_a2_2 = static_cast<float> ((1.0 - std::sqrt (2.0) * tanW + tanW2) / a0_2);
    }

    double m_sampleRate = 44100.0;
    float m_cutoff = 25.0f;

    // 1st order states
    float m_x1 = 0.0f, m_y1 = 0.0f;
    float m_b0_1 = 1.0f, m_b1_1 = -1.0f, m_a1_1 = 0.0f;

    // 2nd order states
    float m_x2 = 0.0f, m_x3 = 0.0f, m_y2 = 0.0f, m_y3 = 0.0f;
    float m_b0_2 = 1.0f, m_b1_2 = -2.0f, m_b2_2 = 1.0f, m_a1_2 = 0.0f, m_a2_2 = 0.0f;
};

} // namespace sr_dsp
