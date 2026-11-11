#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include "PolyBLEP.h"

namespace KindnesDSP
{

/**
 * VRC6 Audio Expansion Emulation (Dn-FamiTracker / nsfplay vrc6 model).
 * 
 * Featured in Japanese Famicom classics like Castlevania III (Akumajou Densetsu),
 * Esper Dream 2, and Madara.
 * Adds:
 * - 2x Pulse Channels with 8 hardware 16-step duty cycles (0..7 => 6.25% to 50.0%)
 * - 1x Sawtooth Channel with authentic 14-step, 8-bit wrapping accumulator
 *   (`(k * accumRate) & 0xFF`, outputting top 5 bits `accum >> 3` with PolyBLEP
 *   edge anti-aliasing and authentic 8-bit wrap-around overdrive at high volumes)
 */
class ExpansionVRC6
{
public:
    ExpansionVRC6()
    {
        reset();
    }

    void reset() noexcept
    {
        p1Phase_ = 0.0f;
        p2Phase_ = 0.0f;
        sawPhase_ = 0.0f;

        p1Freq_ = 440.0f;
        p2Freq_ = 440.0f;
        sawFreq_ = 220.0f;

        p1Duty_ = 1; // 12.5% (2/16)
        p2Duty_ = 3; // 25.0% (4/16)
        
        p1Vol_ = 0;
        p2Vol_ = 0;
        sawVol_ = 0;

        enabled_ = false;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
    }

    void setEnabled(bool enabled) noexcept { enabled_ = enabled; }
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_; }

    void setPulse1(float freqHz, int dutyStep, int vol) noexcept
    {
        p1Freq_ = std::max(10.0f, std::min(freqHz, sampleRate_ * 0.48f));
        p1Duty_ = std::clamp(dutyStep, 0, 7);
        p1Vol_ = std::clamp(vol, 0, 15);
    }

    void setPulse2(float freqHz, int dutyStep, int vol) noexcept
    {
        p2Freq_ = std::max(10.0f, std::min(freqHz, sampleRate_ * 0.48f));
        p2Duty_ = std::clamp(dutyStep, 0, 7);
        p2Vol_ = std::clamp(vol, 0, 15);
    }

    void setSawtooth(float freqHz, int vol) noexcept
    {
        sawFreq_ = std::max(10.0f, std::min(freqHz, sampleRate_ * 0.48f));
        sawVol_ = std::clamp(vol, 0, 15);
    }

    /**
     * Processes one audio sample and returns the combined VRC6 output [-1.0, 1.0].
     */
    float processSample() noexcept
    {
        if (!enabled_)
            return 0.0f;

        // Pulse 1 (16-step duty counter: (duty + 1) / 16.0f -> 6.25% to 50.0%)
        float p1Out = 0.0f;
        if (p1Vol_ > 0)
        {
            float dt1 = p1Freq_ / sampleRate_;
            float duty1 = (static_cast<float>(p1Duty_) + 1.0f) / 16.0f;
            float blep1 = PolyBLEP::pulse(p1Phase_, dt1, duty1);
            p1Phase_ += dt1;
            if (p1Phase_ >= 1.0f)
                p1Phase_ -= std::floor(p1Phase_);
            p1Out = blep1 * (static_cast<float>(p1Vol_) / 15.0f);
        }

        // Pulse 2 (16-step duty counter: (duty + 1) / 16.0f -> 6.25% to 50.0%)
        float p2Out = 0.0f;
        if (p2Vol_ > 0)
        {
            float dt2 = p2Freq_ / sampleRate_;
            float duty2 = (static_cast<float>(p2Duty_) + 1.0f) / 16.0f;
            float blep2 = PolyBLEP::pulse(p2Phase_, dt2, duty2);
            p2Phase_ += dt2;
            if (p2Phase_ >= 1.0f)
                p2Phase_ -= std::floor(p2Phase_);
            p2Out = blep2 * (static_cast<float>(p2Vol_) / 15.0f);
        }

        // Sawtooth: Authentic VRC6 14-step (7 active even additions), 8-bit wrapping accumulator
        // with top-5-bit DAC output (`accum >> 3`) and PolyBLEP staircase edge anti-aliasing.
        float sawOut = 0.0f;
        if (sawVol_ > 0)
        {
            float dtSaw = sawFreq_ / sampleRate_;
            sawPhase_ += dtSaw;
            if (sawPhase_ >= 1.0f)
                sawPhase_ -= std::floor(sawPhase_);

            int accumRate = computeAccumRate(sawVol_);
            float stepFloat7 = sawPhase_ * 7.0f;
            int k = static_cast<int>(stepFloat7) % 7;
            if (k < 0) k += 7;
            float subPhase = stepFloat7 - std::floor(stepFloat7);
            float subDt = dtSaw * 7.0f;

            float curr5Bit = static_cast<float>(((k * accumRate) & 0xFF) >> 3);
            float prev5Bit = static_cast<float>((((k + 6) % 7 * accumRate) & 0xFF) >> 3);
            float next5Bit = static_cast<float>((((k + 1) % 7 * accumRate) & 0xFF) >> 3);

            float blep = PolyBLEP::staircaseBlep(subPhase, subDt, curr5Bit - prev5Bit, next5Bit - curr5Bit);
            float dac5Bit = std::clamp(curr5Bit + blep, 0.0f, 31.0f);

            // Compute cycle mean to center DC while preserving exact hardware step amplitude
            float mean5Bit = 0.0f;
            for (int s = 0; s < 7; ++s)
                mean5Bit += static_cast<float>(((s * accumRate) & 0xFF) >> 3);
            mean5Bit /= 7.0f;

            sawOut = (dac5Bit - mean5Bit) / 15.5f;
        }

        return (p1Out * 0.35f + p2Out * 0.35f + sawOut * 0.65f) * 0.45f;
    }

private:
    static inline int computeAccumRate(int vol15) noexcept
    {
        if (vol15 <= 0)
            return 0;
        // For vol 1..13, linear 6-bit accumulator rate 3..42 (clean 14-step saw, 6*42 = 252 <= 255).
        // For vol 14..15, pushes accumRate to 46..50 (> 42), triggering the classic Dn-FamiTracker
        // VRC6 8-bit accumulator wrap-around distortion (`& 0xFF`) on the top steps!
        if (vol15 <= 13)
            return (vol15 * 42) / 13;
        return 42 + (vol15 - 13) * 4;
    }
    float sampleRate_{ 44100.0f };
    bool enabled_{ false };

    float p1Phase_{ 0.0f };
    float p2Phase_{ 0.0f };
    float sawPhase_{ 0.0f };

    float p1Freq_{ 440.0f };
    float p2Freq_{ 440.0f };
    float sawFreq_{ 220.0f };

    int p1Duty_{ 4 };
    int p2Duty_{ 4 };

    int p1Vol_{ 0 };
    int p2Vol_{ 0 };
    int sawVol_{ 0 };
};

} // namespace KindnesDSP
