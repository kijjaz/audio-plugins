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

        customDrumBodyEnv_ = 0.0f;
        customDrumNoiseEnv_ = 0.0f;
        customDrumTriPhase_ = 0.0f;
        customDrumNoiseTimer_ = 0;
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
     * Triggers one of the 32 Drum & Percussion voices combining 1-Bit DPCM sample
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
            case 0: // 01. Punch Bass Drum
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
            case 20: // 21. Dramatic 1-Bit Timpani
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

    struct CustomDrumParams
    {
        int designMode{ 0 }; // 0 = Easy, 1 = Flexible
        int templateId{ 1 };

        // Easy Macros
        float macroPunch{ 0.7f };
        float macroBody{ 0.5f };
        float macroSnappy{ 0.4f };
        float macroGrit{ 0.2f };

        // Flexible / Deep Controls: Tonal Body & Pitch Modulation
        int pitchCurve{ 0 }; // 0=ExponentialDrop, 1=DualStageDrop, 2=LaserLinearSweep, 3=PitchScoopRise, 4=FMChirpModulation
        float pitchTimeMs{ 60.0f };
        float fmRateHz{ 120.0f };
        float fmDepthHz{ 150.0f };

        float bodyStartPitchHz{ 215.0f };
        float bodyEndPitchHz{ 47.0f };
        float bodyDecayMs{ 120.0f };
        int bodyWaveform{ 0 }; // 0=2A03 Triangle, 1=Pure Sine, 2=VRC6 Saw Overdrive, 3=Pulse 25%
        float bodyLevel{ 0.95f };

        // Transient Click Layer
        int clickType{ 0 }; // 0=Off, 1=2A03 Impulse Spike, 2=Square Pop, 3=Noise Snap
        float clickLevel{ 0.4f };

        // Noise Layer
        int noiseMode{ 0 };    // 0=Mode 0 White, 1=Mode 1 Metallic 93-Step
        int noisePeriod{ 3 };  // 0..15 Tone rate
        float noiseDecayMs{ 80.0f };
        int noisePattern{ 0 }; // 0=Single Burst, 1=Multi-Flam Clap, 2=Gated Snare, 3=Low-Rumble Swell, 4=Glitch Stutter
        float noiseLevel{ 0.5f };

        // 1-Bit DPCM Hardware Crusher & Blend
        int dpcmDeltaStep{ 2 };
        int dpcmRateIndex{ 15 };
        float dpcmGrit{ 0.2f };
        float dpcmBlend{ 0.5f };
        int dpcmSampleVoice{ 0 };

        // MIDI DAW Integration
        int midiMode{ 1 }; // 0=Audition Button Only, 1=Trigger on MIDI Note 36 (C1 Kick) & 38 (D1 Snare), 2=Tuned Chromatic Synth
    };

    void setCustomDrumParams(const CustomDrumParams& p) noexcept { customDrum_ = p; }
    [[nodiscard]] const CustomDrumParams& getCustomDrumParams() const noexcept { return customDrum_; }

    void applyCustomDrumTemplate(int templateIdx)
    {
        customDrum_.templateId = templateIdx;
        switch (templateIdx)
        {
            case 1: // 01. Punch Kick (Dual-Stage Click Dive)
                customDrum_.pitchCurve = 1; // DualStageDrop
                customDrum_.pitchTimeMs = 45.0f;
                customDrum_.clickType = 1; // Impulse Spike
                customDrum_.clickLevel = 0.5f;
                customDrum_.bodyStartPitchHz = 240.0f; customDrum_.bodyEndPitchHz = 46.0f; customDrum_.bodyDecayMs = 130.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.98f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 3; customDrum_.noiseDecayMs = 35.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.40f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.25f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 0;
                break;
            case 2: // 02. Mega Snare
                customDrum_.pitchCurve = 0; // ExponentialDrop
                customDrum_.pitchTimeMs = 30.0f;
                customDrum_.clickType = 2; // Square Pop
                customDrum_.clickLevel = 0.4f;
                customDrum_.bodyStartPitchHz = 190.0f; customDrum_.bodyEndPitchHz = 95.0f; customDrum_.bodyDecayMs = 90.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.75f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 2; customDrum_.noiseDecayMs = 180.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.85f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.15f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 1;
                break;
            case 3: // 03. Closed Hi-Hat
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 10.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 40.0f; customDrum_.bodyEndPitchHz = 40.0f; customDrum_.bodyDecayMs = 10.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.0f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 0; customDrum_.noiseDecayMs = 35.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.85f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.6f; customDrum_.dpcmSampleVoice = 2;
                break;
            case 4: // 04. Open Hi-Hat
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 10.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 40.0f; customDrum_.bodyEndPitchHz = 40.0f; customDrum_.bodyDecayMs = 10.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.0f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 1; customDrum_.noiseDecayMs = 280.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.90f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.6f; customDrum_.dpcmSampleVoice = 3;
                break;
            case 5: // 05. 8-Bit Hand Clap
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 20.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 120.0f; customDrum_.bodyEndPitchHz = 60.0f; customDrum_.bodyDecayMs = 30.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.2f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 3; customDrum_.noiseDecayMs = 160.0f;
                customDrum_.noisePattern = 1; customDrum_.noiseLevel = 0.95f; // 3-Burst Multi-Flam
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.2f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 4;
                break;
            case 6: // 06. High Rack Tom
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 50.0f;
                customDrum_.clickType = 1;
                customDrum_.clickLevel = 0.3f;
                customDrum_.bodyStartPitchHz = 330.0f; customDrum_.bodyEndPitchHz = 92.0f; customDrum_.bodyDecayMs = 150.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.95f;
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 4; customDrum_.noiseDecayMs = 25.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.25f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.4f; customDrum_.dpcmSampleVoice = 5;
                break;
            case 7: // 07. Low Floor Tom
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 70.0f;
                customDrum_.clickType = 1;
                customDrum_.clickLevel = 0.35f;
                customDrum_.bodyStartPitchHz = 210.0f; customDrum_.bodyEndPitchHz = 55.0f; customDrum_.bodyDecayMs = 220.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.98f;
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 4; customDrum_.noiseDecayMs = 30.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.25f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.4f; customDrum_.dpcmSampleVoice = 6;
                break;
            case 8: // 08. Arcade Crash Cymbal
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 10.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 50.0f; customDrum_.bodyEndPitchHz = 50.0f; customDrum_.bodyDecayMs = 10.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.0f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 2; customDrum_.noiseDecayMs = 650.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.95f;
                customDrum_.dpcmDeltaStep = 3; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.3f;
                customDrum_.dpcmBlend = 0.7f; customDrum_.dpcmSampleVoice = 7;
                break;
            case 9: // 09. VRC6 Overdrive Kick
                customDrum_.pitchCurve = 1; // DualStage
                customDrum_.pitchTimeMs = 45.0f;
                customDrum_.clickType = 2; // Square Pop
                customDrum_.clickLevel = 0.5f;
                customDrum_.bodyStartPitchHz = 320.0f; customDrum_.bodyEndPitchHz = 42.0f; customDrum_.bodyDecayMs = 160.0f;
                customDrum_.bodyWaveform = 2; customDrum_.bodyLevel = 0.98f; // VRC6 Saw Overdrive
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 2; customDrum_.noiseDecayMs = 40.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.55f;
                customDrum_.dpcmDeltaStep = 3; customDrum_.dpcmRateIndex = 14; customDrum_.dpcmGrit = 0.4f;
                customDrum_.dpcmBlend = 0.6f; customDrum_.dpcmSampleVoice = 8;
                break;
            case 10: // 10. Metallic Robo-Snare
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 30.0f;
                customDrum_.clickType = 3; // Noise Snap
                customDrum_.clickLevel = 0.4f;
                customDrum_.bodyStartPitchHz = 200.0f; customDrum_.bodyEndPitchHz = 88.0f; customDrum_.bodyDecayMs = 100.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.75f;
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 4; customDrum_.noiseDecayMs = 170.0f; // 93-step metallic
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.95f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.25f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 9;
                break;
            case 11: // 11. Chiptune Rimshot
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 15.0f;
                customDrum_.clickType = 1;
                customDrum_.clickLevel = 0.6f;
                customDrum_.bodyStartPitchHz = 520.0f; customDrum_.bodyEndPitchHz = 240.0f; customDrum_.bodyDecayMs = 35.0f;
                customDrum_.bodyWaveform = 1; customDrum_.bodyLevel = 0.85f; // Saturated Sine click
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 1; customDrum_.noiseDecayMs = 25.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.60f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.15f;
                customDrum_.dpcmBlend = 0.4f; customDrum_.dpcmSampleVoice = 10;
                break;
            case 12: // 12. Arcade Cowbell
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 10.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 540.0f; customDrum_.bodyEndPitchHz = 540.0f; customDrum_.bodyDecayMs = 110.0f;
                customDrum_.bodyWaveform = 3; customDrum_.bodyLevel = 0.75f; // Dual tone / Pulse
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 3; customDrum_.noiseDecayMs = 30.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.35f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 11;
                break;
            case 13: // 13. FDS FM Simmons Tom
                customDrum_.pitchCurve = 4; // FM Chirp Modulation
                customDrum_.pitchTimeMs = 70.0f;
                customDrum_.fmRateHz = 180.0f;
                customDrum_.fmDepthHz = 110.0f;
                customDrum_.clickType = 1;
                customDrum_.clickLevel = 0.3f;
                customDrum_.bodyStartPitchHz = 270.0f; customDrum_.bodyEndPitchHz = 78.0f; customDrum_.bodyDecayMs = 220.0f;
                customDrum_.bodyWaveform = 1; customDrum_.bodyLevel = 0.92f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 4; customDrum_.noiseDecayMs = 30.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.20f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 12;
                break;
            case 14: // 14. Gated Boss Battle Snare
                customDrum_.pitchCurve = 0;
                customDrum_.pitchTimeMs = 35.0f;
                customDrum_.clickType = 2;
                customDrum_.clickLevel = 0.45f;
                customDrum_.bodyStartPitchHz = 210.0f; customDrum_.bodyEndPitchHz = 95.0f; customDrum_.bodyDecayMs = 110.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.80f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 3; customDrum_.noiseDecayMs = 260.0f;
                customDrum_.noisePattern = 2; customDrum_.noiseLevel = 0.95f; // Gated Plateau
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.2f;
                customDrum_.dpcmBlend = 0.5f; customDrum_.dpcmSampleVoice = 13;
                break;
            case 15: // 15. Laser Zapper FX
                customDrum_.pitchCurve = 2; // LaserLinearSweep
                customDrum_.pitchTimeMs = 80.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 1800.0f; customDrum_.bodyEndPitchHz = 75.0f; customDrum_.bodyDecayMs = 95.0f;
                customDrum_.bodyWaveform = 3; customDrum_.bodyLevel = 0.95f; // 25% Pulse
                customDrum_.noiseMode = 1; customDrum_.noisePeriod = 0; customDrum_.noiseDecayMs = 20.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.35f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.2f;
                customDrum_.dpcmBlend = 0.3f; customDrum_.dpcmSampleVoice = 26;
                break;
            case 16: // 16. Boss Defeat Cataclysm (2.6s Extended Low Rumble)
                customDrum_.pitchCurve = 1; // DualStage
                customDrum_.pitchTimeMs = 180.0f;
                customDrum_.clickType = 3; // Noise Snap
                customDrum_.clickLevel = 0.6f;
                customDrum_.bodyStartPitchHz = 110.0f; customDrum_.bodyEndPitchHz = 24.0f; customDrum_.bodyDecayMs = 1600.0f;
                customDrum_.bodyWaveform = 0; customDrum_.bodyLevel = 0.98f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 13; customDrum_.noiseDecayMs = 2400.0f;
                customDrum_.noisePattern = 3; customDrum_.noiseLevel = 0.98f; // LowRumbleSwell
                customDrum_.dpcmDeltaStep = 4; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.6f;
                customDrum_.dpcmBlend = 0.7f; customDrum_.dpcmSampleVoice = 21;
                break;
            case 17: // 17. Comic Spring Boing (Rising Pitch Scoop)
                customDrum_.pitchCurve = 3; // PitchScoopRise
                customDrum_.pitchTimeMs = 160.0f;
                customDrum_.clickType = 0;
                customDrum_.clickLevel = 0.0f;
                customDrum_.bodyStartPitchHz = 90.0f; customDrum_.bodyEndPitchHz = 460.0f; customDrum_.bodyDecayMs = 280.0f;
                customDrum_.bodyWaveform = 3; customDrum_.bodyLevel = 0.92f;
                customDrum_.noiseMode = 0; customDrum_.noisePeriod = 2; customDrum_.noiseDecayMs = 15.0f;
                customDrum_.noisePattern = 0; customDrum_.noiseLevel = 0.20f;
                customDrum_.dpcmDeltaStep = 2; customDrum_.dpcmRateIndex = 15; customDrum_.dpcmGrit = 0.1f;
                customDrum_.dpcmBlend = 0.3f; customDrum_.dpcmSampleVoice = 28;
                break;
            default:
                break;
        }
    }

    void triggerCustomDrum(float velocity = 1.0f, float noteFreqMult = 1.0f) noexcept
    {
        velocity = std::clamp(velocity, 0.05f, 1.0f);
        noteFreqMult = std::clamp(noteFreqMult, 0.1f, 10.0f);

        // Apply Easy Macros if in Easy Mode
        float startPitch = customDrum_.bodyStartPitchHz;
        float endPitch = customDrum_.bodyEndPitchHz;
        float bodyDecay = customDrum_.bodyDecayMs;
        float bodyLevel = customDrum_.bodyLevel;
        float noiseLvl = customDrum_.noiseLevel;
        float noiseDecay = customDrum_.noiseDecayMs;
        int deltaStep = customDrum_.dpcmDeltaStep;
        float dpcmGrit = customDrum_.dpcmGrit;

        if (customDrum_.designMode == 0) // Easy Mode
        {
            startPitch = 60.0f + customDrum_.macroPunch * 500.0f;
            endPitch = 30.0f + (1.0f - customDrum_.macroPunch) * 70.0f;
            bodyDecay = 40.0f + customDrum_.macroBody * 360.0f;
            bodyLevel = 0.95f;
            noiseLvl = customDrum_.macroSnappy;
            noiseDecay = 20.0f + customDrum_.macroSnappy * 280.0f;
            deltaStep = 1 + static_cast<int>(customDrum_.macroGrit * 5.0f);
            dpcmGrit = customDrum_.macroGrit;
        }

        // Tonal Body & Pitch State
        customDrumStartPitchFinal_ = startPitch * noteFreqMult;
        customDrumEndPitchFinal_ = endPitch * noteFreqMult;
        customDrumTriFreq_ = customDrumStartPitchFinal_;
        customDrumTriTargetFreq_ = customDrumEndPitchFinal_;

        float bDecaySec = std::max(0.005f, bodyDecay * 0.001f);
        customDrumBodyDecayCoeff_ = std::exp(-1.0f / (bDecaySec * static_cast<float>(sampleRate_)));
        customDrumBodyEnv_ = bodyLevel * velocity;
        customDrumTriPhase_ = 0.0f;

        // Transient Click Initialization
        customDrumClickCountdown_ = 0;
        customDrumClickVal_ = 0.0f;
        if (customDrum_.clickType > 0 && customDrum_.clickLevel > 0.005f)
        {
            if (customDrum_.clickType == 1) // 2A03 Impulse Spike (2 samples)
                customDrumClickCountdown_ = 2;
            else if (customDrum_.clickType == 2) // Square Pop (32 samples)
                customDrumClickCountdown_ = static_cast<int>(0.0005 * sampleRate_);
            else if (customDrum_.clickType == 3) // Noise Snap (48 samples)
                customDrumClickCountdown_ = static_cast<int>(0.0008 * sampleRate_);
        }

        // Noise
        noise_.setMode(customDrum_.noiseMode);
        noise_.setPeriodIndex(customDrum_.noisePeriod);
        float nDecaySec = std::max(0.005f, noiseDecay * 0.001f);
        customDrumNoiseDecayCoeff_ = std::exp(-1.0f / (nDecaySec * static_cast<float>(sampleRate_)));
        customDrumNoiseEnv_ = noiseLvl * velocity;
        customDrumNoiseTimer_ = 0;
        customDrumTimerSamples_ = 0;

        // 1-Bit DPCM Layer
        if (customDrum_.dpcmBlend > 0.01f)
        {
            dpcm_.setDeltaStep(deltaStep);
            dpcm_.setRateIndex(customDrum_.dpcmRateIndex);
            dpcm_.setVelToSlew(dpcmGrit);
            dpcm_.triggerPad(customDrum_.dpcmSampleVoice, velocity * customDrum_.dpcmBlend);
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

        // 0. Custom Drum Designer Direct MIDI Integration
        if (customDrum_.midiMode == 1) // Note 36 (C1 Kick) & Note 38 (D1 Snare)
        {
            if (noteNumber == 36 || noteNumber == 38)
            {
                triggerCustomDrum(velocity, 1.0f);
                return;
            }
        }
        else if (customDrum_.midiMode == 2) // Tuned Chromatic Synth across keyboard
        {
            // Note 60 (C4) is unity tuning (1.0x). Note 36 (C1) = 0.25x for fat 808 sub bass!
            float noteRatio = std::pow(2.0f, static_cast<float>(noteNumber - 60) / 12.0f);
            triggerCustomDrum(velocity, noteRatio);
            return;
        }

        // 1A. Pure Drum Kit Mode: EVERY octave on the keyboard (C..B) plays the 8 Drum Pads!
        if (isPureDrumKitMode())
        {
            int pad = mapMidiNoteToDrumPad(noteNumber);
            triggerHybridDrumPad(pad, velocity);

            auto it = std::find(heldNotes_.begin(), heldNotes_.end(), noteNumber);
            if (it == heldNotes_.end())
                heldNotes_.push_back(noteNumber);

            // When Arpeggiator is enabled on a Drum Kit preset, holding a key runs the 16th-note 8-Bit Beat!
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

            // Built-in 16th-Note Hybrid 8-Bit Drum Beat Sequencer (when Demo Beat or Key-Held Drum Beat is active)
            if (dpcm_.isDemoBeatEnabled() || keyHeldDrumBeat_)
            {
                float stepsPerSec = static_cast<float>(std::max(60.0, hostBpm_) * (4.0 / 60.0)); // 16th notes
                drumBeatPhase_ += stepsPerSec / static_cast<float>(sampleRate_);
                if (drumBeatPhase_ >= 1.0f)
                {
                    drumBeatPhase_ -= 1.0f;
                    int bankBase = getActiveDrumBank() * 8;
                    // Classic 16-step Arcade Drum Groove (Kick, Closed Hat, Open Hat, Snare, Clap, High/Low Tom, Crash):
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

            // Custom Algorithmic Drum Noise Layer
            if (customDrumNoiseEnv_ > 0.001f)
            {
                float patternGain = 1.0f;
                customDrumNoiseTimer_++;
                if (customDrum_.noisePattern == 1) // Multi-Flam Clap (3 micro bursts)
                {
                    float tMs = (static_cast<float>(customDrumNoiseTimer_) / static_cast<float>(sampleRate_)) * 1000.0f;
                    if (tMs < 14.0f) patternGain = 1.0f - (tMs / 14.0f) * 0.7f;
                    else if (tMs < 28.0f) patternGain = 1.0f - ((tMs - 14.0f) / 14.0f) * 0.7f;
                    else if (tMs < 42.0f) patternGain = 1.0f - ((tMs - 28.0f) / 14.0f) * 0.5f;
                }
                else if (customDrum_.noisePattern == 2) // Gated Snare Plateau
                {
                    float tMs = (static_cast<float>(customDrumNoiseTimer_) / static_cast<float>(sampleRate_)) * 1000.0f;
                    if (tMs < 45.0f) patternGain = 1.0f;
                    else patternGain = std::exp(-(tMs - 45.0f) * 0.15f);
                }
                else if (customDrum_.noisePattern == 3) // Low-Rumble Swell (Sub-bass rumble for dramatic explosions)
                {
                    float t = static_cast<float>(customDrumNoiseTimer_) / static_cast<float>(sampleRate_);
                    float swell = std::sin(std::min(1.0f, t / 0.15f) * 1.570796f);
                    patternGain = swell * 1.1f;
                }
                else if (customDrum_.noisePattern == 4) // Glitch Frame Stutter
                {
                    float tMs = (static_cast<float>(customDrumNoiseTimer_) / static_cast<float>(sampleRate_)) * 1000.0f;
                    int frame = static_cast<int>(tMs / 18.0f);
                    patternGain = ((frame % 2) == 0) ? 1.0f : 0.18f;
                }
                noise_.gate(true);
                noise_.setVolume(15);
                drumNoiseBoost += noise_.processSample() * (customDrumNoiseEnv_ * patternGain);
                customDrumNoiseEnv_ *= customDrumNoiseDecayCoeff_;
                if (customDrumNoiseEnv_ <= 0.001f)
                {
                    customDrumNoiseEnv_ = 0.0f;
                    if (activeKeyCount_ == 0 && drumNoiseEnv_ <= 0.001f)
                        noise_.gate(false);
                }
            }

            float noiseSample = drumNoiseBoost;
            if (noiseVol_ > 0 && activeVoiceCount > 0 && drumNoiseEnv_ <= 0.001f && customDrumNoiseEnv_ <= 0.001f)
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

            // Custom Algorithmic Drum Tonal Body (Multi-Curve Pitch Modulation Engine)
            if (customDrumBodyEnv_ > 0.001f)
            {
                customDrumTimerSamples_++;
                float tMs = (static_cast<float>(customDrumTimerSamples_) / static_cast<float>(sampleRate_)) * 1000.0f;

                switch (customDrum_.pitchCurve)
                {
                    case 0: // Exponential Drop (classic punch)
                    {
                        float factor = std::exp(-tMs / std::max(1.0f, customDrum_.pitchTimeMs));
                        customDrumTriFreq_ = customDrumEndPitchFinal_ + (customDrumStartPitchFinal_ - customDrumEndPitchFinal_) * factor;
                        break;
                    }
                    case 1: // Dual-Stage Drop (steep 12ms click dive, followed by body decay)
                    {
                        if (tMs < 12.0f)
                        {
                            float factor = std::exp(-tMs / 3.5f);
                            customDrumTriFreq_ = (customDrumStartPitchFinal_ * 0.5f) + (customDrumStartPitchFinal_ * 0.5f) * factor;
                        }
                        else
                        {
                            float factor = std::exp(-(tMs - 12.0f) / std::max(1.0f, customDrum_.pitchTimeMs));
                            customDrumTriFreq_ = customDrumEndPitchFinal_ + ((customDrumStartPitchFinal_ * 0.5f) - customDrumEndPitchFinal_) * factor;
                        }
                        break;
                    }
                    case 2: // Laser Linear Sweep (constant frequency glide)
                    {
                        float sweepDur = std::max(5.0f, customDrum_.pitchTimeMs);
                        if (tMs < sweepDur)
                        {
                            float alpha = tMs / sweepDur;
                            float sF = std::max(10.0f, customDrumStartPitchFinal_);
                            float eF = std::max(10.0f, customDrumEndPitchFinal_);
                            customDrumTriFreq_ = sF * std::pow(eF / sF, alpha);
                        }
                        else
                        {
                            customDrumTriFreq_ = customDrumEndPitchFinal_;
                        }
                        break;
                    }
                    case 3: // Pitch Scoop Rise (rising pitch bend for boings / comic springs)
                    {
                        float riseDur = std::max(10.0f, customDrum_.pitchTimeMs);
                        if (tMs < riseDur)
                        {
                            float alpha = tMs / riseDur;
                            customDrumTriFreq_ = customDrumStartPitchFinal_ + (customDrumEndPitchFinal_ - customDrumStartPitchFinal_) * (1.0f - std::exp(-alpha * 3.5f));
                        }
                        else
                        {
                            customDrumTriFreq_ = customDrumEndPitchFinal_;
                        }
                        break;
                    }
                    case 4: // FM Chirp Modulation (vibrato/chirp for Simmons toms & UFO sirens)
                    {
                        float baseFactor = std::exp(-tMs / std::max(1.0f, customDrum_.pitchTimeMs));
                        float mod = std::sin((tMs * 0.001f) * customDrum_.fmRateHz * 6.2831853f) * customDrum_.fmDepthHz * baseFactor;
                        customDrumTriFreq_ = std::max(20.0f, customDrumEndPitchFinal_ + (customDrumStartPitchFinal_ - customDrumEndPitchFinal_) * baseFactor + mod);
                        break;
                    }
                    default:
                        customDrumTriFreq_ = customDrumEndPitchFinal_;
                        break;
                }

                customDrumTriPhase_ += customDrumTriFreq_ / static_cast<float>(sampleRate_);
                if (customDrumTriPhase_ >= 1.0f)
                    customDrumTriPhase_ -= std::floor(customDrumTriPhase_);

                float waveOut = 0.0f;
                switch (customDrum_.bodyWaveform)
                {
                    case 0: // 2A03 32-step Triangle
                    {
                        int stepIdx = static_cast<int>(customDrumTriPhase_ * 32.0f) & 31;
                        waveOut = static_cast<float>(TriangleChannel::kTriangleTable[stepIdx]);
                        break;
                    }
                    case 1: // Saturated Sine
                        waveOut = (std::tanh(std::sin(customDrumTriPhase_ * 6.2831853f) * 2.2f) * 0.5f + 0.5f) * 15.0f;
                        break;
                    case 2: // VRC6 Saw Overdrive
                    {
                        int step7 = static_cast<int>(customDrumTriPhase_ * 7.0f) % 7;
                        waveOut = static_cast<float>(((step7 * 50) & 0xFF) >> 3);
                        break;
                    }
                    case 3: // 2A03 25% Pulse
                        waveOut = (customDrumTriPhase_ < 0.25f) ? 15.0f : 0.0f;
                        break;
                }

                // Transient Click Layer Injection
                float clickSample = 0.0f;
                if (customDrumClickCountdown_ > 0)
                {
                    customDrumClickCountdown_--;
                    if (customDrum_.clickType == 1) // 2A03 Impulse Spike
                        clickSample = 15.0f;
                    else if (customDrum_.clickType == 2) // Square Pop
                        clickSample = ((customDrumClickCountdown_ % 4) < 2) ? 15.0f : -15.0f;
                    else if (customDrum_.clickType == 3) // Noise Snap
                        clickSample = noise_.processSample() * 15.0f;
                    clickSample *= customDrum_.clickLevel;
                }

                float totalBody = std::clamp(waveOut * customDrumBodyEnv_ + clickSample, -15.0f, 15.0f);
                sumTri = std::max(sumTri, totalBody);
                customDrumBodyEnv_ *= customDrumBodyDecayCoeff_;
                if (customDrumBodyEnv_ <= 0.001f)
                    customDrumBodyEnv_ = 0.0f;
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

    // Advanced Drum Sound Design Engine
    CustomDrumParams customDrum_;
    float customDrumTriFreq_{ 215.0f };
    float customDrumTriTargetFreq_{ 47.0f };
    float customDrumStartPitchFinal_{ 215.0f };
    float customDrumEndPitchFinal_{ 47.0f };
    float customDrumTriPhase_{ 0.0f };
    float customDrumBodyEnv_{ 0.0f };
    float customDrumBodyDecayCoeff_{ 0.998f };
    int customDrumBodyWave_{ 0 };
    float customDrumNoiseEnv_{ 0.0f };
    float customDrumNoiseDecayCoeff_{ 0.998f };
    int customDrumNoiseTimer_{ 0 };
    int customDrumNoisePattern_{ 0 };
    int customDrumTimerSamples_{ 0 };
    int customDrumClickCountdown_{ 0 };
    float customDrumClickVal_{ 0.0f };
};

} // namespace KindnesDSP
