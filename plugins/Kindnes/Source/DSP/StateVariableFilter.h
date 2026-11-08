#pragma once

#include <cmath>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Resonant Zero-Delay Feedback (ZDF) State Variable Filter.
 * Features:
 * - Clean self-oscillating resonance without numerical blowup
 * - Multi-mode: Low-Pass, High-Pass, Band-Pass, Notch
 * - Sample-rate invariant trapezoidal integration
 */
class StateVariableFilter
{
public:
    enum class Mode
    {
        LowPass = 0,
        HighPass = 1,
        BandPass = 2,
        Notch = 3,
        Bypass = 4
    };

    StateVariableFilter() = default;

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = std::max(8000.0, sampleRate);
        updateCoefficients();
    }

    void setParameters(Mode mode, float cutoffHz, float resonance) noexcept
    {
        mode_ = mode;
        cutoff_ = std::clamp(cutoffHz, 20.0f, static_cast<float>(sampleRate_ * 0.48));
        resonance_ = std::clamp(resonance, 0.1f, 12.0f);
        updateCoefficients();
    }

    void reset() noexcept
    {
        s1_ = 0.0f;
        s2_ = 0.0f;
    }

    inline float processSample(float input) noexcept
    {
        if (mode_ == Mode::Bypass)
            return input;

        // Zero-delay feedback SVF calculation
        float hp = (input - (2.0f * R_ + g_) * s1_ - s2_) * h_;
        float bp = g_ * hp + s1_;
        float lp = g_ * bp + s2_;

        s1_ = g_ * hp + bp;
        s2_ = g_ * bp + lp;

        // Anti-denormal protection
        if (std::abs(s1_) < 1e-15f) s1_ = 0.0f;
        if (std::abs(s2_) < 1e-15f) s2_ = 0.0f;

        switch (mode_)
        {
            case Mode::LowPass:  return lp;
            case Mode::HighPass: return hp;
            case Mode::BandPass: return bp;
            case Mode::Notch:    return input - bp * 2.0f * R_;
            case Mode::Bypass:   return input;
        }
        return lp;
    }

private:
    void updateCoefficients() noexcept
    {
        constexpr float pi = 3.14159265358979323846f;
        float wd = 2.0f * pi * cutoff_;
        float T = 1.0f / static_cast<float>(sampleRate_);
        float wa = (2.0f / T) * std::tan(wd * T * 0.5f);
        g_ = wa * T * 0.5f;
        R_ = 1.0f / (2.0f * resonance_);
        h_ = 1.0f / (1.0f + 2.0f * R_ * g_ + g_ * g_);
    }

    double sampleRate_{ 44100.0 };
    Mode mode_{ Mode::LowPass };
    float cutoff_{ 20000.0f };
    float resonance_{ 0.707f };

    float g_{ 0.0f };
    float R_{ 0.5f };
    float h_{ 1.0f };

    float s1_{ 0.0f };
    float s2_{ 0.0f };
};

} // namespace KindnesDSP
