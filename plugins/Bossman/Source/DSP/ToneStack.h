#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

/**
 * Discretized '59 Fender Bassman 5F6-A Tone Stack
 * Based on: David T. Yeh and Julius O. Smith,
 * "Discretization of the '59 Fender Bassman Tone Stack", DAFx-06 (CCRMA, Stanford).
 *
 * Implemented using exact continuous-time nodal polynomials and Bilinear Transform
 * in Transposed Direct Form II (TDF-II) for numerical robustness.
 */
class ToneStack {
public:
    ToneStack() {
        reset();
    }

    void prepare(const juce::dsp::ProcessSpec &spec) {
        sampleRate = spec.sampleRate;
        reset();
        updateCoefficients();
    }

    void reset() {
        d1 = 0.0;
        d2 = 0.0;
        d3 = 0.0;
    }

    /**
     * Set tone control knobs (range [0.0, 10.0] or [0.0, 1.0]).
     * In the original amp:
     * - Treble (R1 = 250k) is linear taper: t in [0, 1]
     * - Middle (R3 = 25k) is linear taper: m in [0, 1]
     * - Bass (R2 = 1M) is 10% audio (log) taper: l in [0, 1]
     */
    void setKnobs(float bassVal, float midVal, float trebleVal, bool normalized01 = false) {
        float rawB = normalized01 ? bassVal : (bassVal / 10.0f);
        float rawM = normalized01 ? midVal : (midVal / 10.0f);
        float rawT = normalized01 ? trebleVal : (trebleVal / 10.0f);

        // Clamp to [0, 1]
        rawB = juce::jlimit(0.0f, 1.0f, rawB);
        rawM = juce::jlimit(0.0f, 1.0f, rawM);
        rawT = juce::jlimit(0.0f, 1.0f, rawT);

        // Taper mappings:
        // Treble: Linear taper
        t = static_cast<double>(rawT);

        // Middle: Linear taper
        // Avoid singular zero for m=0 in denominator boundary by setting tiny epsilon
        m = juce::jmax(1e-5, static_cast<double>(rawM));

        // Bass: Audio / Logarithmic 10% taper at 50% rotation:
        // Approximated by audio taper curve: l = (pow(10, rawB) - 1) / 9.0 or pow(rawB, 2.5)
        l = juce::jlimit(1e-5, 1.0, static_cast<double>(std::pow(rawB, 2.321928f))); // 0.5^2.322 ≈ 0.20

        updateCoefficients();
    }

    /**
     * Direct parameter access with explicit normalized [0, 1] tapers (t, m, l)
     */
    void setDirectParameters(double treble_t, double mid_m, double bass_l) {
        t = juce::jlimit(0.0, 1.0, treble_t);
        m = juce::jlimit(1e-5, 1.0, mid_m);
        l = juce::jlimit(1e-5, 1.0, bass_l);
        updateCoefficients();
    }

    /**
     * Compute analytical continuous-time complex response H(s) at frequency f (Hz)
     */
    std::complex<double> evaluateAnalogResponse(double freqHz) const {
        double omega = 2.0 * M_PI * freqHz;
        std::complex<double> s(0.0, omega);
        std::complex<double> s2 = s * s;
        std::complex<double> s3 = s2 * s;

        std::complex<double> num = b1 * s + b2 * s2 + b3 * s3;
        std::complex<double> den = a0 + a1 * s + a2 * s2 + a3 * s3;
        return num / den;
    }

    /**
     * Compute discrete-time response H(e^jw) at frequency f (Hz)
     */
    std::complex<double> evaluateDigitalResponse(double freqHz) const {
        double w = 2.0 * M_PI * freqHz / sampleRate;
        std::complex<double> z1 = std::polar(1.0, -w);
        std::complex<double> z2 = std::polar(1.0, -2.0 * w);
        std::complex<double> z3 = std::polar(1.0, -3.0 * w);

        std::complex<double> num = b0_d + b1_d * z1 + b2_d * z2 + b3_d * z3;
        std::complex<double> den = 1.0 + a1_d * z1 + a2_d * z2 + a3_d * z3;
        return num / den;
    }

