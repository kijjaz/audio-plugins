#pragma once

#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class RimshotVoice
 * @brief Models the TR-808 Rimshot / Claves circuit.
 * Uses two specifically tuned bandpass filters excited by a narrow pulse.
 */
class RimshotVoice : public Voice {
public:
  enum Params {
    kTune = 0,
    kLevel,
    kPadding,
    kPadding2,
    kVelToTone,
    kVelToDecay
  };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    // Typical Rimshot frequencies: ~1.7kHz and ~450Hz
    bpf1.setParameters(sampleRate, 1700.0, 10.0);
    bpf2.setParameters(sampleRate, 450.0, 5.0);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    bpf1.reset();
    bpf2.reset();
    slewLimiter.reset();
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kTune:
      tuneParam = value;
      break;
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

  bool isActive() override { return isTriggered && pulseTimer > 0; }
  void trigger(float velocity) override {
    Voice::trigger(velocity);
    pulseTimer = 2; // Very short trigger pulse
  }

  double renderSample() override {
    double input = 0.0;
    if (pulseTimer > 0) {
      input = 1.0 * velocity;
      pulseTimer--;
    }

    // 2. Velocity Modulation
    // Velocity affects filter shift and level
    float modTune = std::fmax(
        0.0f, std::fmin(1.0f, tuneParam + (velocity - 0.5f) * velToTone));

    float accent = SharedResources::getInstance().getAccentLevel();
    double currentVelocity = velocity * (1.0 + accent * 0.4);

    // Tune shifts both filters
    double shift = 0.5 + modTune;
    bpf1.setParameters(sampleRate, 1700.0 * shift, 12.0);
    bpf2.setParameters(sampleRate, 450.0 * shift, 8.0);

    double out = (bpf1.process(input * currentVelocity) +
                  bpf2.process(input * currentVelocity) * 0.5);

    if (std::abs(out) < 0.0001 && pulseTimer == 0)
      isTriggered = false;

    // Saturation
    double slewed = slewLimiter.process(out, 2.0);
    double saturated =
        AnalogKit::diodeSaturate(slewed * 5.0, circuitConfig.diode);

    return saturated * levelParam;
  }

private:
  ZDFResonator bpf1;
  ZDFResonator bpf2;
  AnalogKit::SlewLimiter slewLimiter;

  float tuneParam = 0.5f;
  float levelParam = 0.8f;
  int pulseTimer = 0;
};
