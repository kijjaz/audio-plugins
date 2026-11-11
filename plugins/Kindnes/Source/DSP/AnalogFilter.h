#pragma once

#include <cmath>
#include <algorithm>

namespace KindnesDSP
{

/**
 * 8-Bit Motherboard Analog Audio Output Filtering.
 * 
 * Accurately models the multi-stage analog signal path between the 2A03 DAC
 * and the RCA audio out jack:
 * 1. 1st-order HPF @ 90 Hz (DC decoupling and sub-rumble blocker)
 * 2. 1st-order HPF @ 440 Hz (low-end tilt creating classic 8-bit punch)
 * 3. 3-pole Low-Pass Filter @ ~14 kHz (anti-imaging and vintage warmth)
 */
class AnalogFilter
{
public:
    AnalogFilter()
    {
        reset();
    }

    void reset() noexcept
    {
        hpf1_z1_ = 0.0f;
        hpf2_z1_ = 0.0f;
        lpf_z1_ = 0.0f;
        lpf_z2_ = 0.0f;
        lpf_z3_ = 0.0f;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
        updateCoefficients();
    }

    void setEnabled(bool enabled) noexcept
    {
        enabled_ = enabled;
    }

    void setToneShaping(float warmFactor) noexcept
    {
        // warmFactor in [0.0, 1.0]: 0 = bright, 0.5 = authentic NES, 1.0 = dark CRT TV
        lpfCutoff_ = 8000.0f + (1.0f - warmFactor) * 12000.0f; // 8 kHz to 20 kHz
        updateCoefficients();
    }

    /**
     * Processes one sample through the analog signal conditioning path.
     */
    inline float processSample(float input) noexcept
    {
        if (!enabled_)
            return input;

        // 1. High Pass 1 @ 90 Hz
        float hpf1Out = hpf1_a0_ * (input - hpf1_z1_) + hpf1_b1_ * hpf1_prevOut_;
        hpf1_z1_ = input;
        hpf1_prevOut_ = hpf1Out;

        // 2. High Pass 2 @ 440 Hz
        float hpf2Out = hpf2_a0_ * (hpf1Out - hpf2_z1_) + hpf2_b1_ * hpf2_prevOut_;
        hpf2_z1_ = hpf1Out;
        hpf2_prevOut_ = hpf2Out;

        // 3. 3-pole Low Pass @ ~14 kHz
        lpf_z1_ += lpf_alpha_ * (hpf2Out - lpf_z1_);
        lpf_z2_ += lpf_alpha_ * (lpf_z1_ - lpf_z2_);
        lpf_z3_ += lpf_alpha_ * (lpf_z2_ - lpf_z3_);

        return lpf_z3_;
    }

private:
    void updateCoefficients() noexcept
    {
        constexpr float pi = 3.14159265358979323846f;

        // 90 Hz HPF (Bilinear transform)
        float w90 = 2.0f * sampleRate_ * std::tan(pi * 90.0f / sampleRate_);
        float denom90 = w90 + 2.0f * sampleRate_;
        hpf1_a0_ = (2.0f * sampleRate_) / denom90;
        hpf1_b1_ = (2.0f * sampleRate_ - w90) / denom90;

        // 440 Hz HPF (Bilinear transform)
        float w440 = 2.0f * sampleRate_ * std::tan(pi * 440.0f / sampleRate_);
        float denom440 = w440 + 2.0f * sampleRate_;
        hpf2_a0_ = (2.0f * sampleRate_) / denom440;
        hpf2_b1_ = (2.0f * sampleRate_ - w440) / denom440;

        // LPF alpha: cutoff at lpfCutoff_
        float wc = 2.0f * pi * lpfCutoff_;
        lpf_alpha_ = std::clamp(wc / (wc + sampleRate_), 0.01f, 0.99f);
    }

    float sampleRate_{ 44100.0f };
    bool enabled_{ true };
    float lpfCutoff_{ 14000.0f };

    float hpf1_z1_{ 0.0f };
    float hpf1_prevOut_{ 0.0f };
    float hpf1_a0_{ 0.98f };
    float hpf1_b1_{ 0.98f };

    float hpf2_z1_{ 0.0f };
    float hpf2_prevOut_{ 0.0f };
    float hpf2_a0_{ 0.95f };
    float hpf2_b1_{ 0.95f };

    float lpf_z1_{ 0.0f };
    float lpf_z2_{ 0.0f };
    float lpf_z3_{ 0.0f };
    float lpf_alpha_{ 0.65f };
};

} // namespace KindnesDSP
