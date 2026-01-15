#pragma once

#include "Voice.h"

/**
 * @class MaracasVoice
 * @brief Models the TR-808 Maracas circuit.
 * Uses high-passed noise with a very short, sharp envelope.
 */
class MaracasVoice : public Voice {
public:
  enum Params {
    kLevel = 0,
    kPadding,
    kPadding2,
    kPadding3,
    kVelToTone,
    kVelToDecay
  };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    envelope = 0.0;
    lastNoise = 0.0;
    lastHpNoise = 0.0;
    slewLimiter.reset();
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kLevel:
      levelParam = value;
      break;
    case kVelToTone:
      velToTone = value;
      break;
    case kVelToDecay:
      velToDecay = value;
      break;
    }
  }

  bool isActive() override { return isTriggered && envelope > 0.0001; }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    envelope = 1.0;
  }

  double renderSample() override {
    if (envelope < 0.0001)
      return 0.0;

    // 1. Shared Noise
    double noise = SharedResources::getInstance().getNoiseSample();

    // 2. Velocity Modulation
    float modBright = std::fmax(
        0.0f,
        std::fmin(1.0f, (0.95f) * (1.0f + (velocity - 0.5f) * velToTone)));
    float modDecay = std::fmax(
        0.0f, std::fmin(1.0f, 0.99f + (velocity - 0.5f) * velToDecay * 0.005f));

    // 2. High Pass Filter (~5kHz)
    double hpNoise = noise - lastNoise + modBright * lastHpNoise;
    lastNoise = noise;
    lastHpNoise = hpNoise;

    // 3. Envelope
    envelope *= modDecay;
    if (envelope < 0.0001)
      isTriggered = false;

    // 4. Output
    float accent = SharedResources::getInstance().getAccentLevel();
    double out = hpNoise * envelope * (1.0 + accent * 0.5) * velocity;

    double slewed = slewLimiter.process(out, 5.0);
    double saturated =
        AnalogKit::diodeSaturate(slewed * 4.0, circuitConfig.diode);

    return saturated * levelParam;
  }

private:
  AnalogKit::SlewLimiter slewLimiter;
  float levelParam = 0.8f;
  double envelope = 0.0;
  double lastNoise = 0.0;
  double lastHpNoise = 0.0;
};
