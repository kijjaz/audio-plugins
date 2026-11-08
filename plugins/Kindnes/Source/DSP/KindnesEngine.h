#pragma once

#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <algorithm>

#include "PulseChannel.h"
#include "TriangleChannel.h"
#include "NoiseChannel.h"
#include "DPCMChannel.h"
#include "NonLinearDAC.h"
#include "AnalogFilter.h"
#include "ArpeggiatorMacro.h"
#include "ExpansionVRC6.h"
#include "FDSChannel.h"
#include "ChiptuneEmbellishments.h"
#include "ADSREnvelope.h"
#include "StateVariableFilter.h"
#include "ModulationLFO.h"

namespace KindnesDSP
{

/**
 * Kindnes Polyphonic Voice with dedicated ADSR envelope and pitch gliding.
 */
struct PolyVoice
{
    PulseChannel pulse1{ true };
    PulseChannel pulse2{ false };
    TriangleChannel triangle;
    ADSREnvelope ampEnv;
    
    bool active{ false };
    int midiNote{ -1 };
    float velocity{ 0.0f };
    float targetFreq{ 440.0f };
    float currentFreq{ 440.0f };
    uint32_t noteAge{ 0 };

    void reset()
    {
        pulse1.reset();
        pulse2.reset();
        triangle.reset();
        ampEnv.reset();
        active = false;
        midiNote = -1;
        velocity = 0.0f;
        noteAge = 0;
    }
};

/**
 * Supercharged Kindnes DSP Engine.
 */
class KindnesEngine
{
public:
    static constexpr int kMaxPolyVoices = 8;

    enum class PlayMode
    {
        ClassicAPU = 0, // Authentic monophonic split
        Polyphonic = 1, // 8-voice APU chords
        UnisonStereo = 2 // Detuned stereo APU lead/pad
    };

    KindnesEngine()
    {
        setSampleRate(44100.0);
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v.reset();

        noise_.reset();
        dpcm_.reset();
        fds_.reset();
        analogFilterL_.reset();
        analogFilterR_.reset();
        svFilterL_.reset();
        svFilterR_.reset();
        lfo1_.reset();
        lfo2_.reset();
        arp_.reset();
        vrc6_.reset();

        pitchBendSemitones_ = 0.0f;
        activeKeyCount_ = 0;
        fdsMidiNote_ = -1;
        heldNotes_.clear();
        polyScaleSmooth_ = 1.0f;
        drumNoiseEnv_ = 0.0f;
        drumTriEnv_ = 0.0f;
        drumTriPhase_ = 0.0f;
        drumTriFreq_ = 120.0f;
        keyHeldDrumBeat_ = false;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = std::max(8000.0, sampleRate);
        rateRatio_ = static_cast<float>(44100.0 / sampleRate_);

        for (auto& v : voices_)
        {
            v.pulse1.setSampleRate(sampleRate_);
            v.pulse2.setSampleRate(sampleRate_);
            v.triangle.setSampleRate(sampleRate_);
            v.ampEnv.setSampleRate(sampleRate_);
        }

        noise_.setSampleRate(sampleRate_);
        dpcm_.setSampleRate(sampleRate_);
        fds_.setSampleRate(sampleRate_);
        embellishments_.setSampleRate(sampleRate_);
        analogFilterL_.setSampleRate(sampleRate_);
        analogFilterR_.setSampleRate(sampleRate_);
        svFilterL_.setSampleRate(sampleRate_);
        svFilterR_.setSampleRate(sampleRate_);
        lfo1_.setSampleRate(sampleRate_);
        lfo2_.setSampleRate(sampleRate_);
        arp_.setSampleRate(sampleRate_);
        vrc6_.setSampleRate(sampleRate_);
    }

    // --- Voice & MIDI Control ---
    [[nodiscard]] int getActiveDrumBank() const noexcept
    {
        int idx = dpcm_.getActiveSampleIndex();
        if (idx >= 32)
            return 0;
        return std::clamp(idx / 8, 0, 3);
    }

    int mapMidiNoteToDrumPad(int noteNumber) const noexcept
    {
        int semi = ((noteNumber % 12) + 12) % 12;
        int bankBase = getActiveDrumBank() * 8;
        // Semitones 0..7 (C..G) map to the 8 pads of the active Drum Bank (bankBase + 0..7).
        // Semitones 8..11 (G#..B) map to the 4 bonus percussion sounds from the next bank (VRC6 Kick, 93-Step Snare, Rimshot, Cowbell)!
        if (semi < 8)
            return bankBase + semi;
        int nextBankBase = ((getActiveDrumBank() + 1) & 3) * 8;
        return nextBankBase + (semi - 8);
    }

    [[nodiscard]] bool isPureDrumKitMode() const noexcept
    {
        return dpcmDrumPadMode_
            && p1Vol_ == 0
            && p2Vol_ == 0
            && triVol_ == 0
            && (!fds_.isEnabled() || fds_.getVolume() == 0)
            && (!vrc6_.isEnabled() || vrc6SawVol_ == 0);
    }

