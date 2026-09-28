#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <complex>

/**
 * Multi-Model Tone Stack Engine
 * Supports 7 iconic amplifier circuits:
 * 1-7: FMV / 5F6-A Nodal Polynomials (Yeh & Smith DAFx-06)
 * 8: Ampeg B-15N Portaflex / James-Baxandall 4th-order nodal transfer matrix
 */
class ToneStack {
public:
    enum class Model {
        FenderBassman = 0,     // '59 Fender Bassman 5F6-A (Tweed reference)
        FenderBassmanAA864,    // '65 Fender Bassman AA864 (Blackface dedicated bass voicing)
        AmpegB15N,             // Ampeg B-15N Portaflex (James / Baxandall bass circuit)
        FenderTwinReverb,      // Fender Twin Reverb AB763 (Blackface, deep scoop at 450Hz)
        MarshallJCM800,        // Marshall JCM800 / 1959 Plexi (33k slope, punchy mids)
        VoxAC30,               // Vox AC30 Top Boost (47pF bright cap, brilliant chime)
        MesaDualRectifier,     // Mesa Boogie Dual Rectifier (680pF presence, modern scoop)
        SoldanoSLO100          // Soldano SLO-100 (Boutique lead balance)
    };

    struct CircuitProfile {
        const char* name;
        double R1;  // Treble pot
        double R2;  // Bass pot
        double R3;  // Mid pot
        double R4;  // Slope resistor
        double C1;  // Treble cap
        double C2;  // Bass cap
        double C3;  // Mid cap
        double bassTaperExp; // Taper exponent for bass pot
    };

    ToneStack() {
        setModel(Model::FenderBassman);
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
        d4 = 0.0;
    }

    void setModel(Model m) {
        currentModel = m;
        switch (currentModel) {
            case Model::FenderBassman:
                activeProfile = { "Fender '59 Bassman", 250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9, 2.32 };
                break;
            case Model::FenderBassmanAA864:
                // '65 Blackface Bassman AA864: 100k slope, 100nF deep bass cap, 47nF mid cap
                activeProfile = { "Fender '65 Bassman AA864", 250e3, 250e3, 25e3, 100e3, 250e-12, 100e-9, 47e-9, 2.0 };
                break;
            case Model::AmpegB15N:
                activeProfile = { "Ampeg B-15N Portaflex", 470e3, 1e6, 10e3, 100e3, 330e-12, 470e-12, 3300e-12, 1.0 };
                break;
            case Model::FenderTwinReverb:
                activeProfile = { "Fender Twin Reverb", 250e3, 250e3, 10e3, 100e3, 250e-12, 100e-9, 47e-9, 2.0 };
                break;
            case Model::MarshallJCM800:
                activeProfile = { "Marshall JCM800", 220e3, 1e6, 22e3, 33e3, 470e-12, 22e-9, 22e-9, 2.32 };
                break;
            case Model::VoxAC30:
                activeProfile = { "Vox AC30 Top Boost", 1e6, 1e6, 10e3, 100e3, 47e-12, 22e-9, 10e-9, 2.0 };
                break;
            case Model::MesaDualRectifier:
                activeProfile = { "Mesa Dual Rectifier", 250e3, 1e6, 50e3, 47e3, 680e-12, 20e-9, 20e-9, 2.32 };
                break;
            case Model::SoldanoSLO100:
                activeProfile = { "Soldano SLO-100", 250e3, 1e6, 25e3, 47e3, 470e-12, 20e-9, 20e-9, 2.32 };
                break;
        }
        updateCoefficients();
    }

    void setKnobs(float bassVal, float midVal, float trebleVal, bool normalized01 = false) {
        float rawB = normalized01 ? bassVal : (bassVal / 10.0f);
        float rawM = normalized01 ? midVal : (midVal / 10.0f);
        float rawT = normalized01 ? trebleVal : (trebleVal / 10.0f);

        rawB = juce::jlimit(0.0f, 1.0f, rawB);
        rawM = juce::jlimit(0.0f, 1.0f, rawM);
        rawT = juce::jlimit(0.0f, 1.0f, rawT);

        t = static_cast<double>(rawT);
        m = juce::jmax(1e-5, static_cast<double>(rawM));
        l = juce::jlimit(1e-5, 1.0, static_cast<double>(std::pow(rawB, activeProfile.bassTaperExp)));

        updateCoefficients();
    }

