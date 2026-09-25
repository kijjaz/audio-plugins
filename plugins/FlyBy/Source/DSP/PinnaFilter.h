#pragma once
#include <cmath>
#include <algorithm>
#include "SpatialMath.h"

namespace FlyBy::DSP {

/**
 * Standard 2nd-order Direct Form II Transposed Biquad Filter.
 * Used for dynamic pinna spectral notches and torso comb filtering.
 */
class BiquadFilter {
public:
    BiquadFilter() = default;

    void reset() {
        s1 = s2 = 0.0f;
    }

    void setNotch(float frequencyHz, float q, float sampleRate) {
        frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.48f);
        q = std::clamp(q, 0.1f, 20.0f);

        float w0 = 2.0f * PI * (frequencyHz / sampleRate);
        float cosW0 = std::cos(w0);
        float sinW0 = std::sin(w0);
        float alpha = sinW0 / (2.0f * q);

        float a0 = 1.0f + alpha;
        b0 = 1.0f / a0;
        b1 = (-2.0f * cosW0) / a0;
        b2 = 1.0f / a0;
        a1 = (-2.0f * cosW0) / a0;
        a2 = (1.0f - alpha) / a0;
    }

    void setPeaking(float frequencyHz, float gainDb, float q, float sampleRate) {
        frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.48f);
        q = std::clamp(q, 0.1f, 20.0f);

        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * PI * (frequencyHz / sampleRate);
        float cosW0 = std::cos(w0);
        float sinW0 = std::sin(w0);
        float alpha = sinW0 / (2.0f * q);

        float a0 = 1.0f + alpha / A;
        b0 = (1.0f + alpha * A) / a0;
        b1 = (-2.0f * cosW0) / a0;
        b2 = (1.0f - alpha * A) / a0;
        a1 = (-2.0f * cosW0) / a0;
        a2 = (1.0f - alpha / A) / a0;
    }

    void setHighShelf(float frequencyHz, float gainDb, float sampleRate) {
        frequencyHz = std::clamp(frequencyHz, 20.0f, sampleRate * 0.48f);
        float A = std::pow(10.0f, gainDb / 40.0f);
        float w0 = 2.0f * PI * (frequencyHz / sampleRate);
        float cosW0 = std::cos(w0);
        float sinW0 = std::sin(w0);
        float alpha = sinW0 * 0.5f * std::sqrt(2.0f);

        float a0 = (A + 1.0f) - (A - 1.0f) * cosW0 + 2.0f * std::sqrt(A) * alpha;
        b0 = (A * ((A + 1.0f) + (A - 1.0f) * cosW0 + 2.0f * std::sqrt(A) * alpha)) / a0;
        b1 = (-2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosW0)) / a0;
        b2 = (A * ((A + 1.0f) + (A - 1.0f) * cosW0 - 2.0f * std::sqrt(A) * alpha)) / a0;
        a1 = (2.0f * ((A - 1.0f) - (A + 1.0f) * cosW0)) / a0;
        a2 = ((A + 1.0f) - (A - 1.0f) * cosW0 - 2.0f * std::sqrt(A) * alpha) / a0;
    }

    inline float process(float in) {
        float out = b0 * in + s1;
        s1 = b1 * in - a1 * out + s2;
        s2 = b2 * in - a2 * out;
        return out;
    }

private:
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;
    float s1 = 0.0f, s2 = 0.0f;
};

/**
 * PinnaFilter
 * Responsible for creating the vertical perception (elevation cues):
 * 1. Primary Pinna Notch (N1): sweeps from ~6.2 kHz (looking down/below) up to ~11.5 kHz (zenith overhead)
 * 2. Secondary High-Q Notch (N2): ~9.5 kHz - 13.5 kHz, sharpens as elevation reaches zenith
 * 3. Overhead Presence Shelf: subtle high-frequency openness as elevation rises above horizon
 * 4. Torso/Clavicle Reflection Notch: appears when source is below horizon (0 to -40 deg) around 800 Hz - 1.3 kHz
 * 5. Pinna Scale Calibration: allows fine-tuning frequency bounds to match listener's unique ear geometry (+/- 25%)
 */