    /**
     * Process single sample using Transposed Direct Form II (TDF-II)
     */
    inline float processSample(float input) noexcept {
        double in = static_cast<double>(input);
        double out = b0_d * in + d1;

        d1 = b1_d * in - a1_d * out + d2;
        d2 = b2_d * in - a2_d * out + d3;
        d3 = b3_d * in - a3_d * out;

        return static_cast<float>(out);
    }

private:
    void updateCoefficients() {
        // Physical Component Values for Fender '59 Bassman 5F6-A
        constexpr double C1 = 0.25e-9;   // 250 pF
        constexpr double C2 = 20.0e-9;   // 20 nF
        constexpr double C3 = 20.0e-9;   // 20 nF
        constexpr double R1 = 250.0e3;   // 250 kOhm (Treble pot)
        constexpr double R2 = 1.0e6;     // 1 MOhm (Bass pot)
        constexpr double R3 = 25.0e3;    // 25 kOhm (Mid pot)
        constexpr double R4 = 56.0e3;    // 56 kOhm (Slope resistor)

        // Continuous-time polynomial coefficients (Yeh & Smith Eq. 1)
        a0 = 1.0;
        a1 = (C1*R1 + C1*R3 + C2*R3 + C2*R4 + C3*R4) + m*C3*R3 + l*(C1*R2 + C2*R2);
        a2 = (m*(C1*C3*R1*R3 - C2*C3*R3*R4 + C1*C3*R3*R3 + C2*C3*R3*R3)
              + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
              - m*m*(C1*C3*R3*R3 + C2*C3*R3*R3)
              + l*(C1*C2*R2*R4 + C1*C2*R1*R2 + C1*C3*R2*R4 + C2*C3*R2*R4)
              + (C1*C2*R1*R4 + C1*C3*R1*R4 + C1*C2*R3*R4 + C1*C2*R1*R3 + C1*C3*R3*R4 + C2*C3*R3*R4));
        a3 = (l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
              - m*m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
              + m*(C1*C2*C3*R3*R3*R4 + C1*C2*C3*R1*R3*R3 - C1*C2*C3*R1*R3*R4)
              + l*C1*C2*C3*R1*R2*R4 + C1*C2*C3*R1*R3*R4);

        b1 = t*C1*R1 + m*C3*R3 + l*(C1*R2 + C2*R2) + (C1*R3 + C2*R3);
        b2 = (t*(C1*C2*R1*R4 + C1*C3*R1*R4)
              - m*m*(C1*C3*R3*R3 + C2*C3*R3*R3)
              + m*(C1*C3*R1*R3 + C1*C3*R3*R3 + C2*C3*R3*R3)
              + l*(C1*C2*R1*R2 + C1*C2*R2*R4 + C1*C3*R2*R4)
              + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
              + (C1*C2*R1*R3 + C1*C2*R3*R4 + C1*C3*R3*R4));
        b3 = (l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
              - m*m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
              + m*(C1*C2*C3*R1*R3*R3 + C1*C2*C3*R3*R3*R4)
              + t*C1*C2*C3*R1*R3*R4 - t*m*C1*C2*C3*R1*R3*R4
              + t*l*C1*C2*C3*R1*R2*R4);

        // Discretization via Bilinear Transform: s = c * (1 - z^-1) / (1 + z^-1)
        double c = 2.0 * sampleRate;
        double c2 = c * c;
        double c3 = c2 * c;

        double B0 = -b1*c - b2*c2 - b3*c3;
        double B1 = -b1*c + b2*c2 + 3.0*b3*c3;
        double B2 =  b1*c + b2*c2 - 3.0*b3*c3;
        double B3 =  b1*c - b2*c2 + b3*c3;

        double A0 = -a0 - a1*c - a2*c2 - a3*c3;
        double A1 = -3.0*a0 - a1*c + a2*c2 + 3.0*a3*c3;
        double A2 = -3.0*a0 + a1*c + a2*c2 - 3.0*a3*c3;
        double A3 = -a0 + a1*c - a2*c2 + a3*c3;

        // Normalization by A0
        double invA0 = 1.0 / A0;
        b0_d = B0 * invA0;
        b1_d = B1 * invA0;
        b2_d = B2 * invA0;
        b3_d = B3 * invA0;

        a1_d = A1 * invA0;
        a2_d = A2 * invA0;
        a3_d = A3 * invA0;
    }

    double sampleRate = 44100.0;
    double t = 0.5;
    double m = 0.5;
    double l = 0.5;

    // Continuous coefficients
    double a0 = 1.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double b1 = 0.0, b2 = 0.0, b3 = 0.0;

    // Normalized discrete coefficients
    double b0_d = 0.0, b1_d = 0.0, b2_d = 0.0, b3_d = 0.0;
    double a1_d = 0.0, a2_d = 0.0, a3_d = 0.0;

    // TDF-II state registers
    double d1 = 0.0;
    double d2 = 0.0;
    double d3 = 0.0;
};
