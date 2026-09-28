#include "PluginEditor.h"
#include "PluginProcessor.h"

Fender59Editor::Fender59Editor(Fender59AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  setSize(400, 300);
}

Fender59Editor::~Fender59Editor() {}

void Fender59Editor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::black);
  g.setColour(juce::Colours::white);
  g.setFont(15.0f);
  g.drawFittedText("Fender59 (Generic Editor)", getLocalBounds(),
                   juce::Justification::centred, 1);
}

void Fender59Editor::resized() {}
