#include "PluginEditor.h"
#include "PluginProcessor.h"

IronStackEditor::IronStackEditor(IronStackAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  setLookAndFeel(&lookAndFeel);

  // 1. Visualizer
  addAndMakeVisible(visualizer);

  // 2. Selectors (Amp & Cab)
  juce::StringArray models = {
      "Fender '59 Bassman (5F6-A)",
      "Fender '65 Bassman (AA864)",
      "Ampeg B-15N Portaflex",
      "Ampeg B-100R Rocket Bass",
      "Marshall Super Bass 100",
      "Fender Twin Reverb (AB763)",
      "Marshall JCM800 / 1959 Plexi",
      "Vox AC30 Top Boost",
      "Mesa Dual Rectifier",
      "Soldano SLO-100"
  };
  ampSelector.addItemList(models, 1);
  addAndMakeVisible(ampSelector);
  ampAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      audioProcessor.apvts, "ampModel", ampSelector);

  juce::StringArray cabs = {
      "4x10 Bassman Neo (Tone3000)",
      "2x12 Twin C12N (Tone3000)",
      "2x15 '70 Bassman CTS (Tone3000)",
      "8x10 Ampeg SVT Fridge",
      "1x18 Acoustic 360 Horn",
      "2x10 Eminence Legend (Vented)",
      "4x10 Eminence Legend (Vented)",
      "2x12 Hartke Pro 2200 (Tone3000)",
      "4x12 Marshall 1960A (Tone3000)",
      "4x12 Mesa Recto V30 (Tone3000)",
      "2x12 '66 Bassman C12NA (Tone3000)",
      "Bypass (Direct Out)"
  };
  cabSelector.addItemList(cabs, 1);
  addAndMakeVisible(cabSelector);
  cabAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
      audioProcessor.apvts, "cabModel", cabSelector);

  // 3. Precision Rotary Knobs
  auto setupKnob = [this](juce::Slider& knob, juce::Label& label, const juce::String& text,
                          const juce::String& paramId,
                          std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment) {
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 18);
    addAndMakeVisible(knob);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font("Georgia", 11.5f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, ironstack::IronStackLookAndFeel::goldAccent);
    label.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(label);

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, paramId, knob);
  };

  setupKnob(driveKnob, driveLabel, "DRIVE", "drive", driveAttachment);
  setupKnob(bassKnob, bassLabel, "BASS", "bass", bassAttachment);
  setupKnob(midKnob, midLabel, "MIDDLE", "mid", midAttachment);
  setupKnob(trebleKnob, trebleLabel, "TREBLE", "treble", trebleAttachment);
  setupKnob(volumeKnob, volumeLabel, "VOLUME", "volume", volumeAttachment);

  // Pro Console size: comfortable width to host logo + 2 wide selectors + badge without collision
  setSize(760, 500);
  startTimerHz(30); // 30 FPS visual feedback
}

IronStackEditor::~IronStackEditor() {
  setLookAndFeel(nullptr);
}

void IronStackEditor::timerCallback() {
  // Update kinetic tube filament glow from preamp DC bias and drive
  float bias = std::abs(audioProcessor.getTubeStage().getBiasDc());
  float targetGlow = juce::jlimit(0.15f, 1.0f, 0.20f + bias * 1.5f + (float)driveKnob.getValue() / 70.0f);
  tubeGlow += 0.15f * (targetGlow - tubeGlow);

  // Refresh transfer curve visualizer
  visualizer.updateCurves(audioProcessor.getToneStack(), audioProcessor.getCabinet());

  // Repaint tube filament area
  repaint(getLocalBounds().removeFromTop(240));
}

