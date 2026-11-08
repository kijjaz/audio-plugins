#pragma once

#include <cmath>
#include <algorithm>
#include <vector>

namespace KindnesDSP
{

/**
 * Chiptune Embellishments & Macro Effects:
 * 
 * Recreates the virtuoso tracker techniques developed by 1980s game sound programmers:
 * - Delayed Vibrato: Pitch vibrato blooms smoothly after note has been held
 * - Pitch Scoop / Drop: Transient attack pitch glides (e.g. -2 semitones up, +12 semitones laser dive)
 * - Duty Cycle Attack Morph: Transient duty bite (e.g. 12.5% attack -> 50% sustain body)
 * - Chiptune Pseudo-Echo: Automatic ghost note delay line for Pulse 2 with duty & volume offsets
 */
class ChiptuneEmbellishments
{
public:
    ChiptuneEmbellishments() = default;

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = std::max(8000.0, sampleRate);
        updateCoefficients();
    }

    void setScoopParameters(float scoopSemitones, float scoopTimeMs) noexcept
    {
        scoopAmountSemitones_ = scoopSemitones; // e.g. -2.0 for scoop up, +12.0 for laser dive
        scoopTimeSec_ = std::max(1.0f, scoopTimeMs) * 0.001f;
        updateCoefficients();
    }

    void setDelayedVibrato(float delayMs, float rampMs, float targetCents, float rateHz) noexcept
    {
        vibratoDelaySec_ = std::max(0.0f, delayMs) * 0.001f;
        vibratoRampSec_ = std::max(10.0f, rampMs) * 0.001f;
        vibratoTargetCents_ = targetCents;
        vibratoRateHz_ = std::clamp(rateHz, 0.5f, 20.0f);
    }

    void setDutyMorph(bool enabled, int attackDuty, int sustainDuty, int holdFrames) noexcept
    {
        dutyMorphEnabled_ = enabled;
        attackDuty_ = std::clamp(attackDuty, 0, 4);
        sustainDuty_ = std::clamp(sustainDuty, 0, 4);
        dutyHoldFrames_ = std::clamp(holdFrames, 1, 15);
    }

    void setPseudoEcho(bool enabled, int delayFrames60Hz, float echoVolumeMult, int echoDuty) noexcept
    {
        pseudoEchoEnabled_ = enabled;
        echoDelayFrames_ = std::clamp(delayFrames60Hz, 1, 24);
        echoVolMult_ = std::clamp(echoVolumeMult, 0.0f, 1.0f);
        echoDuty_ = std::clamp(echoDuty, 0, 4);
    }

    [[nodiscard]] bool isPseudoEchoEnabled() const noexcept { return pseudoEchoEnabled_; }
    [[nodiscard]] int getEchoDuty() const noexcept { return echoDuty_; }
    [[nodiscard]] float getEchoVolMult() const noexcept { return echoVolMult_; }

    void noteOn(int noteNumber, float velocity) noexcept
    {
        active_ = true;
        noteTime_ = 0.0f;
        currentScoop_ = scoopAmountSemitones_;
        vibratoPhase_ = 0.0f;
        vibratoCurrentCents_ = 0.0f;
        frameCounter_ = 0;
        samplesSinceLastFrame_ = 0;

        // Push note to pseudo-echo queue
        if (pseudoEchoEnabled_)
        {
            EchoEvent ev;
            ev.delaySamplesRemaining = static_cast<int>((echoDelayFrames_ / 60.0f) * sampleRate_);
            ev.note = noteNumber;
            ev.velocity = velocity * echoVolMult_;
            ev.active = true;
            echoQueue_.push_back(ev);
        }
    }

    void noteOff() noexcept
    {
        active_ = false;
    }

    [[nodiscard]] int getActiveDuty(int fallbackDuty) const noexcept
    {
        if (!dutyMorphEnabled_ || !active_)
            return fallbackDuty;

        return (frameCounter_ < dutyHoldFrames_) ? attackDuty_ : sustainDuty_;
    }

