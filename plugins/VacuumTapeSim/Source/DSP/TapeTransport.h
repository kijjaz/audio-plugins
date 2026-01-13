#pragma once
#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>
#include <random>

class TapeTransport {
public:
  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;
    delayLine.prepare(spec);
    delayLine.setMaximumDelayInSamples(spec.sampleRate *
                                       0.1); // 100ms max jitter

    phaseWow = 0.0f;
  }

  void setParameters(float wow, float flutter) {
    this->wowAmount = wow;
    this->flutterAmount = flutter;
  }

  float processSample(int channel, float input) {
    // Wow: Low frequency cyclical drift with "drunk" characteristics
    // We add a tiny amount of random drift to the frequency each sample
    std::uniform_real_distribution<float> freqDriftDist(-0.0001f, 0.0001f);
    targetFreq += freqDriftDist(gen);
    targetFreq = std::clamp(targetFreq, 0.5f, 2.0f);

    // Smooth the frequency change
    currentFreq = currentFreq * 0.999f + targetFreq * 0.001f;

    phaseWow += (2.0f * juce::MathConstants<float>::pi * currentFreq) /
                (float)sampleRate;
    if (phaseWow > 2.0f * juce::MathConstants<float>::pi)
      phaseWow -= 2.0f * juce::MathConstants<float>::pi;

    // Drunk walk offset for extra organic drifting
    std::uniform_real_distribution<float> wanderDist(-0.001f, 0.001f);
    drunkWander += wanderDist(gen);
    drunkWander = std::clamp(drunkWander, -1.0f, 1.0f);

    float wowMod =
        (std::sin(phaseWow) + drunkWander * 0.5f) * wowAmount * 50.0f;

    // Flutter: High frequency stochastic jitter
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    float flutterMod = dist(gen) * flutterAmount * 10.0f;

    float totalDelayMs = 10.0f + wowMod + flutterMod; // Base 10ms delay
    float totalDelaySamples = totalDelayMs * (float)sampleRate / 1000.0f;

    delayLine.pushSample(channel, input);
    return delayLine.popSample(channel, totalDelaySamples);
  }

private:
  double sampleRate = 44100.0;
  juce::dsp::DelayLine<float> delayLine{48000};
  float phaseWow = 0.0f;
  float currentFreq = 1.0f;
  float targetFreq = 1.0f;
  float drunkWander = 0.0f;
  float wowAmount = 0.01f;
  float flutterAmount = 0.01f;

  std::mt19937 gen{std::random_device{}()};
};
