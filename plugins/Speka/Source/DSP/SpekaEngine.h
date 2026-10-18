#pragma once

#include "PITTimer.h"
#include "ElectricalCircuit.h"
#include "MechanicalSpeaker.h"
#include "ChassisAcoustics.h"

namespace Speka {

/**
 * SpekaEngine: Master coordinator coupling the digital timer modulation,
 * electrical driver circuit, electromechanical cone dynamics,
 * and chassis acoustics.
 */
class SpekaEngine {
public:
    SpekaEngine() = default;

    void prepare(double sampleRate) {
        fs = sampleRate;
        invFs = 1.0 / fs;

        constexpr int kSubSteps = 4;
        timer.prepare(sampleRate * kSubSteps);
        circuit.prepare(sampleRate * kSubSteps);
        speaker.prepare(sampleRate);
        chassis.prepare(sampleRate);

        // Input and Output DC blockers at 15 Hz
        dcAlpha = static_cast<float>(1.0 - (2.0 * M_PI * 15.0 / fs));
        inDcX1 = 0.0f;
        inDcY1 = 0.0f;
        outDcX1 = 0.0f;
        outDcY1 = 0.0f;

        updateGain();
        updateGateThresholds();
    }

    void reset() {
        timer.reset();
        circuit.reset();
        speaker.reset();
        chassis.reset();
        inDcX1 = 0.0f;
        inDcY1 = 0.0f;
        outDcX1 = 0.0f;
        outDcY1 = 0.0f;
        gateEnv_ = 0.0f;
    }

    // --- Parameter Setters ---
    void setModulationMode(ModulationMode mode) {
        timer.setMode(mode);
    }

    void setCarrierFrequency(float carrierHz) {
        timer.setCarrierFrequency(carrierHz);
    }

    void setJitterAmount(float amount) {
        timer.setJitterAmount(amount);
    }

    void setSpeakerType(SpeakerType type) {
        speaker.setSpeakerType(type);
    }

    void setDriveDb(float driveDb) {
        driveDb_ = driveDb;
        updateGain();
    }

    void setChassisResonance(float amount) {
        chassis.setCaseAmount(amount);
    }

    void setChassisRattle(float amount) {
        chassis.setRattleAmount(amount);
    }

    void setPowerSag(float amount) {
        circuit.setPowerSupplySagAmount(amount);
    }

    void setWetDryMix(float mix01) {
        mix_ = std::clamp(mix01, 0.0f, 1.0f);
    }

    void setCarrierBleed(float bleed) {
        chassis.setCarrierBleed(bleed);
    }

    void setAutoGate(bool enabled) {
        autoGateEnabled_ = enabled;
    }

    void setGateThresholdDb(float threshDb) {
        gateThresholdDb_ = threshDb;
        updateGateThresholds();
    }

    void setOutputTrimDb(float trimDb) {
        outputTrimDb_ = trimDb;
        updateGain();
    }