    [[nodiscard]] float getCurrentPitchOffsetSemitones() const noexcept
    {
        if (!active_)
            return 0.0f;

        float vibSemi = (vibratoCurrentCents_ / 100.0f) * std::sin(vibratoPhase_ * 6.2831853f);
        return currentScoop_ + vibSemi;
    }

    /**
     * Ticked every audio sample. Checks echo queue and updates pitch/vibrato envelopes.
     */
    void processSample(int& outEchoNote, float& outEchoVel, bool& outEchoTrigger) noexcept
    {
        outEchoTrigger = false;

        if (active_)
        {
            float dt = 1.0f / static_cast<float>(sampleRate_);
            noteTime_ += dt;

            // 1. Decay pitch scoop
            currentScoop_ += (0.0f - currentScoop_) * scoopDecayCoeff_;

            // 2. Delayed vibrato bloom
            if (noteTime_ > vibratoDelaySec_)
            {
                float rampProgress = (noteTime_ - vibratoDelaySec_) / vibratoRampSec_;
                rampProgress = std::clamp(rampProgress, 0.0f, 1.0f);
                vibratoCurrentCents_ = vibratoTargetCents_ * rampProgress;
                vibratoPhase_ += vibratoRateHz_ * dt;
                if (vibratoPhase_ >= 1.0f)
                    vibratoPhase_ -= 1.0f;
            }

            // 3. 60Hz frame counter for duty morphing
            samplesSinceLastFrame_++;
            if (samplesSinceLastFrame_ >= samplesPer60HzFrame_)
            {
                samplesSinceLastFrame_ -= samplesPer60HzFrame_;
                frameCounter_++;
            }
        }

        // 4. Process Pseudo-Echo queue
        for (auto& ev : echoQueue_)
        {
            if (ev.active)
            {
                ev.delaySamplesRemaining--;
                if (ev.delaySamplesRemaining <= 0)
                {
                    outEchoNote = ev.note;
                    outEchoVel = ev.velocity;
                    outEchoTrigger = true;
                    ev.active = false;
                }
            }
        }

        // Clean finished echo events
        echoQueue_.erase(std::remove_if(echoQueue_.begin(), echoQueue_.end(),
            [](const EchoEvent& e) { return !e.active; }), echoQueue_.end());
    }

private:
    void updateCoefficients() noexcept
    {
        samplesPer60HzFrame_ = static_cast<int>(sampleRate_ / 60.0);
        // Exponential decay towards target 0
        scoopDecayCoeff_ = 1.0f - std::exp(-5.0f / (scoopTimeSec_ * static_cast<float>(sampleRate_)));
    }

    struct EchoEvent
    {
        int delaySamplesRemaining{ 0 };
        int note{ 60 };
        float velocity{ 0.5f };
        bool active{ false };
    };

    double sampleRate_{ 44100.0 };
    int samplesPer60HzFrame_{ 735 };

    bool active_{ false };
    float noteTime_{ 0.0f };

    // Pitch Scoop
    float scoopAmountSemitones_{ 0.0f };
    float scoopTimeSec_{ 0.05f };
    float currentScoop_{ 0.0f };
    float scoopDecayCoeff_{ 0.01f };

    // Delayed Vibrato
    float vibratoDelaySec_{ 0.15f };
    float vibratoRampSec_{ 0.20f };
    float vibratoTargetCents_{ 0.0f };
    float vibratoRateHz_{ 5.5f };
    float vibratoPhase_{ 0.0f };
    float vibratoCurrentCents_{ 0.0f };

    // Duty Morph
    bool dutyMorphEnabled_{ false };
    int attackDuty_{ 0 };  // 12.5%
    int sustainDuty_{ 2 }; // 50%
    int dutyHoldFrames_{ 3 };
    int frameCounter_{ 0 };
    int samplesSinceLastFrame_{ 0 };

    // Pseudo-Echo
    bool pseudoEchoEnabled_{ false };
    int echoDelayFrames_{ 4 }; // ~66ms
    float echoVolMult_{ 0.5f };
    int echoDuty_{ 0 };        // Thin 12.5% ghost note
    std::vector<EchoEvent> echoQueue_;
};

} // namespace KindnesDSP
