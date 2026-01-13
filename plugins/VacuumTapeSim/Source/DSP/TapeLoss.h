#pragma once
#include <juce_dsp/juce_dsp.h>

/**
 * Simplified Wallace Loss simulation.
 * High-speed (30 ips) has more high-end.
 * Low-speed (9.5 ips) has more loss.
 */
class TapeLoss {
public:
  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;

    juce::dsp::IIR::Coefficients<float>::Ptr coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 18000.0f);
    filter.prepare(spec);
    filter.coefficients = coefficients;
  }

  void setSpeed(float ips) {
    // Speed in ips: 9.5, 15.0, 30.0
    // Higher speed = higher cutoff frequency.
    // We now use a more aggressive range for audible effect.
    float cutoff = 5000.0f + (ips / 30.0f) * 13000.0f;
    cutoff = std::min(cutoff, (float)sampleRate * 0.45f);

    *filter.coefficients =
        *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);
  }

  float processSample(float input) { return filter.processSample(input); }

private:
  double sampleRate = 44100.0;
  juce::dsp::IIR::Filter<float> filter;
};
