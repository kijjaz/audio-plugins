#pragma once

#include "../DSP/ZDFResonator.h"
#include "Voice.h"

/**
 * @class SnareVoice
 * @brief Models the TR-808 Snare Drum circuit.
 * Consists of dual oscillating body resonators and a snappy noise stage.
 */
class SnareVoice : public Voice {
public:
  enum Params {
    kTune = 0,
    kDecay,
    kTone,
    kSnappy,
    kLevel,
    kVelToTone,
    kVelToDecay
  };

  void prepare(double sampleRate) override {
    Voice::prepare(sampleRate);
    resonatorHigh.setParameters(sampleRate, 330.0, 15.0);
    resonatorLow.setParameters(sampleRate, 180.0, 15.0);
    slewLimiter.prepare(sampleRate);
    toneLPF.reset();

    // Smoothers
    tuneParam.reset(sampleRate, 0.02);
    decayParam.reset(sampleRate, 0.02);
    toneParam.reset(sampleRate, 0.02);
    snappyParam.reset(sampleRate, 0.02);
    levelParam.reset(sampleRate, 0.02);
    velToTone.reset(sampleRate, 0.02);
    velToDecay.reset(sampleRate, 0.02);
  }

  void reset() override {
    pulseTimer = 0;
    resonatorHigh.reset();
    resonatorLow.reset();
    snappyEnvelope = 0.0;
    toneLPF.reset();
    slewLimiter.reset();
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
    case kSnappy:
      snappyParam.setTargetValue(value);
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
    pulseTimer = (int)(0.001 * sampleRate);
    snappyEnvelope = 1.0;
  }

  double renderSample() override {
    double outputLevel = levelParam.getNextValue();
    double currentSnappy = snappyParam.getNextValue();
    double currentTone = toneParam.getNextValue();
    double currentDecay = decayParam.getNextValue();
    double currentV2T = velToTone.getNextValue();
    double currentV2D = velToDecay.getNextValue();
    /* Tune not used in render yet? */ tuneParam.getNextValue();

    // 1. Exciter pulse
    double input = 0.0;
    if (pulseTimer > 0) {
      input = 1.0 * velocity;
      pulseTimer--;
    }

    // 2. Velocity Modulation
    // Velocity affects snappy intensity and tone brightness
    // Note: juce::jlimit is not available here, using std::fmax/fmin or manual
    // clamp
    float modSnappy = std::fmax(
        0.0f, std::fmin(1.0f, currentSnappy *
                                  (1.0f + (velocity - 0.5f) * currentV2T)));
    float modDecay = std::fmax(
        0.0f,
        std::fmin(1.0f, currentTone * (1.0f + (velocity - 0.5f) * currentV2D)));

    // Update Resonators Param
    double q = 10.0 + (modDecay * 10.0);
    resonatorHigh.setParameters(sampleRate, 330.0 * 1.0, q);
    resonatorLow.setParameters(sampleRate, 180.0 * 1.0, q);

    // 3. Bodies
    double body1 = resonatorHigh.process(input);
    double body2 = resonatorLow.process(input);
    double bodySum = (body1 + body2) * 0.5;

    // 4. Snappy (Noise)
    double noise = SharedResources::getInstance().getNoiseSample();
    snappyEnvelope *= 0.9992;
    if (snappyEnvelope < 0.0001) {
      snappyEnvelope = 0.0;
      isTriggered = false;
    }

    double hpfFilteredAmount = 0.9 + (circuitConfig.noiseColor * 0.08);
    noiseHPF = noise * (1.0 - hpfFilteredAmount) + noiseHPF * hpfFilteredAmount;
    double highPassedNoise = noise - noiseHPF;

    double snappySignal = highPassedNoise * snappyEnvelope * modSnappy;

    // 5. Output
    double mixed = (bodySum * 0.4) + (snappySignal * 0.6);

    // Saturation
    double output =
        AnalogKit::diodeSaturate(mixed * (2.0 + velocity), circuitConfig.diode);
    return output * outputLevel;
  }

private:
  ZDFResonator resonatorHigh;
  ZDFResonator resonatorLow;
  AnalogKit::SlewLimiter slewLimiter;
  AnalogKit::OnePoleLPF toneLPF;

  juce::LinearSmoothedValue<float> tuneParam{0.5f};
  juce::LinearSmoothedValue<float> decayParam{0.5f};
  juce::LinearSmoothedValue<float> toneParam{0.5f};
  juce::LinearSmoothedValue<float> snappyParam{0.5f};
  juce::LinearSmoothedValue<float> levelParam{0.8f};
  juce::LinearSmoothedValue<float> velToTone{0.0f};
  juce::LinearSmoothedValue<float> velToDecay{0.0f};

  int pulseTimer = 0;
  double snappyEnvelope = 0.0;
  double noiseHPF = 0.0; // Filter state for snappy
  double lastNoise = 0.0;
  double lastHpNoise = 0.0;
};
