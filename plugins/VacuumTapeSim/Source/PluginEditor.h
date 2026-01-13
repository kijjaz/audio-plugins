#pragma once
#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

class CarbonGoldLookAndFeel : public juce::LookAndFeel_V4 {
public:
  CarbonGoldLookAndFeel();
  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, float rotaryStartAngle,
                        float rotaryEndAngle, juce::Slider &) override;
  void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, float minSliderPos, float maxSliderPos,
                        const juce::Slider::SliderStyle,
                        juce::Slider &) override;
  void drawToggleButton(juce::Graphics &g, juce::ToggleButton &button,
                        bool shouldDrawButtonAsHighlighted,
                        bool shouldDrawButtonAsDown) override;
};

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
  CarbonGoldLookAndFeel lnf;

  juce::Font playfairRegular, playfairItalic, playfairBold;
  juce::Font epilogueBold;

  // Parameters
  struct ParamGroup {
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::Label>> labels;
    std::vector<std::unique_ptr<juce::Component>> customComponents;
    std::vector<
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>>
        attachments;
  };

  ParamGroup compGroup, tapeGroup, modGroup, masterGroup;

  // Animation state
  float reelAngle = 0.0f;
  float tubeGlow = 0.0f;

  void createSlider(ParamGroup &group, juce::String paramID,
                    juce::String labelText, bool isRotary,
                    bool isLarge = false);
  void drawRecessedPanel(juce::Graphics &g, juce::Rectangle<int> area);
  void drawSectionHeader(juce::Graphics &g, juce::Rectangle<int> &area,
                         juce::String title, juce::String subText);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VacuumTapeAudioProcessorEditor)
};
