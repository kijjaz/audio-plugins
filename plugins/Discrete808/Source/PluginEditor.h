#pragma once

#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/HelpComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <memory>

class Discrete808AudioProcessorEditor : public juce::AudioProcessorEditor,
                                        public juce::Timer {
public:
  Discrete808AudioProcessorEditor(Discrete808AudioProcessor &);
  ~Discrete808AudioProcessorEditor() override;

  void paint(juce::Graphics &) override;
  void resized() override;
  void mouseDown(const juce::MouseEvent &e) override;
  void timerCallback() override { repaint(); }

private:
  Discrete808AudioProcessor &audioProcessor;
  CustomLookAndFeel lnf;

  HelpComponent helpComponent;
  juce::TextButton helpButton{"HELP"};

  // Parameters - Sliders & Attachments
  struct ChannelControls {
    std::unique_ptr<juce::Slider> tuneSlider;
    std::unique_ptr<juce::Slider> decaySlider;
    std::unique_ptr<juce::Slider> levelSlider;
    std::unique_ptr<juce::Slider> velToneSlider;
    std::unique_ptr<juce::Slider> velDecaySlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        tuneAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        decayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        levelAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        velToneAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        velDecayAttach;
  };

  std::map<int, std::unique_ptr<ChannelControls>> channelStrips;

  // Master Section
  juce::Slider accentSlider;
  juce::Slider masterLevelSlider;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      accentAttach;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
      masterLevelAttach;

  int selectedVoiceIndex = 0;

  void setupChannel(int index, const juce::String &prefix);
  void drawChannelStrip(juce::Graphics &g, int index, int x, int y, int w,
                        int h);
  void drawMaintenanceHatch(juce::Graphics &g, juce::Rectangle<int> bounds);

  juce::String getVoiceName(int index);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Discrete808AudioProcessorEditor)
};
