#pragma once

#include <cmath>
#include <algorithm>
#include <random>
#include "MechanicalSpeaker.h" // For Biquad

namespace Speka {

/**
 * LowpassFilter: 2nd-order Butterworth low-pass filter for grille baffle damping.
 */
class LowpassFilter {
public:
    void setCutoff(double sampleRate, double cutoffHz) {
        double w0 = 2.0 * M_PI * cutoffHz / sampleRate;
        double alpha = std::sin(w0) / (2.0 * 0.70710678118);
        double cosw = std::cos(w0);

        double b0 = (1.0 - cosw) / 2.0;
        double b1 = 1.0 - cosw;
        double b2 = (1.0 - cosw) / 2.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cosw;
        double a2 = 1.0 - alpha;

        b0_ = static_cast<float>(b0 / a0);
        b1_ = static_cast<float>(b1 / a0);
        b2_ = static_cast<float>(b2 / a0);
        a1_ = static_cast<float>(a1 / a0);
        a2_ = static_cast<float>(a2 / a0);
    }

    void reset() {
        z1 = 0.0f;
        z2 = 0.0f;
    }

    inline float process(float in) {
        float out = b0_ * in + z1;
        z1 = b1_ * in - a1_ * out + z2;
        z2 = b2_ * in - a2_ * out;
        return out;
    }

private:
    float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f, a1_ = 0.0f, a2_ = 0.0f;
    float z1 = 0.0f, z2 = 0.0f;
};

/**
 * ChassisAcoustics: Simulates the metal PC enclosure cavity modes,
 * the front plastic slotted bezel acoustic impedance, and excursion-triggered
 * chassis screw & panel chatter.
 */
class ChassisAcoustics {
public:
    ChassisAcoustics()
        : rng(4242)
        , noiseDist(-1.0f, 1.0f)
    {}

    void prepare(double sampleRate) {
        fs = sampleRate;
        invFs = 1.0 / fs;
        reset();
        configureFilters();
    }

    void reset() {
        bezelFilter1.reset();
        bezelFilter2.reset();
        cavity1.reset();
        cavity2.reset();
        cavity3.reset();
        cavity4.reset();
        rattleBandpass.reset();
        rattleEnergy = 0.0f;
    }

    void setCaseAmount(float amount) {
        caseAmount = std::clamp(amount, 0.0f, 1.0f);
    }

    void setRattleAmount(float amount) {
        rattleAmount = std::clamp(amount, 0.0f, 1.0f);
    }

    void setCarrierBleed(float bleed) {
        carrierBleed = std::clamp(bleed, 0.0f, 1.0f);
    }

    /**
     * Process one sample of acoustic sound pressure through the chassis and grille.
     * @param acousticIn Raw acoustic pressure from cone
     * @param coneDisplacement Current mechanical cone position in meters
     * @param coneVelocity Current cone velocity in m/s
     * @return Final sound pressure radiating into room
     */
    float processSample(float acousticIn, float coneDisplacement, float coneVelocity) {
        // 1. Cascaded 4th-order front plastic slotted bezel filtering (acoustic low-pass screening)
        // This steeply attenuates carrier whine (>6 kHz) while preserving full musical band (up to 5.5 kHz)
        float filteredGrille = bezelFilter1.process(acousticIn);
        filteredGrille = bezelFilter2.process(filteredGrille);

        // Blend between steep acoustic case filtering and raw circuit bleed
        float grilleSound = (1.0f - carrierBleed * 0.85f) * filteredGrille + (carrierBleed * 0.85f) * acousticIn;

        // 2. Internal metal cavity standing waves
        float cav1 = cavity1.process(filteredGrille);
        float cav2 = cavity2.process(filteredGrille);
        float cav3 = cavity3.process(filteredGrille);
        float cav4 = cavity4.process(filteredGrille);

        float cavitySound = (0.35f * cav1 + 0.30f * cav2 + 0.20f * cav3 + 0.15f * cav4) * caseAmount;

        // 3. Excursion-triggered chassis rattle and panel chatter
        constexpr float xRattleThresh = 0.00032f; // 0.32 mm excursion threshold
        float absDisp = std::abs(coneDisplacement);
        float excess = std::max(0.0f, absDisp - xRattleThresh);

        // Leaky envelope follower for rattle excitation
        float alphaRattle = static_cast<float>(1.0 - std::exp(-invFs / 0.008)); // 8ms release
        rattleEnergy += alphaRattle * (excess * 2500.0f - rattleEnergy);
        rattleEnergy = std::clamp(rattleEnergy, 0.0f, 1.0f);

        float rattleSignal = 0.0f;
        if (rattleAmount > 0.001f && rattleEnergy > 0.001f) {
            float white = noiseDist(rng);
            float bpNoise = rattleBandpass.process(white);
            float direction = (coneVelocity >= 0.0f) ? 1.0f : -1.0f;
            rattleSignal = bpNoise * rattleEnergy * rattleAmount * direction * 0.15f;
        }

        // Combine direct grille sound, internal cavity reverberation, and chassis chatter
        return grilleSound + cavitySound + rattleSignal;
    }

private:
    void configureFilters() {
        if (fs <= 0.0) return;

        // Front bezel slotted grille 4th-order acoustic screening LPF (Butterworth at 5.5 kHz)
        bezelFilter1.setCutoff(fs, 5500.0);
        bezelFilter2.setCutoff(fs, 5500.0);

        // Internal steel chassis cavity resonances
        cavity1.setBandpass(fs, 185.0, 6.0);
        cavity2.setBandpass(fs, 340.0, 5.5);
        cavity3.setBandpass(fs, 520.0, 4.0);
        cavity4.setBandpass(fs, 790.0, 3.5);

        // Chassis rattle bandpass (metallic mechanical rattle spectrum 900 Hz - 3.2 kHz)
        rattleBandpass.setBandpass(fs, 1600.0, 1.2);
    }

    double fs = 44100.0;
    double invFs = 1.0 / 44100.0;

    float caseAmount = 0.6f;
    float rattleAmount = 0.35f;
    float carrierBleed = 0.08f; // Subtle authentic background whine by default (not screeching)

    LowpassFilter bezelFilter1;
    LowpassFilter bezelFilter2;
    Biquad cavity1;
    Biquad cavity2;
    Biquad cavity3;
    Biquad cavity4;

    Biquad rattleBandpass;
    float rattleEnergy = 0.0f;

    std::mt19937 rng;
    std::uniform_real_distribution<float> noiseDist;
};

} // namespace Speka
