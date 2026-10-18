#pragma once

#include <cmath>
#include <algorithm>

namespace Speka {

/**
 * ElectricalCircuit: Simulates the 7406 open-collector inverter stage,
 * the 33-ohm ballast resistor, the RL voice-coil with motional back-EMF,
 * and the power supply rail droop.
 */
class ElectricalCircuit {
public:
    ElectricalCircuit() = default;

    void prepare(double sampleRate) {
        fs = sampleRate;
        invFs = 1.0 / fs;
        reset();
    }

    void reset() {
        i_coil = 0.0f;
        supplyDroop = 0.0f;
    }

    void setPowerSupplySagAmount(float amount) {
        sagAmount = std::clamp(amount, 0.0f, 1.0f);
    }

    /**
     * Process one sample.
     * @param bitValue 1-bit input (0.0 or 1.0, or anti-aliased float in [0.0, 1.0])
     * @param coneVelocity Current mechanical velocity of voice coil (m/s)
     * @param blCurrent Current magnetic force factor Bl(x) in N/A
     * @return Output current i_coil through voice coil (Amperes)
     */
    float processSample(float bitValue, float coneVelocity, float blCurrent) {
        // Power rail sag (leaky integrator driven by average current)
        float targetDroop = (i_coil * 3.5f) * sagAmount;
        float droopAlpha = static_cast<float>(1.0 - std::exp(-invFs / 0.015)); // 15ms time constant
        supplyDroop += droopAlpha * (targetDroop - supplyDroop);
        float vRail = std::max(3.8f, 5.0f - supplyDroop);

        // Motional Back-EMF: eb = Bl * v
        float backEMF = blCurrent * coneVelocity;

        // Sub-stepping to maintain numerical stability with small L/R time constant
        constexpr int kSubSteps = 4;
        float dtSub = static_cast<float>(invFs) / static_cast<float>(kSubSteps);

        for (int step = 0; step < kSubSteps; ++step) {
            // When gate is active (bitValue > 0), transistor conducts to GND.
            // Effective circuit: V_drive = vRail * bitValue, R_total = R_ballast + R_e + R_sat
            // When gate is off: inductor attempts to maintain current; decays rapidly through reverse clamp
            float vDrive = vRail * bitValue;
            float rSwitch = (bitValue > 0.05f) ? 0.5f : (200.0f + 1000.0f * (1.0f - bitValue));
            float rTotal = R_ballast + R_e + rSwitch;

            // di/dt = (V_drive - R_total * i - backEMF) / L_e
            float di_dt = (vDrive - (rTotal * i_coil) - backEMF) / L_e;

            i_coil += di_dt * dtSub;

            // Voice coil current in single-ended open-collector cannot meaningfully reverse
            // because the NPN collector diode blocks reverse conduction from ground
            if (i_coil < 0.0f) {
                i_coil = 0.0f;
            }
        }

        // Clamp maximum current to physical limits of 5V through 33 + 8 ohms
        i_coil = std::clamp(i_coil, 0.0f, 0.16f);

        return i_coil;
    }

    float getCurrentAmperes() const { return i_coil; }
    float getSupplyVoltage() const { return 5.0f - supplyDroop; }

private:
    double fs = 44100.0;
    double invFs = 1.0 / 44100.0;

    // Component values from IBM 5150 schematic
    const float R_ballast = 33.0f;   // 33 Ohm series current limiter
    const float R_e       = 8.0f;    // 8 Ohm voice coil nominal DC resistance
    const float L_e       = 0.0002f; // 0.2 mH voice coil inductance

    float sagAmount = 0.5f;

    // State variables
    float i_coil = 0.0f;
    float supplyDroop = 0.0f;
};

} // namespace Speka
