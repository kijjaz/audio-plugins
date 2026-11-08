#pragma once

#include <cmath>
#include <algorithm>
#include <random>

namespace KindnesDSP
{

/**
 * Multi-waveform Modulation LFO for Vibrato, Auto-PWM, and Filter Sweeps.
 */
class ModulationLFO
{
public:
    enum class Waveform
    {
        Triangle = 0,
        Sine = 1,
        Saw = 2,
        Square = 3,
        SampleAndHold = 4
    };

    ModulationLFO() = default;

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = std::max(8000.0, sampleRate);
    }

    void setFrequency(float freqHz) noexcept
    {
        frequency_ = std::clamp(freqHz, 0.05f, 30.0f);
    }

    void setWaveform(Waveform wave) noexcept
    {
        waveform_ = wave;
    }

    void reset() noexcept
    {
        phase_ = 0.0f;
        shVal_ = 0.0f;
    }

    /**
     * Advances LFO by one sample and returns output in [-1.0, 1.0].
     */
    inline float processSample() noexcept
    {
        float dt = frequency_ / static_cast<float>(sampleRate_);
        float prevPhase = phase_;
        phase_ += dt;
        if (phase_ >= 1.0f)
        {
            phase_ -= 1.0f;
            // New random value for Sample & Hold
            shVal_ = (static_cast<float>(rng_()) / static_cast<float>(rng_.max())) * 2.0f - 1.0f;
        }

        switch (waveform_)
        {
            case Waveform::Triangle:
                return 2.0f * std::fabs(2.0f * (phase_ - std::floor(phase_ + 0.5f))) - 1.0f;

            case Waveform::Sine:
                return std::sin(phase_ * 6.2831853f);

            case Waveform::Saw:
                return 2.0f * phase_ - 1.0f;

            case Waveform::Square:
                return (phase_ < 0.5f) ? 1.0f : -1.0f;

            case Waveform::SampleAndHold:
                return shVal_;
        }
        return 0.0f;
    }

private:
    double sampleRate_{ 44100.0 };
    float frequency_{ 4.0f };
    Waveform waveform_{ Waveform::Triangle };
    float phase_{ 0.0f };
    float shVal_{ 0.0f };
    std::minstd_rand rng_{ 1337 };
};

} // namespace KindnesDSP
