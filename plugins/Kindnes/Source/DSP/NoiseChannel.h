#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Ricoh 2A03 Noise Channel.
 * Features:
 * - 15-bit Linear Feedback Shift Register (LFSR)
 * - Mode 0: 32,767-step pseudo-random white noise (bit 1 ^ bit 0)
 * - Mode 1: 93-step short metallic looping buzz (bit 6 ^ bit 0)
 * - 16 Standard NTSC timer periods (clocked by CPU timer)
 * - Modern continuous pitch tuning mode
 * - 4-bit volume envelope (0..15)
 */
class NoiseChannel
{
public:
    // 16 Standard 2A03 NTSC timer periods (CPU cycles per shift)
    static constexpr uint16_t kNoisePeriodsNTSC[16] = {
        4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068
    };

    NoiseChannel()
    {
        reset();
    }

    void reset() noexcept
    {
        shiftRegister_ = 1; // Initialized to 1 in hardware
        mode_ = 0;          // 0 = 32767 steps, 1 = 93 steps metallic
        periodIndex_ = 0;
        phaseAccumulator_ = 0.0f;
        amplitude_ = 15;
        continuousPitch_ = false;
        customFrequency_ = 1000.0f;

        // Envelope
        constantVolume_ = true;
        envelopeLoop_ = false;
        envelopePeriod_ = 15;
        envelopeDivider_ = 0;
        decayCounter_ = 15;
        envelopeStartFlag_ = false;

        enabled_ = true;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
    }

    void setMode(int mode) noexcept
    {
        mode_ = (mode != 0) ? 1 : 0;
    }

    [[nodiscard]] int getMode() const noexcept { return mode_; }

    void setPeriodIndex(int index) noexcept
    {
        periodIndex_ = std::clamp(index, 0, 15);
        continuousPitch_ = false;
    }

    void setContinuousFrequency(float freqHz) noexcept
    {
        continuousPitch_ = true;
        customFrequency_ = std::clamp(freqHz, 10.0f, 100000.0f);
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

    void setEnabled(bool enabled) noexcept
    {
        enabled_ = enabled;
        if (!enabled)
            gated_ = false;
    }

    void gate(bool active) noexcept
    {
        gated_ = active;
        if (active)
        {
            envelopeStartFlag_ = true;
            decayCounter_ = 15;
            envelopeDivider_ = envelopePeriod_;
        }
    }

    /**
     * Ticked by Frame Sequencer at 240 Hz.
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
                else
                {
                    // Envelope ended
                    gated_ = false;
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
     * Advances the noise generator and returns raw 4-bit output (0 to 15).
     */
    float processSample() noexcept
    {
        if (!enabled_ || !gated_ || amplitude_ == 0)
            return 0.0f;

        // Shift clock rate in Hz:
        constexpr float fCPU = 1789773.0f;
        float shiftRateHz = continuousPitch_ ? customFrequency_
                                             : (fCPU / static_cast<float>(kNoisePeriodsNTSC[periodIndex_]));

        float phaseStep = shiftRateHz / sampleRate_;

        // Dn-FamiTracker / nsfplay sub-cycle time-weighted LFSR averaging (accum / accum_clocks)
        // Eliminates point-sampling aliasing at high noise shift rates (periods 0..3 up to 447 kHz)
        // and guarantees consistent spectral density between 44.1 kHz and 96 kHz.
        float rem = phaseStep;
        float accum = 0.0f;
        float currentBit = ((shiftRegister_ & 1u) == 0u) ? 1.0f : 0.0f;

        int safetyCount = 0;
        while (phaseAccumulator_ + rem >= 1.0f && safetyCount < 64)
        {
            float dt = 1.0f - phaseAccumulator_;
            accum += currentBit * dt;
            rem -= dt;
            phaseAccumulator_ = 0.0f;
            stepLFSR();
            currentBit = ((shiftRegister_ & 1u) == 0u) ? 1.0f : 0.0f;
            ++safetyCount;
        }

        accum += currentBit * rem;
        phaseAccumulator_ += rem;
        if (phaseAccumulator_ >= 1.0f)
            phaseAccumulator_ -= std::floor(phaseAccumulator_);

        float avgGate = (phaseStep > 1e-6f) ? (accum / phaseStep) : currentBit;
        return avgGate * static_cast<float>(amplitude_);
    }

private:
    void stepLFSR() noexcept
    {
        // Mode 0: feedback = bit 0 ^ bit 1
        // Mode 1: feedback = bit 0 ^ bit 6
        uint16_t tapBit = (mode_ == 0) ? ((shiftRegister_ >> 1) & 1)
                                       : ((shiftRegister_ >> 6) & 1);
        uint16_t feedback = (shiftRegister_ & 1) ^ tapBit;
        shiftRegister_ = (shiftRegister_ >> 1) | (feedback << 14);
    }

    float sampleRate_{ 44100.0f };
    uint16_t shiftRegister_{ 1 };
    int mode_{ 0 };
    int periodIndex_{ 0 };
    float phaseAccumulator_{ 0.0f };
    int amplitude_{ 15 };

    bool continuousPitch_{ false };
    float customFrequency_{ 1000.0f };

    bool constantVolume_{ true };
    bool envelopeLoop_{ false };
    int envelopePeriod_{ 15 };
    int envelopeDivider_{ 0 };
    int decayCounter_{ 15 };
    bool envelopeStartFlag_{ false };
    bool enabled_{ true };
    bool gated_{ false };
};

} // namespace KindnesDSP