    /**
     * Triggers one of the 32 NES Drum & Percussion voices combining 1-Bit DPCM sample
     * playback with authentic 2A03 Triangle pitch-dive & LFSR Noise hardware transients.
     */
    void triggerHybridDrumPad(int padIndex, float velocity = 1.0f) noexcept
    {
        padIndex = std::clamp(padIndex, 0, 31);
        velocity = std::clamp(velocity, 0.05f, 1.0f);
        dpcm_.triggerPad(padIndex, velocity);

        // Choke Open Hi-Hat noise tail when Closed Hi-Hat (2) is triggered
        if (padIndex == 2 && lastTriggeredDrumPad_ == 3)
            drumNoiseEnv_ *= 0.15f;
        lastTriggeredDrumPad_ = padIndex;

        // Layer authentic 2A03 hardware drum transients (Dn-FamiTracker style) across all 32 voices
        switch (padIndex)
        {
            case 0: // 01. Sunsoft Punch Kick
                drumTriFreq_ = 215.0f; drumTriTargetFreq_ = 47.0f; drumTriEnv_ = velocity;
                drumNoiseEnv_ = 0.45f * velocity; drumNoiseDecay_ = 0.9965f;
                noise_.setMode(0); noise_.setPeriodIndex(3);
                break;
            case 1: // 02. Mega Crisp Snare
                drumTriFreq_ = 190.0f; drumTriTargetFreq_ = 95.0f; drumTriEnv_ = 0.72f * velocity;
                drumNoiseEnv_ = velocity; drumNoiseDecay_ = 0.9990f;
                noise_.setMode(0); noise_.setPeriodIndex(2);
                break;
            case 2: // 03. Closed Hi-Hat
                drumNoiseEnv_ = 0.85f * velocity; drumNoiseDecay_ = 0.9976f;
                noise_.setMode(0); noise_.setPeriodIndex(0);
                break;
            case 3: // 04. Open Hi-Hat
                drumNoiseEnv_ = 0.90f * velocity; drumNoiseDecay_ = 0.99945f;
                noise_.setMode(0); noise_.setPeriodIndex(1);
                break;
            case 4: // 05. 8-Bit Hand Clap
                drumNoiseEnv_ = 0.95f * velocity; drumNoiseDecay_ = 0.9989f;
                noise_.setMode(0); noise_.setPeriodIndex(3);
                break;
            case 5: // 06. High Rack Tom
                drumTriFreq_ = 320.0f; drumTriTargetFreq_ = 92.0f; drumTriEnv_ = 0.95f * velocity;
                break;
            case 6: // 07. Low Floor Tom
                drumTriFreq_ = 210.0f; drumTriTargetFreq_ = 55.0f; drumTriEnv_ = 0.98f * velocity;
                break;
            case 7: // 08. Arcade Crash Cymbal
                drumNoiseEnv_ = 0.95f * velocity; drumNoiseDecay_ = 0.99965f;
                noise_.setMode(0); noise_.setPeriodIndex(2);
                break;
            case 8: // 09. VRC6 Overdriven Synth Kick
                drumTriFreq_ = 300.0f; drumTriTargetFreq_ = 42.0f; drumTriEnv_ = velocity;
                drumNoiseEnv_ = 0.55f * velocity; drumNoiseDecay_ = 0.9970f;
                noise_.setMode(1); noise_.setPeriodIndex(2);
                break;
            case 9: // 10. 93-Step Metallic Robo-Snare
                drumTriFreq_ = 200.0f; drumTriTargetFreq_ = 88.0f; drumTriEnv_ = 0.75f * velocity;
                drumNoiseEnv_ = 0.95f * velocity; drumNoiseDecay_ = 0.9989f;
                noise_.setMode(1); noise_.setPeriodIndex(4);
                break;
            case 10: // 11. Chiptune Rimshot
                drumTriFreq_ = 520.0f; drumTriTargetFreq_ = 240.0f; drumTriEnv_ = 0.85f * velocity;
                drumNoiseEnv_ = 0.6f * velocity; drumNoiseDecay_ = 0.9955f;
                noise_.setMode(1); noise_.setPeriodIndex(1);
                break;
            case 11: // 12. 8-Bit Arcade Cowbell
                drumTriFreq_ = 540.0f; drumTriTargetFreq_ = 540.0f; drumTriEnv_ = 0.65f * velocity;
                drumNoiseEnv_ = 0.35f * velocity; drumNoiseDecay_ = 0.9960f;
                noise_.setMode(1); noise_.setPeriodIndex(3);
                break;
            case 12: // 13. FDS 2C33 FM Simmons Tom
                drumTriFreq_ = 260.0f; drumTriTargetFreq_ = 72.0f; drumTriEnv_ = 0.90f * velocity;
                break;
            case 13: // 14. Gated Boss Battle Snare
                drumTriFreq_ = 210.0f; drumTriTargetFreq_ = 98.0f; drumTriEnv_ = 0.85f * velocity;
                drumNoiseEnv_ = velocity; drumNoiseDecay_ = 0.9993f;
                noise_.setMode(0); noise_.setPeriodIndex(3);
                break;
            case 14: // 15. Chiptune Shaker
                drumNoiseEnv_ = 0.80f * velocity; drumNoiseDecay_ = 0.9982f;
                noise_.setMode(0); noise_.setPeriodIndex(0);
                break;
            case 15: // 16. FDS Metallic Ride Bell
                drumNoiseEnv_ = 0.70f * velocity; drumNoiseDecay_ = 0.9992f;
                noise_.setMode(1); noise_.setPeriodIndex(0);
                break;
            case 20: // 21. Konami 1-Bit Timpani
                drumTriFreq_ = 148.0f; drumTriTargetFreq_ = 98.0f; drumTriEnv_ = 0.95f * velocity;
                break;
            case 21: // 22. 2A03 Deep Sub-Boom Kick
                drumTriFreq_ = 165.0f; drumTriTargetFreq_ = 38.0f; drumTriEnv_ = velocity;
                break;
            case 22: // 23. Slope-Overload Fuzz Snare
                drumNoiseEnv_ = 0.95f * velocity; drumNoiseDecay_ = 0.9991f;
                noise_.setMode(1); noise_.setPeriodIndex(5);
                break;
            case 29: // 30. Boss Explosion Rumble
                drumTriFreq_ = 110.0f; drumTriTargetFreq_ = 32.0f; drumTriEnv_ = 0.95f * velocity;
                drumNoiseEnv_ = velocity; drumNoiseDecay_ = 0.9996f;
                noise_.setMode(0); noise_.setPeriodIndex(12);
                break;
            default: // Samples 16..19, 23..28, 30..31: crisp transient accent
                drumNoiseEnv_ = 0.35f * velocity; drumNoiseDecay_ = 0.9975f;
                noise_.setMode(1); noise_.setPeriodIndex(4);
                break;
        }
    }

