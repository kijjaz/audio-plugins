#include "PluginEditor.h"
#include "PluginProcessor.h"
#include <BinaryData.h>

CableLiftAudioProcessorEditor::CableLiftAudioProcessorEditor(
    CableLiftAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
  // Load Asset
  bgImage = juce::ImageCache::getFromMemory(
      BinaryData::cable_lift_stand_png, BinaryData::cable_lift_stand_pngSize);

  // Look And Feel
  setLookAndFeel(&lnf);

  // Fonts
  auto regularFont = juce::Font("Inter", 12.0f, juce::Font::plain);
  auto boldFont = juce::Font("Inter", 14.0f, juce::Font::bold);

  // === Lift Level ===
  liftLevelLabel.setText("Elevation Level", juce::dontSendNotification);
  liftLevelLabel.setFont(regularFont);
  liftLevelLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
  addAndMakeVisible(liftLevelLabel);

  liftLevelSlider.setSliderStyle(juce::Slider::LinearHorizontal);
  liftLevelSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
  liftLevelSlider.addListener(this);
  addAndMakeVisible(liftLevelSlider);

  liftLevelDisplay.setText("GROUND ZERO", juce::dontSendNotification);
  liftLevelDisplay.setFont(boldFont);
  liftLevelDisplay.setColour(juce::Label::textColourId, juce::Colours::gold);
  liftLevelDisplay.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(liftLevelDisplay);

  liftLevelAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, "liftLevel", liftLevelSlider);

  // === Sonic Purity ===
  purityLabel.setText("Sonic Purity", juce::dontSendNotification);
  purityLabel.setFont(regularFont);
  purityLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
  purityLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(purityLabel);

  puritySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  puritySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  puritySlider.setColour(juce::Slider::textBoxTextColourId,
                         juce::Colours::gold);
  puritySlider.setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colours::transparentBlack);
  addAndMakeVisible(puritySlider);
  purityAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, "sonicPurity", puritySlider);

  // === Static ===
  staticLabel.setText("Static Red.", juce::dontSendNotification);
  staticLabel.setFont(regularFont);
  staticLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
  staticLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(staticLabel);

  staticSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  staticSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  staticSlider.setColour(juce::Slider::textBoxTextColourId,
                         juce::Colours::gold);
  staticSlider.setColour(juce::Slider::textBoxOutlineColourId,
                         juce::Colours::transparentBlack);
  addAndMakeVisible(staticSlider);
  staticAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, "staticReduction", staticSlider);

  // === Focus ===
  focusLabel.setText("Focus", juce::dontSendNotification);
  focusLabel.setFont(regularFont);
  focusLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
  focusLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(focusLabel);

  focusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  focusSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  focusSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::gold);
  focusSlider.setColour(juce::Slider::textBoxOutlineColourId,
                        juce::Colours::transparentBlack);
  addAndMakeVisible(focusSlider);
  focusAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, "focusDetail", focusSlider);

  setSize(400, 500);
}

CableLiftAudioProcessorEditor::~CableLiftAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void CableLiftAudioProcessorEditor::paint(juce::Graphics &g) {
  // Background Gradient + Image (masked manually in paint?)
  // JUCE doesn't do "CSS Masking" easily. We'll just draw the image with
  // opacity.

  g.fillAll(juce::Colour(0xff1a1a1a)); // Dark background

  // Draw BG Image top half
  if (bgImage.isValid()) {
    g.setOpacity(0.8f);
    g.drawImage(bgImage, 0, 0, getWidth(), 250, 0, 0, bgImage.getWidth(),
                bgImage.getHeight());

    // Gradient overlay to fade to black
    juce::ColourGradient grad(juce::Colours::transparentBlack, 0, 150,
                              juce::Colour(0xff0a0a0a), 0, 250, false);
    g.setGradientFill(grad);
    g.fillRect(0, 150, getWidth(), 100);
  }

  g.setOpacity(1.0f);

  // Header
  // Outline strategy: draw text in black at offsets, then gold on top
  juce::Font headerFont("Inter", 20.0f, juce::Font::bold);
  g.setFont(headerFont);

  // Shadow/Outline
  g.setColour(juce::Colours::black);
  g.drawText("Cable Lift Enhancer", 2, 22, getWidth(), 30,
             juce::Justification::centredTop);
  g.drawText("Cable Lift Enhancer", -2, 18, getWidth(), 30,
             juce::Justification::centredTop);

  g.setColour(juce::Colours::gold);
  g.drawText("Cable Lift Enhancer", 0, 20, getWidth(), 30,
             juce::Justification::centredTop);

  juce::Font subFont("Inter", 12.0f, juce::Font::plain);
  g.setFont(subFont);
  g.setColour(juce::Colours::black);
  g.drawText("Digital Resonance Dampener", 1, 51, getWidth(), 20,
             juce::Justification::centredTop);
  g.setColour(juce::Colours::grey);
  g.drawText("Digital Resonance Dampener", 0, 50, getWidth(), 20,
             juce::Justification::centredTop);
}

void CableLiftAudioProcessorEditor::resized() {
  auto area = getLocalBounds();
  auto topSection = area.removeFromTop(250); // Image area

  auto controls = area; // Bottom half
  controls.reduce(20, 20);

  // Lift Level (Big Slider)
  auto liftArea = controls.removeFromTop(80);
  liftLevelLabel.setBounds(liftArea.removeFromTop(20));
  liftLevelDisplay.setBounds(liftArea.removeFromBottom(20));
  liftLevelSlider.setBounds(liftArea);

  controls.removeFromTop(20); // Spacer

  // 3 Knobs Row
  auto knobWidth = controls.getWidth() / 3;

  auto pArea = controls.removeFromLeft(knobWidth);
  purityLabel.setBounds(pArea.removeFromTop(20));
  puritySlider.setBounds(pArea);

  auto sArea = controls.removeFromLeft(knobWidth);
  staticLabel.setBounds(sArea.removeFromTop(20));
  staticSlider.setBounds(sArea);

  auto fArea = controls.removeFromLeft(knobWidth);
  focusLabel.setBounds(fArea.removeFromTop(20));
  focusSlider.setBounds(fArea);
}

void CableLiftAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
  if (slider == &liftLevelSlider) {
    float val = (float)slider->getValue();
    juce::String text;
    if (val < 1.5f)
      text = "LEVEL 1: GROUND ZERO";
    else if (val < 2.5f)
      text = "LEVEL 2: MID-AIR ELEVATION";
    else if (val < 3.5f)
      text = "LEVEL 3: HIGH-ALTITUDE LIFT";
    else
      text = "LEVEL 4: SKY-SCRAPER ZENITH";

    liftLevelDisplay.setText(text, juce::dontSendNotification);
  }
}
