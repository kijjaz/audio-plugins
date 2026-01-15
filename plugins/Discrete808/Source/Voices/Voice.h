#pragma once

#include "../DSP/AnalogKit.h"
#include "../DSP/SharedResources.h"
#include <algorithm>
#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * @class Voice
 * @brief Base class for all Discrete808 voices.
 */
class Voice {
public:
  virtual ~Voice() {}

  virtual void prepare(double sampleRate) {
    this->sampleRate = sampleRate;
    reset();
  }

  virtual void reset() {}

  // Parameter index usually matches the enum in the derived class
  virtual void setParameter(int parameterIndex, float value) {}

  virtual void setCircuitConfig(AnalogKit::CircuitConfig config) {
    circuitConfig = config;
  }

  // Set unique ID for this voice instance (helps with per-voice drift)
  void setVoiceID(int id) { voiceID = id; }

  // Trigger the voice with a velocity (0.0 to 1.0)
  virtual void trigger(float velocity) {
    this->velocity = velocity;
    isTriggered = true;
    envelopeValue = 1.0f; // Reset envelope for soft restart
  }

  // Check if voice is still audibly active (for tail preservation)
  virtual bool isActive() { return isTriggered; }

  /**
   * @brief Process a block of audio.
   */
  virtual void process(float *output, int numSamples) {
    if (!isActive())
      return;

    for (int i = 0; i < numSamples; ++i) {
      output[i] += (float)renderSample();
    }
  }

  // Render a single sample (must be implemented by derived class)
  virtual double renderSample() = 0;

protected:
  double sampleRate = 44100.0;
  float velocity = 0.0f;
  bool isTriggered = false;
  float envelopeValue = 0.0f; // Track main envelope state
  int voiceID = 0;
  AnalogKit::CircuitConfig circuitConfig;

  // Modulation Parameters
  float velToTone = 0.0f;
  float velToDecay = 0.0f;
};