    void noteOn(int noteNumber, float velocity) noexcept
    {
        velocity = std::clamp(velocity, 0.0f, 1.0f);
        if (velocity <= 0.0f)
        {
            noteOff(noteNumber);
            return;
        }

        // 1A. Pure Drum Kit Mode: EVERY octave on the keyboard (C..B) plays the 8 NES Drum Pads!
        if (isPureDrumKitMode())
        {
            int pad = mapMidiNoteToDrumPad(noteNumber);
            triggerHybridDrumPad(pad, velocity);

            auto it = std::find(heldNotes_.begin(), heldNotes_.end(), noteNumber);
            if (it == heldNotes_.end())
                heldNotes_.push_back(noteNumber);

            // When Arpeggiator is enabled on a Drum Kit preset, holding a key runs the 16th-note NES Beat!
            if (arp_.isEnabled())
            {
                if (!keyHeldDrumBeat_)
                {
                    drumBeatPhase_ = 0.0f;
                    drumBeatStep_ = 1;
                }
                keyHeldDrumBeat_ = true;
            }
            return;
        }

        // 1B. Split Keyboard Drum Mode (when melodic oscillators are active):
        // Notes below C2 (noteNumber < 48, e.g. C1..B1) trigger the 8 Drum Pads exclusively without droning synth notes!
        if (dpcmDrumPadMode_ && noteNumber < 48)
        {
            int pad = mapMidiNoteToDrumPad(noteNumber);
            triggerHybridDrumPad(pad, velocity);
            return;
        }

        activeKeyCount_++;

        // Track held notes stack (most recent at back)
        auto it = std::find(heldNotes_.begin(), heldNotes_.end(), noteNumber);
        if (it != heldNotes_.end())
            heldNotes_.erase(it);
        heldNotes_.push_back(noteNumber);

        arp_.setHeldNotes(heldNotes_);
        arp_.trigger();

        if (dpcmChromatic_)
        {
            dpcm_.triggerNote(noteNumber, velocity);
        }

        // 2. Chiptune embellishments (attack pitch scoop, delayed vibrato trigger)
        embellishments_.noteOn(noteNumber, velocity);

        // 3. FDS 2C33 Note Tracking
        if (fds_.isEnabled())
        {
            fdsMidiNote_ = noteNumber;
            fds_.setFrequency(midiToFreq(static_cast<float>(noteNumber) + pitchBendSemitones_));
        }

        if (arp_.isEnabled())
        {
            // Dedicated Arpeggiator Voice Routing:
            // Voice 0 drives the arpeggiated stream (with pulse 2 unison/detune if enabled).
            auto& v = voices_[0];
            bool wasActive = v.active;
            v.active = true;
            v.midiNote = heldNotes_.front(); // Lowest held note acts as root anchor
            v.velocity = velocity;
            v.targetFreq = midiToFreq(static_cast<float>(v.midiNote));
            if (!wasActive || glideTime_ <= 0.001f)
                v.currentFreq = v.targetFreq;

            updateVoiceFrequencies(v);

            if (!wasActive)
            {
                if (p1Vol_ > 0) v.pulse1.triggerEnvelope();
                if (p2Vol_ > 0) v.pulse2.triggerEnvelope();
                if (triVol_ > 0) v.triangle.triggerNote();
                v.ampEnv.noteOn();
            }

            if (noiseVol_ > 0)
                noise_.gate(true);
        }
        else if (playMode_ == PlayMode::ClassicAPU)
        {
            // Mono APU routing with true Legato & Last-Note Priority Stack
            auto& v = voices_[0];
            bool wasActive = v.active;
            v.active = true;
            v.midiNote = noteNumber;
            v.velocity = velocity;
            v.targetFreq = midiToFreq(static_cast<float>(noteNumber));

            if (!wasActive)
            {
                v.currentFreq = v.targetFreq;
                updateVoiceFrequencies(v);
                if (triVol_ > 0)
                {
                    v.triangle.setFrequency(v.currentFreq);
                    v.triangle.triggerNote();
                }
                if (p1Vol_ > 0) v.pulse1.triggerEnvelope();
                if (p2Vol_ > 0) v.pulse2.triggerEnvelope();
                v.ampEnv.noteOn();
            }
            else
            {
                // Legato transition: smoothly glide without retriggering envelope
                if (glideTime_ <= 0.001f)
                {
                    v.currentFreq = v.targetFreq;
                    if (p1Vol_ > 0) v.pulse1.triggerEnvelope();
                    if (p2Vol_ > 0) v.pulse2.triggerEnvelope();
                    if (triVol_ > 0) v.triangle.triggerNote();
                }
                updateVoiceFrequencies(v);
            }

            if (noiseVol_ > 0)
                noise_.gate(true);

            if (!dpcmDrumPadMode_ && !dpcmChromatic_ && noteNumber >= 84)
            {
                if (noteNumber == 84) dpcm_.selectBuiltinSample(0); // Kick
                else if (noteNumber == 86) dpcm_.selectBuiltinSample(1); // Snare
                else if (noteNumber == 88) dpcm_.selectBuiltinSample(4); // Tom
                dpcm_.triggerNote();
            }
        }
        else
        {
            // Polyphonic / Unison allocator
            int voiceIndex = findFreeVoice(noteNumber);
            auto& v = voices_[voiceIndex];
            v.active = true;
            v.midiNote = noteNumber;
            v.velocity = velocity;
            v.noteAge = 0;
            v.targetFreq = midiToFreq(static_cast<float>(noteNumber));
            if (glideTime_ <= 0.001f)
                v.currentFreq = v.targetFreq;

            updateVoiceFrequencies(v);

            v.pulse1.triggerEnvelope();
            v.pulse2.triggerEnvelope();
            v.triangle.triggerNote();
            v.ampEnv.noteOn();

            if (noiseVol_ > 0)
                noise_.gate(true);
        }
    }

