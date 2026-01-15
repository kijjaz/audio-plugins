#pragma once

#include "AnalogKit.h"

/**
 * @class SharedResources
 * @brief Singleton container for shared DSP state (Noise, Accent Bus).
 *
 * In the TR-808, many voices share the same noise source and are affected
 * by a global accent voltage. This class simulates those global behaviors.
 */
class SharedResources {
public:
  static SharedResources &getInstance() {
    static SharedResources instance;
    return instance;
  }

  void setAccentLevel(float level) { accentLevel = level; }
  float getAccentLevel() const { return accentLevel; }

  /**
   * @brief Generates the shared "transistor noise" sample.
   * All voices calling this in the same process callback will get the same
   * value.
   */
  void updateNoise() { currentNoiseSample = noiseSource.getNextSample(); }

  float getNoiseSample() const { return (float)currentNoiseSample; }

private:
  SharedResources() = default;

  float accentLevel = 0.0f;
  double currentNoiseSample = 0.0;
  AnalogKit::NoiseSource noiseSource;

  // Prevent copying
  SharedResources(const SharedResources &) = delete;
  SharedResources &operator=(const SharedResources &) = delete;
};
