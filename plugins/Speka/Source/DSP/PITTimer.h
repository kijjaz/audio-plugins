#pragma once

#include <cmath>
#include <algorithm>
#include <random>
#include <cstdint>

namespace Speka {

enum class ModulationMode {
    RealSoundPWM = 0,
    SigmaDeltaPDM,
    DeltaModulation,
    Direct1Bit
};

/**
 * PITTimer: Emulates the Intel 8253/8254 Programmable Interval Timer (PIT)
 * and vintage 1-bit audio modulation routines (RealSound, Sigma-Delta, DeltaMod).
 */
class PITTimer {
public:
    static constexpr double kPITClockHz = 1193181.8181818; // Master 1.193182 MHz clock

    PITTimer()
        : rng(1337)
        , jitterDist(0.0f, 1.0f)
    {}

    void prepare(double sampleRate) {
        fs = sampleRate;
        invFs = 1.0 / fs;
        reset();
        updateCarrier();
        updateRateScaling();
    }

    void reset() {
        phase = 0.0f;
        lastRawState = 0.0f;
        sigma1 = 0.0f;
        sigma2 = 0.0f;
        deltaIntegrator = 0.0f;
        lastOutput = 0.0f;
    }

    void setMode(ModulationMode mode) {
        currentMode = mode;
    }

    void setCarrierFrequency(float carrierHz) {
        carrierFreq = std::clamp(carrierHz, 4000.0f, 32000.0f);
        updateCarrier();
    }

    void setJitterAmount(float amount) {
        jitterAmount = std::clamp(amount, 0.0f, 1.0f);
    }

    /**
     * Process one audio sample in range [-1.0, 1.0].
     * Returns 1-bit binary output {0.0, 1.0} with anti-aliased edge smoothing.
     */
    float processSample(float inputSample) {
        // Clamp input
        float in = std::clamp(inputSample, -1.0f, 1.0f);

        switch (currentMode) {
            case ModulationMode::RealSoundPWM:
                return processRealSound(in);
            case ModulationMode::SigmaDeltaPDM:
                return processSigmaDelta(in);
            case ModulationMode::DeltaModulation:
                return processDeltaMod(in);
            case ModulationMode::Direct1Bit:
            default:
                return processDirect1Bit(in);
        }
    }

private:
    void updateCarrier() {
        if (fs <= 0.0) return;
        phaseInc = carrierFreq / static_cast<float>(fs);
        // Number of PIT timer clock ticks per PWM frame
        ticksPerPeriod = std::max(2.0f, std::floor(static_cast<float>(kPITClockHz) / carrierFreq));
    }

    // PolyBLEP anti-aliasing residual for sharp steps
    static inline float polyBLEP(float t) {
        if (t > 0.0f && t < 1.0f) {
            return 2.0f * t - t * t - 1.0f;
        } else if (t > -1.0f && t <= 0.0f) {
            return t * t + 2.0f * t + 1.0f;
        }
        return 0.0f;
    }

    float processRealSound(float in) {
        // Convert audio [-1, 1] to normalized duty cycle [0, 1]
        float duty = std::clamp((in + 1.0f) * 0.5f, 0.0f, 1.0f);

        // Quantize duty cycle to discrete PIT timer tick steps
        float quantizedDuty = std::round(duty * ticksPerPeriod) / ticksPerPeriod;

        // Apply CPU interrupt jitter
        float jitter = 0.0f;
        if (jitterAmount > 0.0001f) {
            jitter = jitterDist(rng) * jitterAmount * 0.08f * phaseInc;
        }

        float dt = std::max(1e-6f, phaseInc + jitter);
        float nextPhase = phase + dt;

        float rawPulse = 0.0f;
        float blep = 0.0f;

        // Check if raw pulse should be high
        if (phase < quantizedDuty) {
            rawPulse = 1.0f;
        }

        // Rising edge at period start (phase wrap around 0)
        if (nextPhase >= 1.0f) {
            float t = (phase - 0.0f) / dt;
            blep += polyBLEP(t);
        }

        // Falling edge at duty cycle transition
        if (phase < quantizedDuty && nextPhase >= quantizedDuty) {
            float t = (phase - quantizedDuty) / dt;
            blep -= polyBLEP(t);
        }

        phase = nextPhase;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }

