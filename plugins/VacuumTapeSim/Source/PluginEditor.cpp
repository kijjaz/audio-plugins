#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

VacuumTapeAudioProcessorEditor::VacuumTapeAudioProcessorEditor(
    VacuumTapeAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  // Initialize custom fonts from binary data
  auto regularTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::PlayfairDisplayRegular_ttf,
      BinaryData::PlayfairDisplayRegular_ttfSize);
  auto italicTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::PlayfairDisplayItalic_ttf,
      BinaryData::PlayfairDisplayItalic_ttfSize);
  auto boldTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::PlayfairDisplayBold_ttf,
      BinaryData::PlayfairDisplayBold_ttfSize);

  playfairRegular = juce::Font(regularTypeface);
  playfairItalic = juce::Font(italicTypeface);
  playfairBold = juce::Font(boldTypeface);

  juce::Colour bgColor = juce::Colour::fromRGB(40, 36, 32);
  juce::Colour goldColor = juce::Colour::fromRGB(196, 174, 134);
  juce::Colour panelColor = juce::Colour::fromRGB(52, 48, 44);

  auto createSlider = [&](juce::String paramID, juce::String labelText) {
    auto slider = std::make_unique<juce::Slider>(labelText);
    slider->setSliderStyle(juce::Slider::LinearVertical);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    slider->setNumDecimalPlacesToDisplay(
        2); // Fix: Prevent long decimal strings

    // Modern Vertical Style: Gold fill on charcoal track
    slider->setColour(juce::Slider::backgroundColourId, bgColor.darker(0.5f));
    slider->setColour(juce::Slider::trackColourId,
                      juce::Colours::gold.withAlpha(0.3f));
    slider->setColour(juce::Slider::thumbColourId, juce::Colours::gold);
    slider->setColour(juce::Slider::textBoxTextColourId,
                      juce::Colour::greyLevel(0.8f));
    slider->setColour(juce::Slider::textBoxOutlineColourId,
                      juce::Colours::transparentBlack);

    addAndMakeVisible(*slider);

    auto attachment =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, paramID, *slider);

    sliders.push_back(std::move(slider));
    attachments.push_back(std::move(attachment));

    // Add label with custom font
    auto label = std::make_unique<juce::Label>(labelText, labelText);
    label->setFont(playfairRegular.withHeight(14.0f));
    label->setJustificationType(juce::Justification::centred);
    label->setColour(juce::Label::textColourId, goldColor.withAlpha(0.7f));
    label->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*label);
    labels.push_back(std::move(label));
  };

  createSlider("COMP_THRESH", "Threshold");
  createSlider("COMP_DRAIN", "Drain");
  createSlider("COMP_RECOV", "Recovery");
  createSlider("COMP_TIME", "Time");

  createSlider("TAPE_DRIVE", "Drive");
  createSlider("TAPE_HYST", "Hysteresis");
  createSlider("TAPE_SPEED", "Speed");
  createSlider("TAPE_WOW", "Wow");
  createSlider("TAPE_FLUTTER", "Flutter");

  createSlider("OUTPUT_VOL", "Volume");
  createSlider("OVERSAMPLING", "Oversampling");

  setSize(700, 700); // Extreme compaction
  startTimerHz(30);  // For meter updates
}

VacuumTapeAudioProcessorEditor::~VacuumTapeAudioProcessorEditor() {}

