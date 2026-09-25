#pragma once
#include <cmath>
#include <algorithm>

class CleverTailDetector
{
public:
    // Prepare tail gate with studio-grade -96dBFS threshold and generous 1.5s hold time
    void prepare(double sampleRate, float noiseFloorDb = -96.0f, float holdTimeSec = 1.5f, float releaseMs = 500.0f)
    {
        fs = static_cast<float>(sampleRate);
        noiseFloorLinear = std::pow(10.0f, noiseFloorDb / 20.0f);
        holdSamples = static_cast<int>(holdTimeSec * fs);
        releaseSamples = static_cast<int>((releaseMs / 1000.0f) * fs);

        alphaAttack = 0.90f;
        alphaDecay = 0.99995f; // Slower release envelope tracker to preserve subtle reverberant tails
        reset();
    }

    void reset()
    {
        envelope = 0.0f;
        activeHoldTimer = 0;
        releaseTimer = 0;
        isReleasing = false;
    }

    // Returns dynamic gain scalar [0.0 to 1.0] to eliminate digital truncation clicks
    inline float process(float leftSample, float rightSample)
    {
        float inputMag = std::max(std::abs(leftSample), std::abs(rightSample));

        if (inputMag > envelope)
            envelope = (1.0f - alphaAttack) * inputMag + alphaAttack * envelope;
        else
            envelope = (1.0f - alphaDecay) * inputMag + alphaDecay * envelope;

        if (envelope > noiseFloorLinear)
        {
            activeHoldTimer = holdSamples;
            isReleasing = false;
            releaseTimer = 0;
            return 1.0f;
        }

        if (activeHoldTimer > 0)
        {
            --activeHoldTimer;
            return 1.0f;
        }

        // Apply smooth Hann release curve
        if (releaseTimer < releaseSamples)
        {
            float progress = static_cast<float>(releaseTimer) / static_cast<float>(releaseSamples);
            float gain = 0.5f * (1.0f + std::cos(3.14159265f * progress));
            ++releaseTimer;
            return gain;
        }

        return 0.0f;
    }

    inline bool isSleeping() const noexcept
    {
        return (envelope <= noiseFloorLinear) && (activeHoldTimer == 0) && (releaseTimer >= releaseSamples);
    }

private:
    float fs = 96000.0f;
    float noiseFloorLinear = 0.0000158f; // -96 dBFS
    int holdSamples = 144000;
    int releaseSamples = 48000;
    float alphaAttack = 0.90f;
    float alphaDecay = 0.99995f;

    float envelope = 0.0f;
    int activeHoldTimer = 0;
    int releaseTimer = 0;
    bool isReleasing = false;
};
