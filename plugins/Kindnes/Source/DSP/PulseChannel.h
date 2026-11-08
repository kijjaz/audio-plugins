#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>
#include "PolyBLEP.h"

namespace KindnesDSP
{

/**
 * Ricoh 2A03 Pulse Channel (Square 1 & Square 2).
 * Features:
 * - 4 Authentic hardware duty cycles (12.5%, 25%, 50%, 75%)
 * - Modern continuous PWM mode (0.1% to 99.9%)
 * - Hardware Sweep Unit (with Pulse 1 1's complement quirk vs Pulse 2 2's complement)
 * - Hardware Volume Envelope & Decay unit (15 volume levels)
 * - Bandlimited PolyBLEP anti-aliasing for pristine tone across all sample rates
 */
class PulseChannel
{
public:
    enum class DutyMode
    {
        Duty12_5 = 0,
        Duty25_0 = 1,
        Duty50_0 = 2,
        Duty75_0 = 3,
        Continuous = 4
    };

    explicit PulseChannel(bool isPulse1 = true)
        : isPulse1Channel_(isPulse1)
    {
        reset();
    }

    void reset() noexcept
    {
        phase_ = 0.0f;
        timerPeriod_ = 0;
        frequency_ = 440.0f;
        amplitude_ = 15; // 0 to 15
        dutyMode_ = DutyMode::Duty50_0;
        continuousDuty_ = 0.5f;
        
        // Sweep Unit
        sweepEnabled_ = false;
        sweepPeriod_ = 0;
        sweepShift_ = 0;
        sweepNegate_ = false;
        sweepDivider_ = 0;
        sweepReload_ = false;

        // Envelope Unit
        constantVolume_ = true;
        envelopeLoop_ = false;
        envelopePeriod_ = 15;
        envelopeDivider_ = 0;
        decayCounter_ = 15;
        envelopeStartFlag_ = false;

        muted_ = false;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
        rateRatio_ = 44100.0f / sampleRate_;
    }

    void setFrequency(float freqHz) noexcept
    {
        frequency_ = std::max(10.0f, std::min(freqHz, sampleRate_ * 0.49f));
        // Compute 11-bit timer equivalent: f = fCPU / (16 * (t + 1))
        // => t = (fCPU / (16 * f)) - 1
        constexpr float fCPU = 1789773.0f; // NTSC 2A03 CPU clock
        float t = (fCPU / (16.0f * frequency_)) - 1.0f;
        timerPeriod_ = std::clamp(static_cast<int>(std::round(t)), 0, 2047);
        checkSweepMute();
    }

    void setTimerPeriod(int period) noexcept
    {
        timerPeriod_ = std::clamp(period, 0, 2047);
        constexpr float fCPU = 1789773.0f;
        frequency_ = fCPU / (16.0f * static_cast<float>(timerPeriod_ + 1));
        checkSweepMute();
    }

    [[nodiscard]] int getTimerPeriod() const noexcept { return timerPeriod_; }
    [[nodiscard]] float getFrequency() const noexcept { return frequency_; }

    void setDutyMode(DutyMode mode) noexcept
    {
        dutyMode_ = mode;
    }

    void setContinuousDuty(float duty) noexcept
    {
        continuousDuty_ = std::clamp(duty, 0.005f, 0.995f);
    }

    [[nodiscard]] float getActiveDuty() const noexcept
    {
        switch (dutyMode_)
        {
            case DutyMode::Duty12_5: return 0.125f;
            case DutyMode::Duty25_0: return 0.250f;
            case DutyMode::Duty50_0: return 0.500f;
            case DutyMode::Duty75_0: return 0.750f;
            case DutyMode::Continuous: return continuousDuty_;
        }
        return 0.5f;
    }

    void setVolume(int vol) noexcept
    {
        amplitude_ = std::clamp(vol, 0, 15);
        constantVolume_ = true;
    }

    void setEnvelopeParameters(bool constantVol, bool loop, int period) noexcept
    {
        constantVolume_ = constantVol;
        envelopeLoop_ = loop;
        envelopePeriod_ = std::clamp(period, 0, 15);
        if (constantVolume_)
            amplitude_ = envelopePeriod_;
    }

    void triggerEnvelope() noexcept
    {
        envelopeStartFlag_ = true;
    }

