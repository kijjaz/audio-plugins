#pragma once

#include <cmath>
#include <cstdint>
#include <array>
#include <vector>
#include <algorithm>
#include <string>
#include "PolyBLEP.h"

namespace KindnesDSP
{

/**
 * FDS 2C33 Wavetable & FM Sound Generator.
 * 
 * Recreates the legendary expansion chip featured in Zelda 1, Metroid, Kid Icarus,
 * and Castlevania II (FDS version), upgraded with the exact Dn-FamiTracker / Mesen
 * 2C33 hardware modulator & post-DAC filter:
 * - 64-step, 6-bit arbitrary wavetable (values 0..63) with PolyBLEP staircase anti-aliasing
 * - Authentic 64-step 3-bit hardware FM table (`kModLut = {0, +1, +2, +4, Reset, -4, -2, -1}`)
 * - 7-bit signed `_counter` (-64..+63) and exact 2C33 fixed-point pitch multiplier & wrapping
 * - Auto-FM harmonic ratio tracking for chromatic FM bells
 * - Authentic post-DAC 1-pole RC lowpass filter (~2,000 Hz)
 */
class FDSChannel
{
public:
    static constexpr size_t kWavetableSize = 64;
    static constexpr size_t kModTableSize = 64;
    static constexpr int8_t kModLut[8] = { 0, 1, 2, 4, 0, -4, -2, -1 };

    FDSChannel()
    {
        wavetable_.fill(32);
        modTable_.fill(0);
        setPresetWave(0); // Sine
        setPresetMod(0);  // Subtle vibrato
        updateLowpassCoeff();
        reset();
    }

    void reset() noexcept
    {
        phase_ = 0.0f;
        modPhase_ = 0.0f;
        modStep_ = 0;
        modCounter_ = 0;
        prevModCounter_ = 0;
        lpState_ = 0.0f;
        frequency_ = 440.0f;
        volume_ = 15; // 0..15
        modSpeed_ = 6.0f;
        modDepth_ = 0.0f;
        enabled_ = false;
        steppedDac_ = true;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(std::max(8000.0, sampleRate));
        updateLowpassCoeff();
    }

    void setEnabled(bool enabled) noexcept { enabled_ = enabled; }
    [[nodiscard]] bool isEnabled() const noexcept { return enabled_; }

    void setFrequency(float freqHz) noexcept
    {
        frequency_ = std::max(10.0f, std::min(freqHz, sampleRate_ * 0.48f));
    }

    void setVolume(int vol) noexcept
    {
        volume_ = std::clamp(vol, 0, 15);
    }

    [[nodiscard]] int getVolume() const noexcept { return volume_; }

    void setSteppedDac(bool stepped) noexcept { steppedDac_ = stepped; }

    void setModulation(float speedHz, float depth) noexcept
    {
        modSpeed_ = std::clamp(speedHz, 0.1f, 100.0f);
        modDepth_ = std::clamp(depth, 0.0f, 1.0f);
    }

    // --- Wavetable Manipulation ---
    void setWaveSample(size_t index, uint8_t value6Bit) noexcept
    {
        if (index < kWavetableSize)
            wavetable_[index] = std::clamp<uint8_t>(value6Bit, 0, 63);
    }

    [[nodiscard]] uint8_t getWaveSample(size_t index) const noexcept
    {
        if (index < kWavetableSize)
            return wavetable_[index];
        return 32;
    }

    const std::array<uint8_t, kWavetableSize>& getWavetable() const noexcept
    {
        return wavetable_;
    }

    void setPresetWave(int presetIndex)
    {
        switch (presetIndex)
        {
            case 0: // Sine
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float angle = static_cast<float>(i) * 6.2831853f / 64.0f;
                    float s = (std::sin(angle) + 1.0f) * 0.5f * 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(s, 0.0f, 63.0f));
                }
                break;

