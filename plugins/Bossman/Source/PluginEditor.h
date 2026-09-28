#pragma once
#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

class Fender59Editor : public juce::AudioProcessorEditor {
public:
  Fender59Editor(Fender59AudioProcessor &);
  ~Fender59Editor() override;
  void paint(juce::Graphics &) override;
  void resized() override;

private:
  Fender59AudioProcessor &audioProcessor;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Fender59Editor)
};
