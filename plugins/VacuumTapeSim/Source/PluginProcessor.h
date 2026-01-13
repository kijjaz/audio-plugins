#pragma once
#include "DSP/TapeHysteresis.h"
#include "DSP/TapeLoss.h"
#include "DSP/TapeTransport.h"
#include "DSP/VacuumCompressor.h"
#include <juce_audio_processors/juce_audio_processors.h>

class VacuumTapeAudioProcessor : public juce::AudioProcessor {
public:
  VacuumTapeAudioProcessor();
  ~VacuumTapeAudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override { return true; }

  const juce::String getName() const override { return "Vacuum Tape Sim"; }
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

  std::atomic<float> gainReductionLevel{0.0f};
  std::atomic<float> saturationLevel{0.0f};
  int currentOversampling = 4;

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  // DSP components for each channel
  std::array<VacuumCompressor, 2> vacuum;
  std::array<TapeHysteresis, 2> hysteresis;
  std::array<TapeLoss, 2> loss;
  TapeTransport transport; // Handles both channels inside for delay management

  // Oversampling
  std::unique_ptr<juce::dsp::Oversampling<float>>
      oversampler1x; // Dummy or bypass
  std::unique_ptr<juce::dsp::Oversampling<float>> oversampler2x;
  std::unique_ptr<juce::dsp::Oversampling<float>> oversampler4x;
  std::unique_ptr<juce::dsp::Oversampling<float>> oversampler8x;

  void processUpsampledBlock(juce::dsp::AudioBlock<float> &block);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VacuumTapeAudioProcessor)
};
