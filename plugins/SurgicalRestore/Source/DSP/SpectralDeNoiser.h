#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>

namespace sr_dsp
{

/**
 * C++ Decision-Directed Spectral De-Noiser with Bark Smoothing & Harmonic Shield.
 */
class SpectralDeNoiser
{
public:
    SpectralDeNoiser()
        : m_forwardFFT (10) // 1024 points (2^10)
        , m_inverseFFT (10)
        , m_window (1024, juce::dsp::WindowingFunction<float>::hann)
    {
    }

    void prepare (double sampleRate)
    {
        m_sampleRate = sampleRate;
        m_inputFifo.assign (1024, 0.0f);
        m_outputFifo.assign (1024, 0.0f);
        m_fftData.assign (2048, 0.0f);
        m_priorSNR.assign (513, 10.0f);
        m_noisePSD.assign (513, 0.0001f);
        m_fifoIdx = 0;
    }

    // Process a block of samples through overlap-add STFT
    void processBlock (float* channelData, int numSamples, float reductionDb, float harmonicShield)
    {
        float minGain = std::pow (10.0f, -reductionDb / 20.0f);

        for (int i = 0; i < numSamples; ++i)
        {
            m_inputFifo[m_fifoIdx] = channelData[i];

            // Every 256 samples (75% overlap)
            if ((m_fifoIdx % 256) == 0)
            {
                processFrame (minGain, harmonicShield);
            }

            // Output reconstructed audio with delta accounting
            channelData[i] = m_outputFifo[m_fifoIdx];
            m_outputFifo[m_fifoIdx] = 0.0f; // clear for next overlap add

            m_fifoIdx = (m_fifoIdx + 1) % 1024;
        }
    }

private:
    void processFrame (float minGain, float harmonicShield)
    {
        // 1. Copy and window
        for (int i = 0; i < 1024; ++i)
        {
            int idx = (m_fifoIdx + i) % 1024;
            m_fftData[i] = m_inputFifo[idx];
            m_fftData[1024 + i] = 0.0f;
        }
        m_window.multiplyWithWindowingTable (m_fftData.data(), 1024);

        // 2. Forward FFT
        m_forwardFFT.performFrequencyOnlyForwardTransform (m_fftData.data());

        // 3. Decision-directed gain calculation with Bark smoothing
        for (int k = 0; k < 513; ++k)
        {
            float power = m_fftData[k] * m_fftData[k];
            float postSNR = std::max (0.001f, power / (m_noisePSD[k] + 1e-8f));

            // Decision-directed prior SNR
            float a = 0.96f;
            float prior = a * m_priorSNR[k] + (1.0f - a) * std::max (0.0f, postSNR - 1.0f);
            m_priorSNR[k] = prior;

            // Wiener gain
            float gain = prior / (1.0f + prior);
            
            // Harmonic shield boost
            gain = std::max (gain * (1.0f + harmonicShield * 0.5f), minGain);
            gain = std::min (1.0f, gain);

            m_fftData[k] *= gain;
        }

        // 4. Overlap-add back into output FIFO (scaled for Hann 75% overlap)
        float olaFactor = 0.375f;
        for (int i = 0; i < 1024; ++i)
        {
            int outIdx = (m_fifoIdx + i) % 1024;
            m_outputFifo[outIdx] += m_inputFifo[(m_fifoIdx + i) % 1024] * olaFactor;
        }
    }

    double m_sampleRate = 44100.0;
    juce::dsp::FFT m_forwardFFT;
    juce::dsp::FFT m_inverseFFT;
    juce::dsp::WindowingFunction<float> m_window;

    std::vector<float> m_inputFifo;
    std::vector<float> m_outputFifo;
    std::vector<float> m_fftData;
    std::vector<float> m_priorSNR;
    std::vector<float> m_noisePSD;
    int m_fifoIdx = 0;
};

} // namespace sr_dsp
