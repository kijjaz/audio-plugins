#pragma once
#include <vector>
#include <array>
#include <cmath>
#include <numeric>
#include <algorithm>

class BeamSeededFDN
{
public:
    static constexpr int NUM_LINES = 16;

    void prepare(double sampleRate, float targetRt60, float roomVolumeM3, float surfaceAreaM2)
    {
        fs = static_cast<float>(sampleRate);
        rt60 = std::max(0.1f, targetRt60);

        // Scale prime delay lengths based on mean free path: d_mean = 4 * V / S
        float mfp = (surfaceAreaM2 > 1.0f) ? (4.0f * roomVolumeM3 / surfaceAreaM2) : 10.0f;
        float scaleFactor = std::max(0.4f, mfp / 8.0f) * (fs / 24000.0f);

        constexpr int primes[NUM_LINES] = {
            1009, 1093, 1153, 1229, 1297, 1381, 1453, 1523,
            1607, 1693, 1777, 1867, 1973, 2081, 2179, 2287
        };

        for (int i = 0; i < NUM_LINES; ++i)
        {
            delayLengths[i] = static_cast<int>(primes[i] * scaleFactor);
            buffers[i].assign(delayLengths[i], 0.0f);
            bufferPointers[i] = 0;
            filterStateHF[i] = 0.0f;
            filterStateLF[i] = 0.0f;
        }

        updateAcousticParameters(rt60, currentDampCutoffHz, currentHfMult, currentBassMult);
    }

    void reset()
    {
        for (int i = 0; i < NUM_LINES; ++i)
        {
            std::fill(buffers[i].begin(), buffers[i].end(), 0.0f);
            bufferPointers[i] = 0;
            filterStateHF[i] = 0.0f;
            filterStateLF[i] = 0.0f;
        }
    }

    // Dynamic, click-free parameter update callable during active playback
    void updateAcousticParameters(float targetRt60, float dampCutoffHz, float hfMult, float bassMult)
    {
        rt60 = std::max(0.08f, targetRt60);
        currentDampCutoffHz = std::clamp(dampCutoffHz, 800.0f, 20000.0f);
        currentHfMult = std::clamp(hfMult, 0.05f, 1.0f);
        currentBassMult = std::clamp(bassMult, 0.2f, 2.5f);

        // Precompute filter coefficients for HF damping and LF crossover (~250 Hz)
        float wCutoffHF = 2.0f * 3.14159265f * currentDampCutoffHz / fs;
        // 1-pole lowpass alpha: alpha = cos(w) - 1 + sqrt(cos^2 - 4cos + 3) ~ w / (w + 1)
        float baseAlphaHF = std::clamp(std::exp(-wCutoffHF), 0.02f, 0.96f);

        float wCutoffLF = 2.0f * 3.14159265f * 250.0f / fs;
        float baseAlphaLF = std::clamp(std::exp(-wCutoffLF), 0.85f, 0.995f);
        alphaLF = baseAlphaLF;

        for (int i = 0; i < NUM_LINES; ++i)
        {
            float delaySec = static_cast<float>(delayLengths[i]) / fs;

            // Mid-band nominal loop gain: G_mid = 10^(-3 * T_i / RT60)
            loopGainsMid[i] = std::pow(10.0f, -3.0f * delaySec / rt60);

            // Low-frequency gain modifier: RT60_bass = RT60_mid * bassMult
            float rt60Bass = rt60 * currentBassMult;
            float targetGainLF = std::pow(10.0f, -3.0f * delaySec / std::max(0.08f, rt60Bass));
            gainDiffLF[i] = (targetGainLF / std::max(1e-5f, loopGainsMid[i])) - 1.0f;

            // High-frequency damping filter coefficient per line:
            // Lines with higher index receive slightly progressive damping for rich organic spatial decay
            float spread = 1.0f + 0.15f * (static_cast<float>(i - NUM_LINES / 2) / NUM_LINES);
            float effectiveAlpha = std::clamp(1.0f - (1.0f - baseAlphaHF) * currentHfMult * spread, 0.05f, 0.98f);
            dampingCoeffsHF[i] = effectiveAlpha;
        }
    }

    // Process a single late energy injection sample and output stereo diffuse reverberation
    inline void processSample(float inputSample, float drive, float& outL, float& outR)
    {
        std::array<float, NUM_LINES> delayOuts;
        float sumOutputs = 0.0f;

        for (int i = 0; i < NUM_LINES; ++i)
        {
            delayOuts[i] = buffers[i][bufferPointers[i]];
            sumOutputs += delayOuts[i];
        }

        // Stereo spatial decorrelation (odd vs even lines with phase decorrelation)
        float leftSum = 0.0f;
        float rightSum = 0.0f;
        for (int i = 0; i < NUM_LINES; i += 2) leftSum += delayOuts[i];
        for (int i = 1; i < NUM_LINES; i += 2) rightSum += delayOuts[i];

        constexpr float normScale = 0.35355f; // 1 / sqrt(8)
        outL = leftSum * normScale;
        outR = rightSum * normScale;

        // Orthogonal Householder feedback matrix reflection: M = I - 2/N * 1*1^T
        float householderTerm = (2.0f / NUM_LINES) * sumOutputs;

        for (int i = 0; i < NUM_LINES; ++i)
        {
            // Base nominal feedback loop
            float feedbackSig = (delayOuts[i] - householderTerm) * loopGainsMid[i];

            // 1. Bass Shelf Damping: adds or cuts low frequency energy below 250 Hz
            filterStateLF[i] = (1.0f - alphaLF) * feedbackSig + alphaLF * filterStateLF[i];
            float shapedFeedback = feedbackSig + gainDiffLF[i] * filterStateLF[i];

            // 2. Late acoustic energy injection (scaled appropriately for audible reverberation)
            float injected = inputSample * 0.85f + shapedFeedback;

            // 3. Soft saturation non-linearity (subtle harmonic warmth under high drive)
            float sat = injected - 0.02f * drive * (injected * injected * injected);

            // 4. Material High-Frequency Damping: 1-pole lowpass filter per line
            filterStateHF[i] = (1.0f - dampingCoeffsHF[i]) * sat + dampingCoeffsHF[i] * filterStateHF[i];

            // Write back to circular delay line
            int ptr = bufferPointers[i];
            buffers[i][ptr] = filterStateHF[i];
            bufferPointers[i] = (ptr + 1) % delayLengths[i];
        }
    }

private:
    float fs = 96000.0f;
    float rt60 = 2.0f;
    float currentDampCutoffHz = 5500.0f;
    float currentHfMult = 0.5f;
    float currentBassMult = 1.0f;
    float alphaLF = 0.98f;

    std::array<int, NUM_LINES> delayLengths{};
    std::array<float, NUM_LINES> loopGainsMid{};
    std::array<float, NUM_LINES> gainDiffLF{};
    std::array<float, NUM_LINES> dampingCoeffsHF{};

    std::array<std::vector<float>, NUM_LINES> buffers;
    std::array<int, NUM_LINES> bufferPointers{};
    std::array<float, NUM_LINES> filterStateHF{};
    std::array<float, NUM_LINES> filterStateLF{};
};
