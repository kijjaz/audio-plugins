#pragma once

#include "Voice.h"

/**
 * @class ClapVoice
 * @brief Models the TR-808 Handclap circuit.
 * Features the iconic 4-pulse stutter and a long reverb-style noise tail.
 */
class ClapVoice : public Voice {
public:
  enum Params { kTone = 0, kDecay, kLevel, kPadding, kVelToTone, kVelToDecay };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    slewLimiter.prepare(sampleRate);
  }

  void reset() override {
    stutterCount = 0;
    delayTimer = 0;
    envelope = 0.0;
    reverbEnv = 0.0;
    isClapping = false;
    slewLimiter.reset();
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kTone:
      toneParam = value;
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

  bool isActive() override { return (isClapping || reverbEnv > 0.0001); }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    stutterCount = 0;
    delayTimer = 0;
    envelope = 0.0;
    reverbEnv = 0.0;
    isClapping = true;
  }

  double renderSample() override {
    if (!isClapping && reverbEnv < 0.0001)
      return 0.0;

    // 1. Stutter Logic (4 pulses approx 10ms apart)
    int pulseInterval = (int)(sampleRate * 0.010);

    if (isClapping) {
      if (delayTimer <= 0) {
        if (stutterCount < 3) {
          envelope = 1.0;
          stutterCount++;
          delayTimer = pulseInterval;
        } else {
          // Final hit + Reverb Tail
          envelope = 1.0;
          reverbEnv = 1.0;
          isClapping = false;
        }
      } else {
        delayTimer--;
      }
    }

    // 2. Audio Generation
    double noise = SharedResources::getInstance().getNoiseSample();

    // Envelopes
    envelope *= 0.6; // Quick staccato claps

    // Tone affects the decay of the tail (Shorter tail = brighter/tighter)
    double tailDecay = 0.999 + (decayParam * 0.0006);
    reverbEnv *= tailDecay;

    double finalEnv =
        (envelope + reverbEnv) *
        (1.0 + SharedResources::getInstance().getAccentLevel() * 0.3);

    // 3. Filtering (Clap is very mid-focused)
    // Simple bandpass logic via toneParam
    double lpAlpha = 0.1 + (toneParam * 0.4);
    lastFiltered = lastFiltered + lpAlpha * (noise * finalEnv - lastFiltered);

    // 4. Output Stage
    double slewRate =
        (circuitConfig.opAmp == AnalogKit::OpAmpModel::kTL072) ? 13.0 : 1.5;
    double slewed = slewLimiter.process(lastFiltered, slewRate);

    double saturated =
        AnalogKit::diodeSaturate(slewed * 4.0, circuitConfig.diode);

    return saturated * levelParam;
  }

private:
  AnalogKit::SlewLimiter slewLimiter;

  float toneParam = 0.5f;
  float decayParam = 0.5f;
  float levelParam = 0.8f;

  bool isClapping = false;
  int stutterCount = 0;
  int delayTimer = 0;

  double envelope = 0.0;
  double reverbEnv = 0.0;
  double lastFiltered = 0.0;
};