void IronStackEditor::paint(juce::Graphics &g) {
  // 1. Matte Carbon Textured Chassis Base
  g.fillAll(ironstack::IronStackLookAndFeel::carbonDark);

  // 2. Subtle Tweed Gold Trim & Header Stripe
  auto topBanner = getLocalBounds().removeFromTop(56).toFloat();
  juce::ColourGradient bannerGrad(ironstack::IronStackLookAndFeel::carbonPanel, topBanner.getX(), topBanner.getY(),
                                  ironstack::IronStackLookAndFeel::carbonDark, topBanner.getX(), topBanner.getBottom(), false);
  g.setGradientFill(bannerGrad);
  g.fillRect(topBanner);

  g.setColour(ironstack::IronStackLookAndFeel::goldAccent.withAlpha(0.6f));
  g.drawHorizontalLine(56, 0.0f, (float)getWidth());

  // 3. Archival Vintage Logo & Typography
  g.setColour(ironstack::IronStackLookAndFeel::goldHighlight);
  g.setFont(juce::Font("Georgia", 22.0f, juce::Font::bold));
  g.drawText("IRONSTACK", 24, 8, 170, 24, juce::Justification::left, false);

  g.setColour(ironstack::IronStackLookAndFeel::tweedGold);
  g.setFont(juce::Font("Georgia", 9.5f, juce::Font::italic));
  g.drawText("ANALOG AMP & TONE SUITE", 25, 33, 170, 16, juce::Justification::left, false);

  // Calibration badge
  auto badgeBounds = juce::Rectangle<float>((float)getWidth() - 110.0f, 14.0f, 92.0f, 28.0f);
  g.setColour(ironstack::IronStackLookAndFeel::carbonMatte);
  g.fillRoundedRectangle(badgeBounds, 4.0f);
  g.setColour(ironstack::IronStackLookAndFeel::goldAccent.withAlpha(0.5f));
  g.drawRoundedRectangle(badgeBounds, 4.0f, 1.0f);
  g.setFont(juce::Font("Georgia", 11.0f, juce::Font::bold));
  g.setColour(ironstack::IronStackLookAndFeel::goldHighlight);
  g.drawText("-18 dBFS", badgeBounds.toNearestInt(), juce::Justification::centred, false);

  // 4. Kinetic 12AX7 Preamp Vacuum Tube Centerpiece
  auto tubeArea = juce::Rectangle<float>(20.0f, 70.0f, 128.0f, 164.0f);
  ironstack::IronStackLookAndFeel::draw12AX7Tube(g, tubeArea, tubeGlow);

  // Pedestal Plate under tube
  g.setColour(ironstack::IronStackLookAndFeel::goldAccent.withAlpha(0.7f));
  g.setFont(juce::Font(9.5f, juce::Font::bold));
  g.drawText("12AX7 PREAMP", 20, 222, 128, 16, juce::Justification::centred, false);

  // 5. Section Header for Tone Stack Controls
  auto ctrlHeaderArea = juce::Rectangle<int>(20, 256, getWidth() - 40, 24);
  g.setColour(ironstack::IronStackLookAndFeel::textOffWhite.withAlpha(0.6f));
  g.setFont(juce::Font(10.5f, juce::Font::bold));
  g.drawText("PHYSICAL CIRCUIT & TONE CONTROLS", ctrlHeaderArea, juce::Justification::left, false);
  g.setColour(ironstack::IronStackLookAndFeel::goldAccent.withAlpha(0.25f));
  g.drawHorizontalLine(276, 20.0f, (float)getWidth() - 20.0f);

  // 6. Corner Industrial Hex Screws
  auto drawScrew = [&g](float x, float y) {
    g.setColour(juce::Colour(0xff333538));
    g.fillEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f);
    g.setColour(juce::Colour(0xff18191b));
    g.drawEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f, 1.0f);
    g.drawLine(x - 2.5f, y, x + 2.5f, y, 1.0f);
  };
  drawScrew(10, 10);
  drawScrew((float)getWidth() - 10, 10);
  drawScrew(10, (float)getHeight() - 10);
  drawScrew((float)getWidth() - 10, (float)getHeight() - 10);
}

void IronStackEditor::resized() {
  // Selectors in header with generous widths and no overlap
  const int badgeRightMargin = 120;
  const int selW = 205;
  const int selH = 28;
  const int selY = 14;

  cabSelector.setBounds(getWidth() - badgeRightMargin - selW, selY, selW, selH);
  ampSelector.setBounds(getWidth() - badgeRightMargin - selW * 2 - 10, selY, selW, selH);

  // Visualizer beside Tube
  visualizer.setBounds(164, 70, getWidth() - 188, 168);

  // Rotary Knobs in Bottom Section
  const int knobY = 296;
  const int labelY = knobY + 98;
  const int knobSize = 88;
  const int numKnobs = 5;
  const int startX = 36;
  const int spacing = (getWidth() - startX * 2 - knobSize) / (numKnobs - 1);

  driveKnob.setBounds(startX + 0 * spacing, knobY, knobSize, knobSize);
  driveLabel.setBounds(startX + 0 * spacing, labelY, knobSize, 18);

  bassKnob.setBounds(startX + 1 * spacing, knobY, knobSize, knobSize);
  bassLabel.setBounds(startX + 1 * spacing, labelY, knobSize, 18);

  midKnob.setBounds(startX + 2 * spacing, knobY, knobSize, knobSize);
  midLabel.setBounds(startX + 2 * spacing, labelY, knobSize, 18);

  trebleKnob.setBounds(startX + 3 * spacing, knobY, knobSize, knobSize);
  trebleLabel.setBounds(startX + 3 * spacing, labelY, knobSize, 18);

  volumeKnob.setBounds(startX + 4 * spacing, knobY, knobSize, knobSize);
  volumeLabel.setBounds(startX + 4 * spacing, labelY, knobSize, 18);
}
