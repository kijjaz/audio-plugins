#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class HelpContentComponent : public juce::Component {
public:
  HelpContentComponent() {
    setupLabel(overview, "--- SYSTEM ARCHITECTURE ---",
               "[MIDI In] -> [GM Decoder] -> [Trigger Matrix]\n"
               "                                  |\n"
               " [Accent] ------------------------+\n"
               "                                  v\n"
               "                          [12x Voices]\n"
               "                          (Analog Kit)\n"
               "                                  |\n"
               "                                  v\n"
               "                          [Master Stage]\n");

    setupLabel(kickDiag, "--- KICK SYNTHESIS ---",
               "[Trig] -> [Pulse Gen] -> [Pitch Env]\n"
               "              |              |\n"
               "              v              v\n"
               "         [Bridged-T Resonator]\n"
               "                 |\n"
               "                 v\n"
               "         [OpAmp] -> [Diode] -> [VCA]\n");

    setupLabel(snareDiag, "--- SNARE SYNTHESIS ---",
               "              [Pitch Env]\n"
               "                  |\n"
               "[Trig] -> [Resonator 1] ---+\n"
               "      |                    |--> [Sum] -> [Out]\n"
               "      +-> [Noise] -> [BPF] -+\n");

    setupLabel(hatsDiag, "--- HATS SYNTHESIS ---",
               "[6x SQ Oscs] -> [Metal Mix] -> [BPF]\n"
               "                                 |\n"
               " [Envelope] ---------------------+\n"
               "                                 v\n"
               "                         [Slew] -> [VCA]\n");

    setupLabel(tomsDiag, "--- TOMS SYNTHESIS ---",
               "[Trig] -> [Pulse] -> [Mix] -> [Resonator] -> [Out]\n"
               "          [Noise] -/\n");

    setupLabel(midiMap, "--- MIDI MAPPING (Ableton Layout) ---",
               "KICK      : 36 (C1)     HI TOM    : 48 (C2)\n"
               "RIMSHOT   : 37 (C#1)    HI TOM 2  : 50 (D2)\n"
               "SNARE     : 38 (D1)     CL HAT    : 42 (F#1)\n"
               "SNARE 2   : 40 (E1)     CL HAT 2  : 44 (G#1)\n"
               "CLAP      : 39 (D#1)    OP HAT    : 46 (A#1)\n"
               "LO TOM    : 41 (F1)     CYMBAL    : 49 (C#2)\n"
               "LO TOM 2  : 43 (G1)     CYMBAL 2  : 51 (D#2)\n"
               "MID TOM   : 45 (A1)     CYMBAL 3  : 52 (E2)\n"
               "MID TOM 2 : 47 (B1)     COWBELL   : 56 (G#2)\n"
               "MARACAS   : 70 (A#3)\n");

    setupLabel(modsText, "--- COMPONENT MODELING ---",
               "OP-AMPS: Sets transient response (Slew).\n"
               "DIODES: Defines clipping saturation curve.\n"
               "CAPS: Simulated aging affects decay times.\n");
  }

  void setupLabel(juce::Label &l, juce::String title, juce::String body) {
    l.setFont(juce::Font("Monaco", 13.0f, juce::Font::plain));
    l.setColour(juce::Label::textColourId, juce::Colours::white);
    l.setText(title + "\n" + body, juce::dontSendNotification);
    addAndMakeVisible(l);
  }

  void resized() override {
    juce::Rectangle<int> area = getLocalBounds().reduced(10);
    overview.setBounds(area.removeFromTop(180));
    kickDiag.setBounds(area.removeFromTop(130));
    snareDiag.setBounds(area.removeFromTop(110));
    hatsDiag.setBounds(area.removeFromTop(110));
    tomsDiag.setBounds(area.removeFromTop(70));
    midiMap.setBounds(area.removeFromTop(200));
    modsText.setBounds(area.removeFromTop(100));
  }

private:
  juce::Label overview, kickDiag, snareDiag, hatsDiag, tomsDiag, midiMap,
      modsText;
};

class HelpComponent : public juce::Component {
public:
  HelpComponent() {
    addAndMakeVisible(titleLabel);
    titleLabel.setText("DISCRETE 808 - ENGINEERING MANUAL",
                       juce::dontSendNotification);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::gold);

    addAndMakeVisible(viewport);
    viewport.setScrollBarsShown(true, false);
    viewport.setViewedComponent(&content, false);

    addAndMakeVisible(closeButton);
    closeButton.setButtonText("EXIT MANUAL");
    closeButton.onClick = [this] { setVisible(false); };
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::black.withAlpha(0.95f));
    g.setColour(juce::Colour(0xffb8860b));
    g.drawRect(getLocalBounds().reduced(2), 2);
  }

  void resized() override {
    juce::Rectangle<int> area = getLocalBounds().reduced(20);
    titleLabel.setBounds(area.removeFromTop(50));
    closeButton.setBounds(getWidth() / 2 - 60, getHeight() - 50, 120, 30);
    area.removeFromBottom(40);
    viewport.setBounds(area);
    content.setSize(viewport.getWidth() - 20, 1000);
  }

private:
  juce::Label titleLabel;
  juce::Viewport viewport;
  HelpContentComponent content;
  juce::TextButton closeButton;
};
