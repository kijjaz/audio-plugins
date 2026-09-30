#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <cmath>

namespace sr_dsp
{

/**
 * C++ High-Speed Levinson-Durbin LPC Residual Engine.
 * Extracts innovation error e[n] with SIMD-friendly vectorization.
 */
class LPCResidualEngine
{
public:
    LPCResidualEngine() = default;

    void prepare (int frameSize = 1024, int order = 16)
    {
        m_frameSize = frameSize;
        m_order = order;
        m_history.assign (order, 0.0f);
        m_aCoeffs.assign (order + 1, 0.0f);
        m_aCoeffs[0] = 1.0f;
    }

    // Single sample filter (real-time streaming)
    inline float processSample (float input)
    {
        // High-pass pre-emphasis: y[n] = x[n] - 0.94 * x[n-1]
        float pre = input - 0.94f * m_prevInput;
        m_prevInput = input;

        // FIR inverse filter: e[n] = pre[n] + sum(a[k] * pre[n-k])
        float err = pre;
        for (int k = 1; k <= m_order; ++k)
        {
            err += m_aCoeffs[k] * m_history[m_order - k];
        }

        // Shift history
        for (int k = 0; k < m_order - 1; ++k)
            m_history[k] = m_history[k + 1];
        m_history[m_order - 1] = pre;

        return err;
    }

    // Update LPC coefficients via Levinson-Durbin on a block
    void calculateCoefficients (const float* block, int numSamples)
    {
        if (numSamples <= m_order) return;

        // Autocorrelation
        std::vector<float> r (m_order + 1, 0.0f);
        for (int k = 0; k <= m_order; ++k)
        {
            float sum = 0.0f;
            for (int n = 0; n < numSamples - k; ++n)
                sum += block[n] * block[n + k];
            r[k] = sum;
        }

        if (r[0] < 1e-9f) return;

        // Levinson-Durbin recursion
        std::vector<float> a (m_order + 1, 0.0f);
        std::vector<float> aPrev (m_order + 1, 0.0f);
        float e = r[0];

        for (int i = 1; i <= m_order; ++i)
        {
            float lambda = 0.0f;
            for (int j = 1; j < i; ++j)
                lambda += aPrev[j] * r[i - j];
            lambda = -(r[i] + lambda) / e;

            a[i] = lambda;
            for (int j = 1; j < i; ++j)
                a[j] = aPrev[j] + lambda * aPrev[i - j];

            e *= (1.0f - lambda * lambda);
            if (e <= 0.0f) break;
            aPrev = a;
        }

        m_aCoeffs[0] = 1.0f;
        for (int k = 1; k <= m_order; ++k)
            m_aCoeffs[k] = a[k];
    }

private:
    int m_frameSize = 1024;
    int m_order = 16;
    float m_prevInput = 0.0f;
    std::vector<float> m_history;
    std::vector<float> m_aCoeffs;
};

} // namespace sr_dsp
