#pragma once
#include "PluginProcessor.h"

class VacuumTapeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       public juce::Timer {
public:
  VacuumTapeAudioProcessorEditor(VacuumTapeAudioProcessor &);
  ~VacuumTapeAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;
  void timerCallback() override;

private:
  VacuumTapeAudioProcessor &audioProcessor;

  juce::Font playfairRegular, playfairItalic, playfairBold;

  // Use a generic slider set for now, to be styled like Carbon & Gold later
  std::vector<std::unique_ptr<juce::Slider>> sliders;
  std::vector<std::unique_ptr<juce::Label>> labels;
  std::vector<
      std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
      attachments;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VacuumTapeAudioProcessorEditor)
};
