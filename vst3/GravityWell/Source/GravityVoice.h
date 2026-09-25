#pragma once
#include "GravityData.h"

// Simple One-Pole Lowpass
struct OnePoleLP {
  float y = 0.0f;
  void reset() { y = 0.0f; }
  float process(float x, float coeff) {
    y += (x - y) * coeff;
    return y;
  }
};

class GravityVoice : public juce::SynthesiserVoice {
public:
  bool canPlaySound(juce::SynthesiserSound *) override { return true; }

  void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *,
                 int /*currentPitchWheelPosition*/) override {
    currentNote = midiNoteNumber;
    currentVelocity = velocity;

    baseFreq = 440.0 * std::pow(2.0, (midiNoteNumber - 69) / 12.0);

    // Reset Phasers
    ph1 = 0.0;
    ph2 = 0.0;
    ph3 = 0.0;
    lp1.reset();
    lp2.reset();

    // ADSR Reset
    envLevel = 0.0f;
    envState = Attack;
  }

  void stopNote(float /*velocity*/, bool allowTailOff) override {
    if (allowTailOff) {
      envState = Release;
    } else {
      clearCurrentNote();
      envState = Idle;
    }
  }

  void pitchWheelMoved(int) override {}
  void controllerMoved(int, int) override {}

  void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample,
                       int numSamples) override {
    if (envState == Idle)
      return;

    // Params from Processor?
    // We need a way to pass per-block params (Gravity Data, Slider Vals).
    // Since Synthesiser::renderNextBlock doesn't pass extra data, we typically
    // use public members or pointers.

    // Let's assume these are set before calling render
    // But render is called by synth.renderNextBlock.
    // We can access a static or referenced data object?
    // Or simpler: The Voice has public float members for control, updated by
    // Processor loop.

    float *outL = outputBuffer.getWritePointer(0, startSample);
    float *outR = outputBuffer.getNumChannels() > 1
                      ? outputBuffer.getWritePointer(1, startSample)
                      : nullptr;
    double sr = getSampleRate();
    if (sr <= 0)
      sr = 44100.0;

    for (int s = 0; s < numSamples; ++s) {
      // --- ADSR ---
      float target = 0.0f;
      float rate = 0.001f; // Default

      switch (envState) {
      case Attack:
        target = 1.2f;
        rate = 0.002f;
        if (envLevel >= 1.0f) {
          envLevel = 1.0f;
          envState = Decay;
        }
        break;
      case Decay:
        target = 0.7f;
        rate = 0.0005f; // Sustain Level
        break;
      case Sustain:
        target = 0.7f;
        rate = 0.00001f;
        break;
      case Release:
        target = 0.0f;
        rate = 0.0005f;
        if (envLevel < 0.001f) {
          envLevel = 0.0f;
          envState = Idle;
          clearCurrentNote();
        }
        break;
      default:
        break;
      }
      envLevel += (target - envLevel) * rate;

      if (envState == Idle)
        break;

      // --- LFO from Gravity ---
      // d12 is distance 0..1
      // LFO = 1.0 - d12 * 0.3 * amt
      float lfo = 1.0f - (params.d12 * 0.3f * params.gravityAmt);
      float gain = envLevel * currentVelocity * lfo * 0.5f;

      // --- Synthesis ---

      // Osc 1 (Sun)
      float s1 = std::sin(ph1);
      ph1 += (2.0 * juce::MathConstants<double>::pi * baseFreq) / sr;
      if (ph1 > juce::MathConstants<double>::twoPi)
        ph1 -= juce::MathConstants<double>::twoPi;

      // Osc 2 (Planet)
      float mod1 = lp1.process(s1, 0.5f) * params.fmIdxPlanet; // Filtered FM
      double f2 = baseFreq * 2.0 + params.detune2;
      float s2 = std::sin(ph2 + mod1) + 0.3f * std::sin((ph2 + mod1) * 2.0f);

      ph2 += (2.0 * juce::MathConstants<double>::pi * f2) / sr;
      if (ph2 > juce::MathConstants<double>::twoPi)
        ph2 -= juce::MathConstants<double>::twoPi;

      // Osc 3 (Moon)
      float mod2 = lp2.process(s2, 0.5f) * params.fmIdxMoon;
      double f3 = f2 * 4.0;
      float s3 = std::sin(ph3 + mod2) * (0.1f + params.flare);

      ph3 += (2.0 * juce::MathConstants<double>::pi * f3) / sr;
      if (ph3 > juce::MathConstants<double>::twoPi)
        ph3 -= juce::MathConstants<double>::twoPi;

      // Mix & Pan
      float L = (s1 * 0.6f * params.sunL) + (s2 * 0.4f * params.plL) +
                (s3 * 0.3f * params.mnL);
      float R = (s1 * 0.6f * params.sunR) + (s2 * 0.4f * params.plR) +
                (s3 * 0.3f * params.mnR);

      outL[s] += L * gain;
      if (outR)
        outR[s] += R * gain;
    }
  }

  // Parameter Object pattern to inject data safely
  struct VoiceParams {
    float gravityAmt = 0.5f;
    float d12 = 0.0f;
    float fmIdxPlanet = 0.0f;
    float fmIdxMoon = 0.0f;
    float detune2 = 0.0f;
    float flare = 0.0f;
    float sunL = 0.5f, sunR = 0.5f;
    float plL = 0.0f, plR = 1.0f;
    float mnL = 0.0f, mnR = 1.0f;
  };

  void updateParams(const VoiceParams &p) { params = p; }

private:
  double baseFreq = 440.0;
  float currentVelocity = 0.0f;
  int currentNote = 0;

  // Osc Phases
  double ph1 = 0, ph2 = 0, ph3 = 0;
  OnePoleLP lp1, lp2;

  // ADSR
  enum State { Idle, Attack, Decay, Sustain, Release };
  State envState = Idle;
  float envLevel = 0.0f;

  VoiceParams params;
};

class GravitySound : public juce::SynthesiserSound {
public:
  bool appliesToNote(int) override { return true; }
  bool appliesToChannel(int) override { return true; }
};
