#pragma once
#include "DSP/Cabinet.h"
#include "DSP/ToneStack.h"
#include "DSP/TubeStage.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class IronStackAudioProcessor : public juce::AudioProcessor {
public:
  IronStackAudioProcessor();
  ~IronStackAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "IronStack"; }
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
  const ToneStack& getToneStack() const noexcept { return toneStackL; }
  const Cabinet& getCabinet() const noexcept { return cabinetL; }
  const TubeStage& getTubeStage() const noexcept { return inputTubeL; }

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // DSP Chains (Supports Mono -> Mono, Mono -> Stereo, and Stereo -> Stereo)
  // Left / Mono Preamp & Tone Stack
  TubeStage inputTubeL;
  ToneStack toneStackL;

  // Right Preamp & Tone Stack (active in true Stereo -> Stereo mode)
  TubeStage inputTubeR;
  ToneStack toneStackR;

  // Cabinet Simulations (Left & Right)
  Cabinet cabinetL;
  Cabinet cabinetR;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IronStackAudioProcessor)
};