    inline float processSample(float input) noexcept {
        double in = static_cast<double>(input);
        double out = b0_d * in + d1;

        d1 = b1_d * in - a1_d * out + d2;
        d2 = b2_d * in - a2_d * out + d3;
        d3 = b3_d * in - a3_d * out + d4;
        d4 = b4_d * in - a4_d * out;

        return static_cast<float>(out);
    }

    std::complex<double> evaluateAnalogResponse(double freqHz) const {
        double omega = 2.0 * M_PI * freqHz;
        std::complex<double> s(0.0, omega);
        std::complex<double> s2 = s * s;
        std::complex<double> s3 = s2 * s;
        std::complex<double> s4 = s3 * s;

        if (currentModel == Model::AmpegB15N) {
            std::complex<double> num = james_N0 + james_N1 * s + james_N2 * s2 + james_N3 * s3 + james_N4 * s4;
            std::complex<double> den = james_D0 + james_D1 * s + james_D2 * s2 + james_D3 * s3 + james_D4 * s4;
            return num / den;
        }

        std::complex<double> num = b1 * s + b2 * s2 + b3 * s3;
        std::complex<double> den = a0 + a1 * s + a2 * s2 + a3 * s3;
        return num / den;
    }

    const CircuitProfile& getProfile() const noexcept { return activeProfile; }

private:
    void updateCoefficients() {
        if (currentModel == Model::AmpegB15N) {
            updateJamesCoefficients();
            return;
        }

        // Standard 3rd-order FMV topology
        const double R1 = activeProfile.R1;
        const double R2 = activeProfile.R2;
        const double R3 = activeProfile.R3;
        const double R4 = activeProfile.R4;
        const double C1 = activeProfile.C1;
        const double C2 = activeProfile.C2;
        const double C3 = activeProfile.C3;

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

        // Bilinear Transform: s = c * (1 - z^-1) / (1 + z^-1)
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

        double invA0 = 1.0 / A0;
        b0_d = B0 * invA0;
        b1_d = B1 * invA0;
        b2_d = B2 * invA0;
        b3_d = B3 * invA0;
        b4_d = 0.0;

        a1_d = A1 * invA0;
        a2_d = A2 * invA0;
        a3_d = A3 * invA0;
        a4_d = 0.0;
    }

