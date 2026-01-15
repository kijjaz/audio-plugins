#pragma once

#include "../DSP/PolyBLEPOscillator.h"
#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class CymbalVoice
 * @brief Models the TR-808 Cymbal circuit.
 * Uses the same 6-oscillator metallic bank as the Hi-Hats but with
 * different filtering and a longer, more complex decay structure.
 */
class CymbalVoice : public Voice {
public:
  enum Params { kTune = 0, kDecay, kLevel, kPadding, kVelToTone, kVelToDecay };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    double freqs[] = {263.0, 400.0, 421.0, 474.0, 587.0, 845.0};
    for (int i = 0; i < 6; ++i) {
      oscs[i].setFrequency(freqs[i], sampleRate);
      oscs[i].randomizePhase(i * 0.777);
    }
    bpf.setParameters(sampleRate, 8000.0, 1.0);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    envelope = 0.0;
    bpf.reset();
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

  bool isActive() override { return isTriggered && envelope > 0.0001; }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    envelope = 1.0;
  }

  double renderSample() override {
    // 1. Oscillators & Modulation
    float modTune = std::fmax(
        0.0f, std::fmin(1.0f, tuneParam + (velocity - 0.5f) * velToTone));
    float modDecay = std::fmax(
        0.0f, std::fmin(1.0f, decayParam + (velocity - 0.5f) * velToDecay));

    double shift = 0.5 + modTune;
    double sum = 0.0;
    for (int i = 0; i < 6; ++i) {
      sum += oscs[i].process();
    }

    // 2. High-pass / Band-pass filtering
    bpf.setParameters(sampleRate, 10000.0 * shift, 0.7);
    double filtered = bpf.process(sum);

    // 3. Envelope
    double envDecay = 0.9995 + (modDecay * 0.00045);
    envDecay = std::pow(envDecay, 1.0 + circuitConfig.capAge);
    envelope *= envDecay;
    if (envelope < 0.0001)
      isTriggered = false;

    // 4. Output
    float accent = SharedResources::getInstance().getAccentLevel();
    double out = filtered * envelope * (1.0 + accent * 0.2) * velocity;

    double slewed = slewLimiter.process(out, 1.5);
    double saturated =
        AnalogKit::diodeSaturate(slewed * 3.0, circuitConfig.diode);

    return saturated * levelParam;
  }

private:
  PolyBLEPOscillator oscs[6];
  ZDFResonator bpf;
  AnalogKit::SlewLimiter slewLimiter;

  float tuneParam = 0.5f;
  float decayParam = 0.5f;
  float levelParam = 0.8f;
  double envelope = 0.0;
};
