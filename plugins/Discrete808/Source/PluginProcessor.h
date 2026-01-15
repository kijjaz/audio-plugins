#pragma once

#include "Voices/ClapVoice.h"
#include "Voices/CowbellVoice.h"
#include "Voices/CymbalVoice.h"
#include "Voices/HiHatVoice.h"
#include "Voices/KickVoice.h"
#include "Voices/MaracasVoice.h"
#include "Voices/RimshotVoice.h"
#include "Voices/SnareVoice.h"
#include "Voices/TomVoice.h"
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>

class Discrete808AudioProcessor : public juce::AudioProcessor {
public:
  enum VoiceIndex {
    kKick = 0,
    kSnare,
    kLowTom,
    kMidTom,
    kHighTom,
    kRimshot,
    kClap,
    kCowbell,
    kMaracas,
    kClosedHat,
    kOpenHat,
    kCymbal,
    kNumVoices
  };

  Discrete808AudioProcessor();
  ~Discrete808AudioProcessor() override;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

  bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
  void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

  juce::AudioProcessorEditor *createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String &newName) override;

  void getStateInformation(juce::MemoryBlock &destData) override;
  void setStateInformation(const void *data, int sizeInBytes) override;

  // Parameters
  juce::AudioProcessorValueTreeState apvts;

private:
  juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

  KickVoice kick;
  SnareVoice snare;
  TomVoice lowTom{TomVoice::kLow};
  TomVoice midTom{TomVoice::kMid};
  TomVoice highTom{TomVoice::kHigh};
  RimshotVoice rimshot;
  ClapVoice clap;
  CowbellVoice cowbell;
  MaracasVoice maracas;
  HiHatVoice closedHat;
  HiHatVoice openHat;
  CymbalVoice cymbal;

  std::vector<Voice *> voices;

  // Central Accent Level (triggered by MIDI Velocity/Accent track)
  float globalAccent = 0.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Discrete808AudioProcessor)
};
