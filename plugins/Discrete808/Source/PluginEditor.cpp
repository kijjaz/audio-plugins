#include "PluginEditor.h"
#include "PluginProcessor.h"

Discrete808AudioProcessorEditor::Discrete808AudioProcessorEditor(
    Discrete808AudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  setLookAndFeel(&lnf);
  setSize(1000, 420); // Adjusted height since hatch is hidden for now

  // 1. Setup Master
  accentSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  accentSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  addAndMakeVisible(accentSlider);
  accentAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "MASTER_ACCENT", accentSlider);

  masterLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
  masterLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  addAndMakeVisible(masterLevelSlider);
  masterLevelAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "MASTER_LEVEL", masterLevelSlider);

  // 2. Setup All Channels (12 voices)
  setupChannel(Discrete808AudioProcessor::kKick, "KICK");
  setupChannel(Discrete808AudioProcessor::kSnare, "SNARE");
  setupChannel(Discrete808AudioProcessor::kLowTom, "LOWTOM");
  setupChannel(Discrete808AudioProcessor::kMidTom, "MIDTOM");
  setupChannel(Discrete808AudioProcessor::kHighTom, "HIGHTOM");
  setupChannel(Discrete808AudioProcessor::kRimshot, "RIMSHOT");
  setupChannel(Discrete808AudioProcessor::kClap, "CLAP");
  setupChannel(Discrete808AudioProcessor::kCowbell, "COWBELL");
  setupChannel(Discrete808AudioProcessor::kMaracas, "MARACAS");
  setupChannel(Discrete808AudioProcessor::kClosedHat, "CLOSEDHAT");
  setupChannel(Discrete808AudioProcessor::kOpenHat, "OPENHAT");
  setupChannel(Discrete808AudioProcessor::kCymbal, "CYMBAL");

  addAndMakeVisible(helpButton);
  helpButton.onClick = [this] { helpComponent.setVisible(true); };
  addChildComponent(helpComponent);

  startTimerHz(30);
}

Discrete808AudioProcessorEditor::~Discrete808AudioProcessorEditor() {
  setLookAndFeel(nullptr);
  stopTimer();
}

void Discrete808AudioProcessorEditor::setupChannel(int index,
                                                   const juce::String &prefix) {
  auto controls = std::make_unique<ChannelControls>();

  auto setupSlider =
      [&](std::unique_ptr<juce::Slider> &s, const juce::String &paramID,
          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
              &a) {
        s = std::make_unique<juce::Slider>();
        s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(*s);
        a = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, paramID, *s);
      };

  setupSlider(controls->levelSlider, prefix + "_LEVEL", controls->levelAttach);
  setupSlider(controls->velToneSlider, prefix + "_VEL_TONE",
              controls->velToneAttach);
  setupSlider(controls->velDecaySlider, prefix + "_VEL_DECAY",
              controls->velDecayAttach);

  // Some voices don't have Tune/Decay in our simplified APVTS layout yet, but
  // let's assume they might
  if (audioProcessor.apvts.getParameter(prefix + "_TUNE") != nullptr)
    setupSlider(controls->tuneSlider, prefix + "_TUNE", controls->tuneAttach);

  if (audioProcessor.apvts.getParameter(prefix + "_DECAY") != nullptr)
    setupSlider(controls->decaySlider, prefix + "_DECAY",
                controls->decayAttach);
  else if (audioProcessor.apvts.getParameter(prefix + "_SNAPPY") != nullptr)
    setupSlider(controls->decaySlider, prefix + "_SNAPPY",
                controls->decayAttach);

  channelStrips[index] = std::move(controls);
}