    void noteOff(int noteNumber) noexcept
    {
        if (isPureDrumKitMode())
        {
            auto it = std::find(heldNotes_.begin(), heldNotes_.end(), noteNumber);
            if (it != heldNotes_.end())
                heldNotes_.erase(it);
            if (heldNotes_.empty() && keyHeldDrumBeat_)
            {
                keyHeldDrumBeat_ = false;
                dpcm_.setDemoBeatEnabled(false);
            }
            return;
        }

        if (dpcmDrumPadMode_ && noteNumber < 48)
            return;

        if (activeKeyCount_ > 0)
            activeKeyCount_--;

        auto it = std::find(heldNotes_.begin(), heldNotes_.end(), noteNumber);
        if (it != heldNotes_.end())
            heldNotes_.erase(it);

        arp_.setHeldNotes(heldNotes_);

        if (arp_.isEnabled())
        {
            if (heldNotes_.empty())
            {
                voices_[0].ampEnv.noteOff();
                if (!useAdsrEnv_)
                {
                    voices_[0].active = false;
                    voices_[0].pulse1.setVolume(0);
                    voices_[0].pulse2.setVolume(0);
                    voices_[0].triangle.setEnabled(false);
                }
                noise_.gate(false);
                embellishments_.noteOff();
                if (fdsMidiNote_ == noteNumber)
                    fdsMidiNote_ = -1;
            }
            else
            {
                // Continue arpeggiating remaining held notes
                voices_[0].midiNote = heldNotes_.front();
                voices_[0].targetFreq = midiToFreq(static_cast<float>(voices_[0].midiNote));
                if (glideTime_ <= 0.001f)
                    voices_[0].currentFreq = voices_[0].targetFreq;
                updateVoiceFrequencies(voices_[0]);
            }
        }
        else if (playMode_ == PlayMode::ClassicAPU)
        {
            if (heldNotes_.empty())
            {
                // All notes released
                voices_[0].ampEnv.noteOff();
                if (!useAdsrEnv_)
                {
                    voices_[0].active = false;
                    voices_[0].pulse1.setVolume(0);
                    voices_[0].pulse2.setVolume(0);
                    voices_[0].triangle.setEnabled(false);
                }
                noise_.gate(false);
                embellishments_.noteOff();
                if (fdsMidiNote_ == noteNumber)
                    fdsMidiNote_ = -1;
            }
            else
            {
                // Fall back to most recently pressed held note (Last Note Priority)!
                int fallbackNote = heldNotes_.back();
                auto& v = voices_[0];
                v.midiNote = fallbackNote;
                v.targetFreq = midiToFreq(static_cast<float>(fallbackNote));
                if (glideTime_ <= 0.001f)
                    v.currentFreq = v.targetFreq;
                updateVoiceFrequencies(v);
            }
        }
        else
        {
            // Polyphonic voice release
            for (auto& v : voices_)
            {
                if (v.active && v.midiNote == noteNumber)
                {
                    v.ampEnv.noteOff();
                    if (!useAdsrEnv_)
                    {
                        v.active = false;
                        v.pulse1.setVolume(0);
                        v.pulse2.setVolume(0);
                        v.triangle.setEnabled(false);
                    }
                }
            }

            if (heldNotes_.empty() || activeKeyCount_ == 0)
            {
                noise_.gate(false);
                embellishments_.noteOff();
                if (fdsMidiNote_ == noteNumber)
                    fdsMidiNote_ = -1;
            }
        }
    }

    void setPitchBend(float semitones) noexcept
    {
        pitchBendSemitones_ = semitones;
        for (auto& v : voices_)
        {
            if (v.active)
                updateVoiceFrequencies(v);
        }
    }

    // --- Parameter Setters ---
    void setPlayMode(PlayMode mode) noexcept { playMode_ = mode; }
    void setAuthenticDAC(bool authentic) noexcept
    {
        authenticDac_ = authentic;
        for (auto& v : voices_)
            v.triangle.setSteppedDacMode(authentic);
        fds_.setSteppedDac(authentic);
    }
    
    // Analog Motherboard Filter
    void setAnalogFilterEnabled(bool enabled) noexcept
    {
        analogFilterL_.setEnabled(enabled);
        analogFilterR_.setEnabled(enabled);
    }
    void setFilterTone(float tone) noexcept
    {
        analogFilterL_.setToneShaping(tone);
        analogFilterR_.setToneShaping(tone);
    }

    // Modern Resonant SVF
    void setSvFilterParameters(int mode, float cutoffHz, float resonance) noexcept
    {
        auto svfMode = static_cast<StateVariableFilter::Mode>(std::clamp(mode, 0, 4));
        svFilterL_.setParameters(svfMode, cutoffHz, resonance);
        svFilterR_.setParameters(svfMode, cutoffHz, resonance);
    }

    // ADSR Envelope
    void setAdsrParameters(bool enabled, float attackMs, float decayMs, float sustain, float releaseMs) noexcept
    {
        useAdsrEnv_ = enabled;
        for (auto& v : voices_)
            v.ampEnv.setParameters(attackMs, decayMs, sustain, releaseMs);
    }

    // Modulation LFOs
    void setLfo1(float rateHz, float vibratoDepthCents, float pwmDepth) noexcept
    {
        lfo1_.setFrequency(rateHz);
        lfo1VibratoCents_ = vibratoDepthCents;
        lfo1PwmDepth_ = pwmDepth;
    }

    void setGlideTime(float glideMs) noexcept
    {
        glideTime_ = std::max(0.0f, glideMs) * 0.001f;
    }

