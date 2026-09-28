#pragma once
#include <cmath>

/**
 * 12AX7 Vacuum Tube Triode Preamp Stage Emulation
 * Models non-linear asymmetric triode transfer curve (even + odd harmonic richness)
 * and grid-conduction compression typical of vintage Fender 5F6-A preamps.
 */
class TubeStage {
public:
    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate;
        reset();
    }

    void reset() {
        biasDc = 0.0f;
    }

    void setDrive(float driveDb) {
        // Map 0 to 60 dB to input voltage multiplier
        drive = std::pow(10.0f, driveDb / 20.0f);
    }

    /**
     * Nonlinear transfer function:
     * Asymmetric triode clipping with gentle cathode compression:
     * - Positive swing: soft compression into grid conduction
     * - Negative swing: gradual cutoff with quadratic curvature (generates rich 2nd harmonic)
     */
    float processSample(float input) noexcept {
        float x = input * drive;

        // Dynamic DC bias drift (Cathode bias self-modulation)
        x -= biasDc * 0.15f;

        float y = 0.0f;
        if (x >= 0.0f) {
            // Soft saturation curve into saturation
            y = std::tanh(1.2f * x) / 1.2f;
        } else {
            // Asymmetric triode cutoff with 2nd harmonic quadratic knee
            // y = x - 0.25 * x^2 for mild negative swing, into hard cutoff
            if (x > -2.0f) {
                y = x + 0.22f * (x * x);
            } else {
                y = -1.12f;
            }
        }

        // Leaky DC tracking (keeps AC centered while letting bias shift under heavy drive)
        biasDc += 0.0005f * (y - biasDc);

        return y;
    }

    float getBiasDc() const noexcept { return biasDc; }
    float getDriveMultiplier() const noexcept { return drive; }

    /**
     * Direct mathematical transfer function f(x) for static distortion shape tests
     */
    static float staticTransfer(float x, float driveGain = 1.0f) {
        float vx = x * driveGain;
        if (vx >= 0.0f) {
            return std::tanh(1.2f * vx) / 1.2f;
        } else {
            if (vx > -2.0f) {
                return vx + 0.22f * (vx * vx);
            } else {
                return -1.12f;
            }
        }
    }

private:
    double sampleRate = 44100.0;
    float drive = 1.0f;
    float biasDc = 0.0f;
};
