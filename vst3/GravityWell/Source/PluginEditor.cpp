#include "PluginEditor.h"
#include "PluginProcessor.h"

GravityWellAudioProcessorEditor::GravityWellAudioProcessorEditor(
    GravityWellAudioProcessor &p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

  // Speed
  speedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  addAndMakeVisible(speedSlider);

  speedLabel.setText("Speed", juce::dontSendNotification);
  speedLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(speedLabel);

  speedAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "speed", speedSlider);

  // Gravity
  gravitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  gravitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  addAndMakeVisible(gravitySlider);

  gravityLabel.setText("Gravity", juce::dontSendNotification);
  gravityLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(gravityLabel);

  gravityAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "gravity", gravitySlider);

  // Dust Amt
  dustAmtSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  dustAmtSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  addAndMakeVisible(dustAmtSlider);

  dustAmtLabel.setText("Dust Amt", juce::dontSendNotification);
  dustAmtLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(dustAmtLabel);

  dustAmtAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "dust_amt", dustAmtSlider);

  // Dust Dens
  dustDensSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
  dustDensSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
  addAndMakeVisible(dustDensSlider);

  dustDensLabel.setText("Dust Dens", juce::dontSendNotification);
  dustDensLabel.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(dustDensLabel);

  dustDensAttach =
      std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
          p.apvts, "dust_dens", dustDensSlider);

  // Help UI
  addAndMakeVisible(helpButton);
  helpButton.onClick = [this] {
    showHelp = !showHelp;
    helpOverlay.setVisible(showHelp);
  };

  addChildComponent(helpOverlay); // Hidden by default
  helpOverlay.setText(
      "GRAVITY WELL v2.01\n\n"
      "Physics Engine: 3-Body Simulation (Sun, Planet, Moon) modulates\n"
      "FM Synthesis indices and oscillator detuning.\n\n"
      "Controls:\n"
      "- Speed: Simulation time rate.\n"
      "- Gravity: Modulation depth.\n"
      "- Dust Amt: Granular synthesis mix.\n"
      "- Dust Dens: Granular density/jitter.\n\n"
      "Created by Kijjaz",
      juce::dontSendNotification);
  helpOverlay.setJustificationType(juce::Justification::centred);
  helpOverlay.setColour(juce::Label::backgroundColourId,
                        juce::Colours::black.withAlpha(0.8f));
  helpOverlay.setColour(juce::Label::textColourId, juce::Colours::white);
  helpOverlay.setColour(juce::Label::outlineColourId, juce::Colours::gold);

  // Load BG
  bgImage = juce::ImageCache::getFromMemory(
      BinaryData::gravity_well_bg_no_text_png,
      BinaryData::gravity_well_bg_no_text_pngSize);

  setSize(500, 400); // Slightly larger
  startTimerHz(60);
}

GravityWellAudioProcessorEditor::~GravityWellAudioProcessorEditor() {
  stopTimer();
}

void GravityWellAudioProcessorEditor::timerCallback() { repaint(); }

void GravityWellAudioProcessorEditor::paint(juce::Graphics &g) {
  g.fillAll(juce::Colours::black);

  if (bgImage.isValid()) {
    g.drawImage(bgImage, getLocalBounds().toFloat());
  }

  // Draw Scope
  g.setColour(juce::Colours::green);
  // Get scope data
  float scopeData[512];
  audioProcessor.getScopeData(scopeData, 256); // 256 samples stereo?

  // Goniometer X/Y
  juce::Path p;
  float cx = getWidth() - 80;
  float cy = getHeight() - 80;
  float scale = 60.0f;

  // Use scopeData L/R for XY
  bool first = true;
  for (int i = 0; i < 256; ++i) {
    float l = scopeData[i * 2];
    float r = scopeData[i * 2 + 1];
    // Goniometer: X = L-R, Y = L+R (Mid/Side) or just L/R?
    // L vs R (XY mode)
    float x = cx + l * scale;
    float y = cy - r * scale;
    if (first) {
      p.startNewSubPath(x, y);
      first = false;
    } else {
      p.lineTo(x, y);
    }
  }
  g.strokePath(p, juce::PathStrokeType(1.0f));

  // Title
  g.setFont(24.0f);
  g.setColour(juce::Colours::gold);
  g.drawText("GRAVITY WELL", 20, 20, 200, 30, juce::Justification::left);

  // Visualizer Animation
  double time = juce::Time::getMillisecondCounter() / 1000.0 * 0.5 *
                audioProcessor.apvts.getRawParameterValue("speed")
                    ->load(); // Use param speed

  // Center
  float cX = getWidth() / 2.0f;
  float cY = 150.0f;

  g.setColour(juce::Colours::gold);
  g.fillEllipse(cX - 15, cY - 15, 30, 30); // Sun

  // Planet
  float pR = 80.0f;
  float pX = cX + std::cos(time) * pR;
  float pY = cY + std::sin(time) * pR;
  g.setColour(juce::Colours::cyan);
  g.fillEllipse(pX - 10, pY - 10, 20, 20);

  // Moon
  float mR = 30.0f;
  float mX = pX + std::cos(time * 2.5) * mR;
  float mY = pY + std::sin(time * 2.5) * mR;
  g.setColour(juce::Colours::white);
  g.fillEllipse(mX - 5, mY - 5, 10, 10);
}

void GravityWellAudioProcessorEditor::resized() {
  int y = 250;
  int w = 70;
  int h = 70;
  int gap = 20;
  int x = 40;

  speedSlider.setBounds(x, y, w, h);
  speedLabel.setBounds(x, y + h, w, 20);

  x += w + gap;
  gravitySlider.setBounds(x, y, w, h);
  gravityLabel.setBounds(x, y + h, w, 20);

  x += w + gap;
  dustAmtSlider.setBounds(x, y, w, h);
  dustAmtLabel.setBounds(x, y + h, w, 20);

  x += w + gap;
  dustDensSlider.setBounds(x, y, w, h);
  dustDensLabel.setBounds(x, y + h, w, 20);

  helpButton.setBounds(getWidth() - 30, 10, 20, 20);
  helpOverlay.setBounds(50, 50, getWidth() - 100, getHeight() - 150);
}
