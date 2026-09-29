#pragma once
#include <cmath>
#include <algorithm>
#include <juce_dsp/juce_dsp.h>

/**
 * High-Voltage Bass/Guitar Tube Preamp Stage with Dynamic Power Supply Sag
 * =========================================================================
 * Electronic Modeling:
 * 1. Pre-emphasis Sub-Bass Coupling Filter (32 Hz 1st-order HP):
 *    Tightens sub-audible infrasound mud before non-linear amplification,
 *    preventing ugly intermodulation distortion while keeping punchy low-mids intact.
 * 2. Dynamic Power Supply Rail Sag (Opto-Tube Envelope Grip):
 *    Simulates high-voltage B+ power supply drop under heavy signal current.
 *    As Gain is pushed, peaks are smoothly compressed (optical/tube bloom)
 *    rather than buzzy solid clipping, making the bass sound massive, thick, and tight.
 * 3. Soft Magnetic Core & Asymmetric Triode Transfer:
 *    Even & odd harmonic richness with soft rounded saturation, providing analog
 *    body and punch without harsh high-frequency buzz.
 * 4. Model-dependent Headroom Voicing:
 *    Bass models (Bassman, Ampeg B-15N, B-100R, Super Bass) receive elevated
 *    linear headroom (+6 to +10 dB) before breakup, matching massive real-world iron.
 */
class TubeStage {
public:
    enum class AmpType {
        BassHead,    // High plate voltage, high headroom, punchy dynamic sag
        LeadGuitar   // Classic high gain, earlier clipping breakup
    };

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate;
        reset();
    }

    void reset() {
        biasDc = 0.0f;
        sagEnvelope = 0.0f;
        subFilterState = 0.0f;
        updateSubFilterCoeff();
    }

    void setAmpType(AmpType type) {
        currentType = type;
        if (currentType == AmpType::BassHead) {
            // Bass amps: Higher headroom scale (1.85x), smoother sag recovery
            baseHeadroomScale = 1.85f;
            baseSagDepth = 0.45f;
            sagAttack = 0.0025f;
            sagRelease = 0.00035f;
        } else {
            // Guitar lead amps: Closer to standard 12AX7 staging
            baseHeadroomScale = 1.0f;
            baseSagDepth = 0.25f;
            sagAttack = 0.005f;
            sagRelease = 0.001f;
        }
        updateCoefficients();
    }

    void setDrive(float driveDb) {
        driveDbParam = driveDb;
        rawDrive = std::pow(10.0f, driveDb / 20.0f);
    }

    void setTight(float tight0to10) {
        // tight = 0.0 -> loose vintage 22 Hz
        // tight = 10.0 -> ultra-tight modern 140 Hz
        tightParam = juce::jlimit(0.0f, 10.0f, tight0to10);
        updateCoefficients();
    }

    void setSag(float sag0to10) {
        // sag = 0.0 -> stiff/punchy solid rail
        // sag = 5.0 -> vintage tube rectifier sag
        // sag = 10.0 -> deep spongy tube bloom
        sagParam = juce::jlimit(0.0f, 10.0f, sag0to10);
        updateCoefficients();
    }

    /**
     * Nonlinear dynamic processing:
     */
    float processSample(float input) noexcept {
        // 1. Pre-emphasis sub-coupling filter (Tight 32Hz highpass)
        // Strips flubby DC/sub-audible rumble before non-linearity
        float hpIn = input - subFilterState;
        subFilterState += subFilterCoeff * hpIn;
        float cleanBass = 0.85f * hpIn + 0.15f * input; // Mild warm low-shelf blending

        // 2. Headroom-scaled drive
        float effectiveDrive = (rawDrive / headroomScale);
        float x = cleanBass * effectiveDrive;

        // 3. Dynamic Power Supply Rail Sag (Envelope Follower)
        float absSignal = std::abs(x);
        if (absSignal > sagEnvelope) {
            sagEnvelope += sagAttack * (absSignal - sagEnvelope);
        } else {
            sagEnvelope += sagRelease * (absSignal - sagEnvelope);
        }

        // Sag compresses effective rail swing dynamically
        // As you drive harder, rails smoothly drop by up to maxSagDepth,
        // producing warm analog bloom and sustain rather than harsh buzz
        float railCompression = 1.0f / (1.0f + maxSagDepth * std::tanh(0.75f * sagEnvelope));

        // Apply dynamic sag & DC cathode bias drift
        x = x * railCompression - biasDc * 0.12f;

        // 4. Asymmetric Triode & Transformer Magnetic Soft Saturation
        float y = 0.0f;
        if (x >= 0.0f) {
            // Soft rounded grid conduction into magnetic transformer saturation
            // Uses gentle algebraic soft-clipper x / sqrt(1 + x^2) blended with tanh
            float x1 = 0.9f * x;
            y = (x1 / std::sqrt(1.0f + x1 * x1 * 0.85f));
        } else {
            // Negative swing: Quadratic triode curvature (rich 2nd harmonic)
            // with wide rounded cutoff
            if (x > -2.2f) {
                y = x + 0.18f * (x * x);
            } else {
                y = -1.33f;
            }
        }

        // 5. Output headroom recovery (preserves calibrated loudness level while keeping tone tight)
        float out = y * headroomScale;

        // Leaky DC tracking (removes bias drift over time)
        biasDc += 0.0004f * (y - biasDc);

        return out;
    }

    float getBiasDc() const noexcept { return biasDc; }
    float getDriveMultiplier() const noexcept { return rawDrive; }
    float getSagEnvelope() const noexcept { return sagEnvelope; }

    static float staticTransfer(float x, float driveGain = 1.0f) {
        float vx = x * driveGain;
        if (vx >= 0.0f) {
            float x1 = 0.9f * vx;
            return (x1 / std::sqrt(1.0f + x1 * x1 * 0.85f));
        } else {
            if (vx > -2.2f) {
                return vx + 0.18f * (vx * vx);
            } else {
                return -1.33f;
            }
        }
    }

private:
    void updateCoefficients() {
        // Continuous Tight highpass cutoff sweep from 20 Hz (loose) to 135 Hz (laser-tight)
        float fc = 20.0f + (tightParam / 10.0f) * 115.0f;
        subFilterCoeff = 1.0f - std::exp(-2.0f * 3.14159265358979323846f * fc / static_cast<float>(sampleRate));

        // Dynamic Sag Depth: scale base sag depth by sagParam (0.0 to 10.0, default 5.0)
        float sagFactor = sagParam / 5.0f;
        maxSagDepth = baseSagDepth * sagFactor;
        headroomScale = baseHeadroomScale;
    }

    void updateSubFilterCoeff() {
        updateCoefficients();
    }

    double sampleRate = 44100.0;
    AmpType currentType = AmpType::BassHead;

    float driveDbParam = 0.0f;
    float rawDrive = 1.0f;
    float tightParam = 2.0f; // Default subtle tightness
    float sagParam = 5.0f;   // Default natural tube bloom

    float baseHeadroomScale = 1.85f;
    float baseSagDepth = 0.45f;
    float headroomScale = 1.85f;
    float maxSagDepth = 0.45f;
    float sagAttack = 0.0025f;
    float sagRelease = 0.00035f;

    float biasDc = 0.0f;
    float sagEnvelope = 0.0f;
    float subFilterState = 0.0f;
    float subFilterCoeff = 0.0045f;
};
