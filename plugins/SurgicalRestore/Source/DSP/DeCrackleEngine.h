#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace sr_dsp
{

/**
 * C++ Real-Time De-Crackle Engine.
 * Measures rolling variance of innovation error to suppress needle chatter.
 */
class DeCrackleEngine
{
public:
    DeCrackleEngine() = default;

    void prepare (double sampleRate)
    {
        m_sampleRate = sampleRate;
        m_invHistory.assign (32, 0.0f);
        m_invHistIdx = 0;
        m_runningSumSq = 0.0f;
    }

    inline float process (float input, float innovation, float sensitivity, float maxReductionDb)
    {
        // Update rolling variance of innovation error
        float oldSq = m_invHistory[m_invHistIdx];
        float newSq = innovation * innovation;
        m_invHistory[m_invHistIdx] = newSq;
        m_invHistIdx = (m_invHistIdx + 1) % 32;

        m_runningSumSq += (newSq - oldSq);
        float localVariance = std::max (0.0f, m_runningSumSq / 32.0f);

        // Smooth energy envelope
        float inAbs = std::abs (input);
        m_env = 0.995f * m_env + 0.005f * inAbs;

        // Relative chatter metric
        float chatter = std::sqrt (localVariance) / (m_env + 0.005f);
        float thresh = 1.8f / std::max (0.1f, sensitivity);

        if (chatter > thresh)
        {
            float excess = std::min (1.0f, (chatter - thresh) / (thresh * 1.5f));
            float minGain = std::pow (10.0f, -maxReductionDb / 20.0f);
            float gain = 1.0f - (1.0f - minGain) * excess;
            return input * gain;
        }

        return input;
    }

private:
    double m_sampleRate = 44100.0;
    std::vector<float> m_invHistory;
    int m_invHistIdx = 0;
    float m_runningSumSq = 0.0f;
    float m_env = 0.05f;
};

} // namespace sr_dsp