    // DPCM Sampler
    void setDPCMParameters(int sampleIndex, bool chromatic, int deltaStep, int rateIndex, bool loop, bool reverse, int volume)
    {
        dpcm_.selectBuiltinSample(sampleIndex);
        dpcmChromatic_ = chromatic;
        dpcm_.setChromaticTracking(chromatic);
        dpcm_.setDeltaStep(deltaStep);
        dpcm_.setRateIndex(rateIndex);
        dpcm_.setLoop(loop);
        dpcm_.setReverse(reverse);
        dpcm_.setVolume(volume);
    }

    void setDpcmDrumPadMode(bool enabled) noexcept { dpcmDrumPadMode_ = enabled; }
    void setDpcmVelToSlew(float amount) noexcept { dpcm_.setVelToSlew(amount); }
    void setThroughDPCMEnabled(bool enabled) noexcept
    {
        throughDpcm_ = enabled;
        dpcm_.setThroughDPCMEnabled(enabled);
    }

    // Chiptune Embellishments Setters
    void setScoopParameters(float scoopSemitones, float scoopTimeMs) noexcept
    {
        embellishments_.setScoopParameters(scoopSemitones, scoopTimeMs);
    }
    void setDelayedVibrato(float delayMs, float rampMs, float targetCents, float rateHz) noexcept
    {
        embellishments_.setDelayedVibrato(delayMs, rampMs, targetCents, rateHz);
    }
    void setDutyMorph(bool enabled, int attackDuty, int sustainDuty, int holdFrames) noexcept
    {
        embellishments_.setDutyMorph(enabled, attackDuty, sustainDuty, holdFrames);
    }
    void setPseudoEcho(bool enabled, int delayFrames, float echoVol, int echoDuty) noexcept
    {
        embellishments_.setPseudoEcho(enabled, delayFrames, echoVol, echoDuty);
    }

    // FDS 2C33 Wavetable Setters
    void setFDSEnabled(bool enabled) noexcept { fds_.setEnabled(enabled); }
    void setFDSVolume(int vol) noexcept { fds_.setVolume(vol); }
    void setFDSPresetWave(int waveIndex) { fds_.setPresetWave(waveIndex); }
    void setFDSPresetMod(int modIndex) { fds_.setPresetMod(modIndex); }
    void setFDSModulation(float speedHz, float depth) noexcept { fds_.setModulation(speedHz, depth); }
    void setFDSSteppedDAC(bool stepped) noexcept { fds_.setSteppedDac(stepped); }

    FDSChannel& getFDS() noexcept { return fds_; }
    ChiptuneEmbellishments& getEmbellishments() noexcept { return embellishments_; }

    void setPulse1Duty(int dutyIndex) noexcept
    {
        pulse1DutyMode_ = static_cast<PulseChannel::DutyMode>(std::clamp(dutyIndex, 0, 4));
        for (auto& v : voices_)
            v.pulse1.setDutyMode(pulse1DutyMode_);
    }

    void setPulse2Duty(int dutyIndex) noexcept
    {
        pulse2DutyMode_ = static_cast<PulseChannel::DutyMode>(std::clamp(dutyIndex, 0, 4));
        for (auto& v : voices_)
            v.pulse2.setDutyMode(pulse2DutyMode_);
    }

    void setPulseContinuousPWM(float pwm) noexcept
    {
        continuousPWM_ = pwm;
        for (auto& v : voices_)
        {
            v.pulse1.setContinuousDuty(continuousPWM_);
            v.pulse2.setContinuousDuty(continuousPWM_);
        }
    }

    void setUnisonDetune(float cents) noexcept { unisonDetuneCents_ = cents; }
    void setStereoSpread(float spread) noexcept { stereoSpread_ = std::clamp(spread, 0.0f, 1.0f); }

    void setNoiseMode(int mode) noexcept { noise_.setMode(mode); }
    void setNoisePeriod(int period) noexcept { noise_.setPeriodIndex(period); }
    void setNoiseVolume(int vol) noexcept { noiseVol_ = std::clamp(vol, 0, 15); }
    void setTriangleVolume(int vol) noexcept { triVol_ = std::clamp(vol, 0, 15); }
    void setPulse1Volume(int vol) noexcept { p1Vol_ = std::clamp(vol, 0, 15); }
    void setPulse2Volume(int vol) noexcept { p2Vol_ = std::clamp(vol, 0, 15); }

    void setArpEnabled(bool enabled) noexcept { arp_.setEnabled(enabled); }
    void setArpRateMode(ArpeggiatorMacro::RateMode mode, double bpm = 120.0) noexcept
    {
        hostBpm_ = std::max(40.0, bpm);
        arp_.setRateMode(mode, bpm);
    }
    void setArpPattern(int patternIndex,
                       const std::vector<int>& pitches,
                       const std::vector<int>& duties = {},
                       const std::vector<int>& volumes = {})
    {
        arp_.setPattern(patternIndex, pitches, duties, volumes);
    }
    void setArpSequence(const std::vector<int>& steps) { arp_.setPitchSequence(steps); }

    void setVRC6Enabled(bool enabled) noexcept { vrc6_.setEnabled(enabled); }
    void setVRC6SawVolume(int vol) noexcept { vrc6SawVol_ = vol; }

    void loadDPCMFile(const float* pcm, int numSamples, float sr)
    {
        dpcm_.encodeAndLoadPCM(pcm, numSamples, sr);
        dpcm_.triggerNote();
    }

    DPCMChannel& getDPCM() noexcept { return dpcm_; }
    NoiseChannel& getNoise() noexcept { return noise_; }
    ArpeggiatorMacro& getArp() noexcept { return arp_; }

