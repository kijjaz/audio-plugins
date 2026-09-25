#pragma once

#include <cmath>
#include <random>

/**
 * @class AnalogKit
 * @brief High-fidelity analog component models for Discrete808 v2.
 *
 * Includes improved saturation curves, slew limiting, and component drift.
 */
class AnalogKit {
public:
  // --- Component Type Definitions ---

  enum class OpAmpModel {
    kIdeal,   // Infinite Slew, linear saturation
    kuPC4558, // Stock 808 (approx 1.0 V/us)
    kTL072,   // "Modern" Mod (approx 13.0 V/us)
    kBroken   // Starved/Dying (approx 0.15 V/us)
  };

  enum class DiodeModel {
    kSilicon,   // Standard (0.7V threshold, hard knee)
    kGermanium, // Vintage (0.3V threshold, soft knee)
    kLED        // High Headroom (1.8V threshold, hard knee)
  };

  /**
   * @struct CircuitConfig
   * @brief Per-voice component configuration.
   */
  struct CircuitConfig {
    OpAmpModel opAmp = OpAmpModel::kuPC4558;
    DiodeModel diode = DiodeModel::kSilicon;
    float capAge = 0.0f;      // 0.0 = Fresh, 1.0 = Aged/Leaky
    float driftAmount = 0.1f; // Thermal instability factor
    float noiseColor = 0.0f;  // -1.0 to 1.0 (Dark to Bright)
  };

  // --- Static Modeling Functions ---

  /**
   * @brief Saturated gain using a high-fidelity diode approximation.
   * Based on the Shockley diode equation: I = Is(exp(Vd/(nVt)) - 1)
   */
  static inline double diodeSaturate(double x, DiodeModel model) {
    switch (model) {
    case DiodeModel::kSilicon: {
      // Harder knee, 0.7V threshold
      const double threshold = 0.7;
      return std::tanh(x / threshold) * threshold;
    }
    case DiodeModel::kGermanium: {
      // Softer knee, 0.3V threshold, more harmonic "warmth"
      const double threshold = 0.3;
      // Asymmetric-ish feel by scaling
      return std::tanh(x * 1.5) * threshold;
    }
    case DiodeModel::kLED: {
      // High headroom, 1.8V threshold
      const double threshold = 1.8;
      return std::tanh(x / threshold) * threshold;
    }
    }
    return std::tanh(x);
  }

  /**
   * @brief Simple Op-Amp clipping (Supply Rail Limit)
   */
  static inline double railClip(double x, double vcc = 12.0) {
    if (x > vcc)
      return vcc;
    if (x < -vcc)
      return -vcc;
    return x;
  }

  // --- DSP Component Classes ---

  /**
   * @brief Slew rate limiter to simulate op-amp bandwidth limits.
   */
  class SlewLimiter {
  public:
    void prepare(double sr) {
      sampleRate = sr;
      reset();
    }
    void reset() { lastOutput = 0.0; }

    inline double process(double input, double v_per_us) {
      // Convert V/us to V/sample
      double maxDelta = (v_per_us * 1.0e6) / sampleRate;
      double delta = input - lastOutput;

      if (delta > maxDelta)
        delta = maxDelta;
      else if (delta < -maxDelta)
        delta = -maxDelta;

      lastOutput += delta;
      return lastOutput;
    }

  private:
    double lastOutput = 0.0;
    double sampleRate = 44100.0;
  };

  /**
   * @brief One-pole Lowpass Filter for simple tone modeling.
   */
  class OnePoleLPF {
  public:
    void reset() { z1 = 0.0; }
    inline double process(double x, double a0) {
      z1 = x * a0 + z1 * (1.0 - a0);
      return z1;
    }

  private:
    double z1 = 0.0;
  };

  /**
   * @brief Transistor Noise Generator (Shared Source)
   * The TR-808 uses a specific white noise generator with a 12kHz LPF.
   */
  class NoiseSource {
  public:
    NoiseSource() : dist(-1.0, 1.0) {
      std::random_device rd;
      rng.seed(rd());
    }

    inline double getNextSample() {
      double white = dist(rng);
      // 12kHz LPF approx at 44.1k: y = 0.8*x + 0.2*y-1
      lastOutput = 0.7 * white + 0.3 * lastOutput;
      return lastOutput;
    }

  private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    double lastOutput = 0.0;
  };
};