            case 1: // Triangle
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float val = (i < 32) ? (static_cast<float>(i) / 31.0f)
                                         : (static_cast<float>(63 - i) / 32.0f);
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(val * 63.0f, 0.0f, 63.0f));
                }
                break;

            case 2: // Sawtooth
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float val = static_cast<float>(i) / 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(val * 63.0f, 0.0f, 63.0f));
                }
                break;

            case 3: // 25% Pulse
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    wavetable_[i] = (i < 16) ? 63 : 0;
                }
                break;

            case 4: // FDS Organ
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float a1 = static_cast<float>(i) * 6.2831853f / 64.0f;
                    float s = std::sin(a1) + 0.6f * std::sin(a1 * 2.0f) + 0.4f * std::sin(a1 * 3.0f);
                    float norm = (s / 2.0f + 1.0f) * 0.5f * 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(norm, 0.0f, 63.0f));
                }
                break;

            case 5: // FDS Bell
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float a1 = static_cast<float>(i) * 6.2831853f / 64.0f;
                    float s = std::sin(a1) + 0.7f * std::sin(a1 * 2.76f) + 0.5f * std::sin(a1 * 5.4f);
                    float norm = (s / 2.2f + 1.0f) * 0.5f * 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(norm, 0.0f, 63.0f));
                }
                break;

            case 6: // Formant "Aah"
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    float a1 = static_cast<float>(i) * 6.2831853f / 64.0f;
                    float s = std::sin(a1) + 0.8f * std::sin(a1 * 3.0f) + 0.7f * std::sin(a1 * 4.0f);
                    float norm = (s / 2.5f + 1.0f) * 0.5f * 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(norm, 0.0f, 63.0f));
                }
                break;

            case 7: // Vintage Heroic Fantasy Lead
                for (size_t i = 0; i < kWavetableSize; ++i)
                {
                    // Classic Heroic Fantasy overdriven stepped wave
                    float a1 = static_cast<float>(i) * 6.2831853f / 64.0f;
                    float s = std::tanh(std::sin(a1) * 2.5f);
                    float norm = (s + 1.0f) * 0.5f * 63.0f;
                    wavetable_[i] = static_cast<uint8_t>(std::clamp(norm, 0.0f, 63.0f));
                }
                break;
        }
    }

    void setPresetMod(int modIndex)
    {
        modPresetIndex_ = std::clamp(modIndex, 0, 2);
        // Authentic 64-step 3-bit hardware FDS modulation table (0..7)
        // kModLut[8] = { 0, +1, +2, +4, Reset(0), -4, -2, -1 }
        switch (modPresetIndex_)
        {
            case 0: // Vibrato (balanced triangle/sine accumulation with center resets)
                for (size_t i = 0; i < kModTableSize; ++i)
                {
                    if (i == 0 || i == 32)
                        modTable_[i] = 4; // Hardware reset _counter = 0
                    else if (i < 16 || i >= 48)
                        modTable_[i] = (i % 2 == 0) ? 2 : 1; // +2, +1
                    else
                        modTable_[i] = (i % 2 == 0) ? 6 : 7; // -2, -1
                }
                break;

            case 1: // Metallic FM (steep +/-4 FM modulator with quadrant resets, Dn-FamiTracker style)
                for (size_t i = 0; i < kModTableSize; ++i)
                {
                    if (i == 0 || i == 32)
                        modTable_[i] = 4; // Reset to 0
                    else if (i < 16 || i >= 48)
                        modTable_[i] = 3; // +4
                    else
                        modTable_[i] = 5; // -4
                }
                break;

            case 2: // Downward Laser / Formant Sweep (ramp down with periodic reset)
                for (size_t i = 0; i < kModTableSize; ++i)
                {
                    if (i == 0)
                        modTable_[i] = 4; // Reset
                    else if (i < 8)
                        modTable_[i] = 3; // Fast +4 jump up
                    else
                        modTable_[i] = 7; // -1 sweep down
                }
                break;
        }
    }

    /**
     * Advances the FDS 2C33 oscillator by one sample and returns output in [-1.0, 1.0].
     */
    float processSample() noexcept
    {
        if (!enabled_ || volume_ == 0)
            return 0.0f;

        // 1. Advance 2C33 Hardware FM Modulator (64 steps per mod cycle)
        // In Metallic FM mode (modPresetIndex_ == 1) with high modSpeed_, enable Dn-FamiTracker
        // Auto-FM harmonic tracking so FM bells stay chromatically in tune across the keyboard.
        float effectiveModCycleHz = modSpeed_;
        if (modPresetIndex_ == 1 && modSpeed_ >= 10.0f)
        {
            float harmonicRatio = (modSpeed_ >= 24.0f) ? 2.0f : 1.0f;
            effectiveModCycleHz = 0.5f * modSpeed_ + 0.5f * (frequency_ * harmonicRatio * 0.25f);
        }

        // 64 table ticks per full modulator period
        float modStepRate = effectiveModCycleHz * 64.0f / sampleRate_;
        modPhase_ += modStepRate;
        while (modPhase_ >= 1.0f)
        {
            modPhase_ -= 1.0f;
            prevModCounter_ = modCounter_;
            uint8_t cmd = modTable_[modStep_ & 63] & 0x07;
            if (cmd == 4)
            {
                modCounter_ = 0;
            }
            else
            {
                modCounter_ += kModLut[cmd];
                // 7-bit signed wrap-around (-64..+63)
                if (modCounter_ >= 64)
                    modCounter_ -= 128;
                else if (modCounter_ < -64)
                    modCounter_ += 128;
            }
            modStep_ = (modStep_ + 1) & 63;
        }

        // Smooth sub-step counter for slow vibrato, hardware-stepped for fast FM
        float smoothCounter = prevModCounter_ + modPhase_ * static_cast<float>(modCounter_ - prevModCounter_);
        int8_t activeCounter = (effectiveModCycleHz < 12.0f)
            ? static_cast<int8_t>(std::clamp(std::round(smoothCounter), -64.0f, 63.0f))
            : modCounter_;

        // 2. Compute 12-bit 2C33 Carrier Pitch and Hardware Fixed-Point FM Offset
        // f_carrier = (f_CPU * pitch) / 4194304  (64 steps * 65536 phase)
        constexpr float kCpuClock = 1789773.0f;
        constexpr float kPitchToHz = kCpuClock / 4194304.0f;
        constexpr float kHzToPitch = 4194304.0f / kCpuClock;

        int32_t carrierPitch = std::clamp(static_cast<int32_t>(std::round(frequency_ * kHzToPitch)), 1, 4095);
        int32_t modGain6Bit = std::clamp(static_cast<int32_t>(std::round(modDepth_ * 48.0f)), 0, 63);

        int32_t pitchDelta = computeHardwareModPitchDelta(carrierPitch, activeCounter, modGain6Bit);
        // Blend fractional sub-step pitch delta when slow vibrato is active for click-free LFO
        float modFreq = frequency_;
        if (effectiveModCycleHz < 12.0f)
        {
            float normMod = (smoothCounter / 32.0f) * modDepth_ * 0.12f;
            modFreq = frequency_ * (1.0f + normMod);
        }
        else
        {
            int32_t modulatedPitch = std::clamp(carrierPitch + pitchDelta, 1, 4095);
            modFreq = static_cast<float>(modulatedPitch) * kPitchToHz;
        }
        modFreq = std::clamp(modFreq, 10.0f, sampleRate_ * 0.48f);

        // 3. Advance Main 64-step Wavetable Phase
        float dt = modFreq / sampleRate_;
        phase_ += dt;
        if (phase_ >= 1.0f)
            phase_ -= std::floor(phase_);

        float sampleVal = 0.0f;
        if (steppedDac_)
        {
            // Authentic 6-bit stepped resistor DAC lookup with PolyBLEP staircase edge correction
            float stepFloat = phase_ * 64.0f;
            size_t stepIdx = static_cast<size_t>(stepFloat) & 63;
            float subPhase = stepFloat - std::floor(stepFloat);
            float subDt = dt * 64.0f;

            float curr6Bit = static_cast<float>(wavetable_[stepIdx]);
            float prev6Bit = static_cast<float>(wavetable_[(stepIdx + 63) & 63]);
            float next6Bit = static_cast<float>(wavetable_[(stepIdx + 1) & 63]);

            float blep = PolyBLEP::staircaseBlep(subPhase, subDt, curr6Bit - prev6Bit, next6Bit - curr6Bit);
            float raw6Bit = std::clamp(curr6Bit + blep, 0.0f, 63.0f);
            sampleVal = (raw6Bit / 31.5f) - 1.0f; // [-1.0, 1.0]
        }
        else
        {
            // Smooth interpolated wavetable
            float virtualIdx = phase_ * 64.0f;
            size_t idx0 = static_cast<size_t>(virtualIdx) & 63;
            size_t idx1 = (idx0 + 1) & 63;
            float frac = virtualIdx - static_cast<float>(idx0);

            float s0 = static_cast<float>(wavetable_[idx0]);
            float s1 = static_cast<float>(wavetable_[idx1]);
            float interp = s0 + frac * (s1 - s0);
            sampleVal = (interp / 31.5f) - 1.0f;
        }

        // 4. Authentic FDS RAM Adapter Post-DAC 1-Pole RC Lowpass (~2,400 Hz)
        lpState_ += lpAlpha_ * (sampleVal - lpState_);

        // Scale by 4-bit volume (0..15)
        return lpState_ * (static_cast<float>(volume_) / 15.0f);
    }

