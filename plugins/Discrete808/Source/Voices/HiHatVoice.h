#pragma once

#include "../DSP/PolyBLEPOscillator.h"
#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class HiHatVoice
 * @brief Models the TR-808 Hi-Hat (Open and Closed).
 * Uses a bank of 6 Schmitt-trigger oscillators processed through a BPF.
 */
class HiHatVoice : public Voice {
public:
  enum Params { kDecay = 0, kTone, kLevel, kPadding, kVelToTone, kVelToDecay };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    double freqs[] = {263.0, 400.0, 421.0, 474.0, 587.0, 845.0};
    for (int i = 0; i < 6; ++i) {
      oscs[i].setFrequency(freqs[i], sampleRate);
      oscs[i].randomizePhase(i * 0.123);
    }
    bpf.setParameters(sampleRate, 7000.0, 2.0);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    envelope = 0.0;
    slewLimiter.reset();
    bpf.reset();
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kDecay:
      decayParam = value;
      break;
    case kTone:
      toneParam = value;
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

  bool isActive() override { return isTriggered && envelope > 0.0001; }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    envelope = 1.0;
  }

  double renderSample() override {
    // 1. Mix 6 Square-wave Oscillators
    double sum = 0.0;
    for (int i = 0; i < 6; ++i) {
      sum += oscs[i].process();
    }

    // 2. Bandpass Filter (High focus)
    // 2. Velocity Modulation
    float modTone = std::fmax(
        0.0f, std::fmin(1.0f, toneParam + (velocity - 0.5f) * velToTone));
    float modDecay = std::fmax(
        0.0f, std::fmin(1.0f, decayParam + (velocity - 0.5f) * velToDecay));

    // 2. Bandpass Filter (High focus)
    double bpfFreq = 3000.0 + (modTone * 7000.0);
    bpf.setParameters(sampleRate, bpfFreq, 1.5);
    double filtered = bpf.process(sum);

    double envDecay = 0.99 + (modDecay * 0.0095);
    envDecay = std::pow(envDecay, 1.0 + circuitConfig.capAge * 0.5);
    envelope *= envDecay;
    if (envelope < 0.0001)
      isTriggered = false;

    // 4. Output Stage
    double out = filtered * envelope *
                 (1.0 + SharedResources::getInstance().getAccentLevel() * 0.3) *
                 velocity;

    // Slew limit for high-frequency transients
    double slewRate =
        (circuitConfig.opAmp == AnalogKit::OpAmpModel::kTL072) ? 13.0 : 1.0;
    double slewed = slewLimiter.process(out, slewRate);

    double output = AnalogKit::diodeSaturate(slewed * 4.0, circuitConfig.diode);

    return output * levelParam;
  }

private:
  PolyBLEPOscillator oscs[6];
  ZDFResonator bpf;
  AnalogKit::SlewLimiter slewLimiter;

  float decayParam = 0.5f;
  float toneParam = 0.5f;
  float levelParam = 0.8f;

  double envelope = 0.0;
};
