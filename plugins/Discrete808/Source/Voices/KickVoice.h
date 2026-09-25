#pragma once

#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class KickVoice
 * @brief Models the TR-808 Bass Drum circuit.
 */
class KickVoice : public Voice {
public:
  enum Params { kTune = 0, kDecay, kTone, kLevel, kVelToTone, kVelToDecay };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    resonator.setParameters(sampleRate, 50.0, 10.0);
    slewLimiter.prepare(sampleRate);
    toneLPF.reset();

    // Smoothers (20ms ramp)
    tuneParam.reset(sampleRate, 0.02);
    decayParam.reset(sampleRate, 0.02);
    toneParam.reset(sampleRate, 0.02);
    levelParam.reset(sampleRate, 0.02);
    velToTone.reset(sampleRate, 0.02);
    velToDecay.reset(sampleRate, 0.02);
  }

  void reset() override {
    pulseTimer = 0;
    pitchEnvelope = 0.0;
    resonator.reset();
    toneLPF.reset();
    slewLimiter.reset();
    // Don't reset smoothers here, or values pop
  }

  void setParameter(int index, float value) override {
    switch (index) {
    case kTune:
      tuneParam.setTargetValue(value);
      break;
    case kDecay:
      decayParam.setTargetValue(value);
      break;
    case kTone:
      toneParam.setTargetValue(value);
      break;
    case kLevel:
      levelParam.setTargetValue(value);
      break;
    case kVelToTone:
      velToTone.setTargetValue(value);
      break;
    case kVelToDecay:
      velToDecay.setTargetValue(value);
      break;
    }
  }

  void trigger(float velocity) override {
    Voice::trigger(velocity);
    pulseTimer = (int)(0.001 * sampleRate); // 1ms pulse
    pitchEnvelope = 1.0;                    // Reset "Sigh"
    isActiveFlag = true;
  }

  // Use a softer condition for isActive to prevent tail clipping
  // 808 Kick can ring for a long time.
  bool isActive() override { return isActiveFlag; }

  double renderSample() override {
    double outputLevel = levelParam.getNextValue(); // Tick smoothers
    double currentTune = tuneParam.getNextValue();
    double currentDecay = decayParam.getNextValue();
    double currentTone = toneParam.getNextValue();
    double currentV2T = velToTone.getNextValue();
    double currentV2D = velToDecay.getNextValue();

    if (!isTriggered)
      return 0.0;

    // 1. Trigger Pulse
    double input = 0.0;
    if (pulseTimer > 0) {
      input = 1.0 * velocity;
      pulseTimer--;
    }

    // 2. Velocity Modulation
    float modTone = std::fmax(
        0.0f, std::fmin(1.0f, currentTone + (velocity - 0.5f) * currentV2T));
    float modDecay = std::fmax(
        0.0f, std::fmin(1.0f, currentDecay + (velocity - 0.5f) * currentV2D));

    // 3. Global Accent Modulation
    float accent = SharedResources::getInstance().getAccentLevel();
    double currentVelocity = velocity * (1.0 + accent * 0.5);

    // 4. Pitch Envelope ("The Sigh")
    double baseFreq = 40.0 + (currentTune * 40.0);
    double pitchSweep = 80.0 * pitchEnvelope;
    double currentFreq = baseFreq + pitchSweep;

    pitchEnvelope *= 0.9994;
    // Don't cut off here! Just clamp to 0 for freq calc
    if (pitchEnvelope < 0.0001) {
      pitchEnvelope = 0.0;
    }

    // 5. Update Resonator
    double Q = 2.0 + (modDecay * modDecay * 80.0);
    resonator.setParameters(sampleRate, currentFreq, Q);

    double rawBody = resonator.process(input * currentVelocity);

    // Energy check to turn off voice
    // Simple envelope follower approximation or just raw value check
    // Since it's a resonator, we check if output is negligible
    if (std::abs(rawBody) < 0.00001 && pulseTimer <= 0 &&
        pitchEnvelope == 0.0) {
      killTimer++;
      if (killTimer > 100) { // arbitrary debounce
        isTriggered = false;
        isActiveFlag = false;
      }
    } else {
      killTimer = 0;
    }

    // 6. Tone / LPF stage
    double a0 = 0.05 + (modTone * 0.4);
    double filtered = toneLPF.process(rawBody, a0);

    // 7. Output Saturation
    double slewRate =
        (circuitConfig.opAmp == AnalogKit::OpAmpModel::kTL072) ? 13.0 : 1.0;
    double slewed = slewLimiter.process(filtered, slewRate);
    double output = AnalogKit::diodeSaturate(slewed * 4.0, circuitConfig.diode);

    return output * outputLevel;
  }

private:
  ZDFResonator resonator;
  AnalogKit::SlewLimiter slewLimiter;
  AnalogKit::OnePoleLPF toneLPF;

  // Render parameters
  juce::LinearSmoothedValue<float> tuneParam{0.94f};
  juce::LinearSmoothedValue<float> decayParam{0.5f};
  juce::LinearSmoothedValue<float> toneParam{0.5f};
  juce::LinearSmoothedValue<float> levelParam{0.7f};
  juce::LinearSmoothedValue<float> velToTone{0.0f};
  juce::LinearSmoothedValue<float> velToDecay{0.0f};

  int pulseTimer = 0;
  double pitchEnvelope = 0.0;
  bool isActiveFlag = false;
  int killTimer = 0;
};
