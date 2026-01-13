#include "PluginEditor.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

// --- CarbonGoldLookAndFeel Implementation ---

CarbonGoldLookAndFeel::CarbonGoldLookAndFeel() {
  setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(220, 180, 100));
  setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(20, 15, 12));
  setColour(juce::Slider::backgroundColourId,
            juce::Colour::fromRGB(45, 35, 30));
}

void CarbonGoldLookAndFeel::drawRotarySlider(juce::Graphics &, int, int, int,
                                             int, float, float, float,
                                             juce::Slider &) {}

void CarbonGoldLookAndFeel::drawLinearSlider(juce::Graphics &g, int x, int y,
                                             int width, int height,
                                             float sliderPos, float, float,
                                             const juce::Slider::SliderStyle,
                                             juce::Slider &) {
  auto gold = juce::Colour::fromRGB(220, 180, 100);
  auto trackH = 4.0f; // Thicker for better visibility
  auto trackArea = juce::Rectangle<float>(x, y + height * 0.5f - trackH * 0.5f,
                                          width, trackH);

  // Track
  g.setColour(juce::Colour::fromRGB(25, 20, 15));
  g.fillRoundedRectangle(trackArea.expanded(1.0f), 2.0f);

  // Fill
  auto fillWidth = std::max(0.0f, sliderPos - x);
  auto fillArea = trackArea.withWidth(fillWidth);
  g.setColour(gold.withAlpha(0.5f));
  g.fillRoundedRectangle(fillArea, 2.0f);

  // Premium Pill Thumb
  auto thumbW = 12.0f;
  auto thumbH = 22.0f;
  auto thumbBounds =
      juce::Rectangle<float>(sliderPos - thumbW * 0.5f,
                             y + height * 0.5f - thumbH * 0.5f, thumbW, thumbH);

  juce::ColourGradient thumbGrad(
      gold.brighter(0.2f), thumbBounds.getX(), thumbBounds.getY(),
      gold.darker(0.3f), thumbBounds.getX(), thumbBounds.getBottom(), false);
  g.setGradientFill(thumbGrad);
  g.fillRoundedRectangle(thumbBounds, 3.0f);

  g.setColour(juce::Colours::black.withAlpha(0.4f));
  g.drawRoundedRectangle(thumbBounds, 3.0f, 1.0f);

  // Grip lines
  g.setColour(juce::Colours::black.withAlpha(0.2f));
  for (float i = 4; i < thumbH - 4; i += 4) {
    g.drawHorizontalLine((int)(thumbBounds.getY() + i), thumbBounds.getX() + 2,
                         thumbBounds.getRight() - 2);
  }
}

void CarbonGoldLookAndFeel::drawToggleButton(juce::Graphics &g,
                                             juce::ToggleButton &button, bool,
                                             bool) {
  auto bounds = button.getLocalBounds().toFloat().reduced(2);
  bool isSelected = button.getToggleState();
  auto gold = juce::Colour::fromRGB(220, 180, 100);

  g.setColour(isSelected ? gold.withAlpha(0.25f)
                         : juce::Colours::black.withAlpha(0.3f));
  g.fillRoundedRectangle(bounds, 4.0f);
  g.setColour(isSelected ? gold : juce::Colours::white.withAlpha(0.2f));
  g.drawRoundedRectangle(bounds, 4.0f, isSelected ? 1.5f : 1.0f);

  g.setColour(isSelected ? gold : juce::Colours::white.withAlpha(0.8f));
  g.setFont(juce::Font(13.0f, juce::Font::bold));
  g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
}

// --- VacuumTapeAudioProcessorEditor Implementation ---

