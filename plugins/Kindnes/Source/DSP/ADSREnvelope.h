#pragma once

#include <cmath>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Fast, punchy exponential ADSR envelope.
 */
class ADSREnvelope
{
public:
    enum class State
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    ADSREnvelope() = default;

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = std::max(8000.0, sampleRate);
        updateCoefficients();
    }

    void setParameters(float attackMs, float decayMs, float sustainLevel, float releaseMs) noexcept
    {
        attackTime_ = std::max(0.5f, attackMs) * 0.001f;
        decayTime_ = std::max(1.0f, decayMs) * 0.001f;
        sustainLevel_ = std::clamp(sustainLevel, 0.0f, 1.0f);
        releaseTime_ = std::max(1.0f, releaseMs) * 0.001f;
        updateCoefficients();
    }

    void noteOn() noexcept
    {
        state_ = State::Attack;
    }

    void noteOff() noexcept
    {
        if (state_ != State::Idle)
            state_ = State::Release;
    }

    void reset() noexcept
    {
        state_ = State::Idle;
        currentValue_ = 0.0f;
    }

    [[nodiscard]] bool isActive() const noexcept
    {
        return state_ != State::Idle;
    }

    [[nodiscard]] State getState() const noexcept { return state_; }
    [[nodiscard]] float getCurrentValue() const noexcept { return currentValue_; }

    [[nodiscard]] float getNextSample() noexcept
    {
        switch (state_)
        {
            case State::Idle:
                currentValue_ = 0.0f;
                break;

            case State::Attack:
                currentValue_ += attackRate_;
                if (currentValue_ >= 1.0f)
                {
                    currentValue_ = 1.0f;
                    state_ = State::Decay;
                }
                break;

            case State::Decay:
                currentValue_ += (sustainLevel_ - currentValue_) * decayCoeff_;
                if (std::abs(currentValue_ - sustainLevel_) < 0.001f)
                {
                    currentValue_ = sustainLevel_;
                    state_ = State::Sustain;
                }
                break;

            case State::Sustain:
                currentValue_ = sustainLevel_;
                break;

            case State::Release:
                currentValue_ += (0.0f - currentValue_) * releaseCoeff_;
                if (currentValue_ <= 0.0005f)
                {
                    currentValue_ = 0.0f;
                    state_ = State::Idle;
                }
                break;
        }

        return currentValue_;
    }

private:
    void updateCoefficients() noexcept
    {
        attackRate_ = 1.0f / (attackTime_ * static_cast<float>(sampleRate_));
        // Exponential decay and release coefficients (60 dB decay time)
        decayCoeff_ = 1.0f - std::exp(-5.0f / (decayTime_ * static_cast<float>(sampleRate_)));
        releaseCoeff_ = 1.0f - std::exp(-5.0f / (releaseTime_ * static_cast<float>(sampleRate_)));
    }

    double sampleRate_{ 44100.0 };
    float attackTime_{ 0.005f };  // 5ms
    float decayTime_{ 0.150f };   // 150ms
    float sustainLevel_{ 0.8f };
    float releaseTime_{ 0.200f }; // 200ms

    float attackRate_{ 0.01f };
    float decayCoeff_{ 0.01f };
    float releaseCoeff_{ 0.01f };

    float currentValue_{ 0.0f };
    State state_{ State::Idle };
};

} // namespace KindnesDSP
