#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class CableLiftAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::Slider::Listener {
public:
  CableLiftAudioProcessorEditor(CableLiftAudioProcessor &);
  ~CableLiftAudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;

  void sliderValueChanged(juce::Slider *slider) override;

private:
  CableLiftAudioProcessor &audioProcessor;

  // UI Elements
  juce::Image bgImage;

  juce::Slider liftLevelSlider;
  juce::Label liftLevelLabel;
  juce::Label liftLevelDisplay;

  juce::Slider puritySlider;
  juce::Label purityLabel;

  juce::Slider staticSlider;
  juce::Label staticLabel;

  juce::Slider focusSlider;
  juce::Label focusLabel;

  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      liftLevelAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      purityAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      staticAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      focusAttach;

  // Custom LookAndFeel
  class CarbonGoldLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    CarbonGoldLookAndFeel() {
      setColour(juce::Slider::thumbColourId, juce::Colours::gold);
      setColour(juce::Slider::trackColourId, juce::Colour(50, 50, 50));
      setColour(juce::Slider::backgroundColourId, juce::Colour(20, 20, 20));
    }
  };

  CarbonGoldLookAndFeel lnf;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CableLiftAudioProcessorEditor)
};