    /**
     * Master audio processing block with optional external audio input.
     */
    void processBlock(const float* inL, const float* inR, float* outL, float* outR, int numSamples) noexcept
    {
        if (numSamples <= 0 || outL == nullptr || outR == nullptr)
            return;

        float glideCoeff = (glideTime_ > 0.001f)
            ? (1.0f - std::exp(-1.0f / (glideTime_ * static_cast<float>(sampleRate_))))
            : 1.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            arp_.processSample();
            int arpOffset = arp_.getCurrentPitchOffset();
            bool isRestStep = arp_.isEnabled() && (arp_.isCurrentStepRest() || arpOffset == ArpeggiatorMacro::kRestOffset);
            int arpDuty = arp_.getCurrentDuty();
            int arpVol = isRestStep ? 0 : arp_.getCurrentVolume();
            float arpVolMult = arp_.isEnabled() ? (static_cast<float>(arpVol) / 15.0f) : 1.0f;

            // Chiptune Embellishments Process
            int echoNote = 60;
            float echoVel = 0.0f;
            bool echoTrigger = false;
            embellishments_.processSample(echoNote, echoVel, echoTrigger);

            if (echoTrigger)
            {
                // Trigger pseudo-echo ghost note on Pulse 2
                auto& v = voices_[0];
                v.pulse2.setFrequency(midiToFreq(static_cast<float>(echoNote)));
                v.pulse2.setDutyMode(static_cast<PulseChannel::DutyMode>(embellishments_.getEchoDuty()));
                v.pulse2.setVolume(static_cast<int>(std::round(p2Vol_ * embellishments_.getEchoVolMult())));
                v.pulse2.triggerEnvelope();
            }

            float embellishmentPitch = embellishments_.getCurrentPitchOffsetSemitones();
            int activeP1Duty = (arp_.isEnabled() && pulse1DutyMode_ != PulseChannel::DutyMode::Continuous)
                ? arpDuty
                : embellishments_.getActiveDuty(static_cast<int>(pulse1DutyMode_));

            // LFO Process
            float lfo1Val = lfo1_.processSample(); // [-1.0, 1.0]
            float vibratoRatio = (lfo1VibratoCents_ > 0.01f)
                ? std::pow(2.0f, (lfo1Val * lfo1VibratoCents_) / 1200.0f) : 1.0f;

            float dynamicPWM = std::clamp(continuousPWM_ + lfo1Val * lfo1PwmDepth_ * 0.45f, 0.02f, 0.98f);

            float sumP1_L = 0.0f, sumP1_R = 0.0f;
            float sumP2_L = 0.0f, sumP2_R = 0.0f;
            float sumTri = 0.0f;

            int activeVoiceCount = 0;
            float leadBaseFreq = 440.0f;
            float leadEnvGain = 0.0f;

            for (size_t vIdx = 0; vIdx < voices_.size(); ++vIdx)
            {
                auto& v = voices_[vIdx];
                if (!v.active)
                    continue;

                // When arp is enabled, voice 0 articulates the arpeggio stream
                if (arp_.isEnabled() && vIdx > 0)
                    continue;

                // Portamento glide
                v.currentFreq += (v.targetFreq - v.currentFreq) * glideCoeff;

                float envGain = 1.0f;
                if (useAdsrEnv_)
                {
                    envGain = v.ampEnv.getNextSample();
                    if (!v.ampEnv.isActive())
                    {
                        v.active = false;
                        continue;
                    }
                }

                activeVoiceCount++;
                v.noteAge++;

                float baseFreq = v.currentFreq;
                if (arp_.isEnabled())
                {
                    int noteToFreq = v.midiNote;
                    if (!isRestStep)
                    {
                        noteToFreq = ArpeggiatorMacro::isFixedNoteValue(arpOffset)
                            ? ArpeggiatorMacro::decodeFixedNoteValue(arpOffset)
                            : (v.midiNote + arpOffset);
                    }
                    baseFreq = midiToFreq(static_cast<float>(noteToFreq) + pitchBendSemitones_ + embellishmentPitch);
                }
                else
                {
                    baseFreq = (v.currentFreq * std::pow(2.0f, (pitchBendSemitones_ + embellishmentPitch) / 12.0f));
                }

                baseFreq *= vibratoRatio;

                if (activeVoiceCount == 1)
                {
                    leadBaseFreq = baseFreq;
                    leadEnvGain = isRestStep ? 0.0f : (envGain * arpVolMult);
                }

                v.pulse1.setFrequency(baseFreq);
                float detuneRatio = std::pow(2.0f, unisonDetuneCents_ / 1200.0f);
                v.pulse2.setFrequency(baseFreq * detuneRatio);
                v.triangle.setFrequency(baseFreq);

                v.pulse1.setDutyMode(static_cast<PulseChannel::DutyMode>(activeP1Duty));
                if (pulse1DutyMode_ == PulseChannel::DutyMode::Continuous)
                    v.pulse1.setContinuousDuty(dynamicPWM);

                v.pulse2.setDutyMode(pulse2DutyMode_);
                if (pulse2DutyMode_ == PulseChannel::DutyMode::Continuous)
                    v.pulse2.setContinuousDuty(dynamicPWM);

                v.pulse1.setVolume(p1Vol_);
                v.pulse2.setVolume(p2Vol_);

                float p1Raw = v.pulse1.processSample();
                float p2Raw = v.pulse2.processSample();
                float triRaw = (triVol_ > 0)
                    ? (v.triangle.processSample() * (static_cast<float>(triVol_) / 15.0f))
                    : 0.0f;

                float p1Sample = isRestStep ? 0.0f : (p1Raw * envGain * arpVolMult);
                float p2Sample = isRestStep ? 0.0f : (p2Raw * envGain * arpVolMult);
                float triSample = isRestStep ? 0.0f : (triRaw * envGain);

                if (playMode_ == PlayMode::UnisonStereo)
                {
                    float pan = stereoSpread_ * 0.5f;
                    sumP1_L += p1Sample * (1.0f + pan);
                    sumP1_R += p1Sample * (1.0f - pan);
                    sumP2_L += p2Sample * (1.0f - pan);
                    sumP2_R += p2Sample * (1.0f + pan);
                }
                else
                {
                    sumP1_L += p1Sample;
                    sumP1_R += p1Sample;
                    sumP2_L += p2Sample;
                    sumP2_R += p2Sample;
                }

                sumTri += triSample;
            }

            float targetScale = (activeVoiceCount > 1)
                ? (1.0f / std::sqrt(static_cast<float>(activeVoiceCount)))
                : 1.0f;
            polyScaleSmooth_ += (targetScale - polyScaleSmooth_) * 0.01f;

            sumP1_L *= polyScaleSmooth_;
            sumP1_R *= polyScaleSmooth_;
            sumP2_L *= polyScaleSmooth_;
            sumP2_R *= polyScaleSmooth_;
            sumTri *= polyScaleSmooth_;

            // Built-in 16th-Note Hybrid NES Drum Beat Sequencer (when Demo Beat or Key-Held Drum Beat is active)
            if (dpcm_.isDemoBeatEnabled() || keyHeldDrumBeat_)
            {
                float stepsPerSec = static_cast<float>(std::max(60.0, hostBpm_) * (4.0 / 60.0)); // 16th notes
                drumBeatPhase_ += stepsPerSec / static_cast<float>(sampleRate_);
                if (drumBeatPhase_ >= 1.0f)
                {
                    drumBeatPhase_ -= 1.0f;
                    int bankBase = getActiveDrumBank() * 8;
                    // Classic 16-step NES Arcade Drum Groove (Kick, Closed Hat, Open Hat, Snare, Clap, High/Low Tom, Crash):
                    static constexpr int kPrimaryPadOffset[16] = {
                        0, 2, 2, 3,  1, 2, 0, 2,  0, 2, 0, 3,  1, 5, 6, 4
                    };
                    static constexpr int kLayerPadOffset[16] = {
                        7, -1, -1, -1,  4, -1, 2, -1,  2, -1, 2, -1,  4, -1, 2, -1
                    };
                    int step = drumBeatStep_ & 15;
                    triggerHybridDrumPad(bankBase + kPrimaryPadOffset[step], 0.92f);
                    if (kLayerPadOffset[step] >= 0)
                        dpcm_.triggerPad(bankBase + kLayerPadOffset[step], 0.65f);
                    drumBeatStep_ = (drumBeatStep_ + 1) & 15;
                }
            }

            // Noise channel (combines sustained noiseVol_ shaped by voice envelope and 2A03 Drum Noise transient)
            float drumNoiseBoost = 0.0f;
            if (drumNoiseEnv_ > 0.001f)
            {
                noise_.gate(true);
                noise_.setVolume(15);
                drumNoiseBoost = noise_.processSample() * drumNoiseEnv_;
                drumNoiseEnv_ *= std::pow(drumNoiseDecay_, rateRatio_);
                if (drumNoiseEnv_ <= 0.001f)
                {
                    drumNoiseEnv_ = 0.0f;
                    if (activeKeyCount_ == 0)
                        noise_.gate(false);
                }
            }

            float noiseSample = drumNoiseBoost;
            if (noiseVol_ > 0 && activeVoiceCount > 0 && drumNoiseEnv_ <= 0.001f)
            {
                noise_.gate(true);
                noise_.setVolume(noiseVol_);
                noiseSample = noise_.processSample() * leadEnvGain;
            }

            // 2A03 Hardware Drum Triangle Pitch-Dive Transient (Kick / Tom / Laser)
            if (drumTriEnv_ > 0.001f)
            {
                drumTriFreq_ += (drumTriTargetFreq_ - drumTriFreq_) * (0.0035f * rateRatio_);
                drumTriPhase_ += drumTriFreq_ / static_cast<float>(sampleRate_);
                if (drumTriPhase_ >= 1.0f)
                    drumTriPhase_ -= std::floor(drumTriPhase_);
                int stepIdx = static_cast<int>(drumTriPhase_ * 32.0f) & 31;
                float triStep = static_cast<float>(TriangleChannel::kTriangleTable[stepIdx]);
                sumTri = std::max(sumTri, triStep * drumTriEnv_);
                drumTriEnv_ *= std::pow(0.9988f, rateRatio_);
                if (drumTriEnv_ <= 0.001f)
                    drumTriEnv_ = 0.0f;
            }

            // DPCM sampler
            float dpcmSample = dpcm_.processSample();

            // FDS 2C33 Wavetable & FM channel (tracks active voice pitch, arp, vibrato, & envelope)
            float fdsSample = 0.0f;
            if (fds_.isEnabled() && activeVoiceCount > 0 && !isRestStep)
            {
                fds_.setFrequency(leadBaseFreq);
                fdsSample = fds_.processSample() * leadEnvGain;
            }

            // Real-time Through-DPCM Processing
            float throughSampleL = 0.0f;
            float throughSampleR = 0.0f;
            if (throughDpcm_ && inL != nullptr)
            {
                throughSampleL = dpcm_.processThroughAudio(inL[i]);
                throughSampleR = (inR != nullptr) ? dpcm_.processThroughAudio(inR[i]) : throughSampleL;
            }

            // Non-linear resistive DAC mixing for Left & Right
            float mixedL = NonLinearDAC::mix(sumP1_L, sumP2_L, sumTri, noiseSample, dpcmSample, authenticDac_);
            float mixedR = NonLinearDAC::mix(sumP1_R, sumP2_R, sumTri, noiseSample, dpcmSample, authenticDac_);

            // FDS Expansion audio injection
            if (fds_.isEnabled())
            {
                mixedL += fdsSample * 0.45f;
                mixedR += fdsSample * 0.45f;
            }

            // Through-DPCM external audio injection
            if (throughDpcm_ && inL != nullptr)
            {
                mixedL += throughSampleL * 0.45f;
                mixedR += throughSampleR * 0.45f;
            }

            // VRC6 Expansion audio injection (Sawtooth + 1/16th thin pulse accompaniment)
            if (vrc6_.isEnabled() && activeVoiceCount > 0 && !isRestStep)
            {
                vrc6_.setSawtooth(leadBaseFreq, vrc6SawVol_);
                vrc6_.setPulse1(leadBaseFreq, 0, vrc6SawVol_ / 2); // 1/16 (6.25%) VRC6 pulse bite
                float vrcSample = vrc6_.processSample() * leadEnvGain;
                mixedL += vrcSample;
                mixedR += vrcSample;
            }

            // Resonant State Variable Filter (ZDF)
            float svfL = svFilterL_.processSample(mixedL);
            float svfR = svFilterR_.processSample(mixedR);

            // Motherboard Analog Filtering (HPF + LPF)
            float filteredL = analogFilterL_.processSample(svfL);
            float filteredR = analogFilterR_.processSample(svfR);

            // Master level calibration (-14.4 dBFS nominal at 0.0 dB trim)
            constexpr float kNominalMasterGain = 0.85f;
            outL[i] = filteredL * kNominalMasterGain;
            outR[i] = filteredR * kNominalMasterGain;
        }
    }

