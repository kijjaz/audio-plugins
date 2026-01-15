#pragma once

#include <cmath>
#include <vector>

/**
 * @brief Zero-Delay Feedback (ZDF) Resonator
 * Based on the TPT (Topology Preserving Transform) structure.
 * Ideal for emulating the Bridged-T networks of the 808.
 */
class ZDFResonator {
public:
  ZDFResonator() : s1(0.0), s2(0.0), a1(0.0), a2(0.0), a3(0.0) {}

  /**
   * @param sampleRate The audio engine sample rate.
   * @param cutoff Frequency in Hz.
   * @param resonance Q factor (1.0 to 50.0+).
   */
  void setParameters(double sampleRate, double cutoff, double resonance) {
    // Protect against bad values
    if (cutoff <= 0.0)
      cutoff = 20.0;
    if (resonance <= 0.001)
      resonance = 0.001;

    // Pre-warp optimization (tan(PI * fc / fs))
    // For very high Q, TPT is stable even with simple bilinear transform
    double g = std::tan(val_PI * cutoff / sampleRate);
    double k = 1.0 / resonance;

    // TPT Coefficients
    // D = 1 + g*(g + k)
    double denominator = 1.0 / (1.0 + g * (g + k));

    a1 = denominator;
    a2 = g * a1;
    a3 = g * a2;
  }

  /**
   * @brief Process a single sample through the resonator.
   * @param input The trigger impulse.
   * @return The resonant "ring" output (Bandpass).
   */
  double process(double input) {
    // TPT Structure for Bandpass
    // y[n] = (g*u[n] + s1[n-1] - g*s2[n-1] - g*k*y[n]) / (1 + g*(g+k)) ?
    // Using the solved form from the paper:

    double v3 = input - s2;
    double v1 = a1 * s1 + a2 * v3;
    double v2 = s2 + a2 * s1 + a3 * v3;

    // Update internal state (Integrators)
    // x[n] = 2*y[n] - x[n-1]
    s1 = 2.0 * v1 - s1;
    s2 = 2.0 * v2 - s2;

    return v2; // Band-pass output
  }

  void reset() {
    s1 = 0.0;
    s2 = 0.0;
  }

private:
  double s1, s2;     // State registers
  double a1, a2, a3; // Coefficients

  const double val_PI = 3.14159265358979323846;
};