class PinnaFilter {
public:
    PinnaFilter() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        reset();
    }

    void reset() {
        notchN1.reset();
        notchN2.reset();
        zenithPresence.reset();
        torsoComb.reset();
        rearPinnaShadow.reset();
        conchaBoost.reset();
        lastEl = -999.0f;
        lastFrontBack = -999.0f;
        lastScale = -999.0f;
        lastStrength = -999.0f;
        lastNotchMult = -999.0f;
    }

    /**
     * @param elevationDeg: -90 to +90 degrees
     * @param frontBackFactor: +1.0 = directly in front, -1.0 = directly behind
     * @param pinnaScale: 0.75 to 1.25 (1.0 = standard anatomical model)
     * @param elevationStrength: 0.0 to 1.0 (depth/intensity of pinna notch cues)
     * @param notchDepthMultiplier: 0.0 to 1.0 (dynamically modulated by TransientPreserver)
     */
    void update(float elevationDeg, float frontBackFactor = 1.0f, float pinnaScale = 1.0f,
                float elevationStrength = 1.0f, float notchDepthMultiplier = 1.0f) {
        // Fast-path change threshold check: skip costly trig & biquad coefficient calculation if virtually unchanged
        if (std::abs(elevationDeg - lastEl) < 0.05f &&
            std::abs(frontBackFactor - lastFrontBack) < 0.01f &&
            std::abs(pinnaScale - lastScale) < 0.005f &&
            std::abs(elevationStrength - lastStrength) < 0.005f &&
            std::abs(notchDepthMultiplier - lastNotchMult) < 0.01f) {
            return;
        }

        lastEl = elevationDeg;
        lastFrontBack = frontBackFactor;
        lastScale = pinnaScale;
        lastStrength = elevationStrength;
        lastNotchMult = notchDepthMultiplier;

        // Clamp elevation to realistic human listener range: -50 deg (below feet) to +90 deg (overhead)
        float elClamped = std::clamp(elevationDeg, -50.0f, 90.0f);
        
        // Normalized elevation factor: 0.0 at -50 deg, ~0.35 at horizon (0 deg), 1.0 at +90 deg
        float normEl = (elClamped + 50.0f) / 140.0f;

        // 1. Primary Notch (N1):
        // At -50 deg: ~6.0 kHz
        // At 0 deg (eye level): ~7.5 kHz
        // At +90 deg (overhead): ~11.2 kHz
        float n1Freq = (6000.0f + normEl * 5200.0f) * pinnaScale;
        // Notch depth varies smoothly with strength and transient multiplier
        float effectiveStrength = elevationStrength * notchDepthMultiplier;
        float n1GainDb = -18.0f * effectiveStrength;
        float n1Q = 5.0f;
        notchN1.setPeaking(n1Freq, n1GainDb, n1Q, sampleRate);

        // 2. Secondary Notch (N2):
        // Becomes pronounced especially at positive elevations (+20 to +90 deg)
        float overheadFactor = std::clamp(elClamped / 90.0f, 0.0f, 1.0f);
        float n2Freq = (9800.0f + overheadFactor * 3200.0f) * pinnaScale;
        float n2GainDb = (-14.0f * overheadFactor) * effectiveStrength;
        float n2Q = 6.5f;
        notchN2.setPeaking(n2Freq, n2GainDb, n2Q, sampleRate);

        // 3. Overhead Presence Shelf (Air / direct ear aperture exposure):
        // Sounds overhead have unobstructed line-of-sight to the superior helix of the pinna
        float shelfGainDb = (overheadFactor * 3.5f) * elevationStrength;
        zenithPresence.setHighShelf(8500.0f * pinnaScale, shelfGainDb, sampleRate);

        // 4. Torso & Shoulder Reflection Comb:
        // When source is below eye level (el < 0), reflections from chest/torso introduce a dip at ~900 Hz - 1.4 kHz
        float belowFactor = std::clamp(-elClamped / 50.0f, 0.0f, 1.0f);
        float torsoFreq = (1200.0f - belowFactor * 350.0f);
        float torsoGainDb = (-8.0f * belowFactor) * elevationStrength;
        torsoComb.setPeaking(torsoFreq, torsoGainDb, 2.8f, sampleRate);

        // 5. Front/Back Pinna Flap Posterior Shadowing:
        // The outer ear pinna flap faces forward. Sounds arriving from behind (Y < 0, frontBackFactor < 0)
        // are heavily shadowed above 4.5 kHz by the cartilaginous flap (up to -4.5 dB).
        float rearFactor = std::clamp(-frontBackFactor, 0.0f, 1.0f);
        float rearCutoff = (4600.0f * pinnaScale);
        float rearGainDb = -4.5f * rearFactor * elevationStrength;
        rearPinnaShadow.setHighShelf(rearCutoff, rearGainDb, sampleRate);

        // 6. Concha Bowl Frontal Acoustic Resonance:
        // Sound entering directly from the front (+Y, frontBackFactor > 0) resonates inside the concha
        // bowl cavity around ~3.2 kHz with +2.2 dB boost.
        float frontFactor = std::clamp(frontBackFactor, 0.0f, 1.0f);
        float conchaFreq = (3200.0f * pinnaScale);
        float conchaGainDb = 2.2f * frontFactor * elevationStrength;
        conchaBoost.setPeaking(conchaFreq, conchaGainDb, 1.8f, sampleRate);
    }

    inline float process(float in) {
        float x = notchN1.process(in);
        x = notchN2.process(x);
        x = zenithPresence.process(x);
        x = torsoComb.process(x);
        x = rearPinnaShadow.process(x);
        x = conchaBoost.process(x);
        return x;
    }

private:
    float sampleRate = 44100.0f;
    BiquadFilter notchN1;
    BiquadFilter notchN2;
    BiquadFilter zenithPresence;
    BiquadFilter torsoComb;
    BiquadFilter rearPinnaShadow;
    BiquadFilter conchaBoost;

    float lastEl = -999.0f;
    float lastFrontBack = -999.0f;
    float lastScale = -999.0f;
    float lastStrength = -999.0f;
    float lastNotchMult = -999.0f;
};

} // namespace FlyBy::DSP
