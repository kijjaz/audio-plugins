#pragma once
#include "DSP/Cabinet.h"
#include "DSP/ToneStack.h"
#include "DSP/TubeStage.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Fender59AudioProcessor : public juce::AudioProcessor {
public:
  Fender59AudioProcessor();
  ~Fender59AudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "Fender59"; }
  bool acceptsMidi() const override { return false; }
  bool producesMidi() const override { return false; }
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0; }

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override {}
  const juce::String getProgramName(int index) override { return "Default"; }
  void changeProgramName(int index, const juce::String &newName) override {}

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  juce::AudioProcessorValueTreeState apvts;
  const ToneStack& getToneStack() const noexcept { return toneStack; }
  const Cabinet& getCabinet() const noexcept { return cabinetL; }
  const TubeStage& getTubeStage() const noexcept { return inputTube; }

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // DSP Chains (Mono -> Stereo Sim)
  // Input Stage (Mono)
  TubeStage inputTube;

  // Tone Stack (Mono)
  ToneStack toneStack;

  // Output Stage (Stereo)
  Cabinet cabinetL;
  Cabinet cabinetR;

  // Oversampling (Optional, maybe later)
  // juce::dsp::Oversampling<float> oversampler;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fender59AudioProcessor)
};