VacuumTapeAudioProcessorEditor::VacuumTapeAudioProcessorEditor(
    VacuumTapeAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  setLookAndFeel(&lnf);

  auto regularTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::PlayfairDisplayRegular_ttf,
      BinaryData::PlayfairDisplayRegular_ttfSize);
  auto boldTypeface = juce::Typeface::createSystemTypefaceFor(
      BinaryData::PlayfairDisplayBold_ttf,
      BinaryData::PlayfairDisplayBold_ttfSize);

  playfairRegular = juce::Font(regularTypeface);
  playfairBold = juce::Font(boldTypeface);
  epilogueBold = juce::Font(juce::Font::getDefaultSansSerifFontName(), 11.0f,
                            juce::Font::bold);

  createSlider(compGroup, "COMP_THRESH", "THRESHOLD", false);
  createSlider(compGroup, "COMP_DRAIN", "DRAIN", false);
  createSlider(compGroup, "COMP_RECOV", "RECOVERY", false);
  createSlider(compGroup, "COMP_TIME", "ATTACK TIME", false);

  createSlider(tapeGroup, "TAPE_DRIVE", "DRIVE", false);
  createSlider(tapeGroup, "TAPE_HYST", "HYSTERESIS", false);
  createSlider(tapeGroup, "TAPE_SPEED", "TAPE SPEED", false);
  createSlider(tapeGroup, "TAPE_WOW", "WOW MOD", false);
  createSlider(tapeGroup, "TAPE_FLUTTER", "FLUTTER", false);

  createSlider(masterGroup, "OVERSAMPLING", "OVERSAMPLE", false);
  createSlider(masterGroup, "OUTPUT_VOL", "OUTPUT VOL", false);

  setSize(500, 620); // Wider and shorter for extreme compactness
  startTimerHz(60);
}

VacuumTapeAudioProcessorEditor::~VacuumTapeAudioProcessorEditor() {
  setLookAndFeel(nullptr);
}

void VacuumTapeAudioProcessorEditor::createSlider(ParamGroup &group,
                                                  juce::String paramID,
                                                  juce::String labelText, bool,
                                                  bool) {
  auto slider = std::make_unique<juce::Slider>(labelText);
  slider->setSliderStyle(juce::Slider::LinearHorizontal);
  slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

  auto valLabel = std::make_unique<juce::Label>(paramID + "_val", "");
  valLabel->setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(),
                               12.0f, juce::Font::bold));
  valLabel->setJustificationType(juce::Justification::right);
  valLabel->setColour(juce::Label::textColourId,
                      juce::Colour::fromRGB(220, 180, 100));
  valLabel->setInterceptsMouseClicks(false, false);
  addAndMakeVisible(*valLabel);

  juce::String suffix = "";
  if (paramID == "TAPE_SPEED")
    suffix = " IPS";
  else if (paramID == "OUTPUT_VOL")
    suffix = " dB";
  else if (paramID == "COMP_TIME")
    suffix = " ms";
  else if (paramID.contains("HYST") || paramID.contains("WOW") ||
           paramID.contains("FLUTTER"))
    suffix = "%";

  auto updateText = [this, slider = slider.get(), valLabel = valLabel.get(),
                     paramID, suffix]() {
    auto *param = audioProcessor.apvts.getParameter(paramID);
    valLabel->setText(param->getCurrentValueAsText() + suffix,
                      juce::dontSendNotification);
  };

  slider->onValueChange = updateText;
  updateText();
  addAndMakeVisible(*slider);

  auto attachment =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          audioProcessor.apvts, paramID, *slider);

  auto label = std::make_unique<juce::Label>(labelText, labelText);
  label->setFont(epilogueBold.withHeight(12.0f));
  label->setJustificationType(juce::Justification::left);
  label->setColour(juce::Label::textColourId,
                   juce::Colours::white.withAlpha(0.7f));
  label->setInterceptsMouseClicks(false, false);
  addAndMakeVisible(*label);

  group.sliders.push_back(std::move(slider));
  group.labels.push_back(std::move(label));
  group.customComponents.push_back(std::move(valLabel));
  group.attachments.push_back(std::move(attachment));
}

