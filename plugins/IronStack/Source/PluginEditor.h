#pragma once
#include "PluginProcessor.h"
#include "UI/IronStackLookAndFeel.h"
#include "UI/ToneVisualizerComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

class IronStackEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
  explicit IronStackEditor(IronStackAudioProcessor &);
  ~IronStackEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

private:
  void timerCallback() override;

  IronStackAudioProcessor &audioProcessor;
  ironstack::IronStackLookAndFeel lookAndFeel;

  // Visualizer Display
  ironstack::ToneVisualizerComponent visualizer;

  // Header Dropdowns
  juce::ComboBox ampSelector;
  juce::ComboBox cabSelector;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ampAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cabAttachment;

  // Rotary Knobs (Carbon & Gold Pro Console)
  juce::Slider driveKnob;
  juce::Slider bassKnob;
  juce::Slider midKnob;
  juce::Slider trebleKnob;
  juce::Slider volumeKnob;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> midAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

  juce::Label driveLabel;
  juce::Label bassLabel;
  juce::Label midLabel;
  juce::Label trebleLabel;
  juce::Label volumeLabel;

  // Kinetic Tube Glow State
  float tubeGlow = 0.25f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IronStackEditor)
};
