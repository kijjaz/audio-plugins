#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include "PolyBLEP.h"

namespace KindnesDSP
{

/**
 * Ricoh 2A03 Triangle Channel.
 * Features:
 * - 32-step 4-bit staircase lookup sequence (15..0..15)
 * - Clocked at full CPU rate: f_tri = f_CPU / (32 * (t + 1))
 * - Linear Counter (ticked at 240 Hz) and Length Counter (ticked at 120 Hz)
 * - Authentic DC Offset Step / Pop: Output latches at current step on halt,
 *   preserving the hardware transient click and bass punch.
 * - Mode toggle: Authentic 4-bit Stepped DAC vs Anti-Aliased Smooth Triangle
 */
class TriangleChannel
{
public:
    static constexpr uint8_t kTriangleTable[32] = {
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };

    TriangleChannel()
    {
        reset();
    }

    void reset() noexcept
    {
        stepIndex_ = 0;
        phase_ = 0.0f;
        timerPeriod_ = 0;
        frequency_ = 220.0f;
        currentStepOutput_ = 0;
        
        linearCounter_ = 127;
        linearReloadValue_ = 127;
        linearReloadFlag_ = false;
        controlFlag_ = true; // Halt / loop flag: maintains linear counter when active

        lengthCounter_ = 255;
        enabled_ = true;
        steppedDacMode_ = true;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
    }

    void setFrequency(float freqHz) noexcept
    {
        frequency_ = std::max(5.0f, std::min(freqHz, sampleRate_ * 0.49f));
        constexpr float fCPU = 1789773.0f;
        // f = fCPU / (32 * (t + 1)) => t = (fCPU / (32 * f)) - 1
        float t = (fCPU / (32.0f * frequency_)) - 1.0f;
        timerPeriod_ = std::clamp(static_cast<int>(std::round(t)), 0, 2047);
    }

    void setTimerPeriod(int period) noexcept
    {
        timerPeriod_ = std::clamp(period, 0, 2047);
        constexpr float fCPU = 1789773.0f;
        frequency_ = fCPU / (32.0f * static_cast<float>(timerPeriod_ + 1));
    }

    [[nodiscard]] int getTimerPeriod() const noexcept { return timerPeriod_; }
    [[nodiscard]] float getFrequency() const noexcept { return frequency_; }

    void setLinearCounter(bool controlFlag, int reloadValue) noexcept
    {
        controlFlag_ = controlFlag;
        linearReloadValue_ = std::clamp(reloadValue, 0, 127);
    }

    void triggerNote() noexcept
    {
        linearReloadFlag_ = true;
        linearCounter_ = linearReloadValue_;
        lengthCounter_ = 255;
        enabled_ = true;
    }

    void setEnabled(bool enabled) noexcept
    {
        enabled_ = enabled;
        if (!enabled)
        {
            lengthCounter_ = 0;
            linearCounter_ = 0;
        }
    }

    void setSteppedDacMode(bool stepped) noexcept
    {
        steppedDacMode_ = stepped;
    }

    /**
     * Ticked by Frame Sequencer at 240 Hz.
     */
    void clockLinearCounter() noexcept
    {
        if (linearReloadFlag_)
        {
            linearCounter_ = linearReloadValue_;
        }
        else if (linearCounter_ > 0)
        {
            linearCounter_--;
        }

        if (!controlFlag_)
        {
            linearReloadFlag_ = false;
        }
    }

    /**
     * Ticked by Frame Sequencer at 120 Hz.
     */
    void clockLengthCounter() noexcept
    {
        if (!controlFlag_ && lengthCounter_ > 0)
        {
            lengthCounter_--;
        }
    }

    /**
     * Advances the triangle wave and returns raw 4-bit output (0 to 15).
     * When muted / silenced, the stepped DAC output holds its current level
     * exactly as in 2A03 silicon (causing DC offset transient pops).
     */
    float processSample() noexcept
    {
        bool isActive = enabled_ && (linearCounter_ > 0) && (timerPeriod_ >= 2);

        if (!isActive)
        {
            // Silicon behavior: when halted, the DAC holds the last value
            return static_cast<float>(currentStepOutput_);
        }

        float dt = frequency_ / sampleRate_;
        phase_ += dt;
        if (phase_ >= 1.0f)
            phase_ -= std::floor(phase_);

        if (steppedDacMode_)
        {
            // 32-step discrete staircase DAC with PolyBLEP step anti-aliasing
            float stepFloat = phase_ * 32.0f;
            stepIndex_ = static_cast<int>(stepFloat) & 31;
            currentStepOutput_ = kTriangleTable[stepIndex_];

            float subPhase = stepFloat - std::floor(stepFloat);
            float subDt = dt * 32.0f;
            float currVal = static_cast<float>(currentStepOutput_);
            float prevVal = static_cast<float>(kTriangleTable[(stepIndex_ + 31) & 31]);
            float nextVal = static_cast<float>(kTriangleTable[(stepIndex_ + 1) & 31]);

            float blep = PolyBLEP::staircaseBlep(subPhase, subDt, currVal - prevVal, nextVal - currVal);
            return std::clamp(currVal + blep, 0.0f, 15.0f);
        }
        else
        {
            // Continuous anti-aliased smooth triangle mapped to [0..15]
            float tri = 2.0f * std::fabs(2.0f * (phase_ - std::floor(phase_ + 0.5f))) - 1.0f; // [-1, +1]
            float val = (tri + 1.0f) * 7.5f; // [0..15]
            currentStepOutput_ = static_cast<uint8_t>(std::clamp(val, 0.0f, 15.0f));
            return val;
        }
    }

private:
    float sampleRate_{ 44100.0f };
    float phase_{ 0.0f };
    float frequency_{ 220.0f };
    int timerPeriod_{ 0 };

    int stepIndex_{ 0 };
    uint8_t currentStepOutput_{ 0 };

    int linearCounter_{ 127 };
    int linearReloadValue_{ 127 };
    bool linearReloadFlag_{ false };
    bool controlFlag_{ false };

    int lengthCounter_{ 255 };
    bool enabled_{ true };
    bool steppedDacMode_{ true };
};

} // namespace KindnesDSP
