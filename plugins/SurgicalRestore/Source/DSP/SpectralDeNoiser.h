#pragma once
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>
#include <algorithm>

namespace sr_dsp
{

/**
 * C++ Production-Grade STFT Overlap-Add Spectral Processor.
 * Uses exact 1024-point FFT + Hann analysis & synthesis windowing (75% overlap, hop=256).
 * Performs true Forward FFT -> Spectral Gain Masking -> Inverse FFT -> Overlap-Add.
 */
class SpectralDeNoiser
{
public:
    static constexpr int fftOrder = 10;
    static constexpr int fftSize = 1024;
    static constexpr int hopSize = 256;
    static constexpr int numBins = fftSize / 2 + 1; // 513

    SpectralDeNoiser()
        : m_fft (fftOrder)
        , m_window (fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
    }

    void prepare (double sampleRate)
    {
        m_sampleRate = sampleRate;
        m_inputFifo.assign (fftSize, 0.0f);
        m_outputFifo.assign (fftSize * 2, 0.0f);
        m_timeDomainBuffer.assign (fftSize, 0.0f);
        m_complexFftData.assign (fftSize * 2, 0.0f);
        m_priorSNR.assign (numBins, 10.0f);
        m_noisePSD.assign (numBins, 0.01f);
        m_fifoWritePos = 0;
        m_samplesSinceLastHop = 0;
        m_delayLine.assign (fftSize, 0.0f);
        m_delayPos = 0;
    }

    void processBlock (float* channelData, int numSamples, float reductionDb, float harmonicShield)
    {
        // When De-Hiss reduction is 0 (or negligible), pass through an exact sample-accurate 1024-sample delay line.
        // This guarantees bit-exact cancellation (< -120 dBFS) in Delta audition mode for untouched clean audio.
        if (reductionDb <= 0.05f)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                float in = channelData[i];
                float delayed = m_delayLine[m_delayPos];
                m_delayLine[m_delayPos] = in;
                m_delayPos = (m_delayPos + 1) % fftSize;
                channelData[i] = delayed;

                // Keep STFT FIFO synced
                m_inputFifo[m_fifoWritePos] = in;
                m_fifoWritePos = (m_fifoWritePos + 1) % fftSize;
                m_samplesSinceLastHop = (m_samplesSinceLastHop + 1) % hopSize;
            }
            return;
        }

        float minGain = std::pow (10.0f, -reductionDb / 20.0f);

        for (int i = 0; i < numSamples; ++i)
        {
            float inSample = channelData[i];

            // Update delay line state to maintain seamless transitions if unbypassed
            m_delayLine[m_delayPos] = inSample;
            m_delayPos = (m_delayPos + 1) % fftSize;

            // 1. Pull output first so delay is exactly fftSize (1024 samples)
            channelData[i] = m_outputFifo[0];
            for (size_t k = 0; k < m_outputFifo.size() - 1; ++k)
                m_outputFifo[k] = m_outputFifo[k + 1];
            m_outputFifo.back() = 0.0f;

            // 2. Push into circular input FIFO
            m_inputFifo[m_fifoWritePos] = inSample;
            m_fifoWritePos = (m_fifoWritePos + 1) % fftSize;
            m_samplesSinceLastHop++;

            // 3. Process STFT frame every hopSize (256 samples)
            if (m_samplesSinceLastHop >= hopSize)
            {
                m_samplesSinceLastHop = 0;
                processFrame (minGain, harmonicShield);
            }
        }
    }

