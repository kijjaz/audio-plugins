#pragma once
#include <juce_dsp/juce_dsp.h>

class Cabinet {
public:
  enum class Type { Left, Right };

  void prepare(const juce::dsp::ProcessSpec &spec, Type t) {
    type = t;
    // Chain: HighPass -> LowPass -> Notch

    // Fender Cab: Jensen P10R simulation (Roughly)
    // Highpass at 80Hz (Open back)
    // Lowpass at 4.5kHz (10 inch speaker rolloff)
    // Mid Scoop (Presence)

    float highPassFreq = 80.0f;
    float lowPassFreq = 4500.0f;

    if (type == Type::Right) {
      // Slight variation for stereo width
      highPassFreq = 85.0f;
      lowPassFreq = 4800.0f;
    }

    highPassFilter.setCoefficients(
        juce::IIRCoefficients::makeHighPass(spec.sampleRate, highPassFreq));
    lowPassFilter.setCoefficients(
        juce::IIRCoefficients::makeLowPass(spec.sampleRate, lowPassFreq));

    // No prepare needed for IIRFilter, just reset
    highPassFilter.reset();
    lowPassFilter.reset();
  }

  float processSample(float input) {
    return lowPassFilter.processSingleSampleRaw(
        highPassFilter.processSingleSampleRaw(input));
  }

private:
  Type type;
  juce::IIRFilter highPassFilter;
  juce::IIRFilter lowPassFilter;
};
