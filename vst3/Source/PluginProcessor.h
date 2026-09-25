#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

class CableLiftAudioProcessor : public juce::AudioProcessor {
public:
  CableLiftAudioProcessor();
  ~CableLiftAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "Cable Lift Enhancer"; }

  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override {}
  const juce::String getProgramName(int index) override { return {}; }
  void changeProgramName(int index, const juce::String &newName) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // Physics State
  juce::Random random;
  double humPhase = 0.0;

  // Filters & State
  float lp1L = 0.0f, lp1R = 0.0f;

  // Delay Buffers (48000 samples approx 1 sec)
  std::vector<float> delayBufferL, delayBufferR;
  int writeIdxL = 0, writeIdxR = 0;
  float jitterWalk = 0.0f;

  // Biquad State
  struct BiquadState {
    float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
  };
  BiquadState bqL, bqR;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CableLiftAudioProcessor)
};
