#pragma once

#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

#include <JuceHeader.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

// Include local headers AFTER JUCE headers
#include "GravityVoice.h"
#include "SpaceDust.h"

using namespace juce;

// Define Params
#define kGravityRows 10000
#define kGravityCols 12

class GravityWellAudioProcessor : public juce::AudioProcessor {
public:
  GravityWellAudioProcessor();
  ~GravityWellAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override {
    return true; // Simplified for now
  }

  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "Gravity Well"; }

  bool acceptsMidi() const override { return true; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override {
    return 2.0;
  } // Tail for SpaceDust

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override {}
  const juce::String getProgramName(int index) override { return {}; }
  void changeProgramName(int index, const juce::String &newName) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // Public for Editor access
  float readHead = 0.0f;
  juce::AudioProcessorValueTreeState apvts;

  // Scope FIFO
  // Simple atomic ring buffer
  static constexpr int kScopeSize = 512;
  float scopeFifo[kScopeSize * 2]; // Interleaved L/R
  std::atomic<int> scopeWritePtr{0};

  // Helper to read latest block (non-blocking)
  // Editor calls this
  void getScopeData(float *dest, int numSamples) {
    // Just grab the last numSamples relative to writePtr
    int wp = scopeWritePtr.load();
    int start = wp - numSamples * 2;
    if (start < 0)
      start += kScopeSize * 2;

    for (int i = 0; i < numSamples; ++i) {
      int idx = (start + i * 2) % (kScopeSize * 2);
      dest[i * 2] = scopeFifo[idx];         // L
      dest[i * 2 + 1] = scopeFifo[idx + 1]; // R
    }
  }

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // Polyphony
  juce::Synthesiser synth;

  // FX
  SpaceDust spaceDust;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GravityWellAudioProcessor)
};