void Discrete808AudioProcessorEditor::paint(juce::Graphics &g) {
  // Premium Carbon Background
  g.fillAll(juce::Colour(0xff111111));

  // Grid Lines (Technical / Engineering Look)
  g.setColour(juce::Colours::white.withAlpha(0.05f));
  for (int i = 0; i < getWidth(); i += 50)
    g.drawVerticalLine(i, 0, getHeight());
  for (int i = 0; i < getHeight(); i += 50)
    g.drawHorizontalLine(i, 0, getWidth());

  // Header
  g.setColour(juce::Colour(0xffb8860b)); // Gold
  g.setFont(juce::Font("Helvetica Neue", 32.0f, juce::Font::bold));
  g.drawText("DISCRETE 808 MKII", 20, 15, 400, 40, juce::Justification::left);

  // Subtext
  g.setColour(juce::Colours::grey);
  g.setFont(juce::Font("Helvetica Neue", 12.0f, juce::Font::plain));
  g.drawText("HIGH-FIDELITY ANALOG COMPONENT MODELING", 22, 50, 400, 20,
             juce::Justification::left);

  // Section Labels
  g.setColour(juce::Colours::white.withAlpha(0.2f));
  g.drawRect(10, 80, getWidth() - 120, 320, 1);
  g.drawText("PERCUSSION ENGINE", 20, 85, 200, 20, juce::Justification::left);

  // Draw Channel Strip backgrounds (visual only)
  int stripW = (getWidth() - 120) / 12;
  for (int i = 0; i < 12; ++i) {
    int x = 10 + i * stripW;
    if (i == selectedVoiceIndex) {
      g.setColour(juce::Colour(0xffb8860b).withAlpha(0.1f));
      g.fillRect(x, 100, stripW, 300);
      g.setColour(juce::Colour(0xffb8860b).withAlpha(0.3f));
      g.drawRect(x, 100, stripW, 300);
    }

    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::Font("Helvetica Neue", 10.0f, juce::Font::bold));
    g.drawText(getVoiceName(i), x, 370, stripW, 20,
               juce::Justification::centred);
  }
}

void Discrete808AudioProcessorEditor::resized() {
  int stripW = (getWidth() - 120) / 12;
  int x = 10;

  for (int i = 0; i < 12; ++i) {
    if (channelStrips.count(i)) {
      auto &s = channelStrips[i];
      int sy = 120;
      if (s->tuneSlider) {
        s->tuneSlider->setBounds(x + 5, sy, stripW - 10, stripW - 10);
        sy += stripW;
      }
      if (s->decaySlider) {
        s->decaySlider->setBounds(x + 5, sy, stripW - 10, stripW - 10);
        sy += stripW;
      }
      if (s->levelSlider) {
        s->levelSlider->setBounds(x + 5, sy, stripW - 10, stripW - 10);
        sy += stripW;
      }

      // Secondary Vel-Sens Knobs (Smaller)
      int smallK = (stripW - 10) / 2;
      if (s->velToneSlider)
        s->velToneSlider->setBounds(x + 5, sy, smallK, smallK);
      if (s->velDecaySlider)
        s->velDecaySlider->setBounds(x + 5 + smallK, sy, smallK, smallK);
    }
    x += stripW;
  }

  // Master
  accentSlider.setBounds(getWidth() - 100, 100, 80, 80);
  masterLevelSlider.setBounds(getWidth() - 70, 200, 40, 180);

  helpButton.setBounds(getWidth() - 80, 15, 60, 30);
  helpComponent.setBounds(getLocalBounds().reduced(100));
}

void Discrete808AudioProcessorEditor::mouseDown(const juce::MouseEvent &e) {
  if (e.y > 100 && e.y < 400) {
    int stripW = (getWidth() - 120) / 12;
    int idx = (e.x - 10) / stripW;
    if (idx >= 0 && idx < 12) {
      selectedVoiceIndex = idx;
      repaint();
    }
  }
}

juce::String Discrete808AudioProcessorEditor::getVoiceName(int index) {
  switch (index) {
  case 0:
    return "BD";
  case 1:
    return "SD";
  case 2:
    return "LT";
  case 3:
    return "MT";
  case 4:
    return "HT";
  case 5:
    return "RS";
  case 6:
    return "CP";
  case 7:
    return "CB";
  case 8:
    return "MA";
  case 9:
    return "CH";
  case 10:
    return "OH";
  case 11:
    return "CY";
  }
  return "";
}

void Discrete808AudioProcessorEditor::drawMaintenanceHatch(
    juce::Graphics &g, juce::Rectangle<int> bounds) {}