private:
    void processFrame (float minGain, float harmonicShield)
    {
        // 1. Extract linear analysis frame from circular input FIFO
        for (int i = 0; i < fftSize; ++i)
        {
            int readIdx = (m_fifoWritePos - fftSize + i + fftSize) % fftSize;
            m_timeDomainBuffer[i] = m_inputFifo[readIdx];
            m_complexFftData[i * 2] = m_timeDomainBuffer[i];
            m_complexFftData[i * 2 + 1] = 0.0f;
        }

        // Apply analysis window
        m_window.multiplyWithWindowingTable (m_timeDomainBuffer.data(), fftSize);
        for (int i = 0; i < fftSize; ++i)
        {
            m_complexFftData[i * 2] = m_timeDomainBuffer[i];
            m_complexFftData[i * 2 + 1] = 0.0f;
        }

        // 2. Forward Complex FFT
        m_fft.perform (reinterpret_cast<const juce::dsp::Complex<float>*> (m_complexFftData.data()),
                       reinterpret_cast<juce::dsp::Complex<float>*> (m_complexFftData.data()), false);

        // 3. Apply Decision-Directed Wiener spectral gain mask
        for (int k = 0; k < numBins; ++k)
        {
            float real = m_complexFftData[k * 2];
            float imag = m_complexFftData[k * 2 + 1];
            float mag = std::sqrt (real * real + imag * imag + 1e-12f);
            // Normalized FFT power
            float power = (mag * mag) / static_cast<float> (fftSize);

            // Adaptive Minimum Statistics noise floor tracking
            if (power < m_noisePSD[k])
                m_noisePSD[k] = 0.90f * m_noisePSD[k] + 0.10f * power; // fast tracking of quiet passages
            else
                m_noisePSD[k] = 0.998f * m_noisePSD[k] + 0.002f * power; // slow rise during loud signal

            // Post-SNR relative to tracked noise floor
            float postSNR = std::max (0.001f, power / (m_noisePSD[k] + 1e-9f));

            // Decision-directed prior SNR (Ephraim-Malah)
            float a = 0.96f;
            float prior = a * m_priorSNR[k] + (1.0f - a) * std::max (0.0f, postSNR - 1.0f);
            m_priorSNR[k] = prior;

            // Wiener gain
            float gain = prior / (1.0f + prior);
            gain = std::max (gain * (1.0f + harmonicShield * 0.4f), minGain);
            gain = std::min (1.0f, gain);

            // Apply gain to complex bin
            m_complexFftData[k * 2]     *= gain;
            m_complexFftData[k * 2 + 1] *= gain;

            // Mirror for symmetric real signal
            if (k > 0 && k < fftSize / 2)
            {
                int mirrorIdx = fftSize - k;
                m_complexFftData[mirrorIdx * 2]     = m_complexFftData[k * 2];
                m_complexFftData[mirrorIdx * 2 + 1] = -m_complexFftData[k * 2 + 1];
            }
        }

        // 4. Inverse Complex FFT
        m_fft.perform (reinterpret_cast<const juce::dsp::Complex<float>*> (m_complexFftData.data()),
                       reinterpret_cast<juce::dsp::Complex<float>*> (m_complexFftData.data()), true);

        // 5. Synthesis window & Overlap-Add into output FIFO
        // In JUCE (Apple vDSP), IFFT already scales by 1/N.
        // The sum of Hann^2 for 75% overlap (hop=256, N=1024) is exactly 1.5.
        // Therefore, the exact unity-gain normalization is 1.0 / 1.5 = 2.0 / 3.0.
        constexpr float norm = 2.0f / 3.0f;

        for (int i = 0; i < fftSize; ++i)
        {
            float synthSample = m_complexFftData[i * 2] * norm;
            m_timeDomainBuffer[i] = synthSample;
        }
        m_window.multiplyWithWindowingTable (m_timeDomainBuffer.data(), fftSize);

        for (int i = 0; i < fftSize; ++i)
        {
            m_outputFifo[i] += m_timeDomainBuffer[i];
        }
    }

    double m_sampleRate = 44100.0;
    juce::dsp::FFT m_fft;
    juce::dsp::WindowingFunction<float> m_window;

    std::vector<float> m_inputFifo;
    std::vector<float> m_outputFifo;
    std::vector<float> m_timeDomainBuffer;
    std::vector<float> m_complexFftData;
    std::vector<float> m_priorSNR;
    std::vector<float> m_noisePSD;

    int m_fifoWritePos = 0;
    int m_samplesSinceLastHop = 0;

    std::vector<float> m_delayLine;
    int m_delayPos = 0;
};

} // namespace sr_dsp