    void processBlock(float* outL, float* outR, int numSamples) noexcept
    {
        processBlock(nullptr, nullptr, outL, outR, numSamples);
    }

private:
    static inline float midiToFreq(float note) noexcept
    {
        return 440.0f * std::pow(2.0f, (note - 69.0f) / 12.0f);
    }

    void updateVoiceFrequencies(PolyVoice& v) noexcept
    {
        float totalNote = static_cast<float>(v.midiNote) + pitchBendSemitones_;
        float baseFreq = midiToFreq(totalNote);

        v.pulse1.setFrequency(baseFreq);
        float detuneRatio = std::pow(2.0f, unisonDetuneCents_ / 1200.0f);
        v.pulse2.setFrequency(baseFreq * detuneRatio);
        v.triangle.setFrequency(baseFreq);
    }

    int findFreeVoice(int noteNumber) noexcept
    {
        // 1. Same-note retrigger reuse
        for (size_t i = 0; i < voices_.size(); ++i)
        {
            if (voices_[i].active && voices_[i].midiNote == noteNumber)
                return static_cast<int>(i);
        }

        // 2. Inactive voice
        for (size_t i = 0; i < voices_.size(); ++i)
        {
            if (!voices_[i].active)
                return static_cast<int>(i);
        }

        // 3. Voice in release phase (prefer voice with lowest envelope level)
        int bestReleaseIdx = -1;
        float lowestGain = 1.0f;
        for (size_t i = 0; i < voices_.size(); ++i)
        {
            if (voices_[i].ampEnv.getState() == ADSREnvelope::State::Release)
            {
                float val = voices_[i].ampEnv.getCurrentValue();
                if (bestReleaseIdx < 0 || val < lowestGain)
                {
                    lowestGain = val;
                    bestReleaseIdx = static_cast<int>(i);
                }
            }
        }
        if (bestReleaseIdx >= 0)
            return bestReleaseIdx;

        // 4. Steal oldest sustaining voice
        int oldestIdx = 0;
        uint32_t maxAge = 0;
        for (size_t i = 0; i < voices_.size(); ++i)
        {
            if (voices_[i].noteAge > maxAge)
            {
                maxAge = voices_[i].noteAge;
                oldestIdx = static_cast<int>(i);
            }
        }
        return oldestIdx;
    }