        float output = rawPulse + blep;
        lastOutput = std::clamp(output, 0.0f, 1.0f);
        return lastOutput;
    }

    float processSigmaDelta(float in) {
        // 2nd-Order Sigma-Delta (CIFB structure)
        // Target unipolar signal in [0.05, 0.95] to prevent integrator blowup
        float target = 0.5f + 0.45f * in;

        float error = target - lastRawState;
        sigma1 += 0.8f * error;
        // Leaky integration to avoid DC wander (scaled with sample rate)
        sigma1 *= sigmaLeak_;

        sigma2 += 0.5f * (sigma1 - 0.5f * lastRawState);
        sigma2 *= sigmaLeak_;

        float bit = (sigma2 >= 0.0f) ? 1.0f : 0.0f;
        lastRawState = bit;
        // Internal Sigma-Delta PDM component calibration (+2.6 dB gain makeup
        // to match PWM nominal modulation energy across the voice coil)
        return bit * 1.35f;
    }

    float processDeltaMod(float in) {
        // 1-Bit Delta Modulation (CVSD / Sierra On-Line 1-bit speech style)
        // Tracks slope with authentic slope-overload character
        float target = in;
        float bit = (target >= deltaIntegrator) ? 1.0f : 0.0f;
        
        // Slope step size with overload non-linearity (sample-rate scaled)
        if (bit > 0.5f) {
            deltaIntegrator += deltaStep_;
        } else {
            deltaIntegrator -= deltaStep_;
        }
        // Leaky integration prevents DC drift and models voice coil acoustic response
        deltaIntegrator *= deltaLeak_;
        deltaIntegrator = std::clamp(deltaIntegrator, -1.0f, 1.0f);

        // Convert demodulated 1-bit trajectory to voice-coil driver voltage [0.0, 1.0]
        float demodulated = (deltaIntegrator + 1.0f) * 0.5f;
        return demodulated;
    }

    float processDirect1Bit(float in) {
        // Zero-crossing comparator with small hysteresis (Buzzer/Beeper mode)
        constexpr float hysteresis = 0.03f;
        if (in > hysteresis) {
            lastRawState = 1.0f;
        } else if (in < -hysteresis) {
            lastRawState = 0.0f;
        }
        // Internal Direct 1-Bit driver component calibration:
        // Full rail-to-rail square waves have a crest factor of 1 (0 dB RMS),
        // whereas PWM audio is modulated at ~10-25% RMS power.
        // Calibrating the driver pulse scale to 0.28f (-11.1 dB) natively matches
        // the acoustic energy envelope of modulated PWM audio.
        return lastRawState * 0.28f;
    }

    void updateRateScaling() {
        if (fs <= 0.0) return;
        float rateRatio = static_cast<float>(44100.0 / fs);
        deltaStep_ = 0.09f * rateRatio;
        deltaLeak_ = std::pow(0.95f, rateRatio);
        sigmaLeak_ = std::pow(0.9995f, rateRatio);
    }

    double fs = 44100.0;
    double invFs = 1.0 / 44100.0;

    ModulationMode currentMode = ModulationMode::RealSoundPWM;
    float carrierFreq = 18900.0f; // Default 18.9 kHz (Mean Streets / RealSound)
    float phaseInc = 0.0f;
    float ticksPerPeriod = 63.0f;
    float jitterAmount = 0.15f;

    float phase = 0.0f;
    float lastRawState = 0.0f;
    float lastOutput = 0.0f;

    // Sigma-Delta state
    float sigma1 = 0.0f;
    float sigma2 = 0.0f;
    float sigmaLeak_ = 0.9995f;

    // Delta Modulation state
    float deltaIntegrator = 0.5f;
    float deltaStep_ = 0.09f;
    float deltaLeak_ = 0.95f;

    // Random number generator for jitter
    std::mt19937 rng;
    std::normal_distribution<float> jitterDist;
};

} // namespace Speka