    void setSweepParameters(bool enabled, int period, int shift, bool negate) noexcept
    {
        sweepEnabled_ = enabled;
        sweepPeriod_ = std::clamp(period, 0, 7);
        sweepShift_ = std::clamp(shift, 0, 7);
        sweepNegate_ = negate;
        sweepReload_ = true;
        checkSweepMute();
    }

    /**
     * Ticked by APU Frame Sequencer at 120 Hz (half-frame).
     */
    void clockSweep() noexcept
    {
        if (sweepDivider_ == 0 && sweepEnabled_ && sweepShift_ > 0 && !muted_)
        {
            int target = computeSweepTarget();
            if (target <= 2047 && timerPeriod_ >= 8)
            {
                setTimerPeriod(target);
            }
        }

        if (sweepDivider_ == 0 || sweepReload_)
        {
            sweepDivider_ = sweepPeriod_;
            sweepReload_ = false;
        }
        else
        {
            sweepDivider_--;
        }
        checkSweepMute();
    }

    /**
     * Ticked by APU Frame Sequencer at 240 Hz (quarter-frame).
     */
    void clockEnvelope() noexcept
    {
        if (envelopeStartFlag_)
        {
            envelopeStartFlag_ = false;
            decayCounter_ = 15;
            envelopeDivider_ = envelopePeriod_;
        }
        else
        {
            if (envelopeDivider_ == 0)
            {
                envelopeDivider_ = envelopePeriod_;
                if (decayCounter_ > 0)
                {
                    decayCounter_--;
                }
                else if (envelopeLoop_)
                {
                    decayCounter_ = 15;
                }
            }
            else
            {
                envelopeDivider_--;
            }
        }

        if (!constantVolume_)
            amplitude_ = decayCounter_;
    }

    /**
     * Advances the pulse oscillator by one sample and returns its raw 4-bit output (0 to 15).
     * PolyBLEP provides sub-sample bandlimited interpolation.
     */
    float processSample() noexcept
    {
        if (muted_ || timerPeriod_ < 8 || frequency_ > sampleRate_ * 0.49f)
            return 0.0f;

        float dt = frequency_ / sampleRate_;
        float duty = getActiveDuty();

        // PolyBLEP pulse in [-1.0, +1.0]
        // Authentic 2A03 Duty 3 (75%) is the bitwise inversion of Duty 1 (25%)
        float blep = (dutyMode_ == DutyMode::Duty75_0)
            ? -PolyBLEP::pulse(phase_, dt, 0.25f)
            : PolyBLEP::pulse(phase_, dt, duty);

        // Advance phase
        phase_ += dt;
        if (phase_ >= 1.0f)
            phase_ -= 1.0f;

        // Map [-1.0, 1.0] to [0.0, 1.0]
        float normPulse = 0.5f * (blep + 1.0f);

        // Scale by 4-bit channel amplitude (0..15)
        return normPulse * static_cast<float>(amplitude_);
    }

private:
    int computeSweepTarget() const noexcept
    {
        int delta = timerPeriod_ >> sweepShift_;
        if (sweepNegate_)
        {
            // Hardware quirk: Pulse 1 subtracts (delta + 1), Pulse 2 subtracts delta
            return isPulse1Channel_ ? (timerPeriod_ - delta - 1) : (timerPeriod_ - delta);
        }
        return timerPeriod_ + delta;
    }

    void checkSweepMute() noexcept
    {
        if (timerPeriod_ < 8)
        {
            muted_ = true;
            return;
        }
        int target = computeSweepTarget();
        muted_ = (target > 2047);
    }

    bool isPulse1Channel_{ true };
    float sampleRate_{ 44100.0f };
    float rateRatio_{ 1.0f };

    float phase_{ 0.0f };
    float frequency_{ 440.0f };
    int timerPeriod_{ 0 };
    int amplitude_{ 15 };

    DutyMode dutyMode_{ DutyMode::Duty50_0 };
    float continuousDuty_{ 0.5f };
    bool muted_{ false };

    // Sweep Unit
    bool sweepEnabled_{ false };
    int sweepPeriod_{ 0 };
    int sweepShift_{ 0 };
    bool sweepNegate_{ false };
    int sweepDivider_{ 0 };
    bool sweepReload_{ false };

    // Envelope Unit
    bool constantVolume_{ true };
    bool envelopeLoop_{ false };
    int envelopePeriod_{ 15 };
    int envelopeDivider_{ 0 };
    int decayCounter_{ 15 };
    bool envelopeStartFlag_{ false };
};

} // namespace KindnesDSP
