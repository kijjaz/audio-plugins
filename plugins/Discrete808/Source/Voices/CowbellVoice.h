#pragma once

#include "../DSP/PolyBLEPOscillator.h"
#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class CowbellVoice
 * @brief Models the TR-808 Cowbell circuit.
 * Uses two detuned square wave oscillators (approx 540Hz and 800Hz)
 * summed and processed through a bandpass filter and an envelope.
 */
class CowbellVoice : public Voice {
public:
  enum Params { kTune = 0, kDecay, kLevel, kPadding, kVelToTone, kVelToDecay };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    osc1.setFrequency(540.0, sampleRate);
    osc2.setFrequency(800.0, sampleRate);
    bpf.setParameters(sampleRate, 600.0, 1.5);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    bpf.reset();
    envelope = 0.0;
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
    osc1.setFrequency(540.0 * shift, sampleRate);
    osc2.setFrequency(800.0 * shift, sampleRate);

    double sum = (osc1.process() + osc2.process()) * 0.5;

    // 2. Bandpass Filter
    bpf.setParameters(sampleRate, 800.0 * shift, 1.2);
    double filtered = bpf.process(sum);

    // 3. Envelope
    double envDecay = 0.999 + (modDecay * 0.0006);
    envDecay = std::pow(envDecay, 1.0 + circuitConfig.capAge);
    envelope *= envDecay;
    if (envelope < 0.0001)
      isTriggered = false;

    // 4. Output
    double accent = SharedResources::getInstance().getAccentLevel();
    double out = filtered * envelope * (1.0 + accent * 0.3) * velocity;

    double slewed = slewLimiter.process(out, 2.0);
    double saturated =
        AnalogKit::diodeSaturate(slewed * 4.0, circuitConfig.diode);

    return saturated * levelParam;
  }

private:
  PolyBLEPOscillator osc1;
  PolyBLEPOscillator osc2;
  ZDFResonator bpf;
  AnalogKit::SlewLimiter slewLimiter;

  float tuneParam = 0.5f;
  float decayParam = 0.5f;
  float levelParam = 0.8f;
  double envelope = 0.0;
};
