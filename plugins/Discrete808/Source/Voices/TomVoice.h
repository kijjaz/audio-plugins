#pragma once

#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class TomVoice
 * @brief Models the TR-808 Tom-Tom circuit.
 */
class TomVoice : public Voice {
public:
  enum Params { kTune = 0, kDecay, kLevel, kPadding, kVelToTone, kVelToDecay };

  enum Type { kLow, kMid, kHigh };

  TomVoice(Type t) : type(t) {
    switch (type) {
    case kLow:
      baseFreq = 75.0;
      break;
    case kMid:
      baseFreq = 115.0;
      break;
    case kHigh:
      baseFreq = 165.0;
      break;
    }
  }

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    resonator.setParameters(sampleRate, baseFreq, 20.0);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    resonator.reset();
    clickEnvelope = 0.0;
    slewLimiter.reset();
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kTune:
      tuneParam = value;
      break;
    case kDecay:
      decayParam = value;
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

  bool isActive() override {
    return isTriggered && (clickEnvelope > 0.001 || pulseTimer > 0);
  }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    pulseTimer = 5;
    clickEnvelope = 1.0;
  }

  double renderSample() override {
    // 1. Exciter (Pulse + Noise Click)
    double input = 0.0;
    if (pulseTimer > 0) {
      input = 1.0;
      pulseTimer--;
    }

    // 2. Velocity Modulation
    // Velocity affects "stick" click intensity and decay length
    float modClick = std::fmax(
        0.0f, std::fmin(1.0f, 0.2f * (1.0f + (velocity - 0.5f) * velToTone)));
    float modDecay = std::fmax(
        0.0f,
        std::fmin(1.0f, decayParam * (1.0f + (velocity - 0.5f) * velToDecay)));

    if (clickEnvelope > 0.001) {
      double noise = SharedResources::getInstance().getNoiseSample();
      input += noise * clickEnvelope * modClick;
      clickEnvelope *= 0.99;
    }

    float accent = SharedResources::getInstance().getAccentLevel();
    double currentVelocity = velocity * (1.0 + accent * 0.4);

    // 3. Resonator
    double currentFreq = baseFreq * (0.5 + tuneParam) *
                         (1.0 + (circuitConfig.driftAmount * 0.01));
    double Q = 5.0 + (modDecay * 40.0);
    resonator.setParameters(sampleRate, currentFreq, Q);

    double out = resonator.process(input * currentVelocity);
    if (std::abs(out) < 0.0001 && clickEnvelope < 0.0001)
      isTriggered = false;

    // 4. Output Stage
    double slewRate =
        (circuitConfig.opAmp == AnalogKit::OpAmpModel::kTL072) ? 13.0 : 1.0;
    double slewed = slewLimiter.process(out, slewRate);
    double output = AnalogKit::diodeSaturate(slewed * 2.0, circuitConfig.diode);

    return output * levelParam;
  }

private:
  Type type;
  ZDFResonator resonator;
  AnalogKit::SlewLimiter slewLimiter;

  double baseFreq = 100.0;
  float tuneParam = 0.5f;
  float decayParam = 0.5f;
  float levelParam = 0.8f;

  int pulseTimer = 0;
  double clickEnvelope = 0.0;
};