void VacuumTapeAudioProcessorEditor::paint(juce::Graphics &g) {
  juce::Colour bgColor = juce::Colour::fromRGB(40, 36, 32);
  juce::Colour panelColor = juce::Colour::fromRGB(52, 48, 44);
  juce::Colour goldColor = juce::Colour::fromRGB(196, 174, 134);

  g.fillAll(bgColor);

  auto area = getLocalBounds().reduced(10);

  // Header Area with Title
  auto headerArea = area.removeFromTop(60);
  g.setColour(panelColor.darker(0.3f));
  g.fillRect(headerArea);

  g.setColour(goldColor);
  g.setFont(playfairBold.withHeight(36.0f));
  g.drawText("VACUUM TAPE SIMULATOR", headerArea, juce::Justification::centred);

  // 1. Feature Image (Top)
  area.removeFromTop(5); // Smaller spacer
  auto imageArea = area.removeFromTop(area.getHeight() * 0.38f);

  auto backgroundImage = juce::ImageCache::getFromMemory(
      BinaryData::background_png, BinaryData::background_pngSize);
  if (backgroundImage.isValid()) {
    g.setColour(juce::Colours::black);
    g.drawRect(imageArea.expanded(2), 2);
    g.drawImageWithin(backgroundImage, imageArea.getX(), imageArea.getY(),
                      imageArea.getWidth(), imageArea.getHeight(),
                      juce::RectanglePlacement::centred);
  }

  // 2. Meters Area (Middle)
  area.removeFromTop(5);
  auto metersArea = area.removeFromTop(70);

  auto drawMeter = [&](juce::Rectangle<int> r, float val, juce::String name) {
    g.setColour(panelColor.darker(0.2f));
    g.fillRoundedRectangle(r.toFloat(), 4.0f);

    auto fillArea = r.reduced(4).toFloat();
    float fillWidth = fillArea.getWidth() * std::clamp(val, 0.0f, 1.0f);

    g.setColour(goldColor.withAlpha(0.6f));
    g.fillRoundedRectangle(fillArea.withWidth(fillWidth), 2.0f);

    g.setColour(juce::Colours::white.withAlpha(0.8f));
    g.setFont(playfairRegular.withHeight(14.0f));
    g.drawText(name, r, juce::Justification::centred);
  };

  drawMeter(metersArea.removeFromTop(35).reduced(100, 5),
            audioProcessor.gainReductionLevel.load(), "VACUUM GAIN REDUCTION");
  drawMeter(metersArea.removeFromTop(35).reduced(100, 5),
            audioProcessor.saturationLevel.load() * 2.0f,
            "RECORDING DRIVE (SATURATION)");

  // 3. Controls Panels (Bottom)
  area.removeFromTop(10);
  g.setColour(panelColor);
  g.fillRoundedRectangle(area.toFloat(), 10.0f);
  g.setColour(panelColor.brighter(0.1f));
  g.drawRoundedRectangle(area.toFloat(), 10.0f, 2.0f);

  // Section labels
  g.setColour(goldColor.withAlpha(0.5f));
  g.setFont(playfairBold.withHeight(16.0f));
  auto labelsArea = area.removeFromTop(20);
  g.drawText("COMPRESSOR", labelsArea.removeFromLeft(area.getWidth() / 2),
             juce::Justification::centred);
  g.drawText("TAPE MACHINE", labelsArea, juce::Justification::centred);
}

void VacuumTapeAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(10);     // Thinner margins
  area.removeFromTop(60);                       // Header
  area.removeFromTop(area.getHeight() * 0.38f); // Slightly smaller image ratio
  area.removeFromTop(5);                        // Spacer
  area.removeFromTop(70);                       // Thinner Meters
  area.removeFromTop(5);                        // Spacer
  area.removeFromTop(20);                       // Section Labels

  auto compArea = area.removeFromLeft(area.getWidth() / 2);
  auto tapeArea = area;

  // Compressor Sliders (Grid)
  auto compRow1 = compArea.removeFromTop(compArea.getHeight() / 2);
  auto compRow2 = compArea;

  auto setSliderAndLabel = [&](int index, juce::Rectangle<int> r) {
    labels[index]->setBounds(r.removeFromTop(20));
    sliders[index]->setBounds(r);
  };

  setSliderAndLabel(0, compRow1.removeFromLeft(compRow1.getWidth() / 2)
                           .reduced(10, 5));     // Thresh
  setSliderAndLabel(1, compRow1.reduced(10, 5)); // Drain
  setSliderAndLabel(
      2,
      compRow2.removeFromLeft(compRow2.getWidth() / 2).reduced(10, 5)); // Recov
  setSliderAndLabel(3, compRow2.reduced(10, 5));                        // Time

  // Tape Sliders
  auto tapeRow1 = tapeArea.removeFromTop(tapeArea.getHeight() / 2);
  auto tapeRow2 = tapeArea;

  int w1 = tapeRow1.getWidth() / 3;
  setSliderAndLabel(4, tapeRow1.removeFromLeft(w1).reduced(10, 5)); // Drive
  setSliderAndLabel(5, tapeRow1.removeFromLeft(w1).reduced(10, 5)); // Hyst
  setSliderAndLabel(6, tapeRow1.reduced(10, 5));                    // Speed

  int w2 = tapeRow2.getWidth() / 4;
  setSliderAndLabel(7, tapeRow2.removeFromLeft(w2).reduced(10, 5)); // Wow
  setSliderAndLabel(8, tapeRow2.removeFromLeft(w2).reduced(10, 5)); // Flutter
  setSliderAndLabel(9, tapeRow2.removeFromLeft(w2).reduced(10, 5)); // Vol
  setSliderAndLabel(10, tapeRow2.reduced(10, 5));                   // OS
}

void VacuumTapeAudioProcessorEditor::timerCallback() { repaint(); }