void VacuumTapeAudioProcessorEditor::paint(juce::Graphics &g) {
  auto woodyDark = juce::Colour::fromRGB(45, 32, 25);
  auto woodyLight = juce::Colour::fromRGB(65, 48, 40);

  // Woody background with grain
  juce::ColourGradient woodGrad(woodyLight, 0, 0, woodyDark, 0,
                                (float)getHeight(), false);
  g.setGradientFill(woodGrad);
  g.fillAll();

  // Grain lines
  g.setColour(juce::Colours::black.withAlpha(0.15f));
  for (int y = 0; y < getHeight(); y += 3) {
    g.drawHorizontalLine(y, 0, (float)getWidth());
  }

  auto area = getLocalBounds();
  auto headerArea = area.removeFromTop(50).reduced(20, 5);
  g.setColour(juce::Colour::fromRGB(220, 180, 100));
  g.setFont(playfairBold.withHeight(28.0f));
  g.drawText("VACUUM TAPE SIM", headerArea, juce::Justification::centred);

  auto contentArea = area.reduced(25, 5);

  // Visualizer Panel (Compact)
  auto displayArea = contentArea.removeFromTop(120);
  drawRecessedPanel(g, displayArea);

  float midX = displayArea.getCentreX();
  float midY = displayArea.getCentreY();

  auto drawReel = [&](float x, float y, float angle) {
    float r = 38.0f;
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawEllipse(x - r, y - r, r * 2, r * 2, 2.0f);
    juce::Path p;
    p.addLineSegment(juce::Line<float>(0, -r * 0.85f, 0, r * 0.85f), 1.5f);
    p.addLineSegment(juce::Line<float>(-r * 0.85f, 0, r * 0.85f, 0), 1.5f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(x, y));
    g.strokePath(p, juce::PathStrokeType(1.0f));
  };

  drawReel(midX - 110, midY, reelAngle);
  drawReel(midX + 110, midY, -reelAngle);

  // Center Tube (Realistic 12AX7 style)
  auto tubeBounds = juce::Rectangle<float>(midX - 22, midY - 45, 44, 85);

  // 1. Tube Shadow (Base)
  g.setColour(juce::Colours::black.withAlpha(0.3f));
  g.fillRoundedRectangle(tubeBounds.translated(2, 4), 14.0f);

  // 2. Glass Envelope
  juce::ColourGradient glassGrad(
      juce::Colours::white.withAlpha(0.12f), tubeBounds.getX(),
      tubeBounds.getY(), juce::Colours::black.withAlpha(0.15f),
      tubeBounds.getRight(), tubeBounds.getBottom(), false);
  g.setGradientFill(glassGrad);
  g.fillRoundedRectangle(tubeBounds, 14.0f);

  // 3. Internal Components (Plate / Anode)
  auto plateArea = tubeBounds.reduced(8, 12);
  g.setColour(juce::Colour::fromRGB(35, 32, 30));
  g.fillRoundedRectangle(plateArea, 4.0f);
  g.setColour(juce::Colours::black.withAlpha(0.4f));
  g.drawRoundedRectangle(plateArea, 4.0f, 1.0f);

  // 4. Mica Spacers
  g.setColour(juce::Colour::fromRGB(60, 55, 50).withAlpha(0.8f));
  g.fillRoundedRectangle(tubeBounds.getCentreX() - 15, tubeBounds.getY() + 10,
                         30, 3, 1.0f);
  g.fillRoundedRectangle(tubeBounds.getCentreX() - 15,
                         tubeBounds.getBottom() - 13, 30, 3, 1.0f);

  // 5. Filament Glow (Dynamic)
  float glowInt = 0.6f + tubeGlow * 0.4f;
  auto filamentColor = juce::Colour::fromRGB(255, 140, 40);

  // Core heat
  g.setColour(filamentColor.withAlpha(glowInt));
  g.fillEllipse(midX - 1.5f, midY - 20, 3.0f, 40.0f);

  // Outer glow
  g.setColour(filamentColor.withAlpha(0.15f * glowInt));
  g.fillEllipse(midX - 15, midY - 25, 30, 50);

  // 6. Glass Highlights (Specular)
  g.setColour(juce::Colours::white.withAlpha(0.2f));
  auto sideHighlight = tubeBounds.withWidth(6).translated(3, 4).withHeight(70);
  g.drawRoundedRectangle(sideHighlight, 12.0f, 1.0f); // Side reflection

  // Dome reflection (Fixing the stray line error)
  juce::Path dome;
  auto arcRect = juce::Rectangle<float>(tubeBounds.getX() + 8,
                                        tubeBounds.getY() + 4, 28, 18);
  dome.addCentredArc(arcRect.getCentreX(), arcRect.getCentreY(),
                     arcRect.getWidth() * 0.5f, arcRect.getHeight() * 0.5f,
                     0.0f, -juce::MathConstants<float>::halfPi * 0.8f,
                     juce::MathConstants<float>::halfPi * 0.8f, true);
  g.setColour(juce::Colours::white.withAlpha(0.35f));
  g.strokePath(dome, juce::PathStrokeType(1.2f));

  // 7. Base pins (Subtle)
  g.setColour(juce::Colour::fromRGB(90, 85, 80));
  float centerX = tubeBounds.getCentreX();
  for (int i = -2; i <= 2; ++i) {
    g.fillRect(centerX + i * 7 - 1, tubeBounds.getBottom(), 2.0f, 5.0f);
  }

  // Meter bars
  auto drawBar = [&](float x, float y, float val) {
    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRect(x, y, 90.0f, 4.0f);
    g.setColour(juce::Colour::fromRGB(220, 180, 100));
    g.fillRect(x, y, 90.0f * std::clamp(val, 0.0f, 1.0f), 4.0f);
  };
  drawBar(midX - 100, midY + 42, audioProcessor.gainReductionLevel.load());
  drawBar(midX + 10, midY + 42, audioProcessor.saturationLevel.load() * 2.0f);
}

