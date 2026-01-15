#pragma once

#include <algorithm>
#include <cmath>

/**
 * @brief PolyBLEP Oscillator
 * Generates an alias-free square wave for the 808's metallic bank.
 */
class PolyBLEPOscillator {
public:
  PolyBLEPOscillator() : phase(0.0), phaseIncrement(0.0) {}

  void setFrequency(double frequency, double sampleRate) {
    currentFreq = frequency;
    if (sampleRate > 0.0)
      phaseIncrement = frequency / sampleRate;
  }

  double getFrequency() const { return currentFreq; }

  // Add random phase for the "inharmonicity" simulation
  void randomizePhase(double seed) { phase = std::fmod(std::abs(seed), 1.0); }

  /**
   * @brief Main process loop
   * @return One sample of an alias-free square wave.
   */
  double process() {
    double output = (phase < 0.5) ? 1.0 : -1.0;

    // Apply PolyBLEP correction at the discontinuities (0.0 and 0.5)
    output += polyBlep(phase);
    output -= polyBlep(std::fmod(phase + 0.5, 1.0));

    // Advance phase
    phase += phaseIncrement;
    if (phase >= 1.0)
      phase -= 1.0;

    return output;
  }

  void reset() { phase = 0.0; }

private:
  double phase = 0.0;
  double phaseIncrement = 0.0;
  double currentFreq = 440.0;

  /**
   * @brief Residual correction function for PolyBLEP
   * Valid for |t| < phaseIncrement
   */
  double polyBlep(double t) {
    double dt = phaseIncrement;
    // Avoid division by zero
    if (dt < 1.0e-9)
      return 0.0;

    if (t < dt) {
      t /= dt;
      // 2nd order polynomial
      return t + t - t * t - 1.0;
    } else if (t > 1.0 - dt) {
      t = (t - 1.0) / dt;
      return t * t + t + t + 1.0;
    }
    return 0.0;
  }
};