    /**
     * Process a single audio sample through the physical simulation pipeline.
     */
    float processSample(float inputSample) {
        float dry = inputSample;

        // 1. Input DC Blocker (centers incoming audio at zero, preventing asymmetric rail bias)
        float inDcBlocked = inputSample - inDcX1 + dcAlpha * inDcY1;
        inDcX1 = inputSample;
        inDcY1 = inDcBlocked;

        // Auto-Gate (Port 0x61 Speaker Shutoff)
        // In real DOS games, Port 0x61 bit 0 & 1 were cleared during silence/pauses,
        // preventing continuous PWM carrier hiss or idle whistles.
        float gateGain = 1.0f;
        if (autoGateEnabled_) {
            float inAbs = std::abs(inDcBlocked);
            float attackA  = static_cast<float>(1.0 - std::exp(-invFs / 0.0002)); // 0.2ms fast attack
            float releaseA = static_cast<float>(1.0 - std::exp(-invFs / 0.030));  // 30ms smooth release
            if (inAbs > gateEnv_) {
                gateEnv_ += attackA * (inAbs - gateEnv_);
            } else {
                gateEnv_ += releaseA * (inAbs - gateEnv_);
            }
            // Squelch with smooth ramp between lower and upper threshold
            float range = std::max(1e-7f, gateThreshUpper_ - gateThreshLower_);
            gateGain = std::clamp((gateEnv_ - gateThreshLower_) / range, 0.0f, 1.0f);
        }

        // 2. Input drive
        float driven = std::clamp(inDcBlocked * driveLinear_, -1.5f, 1.5f);

        // 3. 4x Sub-Stepped Timer Modulation & Voice-Coil Electrical Integration
        // This pushes PWM carrier harmonics up to ~176.4 kHz, eliminating audible foldback aliasing
        // and authentically modeling the voice coil's physical low-pass current integration
        constexpr int kSubSteps = 4;
        float coneVelocity = speaker.getConeVelocity();
        float blCurrent = speaker.getBlCurrent();
        float avgCoilCurrent = 0.0f;

        if (gateGain > 0.0001f) {
            for (int step = 0; step < kSubSteps; ++step) {
                float bit1 = timer.processSample(driven) * gateGain;
                float iSub = circuit.processSample(bit1, coneVelocity, blCurrent);
                avgCoilCurrent += iSub;
            }
            avgCoilCurrent *= (1.0f / static_cast<float>(kSubSteps));
        } else {
            circuit.processSample(0.0f, coneVelocity, blCurrent);
            avgCoilCurrent = 0.0f;
        }

        // 4. Mechanical Speaker Cone Dynamics
        float acousticPressure = speaker.processSample(avgCoilCurrent);

        // 5. Chassis Enclosure & Room Acoustics
        float coneDisplacement = speaker.getConeDisplacement();
        float radiatedSound = chassis.processSample(acousticPressure, coneDisplacement, coneVelocity);

        // 6. Output DC Blocker filter
        float outDcBlocked = radiatedSound - outDcX1 + dcAlpha * outDcY1;
        outDcX1 = radiatedSound;
        outDcY1 = outDcBlocked;

        // Apply gate envelope to final acoustic output for clean silence
        outDcBlocked *= gateGain;

        // 7. Output scaling, soft-limiting, and wet/dry blend
        float wet = softClip(outDcBlocked * driveComp_ * outputTrimLinear_);
        return dry * (1.0f - mix_) + wet * mix_;
    }

private:
    static inline float softClip(float x) {
        constexpr float thresh = 0.85f;
        if (x > thresh) {
            return thresh + (1.0f - thresh) * std::tanh((x - thresh) / (1.0f - thresh));
        } else if (x < -thresh) {
            return -thresh + (1.0f - thresh) * std::tanh((x + thresh) / (1.0f - thresh));
        }
        return x;
    }

    void updateGain() {
        driveLinear_ = std::pow(10.0f, driveDb_ / 20.0f);
        // Internal drive saturation makeup: keeps loudness level consistent
        // when drive is pushed into harmonic distortion, adding grit without level explosion
        if (driveDb_ > 0.0f) {
            driveComp_ = 1.0f / std::pow(driveLinear_, 0.80f);
        } else {
            driveComp_ = 1.0f;
        }
        // Calibrated unity gain at 0 dB trim
        outputTrimLinear_ = std::pow(10.0f, outputTrimDb_ / 20.0f);
    }

    void updateGateThresholds() {
        gateThreshUpper_ = std::pow(10.0f, gateThresholdDb_ / 20.0f);
        // 6 dB hysteresis below upper threshold
        gateThreshLower_ = std::pow(10.0f, (gateThresholdDb_ - 6.0f) / 20.0f);
    }

    double fs = 44100.0;
    double invFs = 1.0 / 44100.0;

    PITTimer timer;
    ElectricalCircuit circuit;
    MechanicalSpeaker speaker;
    ChassisAcoustics chassis;

    float driveDb_ = 0.0f;
    float driveLinear_ = 1.0f;
    float driveComp_ = 1.0f;
    float outputTrimDb_ = 0.0f;
    float outputTrimLinear_ = 1.0f;
    float mix_ = 1.0f;

    // DC Blocker state (Input and Output)
    float dcAlpha = 0.997f;
    float inDcX1 = 0.0f;
    float inDcY1 = 0.0f;
    float outDcX1 = 0.0f;
    float outDcY1 = 0.0f;

    // Auto-Gate / Port 0x61 squelch state
    bool autoGateEnabled_ = true;
    float gateThresholdDb_ = -72.0f; // Default -72 dBFS (clean natural decay, zero chatter)
    float gateThreshUpper_ = 0.000251f;
    float gateThreshLower_ = 0.000125f;
    float gateEnv_ = 0.0f;
};

} // namespace Speka