void VacuumTapeAudioProcessorEditor::drawRecessedPanel(
    juce::Graphics &g, juce::Rectangle<int> area) {
  g.setColour(juce::Colour::fromRGB(30, 22, 18));
  g.fillRoundedRectangle(area.toFloat(), 12.0f);
  g.setColour(juce::Colours::black.withAlpha(0.4f));
  g.drawRoundedRectangle(area.toFloat(), 12.0f, 2.0f);
}

void VacuumTapeAudioProcessorEditor::drawSectionHeader(
    juce::Graphics &g, juce::Rectangle<int> &area, juce::String title,
    juce::String) {
  g.setColour(juce::Colour::fromRGB(220, 180, 100));
  g.setFont(playfairBold.withHeight(20.0f).italicised());
  g.drawText(title, area.removeFromTop(25), juce::Justification::left);
  g.setColour(juce::Colours::black.withAlpha(0.3f));
  g.fillRect(area.getX(), area.getY() - 3, area.getWidth(), 2);
}

void VacuumTapeAudioProcessorEditor::resized() {
  auto area = getLocalBounds().reduced(25, 8);
  area.removeFromTop(50);  // Header
  area.removeFromTop(120); // Display
  area.removeFromTop(10);  // Spacing

  auto layoutInSection = [&](ParamGroup &group, int startIdx, int numItems,
                             int rows) {
    area.removeFromTop(30); // Section Header Spacing

    // Actually paint() draws the header, resized just moves area
    auto sectionArea = area.removeFromTop(rows * 48);
    int itemsPlaced = 0;
    int cols = 2;
    for (int r = 0; r < rows; ++r) {
      auto rowArea = sectionArea.removeFromTop(48);
      for (int c = 0; c < cols; ++c) {
        if (itemsPlaced >= numItems)
          break;
        auto cell = rowArea.removeFromLeft(rowArea.getWidth() / (cols - c));
        auto inner = cell.reduced(8, 4);

        group.labels[startIdx + itemsPlaced]->setBounds(
            inner.removeFromTop(15));
        group.customComponents[startIdx + itemsPlaced]->setBounds(
            inner.removeFromBottom(15));
        group.sliders[startIdx + itemsPlaced]->setBounds(inner);

        itemsPlaced++;
      }
    }
    area.removeFromTop(10);
  };

  // We need a dummy graphics for font metrics if we used them, but we use fixed
  // heights
  layoutInSection(compGroup, 0, 4, 2);
  layoutInSection(tapeGroup, 0, 5, 3);
  layoutInSection(masterGroup, 0, 2, 1);
}

// In the header I need to add juce::Graphics
// g_dummy{juce::Image(juce::Image::RGB, 1, 1, false)}; or similar if I wanted,
// but I'll just hardcode the heights for now to be safe.
// Wait, I used g_dummy above, I should remove it.

void VacuumTapeAudioProcessorEditor::timerCallback() {
  reelAngle += 0.04f;
  if (reelAngle >= juce::MathConstants<float>::twoPi)
    reelAngle -= juce::MathConstants<float>::twoPi;
  tubeGlow =
      std::sin(juce::Time::getMillisecondCounterHiRes() * 0.005f) * 0.5f + 0.5f;
  repaint();
}