    double sampleRate_{ 44100.0 };
    float rateRatio_{ 1.0f };

    PlayMode playMode_{ PlayMode::Polyphonic };
    bool authenticDac_{ true };
    bool useAdsrEnv_{ true };
    float glideTime_{ 0.0f };

    std::vector<int> heldNotes_;
    float polyScaleSmooth_{ 1.0f };

    std::array<PolyVoice, kMaxPolyVoices> voices_;
    NoiseChannel noise_;
    DPCMChannel dpcm_;
    FDSChannel fds_;
    ChiptuneEmbellishments embellishments_;
    AnalogFilter analogFilterL_;
    AnalogFilter analogFilterR_;
    StateVariableFilter svFilterL_;
    StateVariableFilter svFilterR_;
    ModulationLFO lfo1_;
    ModulationLFO lfo2_;
    ArpeggiatorMacro arp_;
    ExpansionVRC6 vrc6_;

    PulseChannel::DutyMode pulse1DutyMode_{ PulseChannel::DutyMode::Duty50_0 };
    PulseChannel::DutyMode pulse2DutyMode_{ PulseChannel::DutyMode::Duty25_0 };
    float continuousPWM_{ 0.5f };

    int p1Vol_{ 15 };
    int p2Vol_{ 12 };
    int triVol_{ 15 };
    int noiseVol_{ 0 };
    int vrc6SawVol_{ 0 };

    float unisonDetuneCents_{ 6.0f };
    float stereoSpread_{ 0.5f };
    float pitchBendSemitones_{ 0.0f };

    float lfo1VibratoCents_{ 0.0f };
    float lfo1PwmDepth_{ 0.0f };

    bool dpcmChromatic_{ false };
    bool dpcmDrumPadMode_{ false };
    bool throughDpcm_{ false };
    int fdsMidiNote_{ -1 };
    int activeKeyCount_{ 0 };

    // 2A03 Hardware Drum Transient & Groove State
    double hostBpm_{ 120.0 };
    float drumNoiseEnv_{ 0.0f };
    float drumNoiseDecay_{ 0.998f };
    float drumTriEnv_{ 0.0f };
    float drumTriPhase_{ 0.0f };
    float drumTriFreq_{ 120.0f };
    float drumTriTargetFreq_{ 48.0f };
    bool keyHeldDrumBeat_{ false };
    float drumBeatPhase_{ 0.0f };
    int drumBeatStep_{ 0 };
    int lastTriggeredDrumPad_{ -1 };
};

} // namespace KindnesDSP
