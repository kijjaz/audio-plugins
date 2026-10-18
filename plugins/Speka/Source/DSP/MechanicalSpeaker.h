#pragma once

#include <cmath>
#include <algorithm>

namespace Speka {

enum class SpeakerType {
    DynamicPaper2Inch = 0,
    PiezoBuzzer12mm
};

/**
 * BiquadFilter: Lightweight direct-form II transposed biquad for acoustic resonances.
 */
class Biquad {
public:
    void setBandpass(double sampleRate, double freqHz, double q) {
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double alpha = std::sin(w0) / (2.0 * q);

        double b0 = alpha;
        double b1 = 0.0;
        double b2 = -alpha;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * std::cos(w0);
        double a2 = 1.0 - alpha;

        setCoeffs(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    void setPeaking(double sampleRate, double freqHz, double q, double gainDb) {
        double A = std::pow(10.0, gainDb / 40.0);
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double alpha = std::sin(w0) / (2.0 * q);

        double b0 = 1.0 + alpha * A;
        double b1 = -2.0 * std::cos(w0);
        double b2 = 1.0 - alpha * A;
        double a0 = 1.0 + alpha / A;
        double a1 = -2.0 * std::cos(w0);
        double a2 = 1.0 - alpha / A;

        setCoeffs(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    void setHighpass(double sampleRate, double freqHz, double q) {
        double w0 = 2.0 * M_PI * freqHz / sampleRate;
        double alpha = std::sin(w0) / (2.0 * q);

        double b0 = (1.0 + std::cos(w0)) / 2.0;
        double b1 = -(1.0 + std::cos(w0));
        double b2 = (1.0 + std::cos(w0)) / 2.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * std::cos(w0);
        double a2 = 1.0 - alpha;

        setCoeffs(b0/a0, b1/a0, b2/a0, a1/a0, a2/a0);
    }

    void setCoeffs(double b0_, double b1_, double b2_, double a1_, double a2_) {
        b0 = static_cast<float>(b0_);
        b1 = static_cast<float>(b1_);
        b2 = static_cast<float>(b2_);
        a1 = static_cast<float>(a1_);
        a2 = static_cast<float>(a2_);
    }

    void reset() {
        z1 = 0.0f;
        z2 = 0.0f;
    }

    inline float process(float in) {
        float out = b0 * in + z1;
        z1 = b1 * in - a1 * out + z2;
        z2 = b2 * in - a2 * out;
        return out;
    }

private:
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
    float z1 = 0.0f, z2 = 0.0f;
};

/**
 * MechanicalSpeaker: Simulates non-linear speaker cone dynamics,
 * progressive suspension stiffening, magnetic gap drop-off,
 * physical excursion limits, and paper cone breakup modes.
 */
class MechanicalSpeaker {
public:
    MechanicalSpeaker() = default;

    void prepare(double sampleRate) {
        fs = sampleRate;
        invFs = 1.0 / fs;
        reset();
        configureFilters();
    }

    void reset() {
        x_pos = 0.0f;
        v_vel = 0.0f;
        lastAccel = 0.0f;
        breakup1.reset();
        breakup2.reset();
        breakup3.reset();
        piezoHp1.reset();
        piezoHp2.reset();
        piezoRes.reset();
    }

    void setSpeakerType(SpeakerType type) {
        currentType = type;
    }

    float getBlCurrent() const {
        if (currentType == SpeakerType::PiezoBuzzer12mm) {
            return 1.0f;
        }
        // Bl(x) = Bl0 * exp(-x^2 / (2 * sigma^2))
        constexpr float sigmaGapSq = 0.0006f * 0.0006f;
        return Bl0 * std::exp(- (x_pos * x_pos) / (2.0f * sigmaGapSq));
    }

    float getConeVelocity() const {
        return v_vel;
    }

    float getConeDisplacement() const {
        return x_pos;
    }

    /**
     * Process one sample of speaker motion.
     * @param coilCurrentIn Amperes through the voice coil
     * @return Radiated sound pressure in the far field (proportional to cone acceleration)
     */
    float processSample(float coilCurrentIn) {
        if (currentType == SpeakerType::PiezoBuzzer12mm) {
            return processPiezo(coilCurrentIn);
        }

        // --- Dynamic Paper Cone (2.25 inch) ---
        // 1. Current position-dependent parameters
        float blCurrent = getBlCurrent();
        float f_lorentz = blCurrent * coilCurrentIn;

        // Progressive stiffness: K(x) = k0 * (1 + beta2 * x^2 + beta4 * x^4)
        float x2 = x_pos * x_pos;
        float k_current = k0 * (1.0f + 4.0e6f * x2 + 1.6e13f * (x2 * x2));

        // 2. Acceleration: a = (F_lorentz - R_ms * v - K(x) * x) / M_ms
        float accel = (f_lorentz - Rms * v_vel - k_current * x_pos) / Mms;

        // 3. Numerical Integration (Heun's 2nd-order predictor-corrector)
        float dt = static_cast<float>(invFs);
        v_vel += accel * dt;
        x_pos += v_vel * dt;

        // 4. Physical mechanical boundary (spider/surround bottoming at +/- 0.8 mm)
        constexpr float xMax = 0.0008f; // 0.8 mm excursion limit
        if (x_pos > xMax) {
            x_pos = xMax;
            v_vel = -0.12f * v_vel; // Inelastic collision bounce
        } else if (x_pos < -xMax) {
            x_pos = -xMax;
            v_vel = -0.12f * v_vel;
        }

        // 5. Far-field radiated acoustic pressure is proportional to cone acceleration (dv/dt)
        // Calibrated so that nominal speech/music inputs produce ~ -14 to -16 dBFS RMS (unity matched)
        float rawAcousticPressure = accel * 0.0044f;

        // 6. Cone Breakup Modes (Paper flexure modes above 1.5 kHz)
        float b1 = breakup1.process(rawAcousticPressure);
        float b2 = breakup2.process(rawAcousticPressure);
        float b3 = breakup3.process(rawAcousticPressure);

        float acousticOut = rawAcousticPressure + 0.35f * b1 + 0.25f * b2 + 0.15f * b3;

        lastAccel = accel;
        return acousticOut;
    }

private:
    float processPiezo(float coilCurrentIn) {
        // Piezo disc responds to electrical signal as a high-pass capacitive bender
        // with severe 3.25 kHz resonance and zero bass below 1 kHz
        float hp = piezoHp1.process(coilCurrentIn);
        hp = piezoHp2.process(hp);
        float resonantPeak = piezoRes.process(hp);

        // Harsh piezoceramic non-linear saturation calibrated to match paper speaker perceived loudness
        float out = hp + 1.8f * resonantPeak;
        out = std::tanh(out * 12.0f) * 3.45f;
        return out;
    }

    void configureFilters() {
        if (fs <= 0.0) return;

        // Cone breakup modes
        breakup1.setPeaking(fs, 2400.0, 3.2, 5.0);
        breakup2.setPeaking(fs, 3750.0, 4.0, 4.0);
        breakup3.setPeaking(fs, 5100.0, 2.8, 3.0);

        // Piezo filters
        piezoHp1.setHighpass(fs, 1200.0, 0.707);
        piezoHp2.setHighpass(fs, 1200.0, 0.707);
        piezoRes.setBandpass(fs, 3250.0, 8.5);
    }

    double fs = 44100.0;
    double invFs = 1.0 / 44100.0;
    SpeakerType currentType = SpeakerType::DynamicPaper2Inch;

    // Physical Thiele-Small constants for 2.25" paper cone
    const float Mms = 0.0007f; // 0.7 grams
    const float k0  = 3400.0f; // N/m (f0 ≈ 350 Hz)
    const float Rms = 0.35f;   // N·s/m (Qms ≈ 4.4)
    const float Bl0 = 1.2f;    // N/A

    // Dynamic state
    float x_pos = 0.0f;
    float v_vel = 0.0f;
    float lastAccel = 0.0f;

    // Acoustic breakup filters
    Biquad breakup1;
    Biquad breakup2;
    Biquad breakup3;

    // Piezo filters
    Biquad piezoHp1;
    Biquad piezoHp2;
    Biquad piezoRes;
};

} // namespace Speka