    void updateJamesCoefficients() {
        // Ampeg B-15N James / Baxandall network
        const double RIN = 38e3;
        const double R1  = 100e3;
        const double RBass   = 1e6;
        const double R2  = 10e3;
        const double R3  = 180e3;
        const double RTreble = 470e3;
        const double RL  = 1e6;
        const double CB1 = 470e-12;
        const double CB2 = 4700e-12;
        const double CT1 = 330e-12;
        const double CT2 = 3300e-12;

        double rotB = juce::jlimit(0.01, 0.99, l);
        double rotT = juce::jlimit(0.01, 0.99, t);

        double RB2 = RBass * rotB;
        double RB1 = RBass * (1.0 - rotB);
        double RT2 = RTreble * rotT;
        double RT1 = RTreble * (1.0 - rotT);

        // Denominator
        james_D4 = (CB1*CB2*CT1*CT2*RB1*RB2*RIN*RL*RT1*RT2
          + CB1*CB2*CT1*CT2*R2*RB1*RB2*RL*RT1*RT2
          + CB1*CB2*CT1*CT2*R1*RB1*RB2*RL*RT1*RT2
          + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RT1*RT2
          + CB1*CB2*CT1*CT2*R2*RB1*RB2*RIN*RT1*RT2
          + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RT1*RT2
          + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RT1*RT2
          + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RT1*RT2
          + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RL*RT2
          + CB1*CB2*CT1*CT2*R1*RB1*RB2*RIN*RL*RT2 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT2
          + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT2 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT2
          + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RIN*RT2
          + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RIN*RT2
          + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RIN*RT2
          + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RL*RT1
          + CB1*CB2*CT1*CT2*R2*RB1*RB2*RIN*RL*RT1 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT1
          + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT1 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT1
          + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RIN*RL + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RIN*RL
          + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RIN*RL);

        james_D3 = (CB2*CT1*CT2*RB2*RIN*RL*RT1*RT2 + CB1*CT1*CT2*RB1*RIN*RL*RT1*RT2
          + CB2*CT1*CT2*RB1*RB2*RL*RT1*RT2 + CB1*CT1*CT2*RB1*RB2*RL*RT1*RT2
          + CB2*CT1*CT2*R2*RB2*RL*RT1*RT2 + CB2*CT1*CT2*R1*RB2*RL*RT1*RT2
          + CB1*CT1*CT2*R2*RB1*RL*RT1*RT2 + CB1*CT1*CT2*R1*RB1*RL*RT1*RT2
          + CB1*CT1*CT2*RB1*RB2*RIN*RT1*RT2 + CB2*CT1*CT2*R3*RB2*RIN*RT1*RT2
          + CB2*CT1*CT2*R2*RB2*RIN*RT1*RT2 + CB1*CT1*CT2*R3*RB1*RIN*RT1*RT2
          + CB1*CT1*CT2*R2*RB1*RIN*RT1*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RT1*RT2
          + CB1*CT1*CT2*R3*RB1*RB2*RT1*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RT1*RT2
          + CB1*CT1*CT2*R1*RB1*RB2*RT1*RT2 + CB2*CT1*CT2*R2*R3*RB2*RT1*RT2
          + CB2*CT1*CT2*R1*R3*RB2*RT1*RT2 + CB2*CT1*CT2*R1*R2*RB2*RT1*RT2
          + CB1*CT1*CT2*R2*R3*RB1*RT1*RT2 + CB1*CT1*CT2*R1*R3*RB1*RT1*RT2
          + CB1*CT1*CT2*R1*R2*RB1*RT1*RT2 + CB2*CT1*CT2*RB1*RB2*RIN*RL*RT2
          + CB1*CB2*CT2*RB1*RB2*RIN*RL*RT2 + CB2*CT1*CT2*R3*RB2*RIN*RL*RT2
          + CB2*CT1*CT2*R1*RB2*RIN*RL*RT2 + CB1*CT1*CT2*R3*RB1*RIN*RL*RT2
          + CB1*CT1*CT2*R1*RB1*RIN*RL*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RL*RT2
          + CB1*CT1*CT2*R3*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RL*RT2
          + CB1*CB2*CT2*R2*RB1*RB2*RL*RT2 + CB1*CT1*CT2*R1*RB1*RB2*RL*RT2
          + CB1*CB2*CT2*R1*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*R3*RB2*RL*RT2
          + CB2*CT1*CT2*R1*R3*RB2*RL*RT2 + CB2*CT1*CT2*R1*R2*RB2*RL*RT2
          + CB1*CT1*CT2*R2*R3*RB1*RL*RT2 + CB1*CT1*CT2*R1*R3*RB1*RL*RT2
          + CB1*CT1*CT2*R1*R2*RB1*RL*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RIN*RT2
          + CB1*CT1*CT2*R3*RB1*RB2*RIN*RT2 + CB1*CB2*CT2*R3*RB1*RB2*RIN*RT2
          + CB2*CT1*CT2*R2*RB1*RB2*RIN*RT2 + CB1*CB2*CT2*R2*RB1*RB2*RIN*RT2
          + CB1*CT1*CT2*R1*RB1*RB2*RIN*RT2 + CB2*CT1*CT2*R2*R3*RB2*RIN*RT2
          + CB2*CT1*CT2*R1*R3*RB2*RIN*RT2 + CB2*CT1*CT2*R1*R2*RB2*RIN*RT2
          + CB1*CT1*CT2*R2*R3*RB1*RIN*RT2 + CB1*CT1*CT2*R1*R3*RB1*RIN*RT2
          + CB1*CT1*CT2*R1*R2*RB1*RIN*RT2 + CB1*CB2*CT2*R2*R3*RB1*RB2*RT2
          + CB1*CB2*CT2*R1*R3*RB1*RB2*RT2 + CB1*CB2*CT2*R1*R2*RB1*RB2*RT2
          + CB1*CT1*CT2*RB1*RB2*RIN*RL*RT1 + CB1*CB2*CT1*RB1*RB2*RIN*RL*RT1
          + CB2*CT1*CT2*R3*RB2*RIN*RL*RT1 + CB2*CT1*CT2*R2*RB2*RIN*RL*RT1
          + CB1*CT1*CT2*R3*RB1*RIN*RL*RT1 + CB1*CT1*CT2*R2*RB1*RIN*RL*RT1
          + CB2*CT1*CT2*R3*RB1*RB2*RL*RT1 + CB1*CT1*CT2*R3*RB1*RB2*RL*RT1
          + CB2*CT1*CT2*R2*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R2*RB1*RB2*RL*RT1
          + CB1*CT1*CT2*R1*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R1*RB1*RB2*RL*RT1
          + CB2*CT1*CT2*R2*R3*RB2*RL*RT1 + CB2*CT1*CT2*R1*R3*RB2*RL*RT1
          + CB2*CT1*CT2*R1*R2*RB2*RL*RT1 + CB1*CT1*CT2*R2*R3*RB1*RL*RT1
          + CB1*CT1*CT2*R1*R3*RB1*RL*RT1 + CB1*CT1*CT2*R1*R2*RB1*RL*RT1
          + CB1*CB2*CT1*R3*RB1*RB2*RIN*RT1 + CB1*CB2*CT1*R2*RB1*RB2*RIN*RT1
          + CB1*CB2*CT1*R2*R3*RB1*RB2*RT1 + CB1*CB2*CT1*R1*R3*RB1*RB2*RT1
          + CB1*CB2*CT1*R1*R2*RB1*RB2*RT1 + CB2*CT1*CT2*R3*RB1*RB2*RIN*RL
          + CB1*CT1*CT2*R3*RB1*RB2*RIN*RL + CB1*CB2*CT2*R3*RB1*RB2*RIN*RL
          + CB1*CB2*CT1*R3*RB1*RB2*RIN*RL + CB2*CT1*CT2*R2*RB1*RB2*RIN*RL
          + CB1*CB2*CT2*R2*RB1*RB2*RIN*RL + CB1*CT1*CT2*R1*RB1*RB2*RIN*RL
          + CB1*CB2*CT1*R1*RB1*RB2*RIN*RL + CB2*CT1*CT2*R2*R3*RB2*RIN*RL
          + CB2*CT1*CT2*R1*R3*RB2*RIN*RL + CB2*CT1*CT2*R1*R2*RB2*RIN*RL
          + CB1*CT1*CT2*R2*R3*RB1*RIN*RL + CB1*CT1*CT2*R1*R3*RB1*RIN*RL
          + CB1*CT1*CT2*R1*R2*RB1*RIN*RL + CB1*CB2*CT2*R2*R3*RB1*RB2*RL
          + CB1*CB2*CT1*R2*R3*RB1*RB2*RL + CB1*CB2*CT2*R1*R3*RB1*RB2*RL
          + CB1*CB2*CT1*R1*R3*RB1*RB2*RL + CB1*CB2*CT2*R1*R2*RB1*RB2*RL
          + CB1*CB2*CT1*R1*R2*RB1*RB2*RL + CB1*CB2*CT1*R2*R3*RB1*RB2*RIN
          + CB1*CB2*CT1*R1*R3*RB1*RB2*RIN + CB1*CB2*CT1*R1*R2*RB1*RB2*RIN);

        james_D2 = (CT1*CT2*RIN*RL*RT1*RT2 + CT1*CT2*RB2*RL*RT1*RT2
          + CT1*CT2*RB1*RL*RT1*RT2 + CT1*CT2*R2*RL*RT1*RT2 + CT1*CT2*R1*RL*RT1*RT2
          + CT1*CT2*RB2*RIN*RT1*RT2 + CT1*CT2*R3*RIN*RT1*RT2 + CT1*CT2*R2*RIN*RT1*RT2
          + CT1*CT2*RB1*RB2*RT1*RT2 + CT1*CT2*R3*RB2*RT1*RT2 + CT1*CT2*R1*RB2*RT1*RT2
          + CT1*CT2*R3*RB1*RT1*RT2 + CT1*CT2*R2*RB1*RT1*RT2 + CT1*CT2*R2*R3*RT1*RT2
          + CT1*CT2*R1*R3*RT1*RT2 + CT1*CT2*R1*R2*RT1*RT2 + CB2*CT2*RB2*RIN*RL*RT2
          + CT1*CT2*RB1*RIN*RL*RT2 + CB1*CT2*RB1*RIN*RL*RT2 + CT1*CT2*R3*RIN*RL*RT2
          + CT1*CT2*R1*RIN*RL*RT2 + CT1*CT2*RB1*RB2*RL*RT2 + CB2*CT2*RB1*RB2*RL*RT2
          + CB1*CT2*RB1*RB2*RL*RT2 + CT1*CT2*R3*RB2*RL*RT2 + CB2*CT2*R2*RB2*RL*RT2
          + CT1*CT2*R1*RB2*RL*RT2 + CB2*CT2*R1*RB2*RL*RT2 + CT1*CT2*R3*RB1*RL*RT2
          + CT1*CT2*R2*RB1*RL*RT2 + CB1*CT2*R2*RB1*RL*RT2 + CB1*CT2*R1*RB1*RL*RT2
          + CT1*CT2*R2*R3*RL*RT2 + CT1*CT2*R1*R3*RL*RT2 + CT1*CT2*R1*R2*RL*RT2
          + CT1*CT2*RB1*RB2*RIN*RT2 + CB1*CT2*RB1*RB2*RIN*RT2 + CT1*CT2*R3*RB2*RIN*RT2
          + CB2*CT2*R3*RB2*RIN*RT2 + CB2*CT2*R2*RB2*RIN*RT2 + CT1*CT2*R1*RB2*RIN*RT2
          + CT1*CT2*R3*RB1*RIN*RT2 + CB1*CT2*R3*RB1*RIN*RT2 + CT1*CT2*R2*RB1*RIN*RT2
          + CB1*CT2*R2*RB1*RIN*RT2 + CT1*CT2*R2*R3*RIN*RT2 + CT1*CT2*R1*R3*RIN*RT2
          + CT1*CT2*R1*R2*RIN*RT2 + CB2*CT2*R3*RB1*RB2*RT2 + CB1*CT2*R3*RB1*RB2*RT2
          + CB2*CT2*R2*RB1*RB2*RT2 + CB1*CT2*R1*RB1*RB2*RT2 + CB2*CT2*R2*R3*RB2*RT2
          + CB2*CT2*R1*R3*RB2*RT2 + CB2*CT2*R1*R2*RB2*RT2 + CB1*CT2*R2*R3*RB1*RT2
          + CB1*CT2*R1*R3*RB1*RT2 + CB1*CT2*R1*R2*RB1*RT2 + CT1*CT2*RB2*RIN*RL*RT1
          + CB2*CT1*RB2*RIN*RL*RT1 + CB1*CT1*RB1*RIN*RL*RT1 + CT1*CT2*R3*RIN*RL*RT1
          + CT1*CT2*R2*RIN*RL*RT1 + CT1*CT2*RB1*RB2*RL*RT1 + CB2*CT1*RB1*RB2*RL*RT1
          + CB1*CT1*RB1*RB2*RL*RT1 + CT1*CT2*R3*RB2*RL*RT1 + CB2*CT1*R2*RB2*RL*RT1
          + CT1*CT2*R1*RB2*RL*RT1 + CB2*CT1*R1*RB2*RL*RT1 + CT1*CT2*R3*RB1*RL*RT1
          + CT1*CT2*R2*RB1*RL*RT1 + CB1*CT1*R2*RB1*RL*RT1 + CB1*CT1*R1*RB1*RL*RT1
          + CT1*CT2*R2*R3*RL*RT1 + CT1*CT2*R1*R3*RL*RT1 + CT1*CT2*R1*R2*RL*RT1
          + CB1*CT1*RB1*RB2*RIN*RT1 + CB2*CT1*R3*RB2*RIN*RT1 + CB2*CT1*R2*RB2*RIN*RT1
          + CB1*CT1*R3*RB1*RIN*RT1 + CB1*CT1*R2*RB1*RIN*RT1 + CB2*CT1*R3*RB1*RB2*RT1
          + CB1*CT1*R3*RB1*RB2*RT1 + CB2*CT1*R2*RB1*RB2*RT1 + CB1*CT1*R1*RB1*RB2*RT1
          + CB2*CT1*R2*R3*RB2*RT1 + CB2*CT1*R1*R3*RB2*RT1 + CB2*CT1*R1*R2*RB2*RT1
          + CB1*CT1*R2*R3*RB1*RT1 + CB1*CT1*R1*R3*RB1*RT1 + CB1*CT1*R1*R2*RB1*RT1
          + CT1*CT2*RB1*RB2*RIN*RL + CB1*CT2*RB1*RB2*RIN*RL + CB2*CT1*RB1*RB2*RIN*RL
          + CB1*CB2*RB1*RB2*RIN*RL + CT1*CT2*R3*RB2*RIN*RL + CB2*CT2*R3*RB2*RIN*RL
          + CB2*CT1*R3*RB2*RIN*RL + CB2*CT2*R2*RB2*RIN*RL + CT1*CT2*R1*RB2*RIN*RL
          + CB2*CT1*R1*RB2*RIN*RL + CT1*CT2*R3*RB1*RIN*RL + CB1*CT2*R3*RB1*RIN*RL
          + CB1*CT1*R3*RB1*RIN*RL + CT1*CT2*R2*RB1*RIN*RL + CB1*CT2*R2*RB1*RIN*RL
          + CB1*CT1*R1*RB1*RIN*RL + CT1*CT2*R2*R3*RIN*RL + CT1*CT2*R1*R3*RIN*RL
          + CT1*CT2*R1*R2*RIN*RL + CB2*CT2*R3*RB1*RB2*RL + CB1*CT2*R3*RB1*RB2*RL
          + CB2*CT1*R3*RB1*RB2*RL + CB1*CT1*R3*RB1*RB2*RL + CB2*CT2*R2*RB1*RB2*RL
          + CB2*CT1*R2*RB1*RB2*RL + CB1*CB2*R2*RB1*RB2*RL + CB1*CT2*R1*RB1*RB2*RL
          + CB1*CT1*R1*RB1*RB2*RL + CB1*CB2*R1*RB1*RB2*RL + CB2*CT2*R2*R3*RB2*RL
          + CB2*CT1*R2*R3*RB2*RL + CB2*CT2*R1*R3*RB2*RL + CB2*CT1*R1*R3*RB2*RL
          + CB2*CT2*R1*R2*RB2*RL + CB2*CT1*R1*R2*RB2*RL + CB1*CT2*R2*R3*RB1*RL
          + CB1*CT1*R2*R3*RB1*RL + CB1*CT2*R1*R3*RB1*RL + CB1*CT1*R1*R3*RB1*RL
          + CB1*CT2*R1*R2*RB1*RL + CB1*CT1*R1*R2*RB1*RL + CB2*CT1*R3*RB1*RB2*RIN
          + CB1*CT1*R3*RB1*RB2*RIN + CB1*CB2*R3*RB1*RB2*RIN + CB2*CT1*R2*RB1*RB2*RIN
          + CB1*CB2*R2*RB1*RB2*RIN + CB1*CT1*R1*RB1*RB2*RIN + CB2*CT1*R2*R3*RB2*RIN
          + CB2*CT1*R1*R3*RB2*RIN + CB2*CT1*R1*R2*RB2*RIN + CB1*CT1*R2*R3*RB1*RIN
          + CB1*CT1*R1*R3*RB1*RIN + CB1*CT1*R1*R2*RB1*RIN + CB1*CB2*R2*R3*RB1*RB2
          + CB1*CB2*R1*R3*RB1*RB2 + CB1*CB2*R1*R2*RB1*RB2);

        james_D1 = (CT2*RIN*RL*RT2 + CT2*RB2*RL*RT2 + CT2*RB1*RL*RT2 + CT2*R2*RL*RT2
          + CT2*R1*RL*RT2 + CT2*RB2*RIN*RT2 + CT2*R3*RIN*RT2 + CT2*R2*RIN*RT2
          + CT2*RB1*RB2*RT2 + CT2*R3*RB2*RT2 + CT2*R1*RB2*RT2 + CT2*R3*RB1*RT2
          + CT2*R2*RB1*RT2 + CT2*R2*R3*RT2 + CT2*R1*R3*RT2 + CT2*R1*R2*RT2
          + CT1*RIN*RL*RT1 + CT1*RB2*RL*RT1 + CT1*RB1*RL*RT1 + CT1*R2*RL*RT1
          + CT1*R1*RL*RT1 + CT1*RB2*RIN*RT1 + CT1*R3*RIN*RT1 + CT1*R2*RIN*RT1
          + CT1*RB1*RB2*RT1 + CT1*R3*RB2*RT1 + CT1*R1*RB2*RT1 + CT1*R3*RB1*RT1
          + CT1*R2*RB1*RT1 + CT1*R2*R3*RT1 + CT1*R1*R3*RT1 + CT1*R1*R2*RT1
          + CT2*RB2*RIN*RL + CB2*RB2*RIN*RL + CT1*RB1*RIN*RL + CB1*RB1*RIN*RL
          + CT2*R3*RIN*RL + CT1*R3*RIN*RL + CT2*R2*RIN*RL + CT1*R1*RIN*RL
          + CT2*RB1*RB2*RL + CT1*RB1*RB2*RL + CB2*RB1*RB2*RL + CB1*RB1*RB2*RL
          + CT2*R3*RB2*RL + CT1*R3*RB2*RL + CB2*R2*RB2*RL + CT2*R1*RB2*RL
          + CT1*R1*RB2*RL + CB2*R1*RB2*RL + CT2*R3*RB1*RL + CT1*R3*RB1*RL
          + CT2*R2*RB1*RL + CT1*R2*RB1*RL + CB1*R2*RB1*RL + CB1*R1*RB1*RL + CT2*R2*R3*RL
          + CT1*R2*R3*RL + CT2*R1*R3*RL + CT1*R1*R3*RL + CT2*R1*R2*RL + CT1*R1*R2*RL
          + CT1*RB1*RB2*RIN + CB1*RB1*RB2*RIN + CT1*R3*RB2*RIN + CB2*R3*RB2*RIN
          + CB2*R2*RB2*RIN + CT1*R1*RB2*RIN + CT1*R3*RB1*RIN + CB1*R3*RB1*RIN
          + CT1*R2*RB1*RIN + CB1*R2*RB1*RIN + CT1*R2*R3*RIN + CT1*R1*R3*RIN
          + CT1*R1*R2*RIN + CB2*R3*RB1*RB2 + CB1*R3*RB1*RB2 + CB2*R2*RB1*RB2
          + CB1*R1*RB1*RB2 + CB2*R2*R3*RB2 + CB2*R1*R3*RB2 + CB2*R1*R2*RB2
          + CB1*R2*R3*RB1 + CB1*R1*R3*RB1 + CB1*R1*R2*RB1);

        james_D0 = (RIN*RL + RB2*RL + RB1*RL + R2*RL + R1*RL + RB2*RIN + R3*RIN + R2*RIN
          + RB1*RB2 + R3*RB2 + R1*RB2 + R3*RB1 + R2*RB1 + R2*R3 + R1*R3 + R1*R2);

        // Numerator
        james_N4 = (CB1*CB2*CT1*CT2*R2*RB1*RB2*RL*RT1*RT2
          + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT2 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT2
          + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT2);

        james_N3 = (CB1*CT1*CT2*RB1*RB2*RL*RT1*RT2 + CB2*CT1*CT2*R2*RB2*RL*RT1*RT2
          + CB1*CT1*CT2*R2*RB1*RL*RT1*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RL*RT2
          + CB1*CT1*CT2*R3*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RL*RT2
          + CB1*CB2*CT2*R2*RB1*RB2*RL*RT2 + CB1*CT1*CT2*R1*RB1*RB2*RL*RT2
          + CB2*CT1*CT2*R2*R3*RB2*RL*RT2 + CB2*CT1*CT2*R1*R3*RB2*RL*RT2
          + CB2*CT1*CT2*R1*R2*RB2*RL*RT2 + CB1*CT1*CT2*R2*R3*RB1*RL*RT2
          + CB1*CT1*CT2*R1*R3*RB1*RL*RT2 + CB1*CT1*CT2*R1*R2*RB1*RL*RT2
          + CB1*CB2*CT1*R2*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R2*R3*RB1*RB2*RL
          + CB1*CB2*CT1*R1*R3*RB1*RB2*RL + CB1*CB2*CT1*R1*R2*RB1*RB2*RL);

        james_N2 = (CT1*CT2*RB2*RL*RT1*RT2 + CT1*CT2*R2*RL*RT1*RT2 + CT1*CT2*RB1*RB2*RL*RT2
          + CB1*CT2*RB1*RB2*RL*RT2 + CT1*CT2*R3*RB2*RL*RT2 + CB2*CT2*R2*RB2*RL*RT2
          + CT1*CT2*R1*RB2*RL*RT2 + CT1*CT2*R3*RB1*RL*RT2 + CT1*CT2*R2*RB1*RL*RT2
          + CB1*CT2*R2*RB1*RL*RT2 + CT1*CT2*R2*R3*RL*RT2 + CT1*CT2*R1*R3*RL*RT2
          + CT1*CT2*R1*R2*RL*RT2 + CB1*CT1*RB1*RB2*RL*RT1 + CB2*CT1*R2*RB2*RL*RT1
          + CB1*CT1*R2*RB1*RL*RT1 + CB2*CT1*R3*RB1*RB2*RL + CB1*CT1*R3*RB1*RB2*RL
          + CB2*CT1*R2*RB1*RB2*RL + CB1*CB2*R2*RB1*RB2*RL + CB1*CT1*R1*RB1*RB2*RL
          + CB2*CT1*R2*R3*RB2*RL + CB2*CT1*R1*R3*RB2*RL + CB2*CT1*R1*R2*RB2*RL
          + CB1*CT1*R2*R3*RB1*RL + CB1*CT1*R1*R3*RB1*RL + CB1*CT1*R1*R2*RB1*RL);

        james_N1 = (CT2*RB2*RL*RT2 + CT2*R2*RL*RT2 + CT1*RB2*RL*RT1 + CT1*R2*RL*RT1
          + CT1*RB1*RB2*RL + CB1*RB1*RB2*RL + CT1*R3*RB2*RL + CB2*R2*RB2*RL
          + CT1*R1*RB2*RL + CT1*R3*RB1*RL + CT1*R2*RB1*RL + CB1*R2*RB1*RL + CT1*R2*R3*RL
          + CT1*R1*R3*RL + CT1*R1*R2*RL);

        james_N0 = RB2*RL + R2*RL;

        // 4th Order Bilinear Transform
        double c = 2.0 * sampleRate;
        double c2 = c * c;
        double c3 = c2 * c;
        double c4 = c2 * c2;

        double vN0 = james_N0;
        double vN1 = james_N1 * c;
        double vN2 = james_N2 * c2;
        double vN3 = james_N3 * c3;
        double vN4 = james_N4 * c4;

        double vD0 = james_D0;
        double vD1 = james_D1 * c;
        double vD2 = james_D2 * c2;
        double vD3 = james_D3 * c3;
        double vD4 = james_D4 * c4;

        // Using pre-computed transformation matrix M:
        // [ 1,  1,  1,  1,  1 ]
        // [ 4,  2,  0, -2, -4 ]
        // [ 6,  0, -2,  0,  6 ]
        // [ 4, -2,  0,  2, -4 ]
        // [ 1, -1,  1, -1,  1 ]
        double B0 =  1.0*vN0 + 1.0*vN1 + 1.0*vN2 + 1.0*vN3 + 1.0*vN4;
        double B1 =  4.0*vN0 + 2.0*vN1 + 0.0*vN2 - 2.0*vN3 - 4.0*vN4;
        double B2 =  6.0*vN0 + 0.0*vN1 - 2.0*vN2 + 0.0*vN3 + 6.0*vN4;
        double B3 =  4.0*vN0 - 2.0*vN1 + 0.0*vN2 + 2.0*vN3 - 4.0*vN4;
        double B4 =  1.0*vN0 - 1.0*vN1 + 1.0*vN2 - 1.0*vN3 + 1.0*vN4;

        double A0 =  1.0*vD0 + 1.0*vD1 + 1.0*vD2 + 1.0*vD3 + 1.0*vD4;
        double A1 =  4.0*vD0 + 2.0*vD1 + 0.0*vD2 - 2.0*vD3 - 4.0*vD4;
        double A2 =  6.0*vD0 + 0.0*vD1 - 2.0*vD2 + 0.0*vD3 + 6.0*vD4;
        double A3 =  4.0*vD0 - 2.0*vD1 + 0.0*vD2 + 2.0*vD3 - 4.0*vD4;
        double A4 =  1.0*vD0 - 1.0*vD1 + 1.0*vD2 - 1.0*vD3 + 1.0*vD4;

        double invA0 = 1.0 / A0;
        b0_d = B0 * invA0;
        b1_d = B1 * invA0;
        b2_d = B2 * invA0;
        b3_d = B3 * invA0;
        b4_d = B4 * invA0;

        a1_d = A1 * invA0;
        a2_d = A2 * invA0;
        a3_d = A3 * invA0;
        a4_d = A4 * invA0;
    }

    Model currentModel = Model::FenderBassman;
    CircuitProfile activeProfile;
    double sampleRate = 44100.0;

    double t = 0.5;
    double m = 0.5;
    double l = 0.5;

    // Continuous 3rd-order coefficients
    double a0 = 1.0, a1 = 0.0, a2 = 0.0, a3 = 0.0;
    double b1 = 0.0, b2 = 0.0, b3 = 0.0;

    // Continuous 4th-order James coefficients
    double james_N0 = 0.0, james_N1 = 0.0, james_N2 = 0.0, james_N3 = 0.0, james_N4 = 0.0;
    double james_D0 = 1.0, james_D1 = 0.0, james_D2 = 0.0, james_D3 = 0.0, james_D4 = 0.0;

    // Normalized discrete coefficients
    double b0_d = 0.0, b1_d = 0.0, b2_d = 0.0, b3_d = 0.0, b4_d = 0.0;
    double a1_d = 0.0, a2_d = 0.0, a3_d = 0.0, a4_d = 0.0;

    // TDF-II state registers (4th-order)
    double d1 = 0.0;
    double d2 = 0.0;
    double d3 = 0.0;
    double d4 = 0.0;
};