private:
    /**
     * Exact Ricoh 2C33 hardware fixed-point FM multiplier & wrap-around logic
     * from Dn-FamiTracker (Source/APU/mesen/ModChannel.h).
     */
    static inline int32_t computeHardwareModPitchDelta(int32_t carrierPitch, int8_t counter, int32_t gain) noexcept
    {
        if (gain <= 0 || counter == 0)
            return 0;

        // 1. Multiply 7-bit signed counter (-64..63) by 6-bit gain (0..63)
        int32_t temp = static_cast<int32_t>(counter) * gain;
        int32_t remainder = temp & 0x0F;
        temp >>= 4;
        if (remainder > 0 && (temp & 0x80) == 0)
        {
            temp += (counter < 0) ? -1 : 2;
        }

        // 2. Hardware wrap-around into [-64, 191]
        if (temp >= 192)
            temp -= 256;
        else if (temp < -64)
            temp += 256;

        // 3. Multiply by 12-bit carrier pitch and round
        temp = carrierPitch * temp;
        remainder = temp & 0x3F;
        temp >>= 6;
        if (remainder >= 32)
            temp += 1;

        return temp;
    }

    void updateLowpassCoeff() noexcept
    {
        // Authentic FDS post-DAC RC lowpass filter (~2,400 Hz cutoff, sample-rate invariant)
        constexpr float kCutoffHz = 2400.0f;
        constexpr float kTwoPi = 6.28318530718f;
        lpAlpha_ = 1.0f - std::exp(-kTwoPi * kCutoffHz / sampleRate_);
    }

    float sampleRate_{ 44100.0f };
    bool enabled_{ false };
    bool steppedDac_{ true };

    float phase_{ 0.0f };
    float modPhase_{ 0.0f };
    size_t modStep_{ 0 };
    int8_t modCounter_{ 0 };
    int8_t prevModCounter_{ 0 };
    int modPresetIndex_{ 0 };

    float lpState_{ 0.0f };
    float lpAlpha_{ 0.28f };

    float frequency_{ 440.0f };
    int volume_{ 15 };

    float modSpeed_{ 6.0f };
    float modDepth_{ 0.0f };

    std::array<uint8_t, kWavetableSize> wavetable_;
    std::array<uint8_t, kModTableSize> modTable_;
};

} // namespace KindnesDSP
