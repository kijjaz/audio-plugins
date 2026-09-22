#include "PluginEditor.h"
#include "DSP/DiodeModels.h"

FriedTofuMBAudioProcessorEditor::FriedTofuMBAudioProcessorEditor(FriedTofuMBAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      scopeComponent(p),
      transferCurveComponent(p, *this)
{
    setLookAndFeel(&customLookAndFeel);

    // Setup Visualizers
    addAndMakeVisible(scopeComponent);
    addAndMakeVisible(transferCurveComponent);

    // Helper lambda for configuring sliders
    auto setupRotary = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        slider.setTextValueSuffix(suffix);
        slider.setColour(juce::Slider::textBoxTextColourId, FriedTofuUI::FriedTofuLookAndFeel::textLight);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(10.5f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::textMuted);
        addAndMakeVisible(label);
    };

    // Master Controls
    setupRotary(masterDriveSlider, masterDriveLabel, "MASTER DRIVE", " dB");
    setupRotary(masterTrimSlider,  masterTrimLabel,  "MASTER TRIM",  " dB");
    setupRotary(masterMixSlider,   masterMixLabel,   "MASTER MIX",   " %");

    // Crossover sliders
    setupRotary(crossoverLowSlider,  crossoverLowLabel,  "XOVER LOW",  " Hz");
    setupRotary(crossoverHighSlider, crossoverHighLabel, "XOVER HIGH", " Hz");

    // Band selector buttons
    const juce::String bandTitles[3] = { "LOW BAND", "MID BAND", "HIGH BAND" };
    for (int b = 0; b < 3; ++b)
    {
        bandSelectButtons[b].setButtonText(bandTitles[b]);
        bandSelectButtons[b].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff21262d));
        bandSelectButtons[b].setColour(juce::TextButton::textColourOffId, FriedTofuUI::FriedTofuLookAndFeel::textLight);
        bandSelectButtons[b].onClick = [this, b] { setSelectedBand(b); };
        addAndMakeVisible(bandSelectButtons[b]);
    }

    // Band Active Controls
    setupRotary(bandDriveSlider,    bandDriveLabel,    "BAND DRIVE", " dB");
    setupRotary(bandNetworkZSlider, bandNetworkZLabel, "NETWORK Z",  " Ω");
    setupRotary(bandGainSlider,     bandGainLabel,     "BAND LEVEL", " dB");

    // Diode Combos
    for (int i = 0; i < FriedTofuDSP::NumDiodeTypes; ++i)
    {
        diodeACombo.addItem(FriedTofuDSP::AvailableDiodes[i].name, i + 1);
        diodeBCombo.addItem(FriedTofuDSP::AvailableDiodes[i].name, i + 1);
    }
    addAndMakeVisible(diodeACombo);
    addAndMakeVisible(diodeBCombo);

    diodeALabel.setText("PATH A (+)", juce::dontSendNotification);
    diodeALabel.setFont(juce::Font(10.5f, juce::Font::bold));
    diodeALabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    addAndMakeVisible(diodeALabel);

    diodeBLabel.setText("PATH B (-)", juce::dontSendNotification);
    diodeBLabel.setFont(juce::Font(10.5f, juce::Font::bold));
    diodeBLabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    addAndMakeVisible(diodeBLabel);

    // Toggles
    addAndMakeVisible(muteToggle);
    addAndMakeVisible(soloToggle);
    addAndMakeVisible(bypassToggle);

    // Header
    titleLabel.setText("FRIED TOFU MULTIBAND", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("LINEAR-PHASE WDF DIODE CLIPPING & SATURATION", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(10.0f, juce::Font::bold));
    subtitleLabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::textMuted);
    addAndMakeVisible(subtitleLabel);

    // Attach master controls
    auto& apvts = audioProcessor.getAPVTS();
    masterDriveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterDrive", masterDriveSlider);
    masterTrimAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterTrim", masterTrimSlider);
    masterMixAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterMix", masterMixSlider);
    xoverLowAttach    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "crossoverLow", crossoverLowSlider);
    xoverHighAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "crossoverHigh", crossoverHighSlider);

    // Connect default selected band (Mid)
    setSelectedBand(1);

    setSize(880, 600);
    startTimerHz(30);
}

FriedTofuMBAudioProcessorEditor::~FriedTofuMBAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void FriedTofuMBAudioProcessorEditor::setSelectedBand(int band)
{
    selectedBand = std::clamp(band, 0, 2);
    updateBandControls();

    for (int b = 0; b < 3; ++b)
    {
        if (b == selectedBand)
        {
            bandSelectButtons[b].setColour(juce::TextButton::buttonColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold.withAlpha(0.35f));
            bandSelectButtons[b].setColour(juce::TextButton::textColourOffId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
        }
        else
        {
            bandSelectButtons[b].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff21262d));
            bandSelectButtons[b].setColour(juce::TextButton::textColourOffId, FriedTofuUI::FriedTofuLookAndFeel::textLight);
        }
    }
    repaint();
}

void FriedTofuMBAudioProcessorEditor::updateBandControls()
{
    auto& apvts = audioProcessor.getAPVTS();
    const juce::String prefixes[3] = { "low_", "mid_", "high_" };
    const juce::String p = prefixes[selectedBand];

    // Detach first
    bandDriveAttach.reset();
    bandNetworkZAttach.reset();
    bandGainAttach.reset();
    diodeAAttach.reset();
    diodeBAttach.reset();
    muteAttach.reset();
    soloAttach.reset();
    bypassAttach.reset();

    // Reattach to current band parameters
    bandDriveAttach    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, p + "drive", bandDriveSlider);
    bandNetworkZAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, p + "networkZ", bandNetworkZSlider);
    bandGainAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, p + "gain", bandGainSlider);
    diodeAAttach       = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, p + "diodeA", diodeACombo);
    diodeBAttach       = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, p + "diodeB", diodeBCombo);
    muteAttach         = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, p + "mute", muteToggle);
    soloAttach         = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, p + "solo", soloToggle);
    bypassAttach       = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, p + "bypass", bypassToggle);
}

void FriedTofuMBAudioProcessorEditor::timerCallback()
{
}

void FriedTofuMBAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background Dark Matte Carbon
    g.fillAll(FriedTofuUI::FriedTofuLookAndFeel::bgDark);

    // Visualizer container panels
    auto bounds = getLocalBounds().reduced(16);
    auto visArea = bounds.removeFromTop(200);

    // Frame for visualizers
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::lineBorder);
    g.drawRoundedRectangle(visArea.toFloat(), 6.0f, 1.0f);

    // Band Section Frame
    auto bandArea = bounds.removeFromTop(210);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::lineBorder);
    g.drawRoundedRectangle(bandArea.toFloat(), 6.0f, 1.0f);

    // Band Section Title
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    const juce::String bandTitles[3] = { "LOW BAND DIODE MATRIX", "MID BAND DIODE MATRIX", "HIGH BAND DIODE MATRIX" };
    g.drawText(bandTitles[selectedBand], bandArea.reduced(8, 4), juce::Justification::topLeft, false);
}

void FriedTofuMBAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(16);

    // Header
    auto headerArea = bounds.removeFromTop(36);
    titleLabel.setBounds(headerArea.removeFromLeft(300));
    subtitleLabel.setBounds(headerArea);

    bounds.removeFromTop(6);

    // Visualizers (Top)
    auto visArea = bounds.removeFromTop(180);
    auto leftVis = visArea.removeFromLeft(visArea.getWidth() / 2).reduced(4);
    auto rightVis = visArea.reduced(4);

    scopeComponent.setBounds(leftVis);
    transferCurveComponent.setBounds(rightVis);

    bounds.removeFromTop(12);

    // Band Select Buttons Bar
    auto bandBtnArea = bounds.removeFromTop(32);
    int btnW = bandBtnArea.getWidth() / 3;
    bandSelectButtons[0].setBounds(bandBtnArea.removeFromLeft(btnW).reduced(4, 2));
    bandSelectButtons[1].setBounds(bandBtnArea.removeFromLeft(btnW).reduced(4, 2));
    bandSelectButtons[2].setBounds(bandBtnArea.reduced(4, 2));

    bounds.removeFromTop(8);

    // Band Controls Area
    auto bandArea = bounds.removeFromTop(140);
    int knobW = 85;
    int knobH = 100;

    auto bandKnobArea = bandArea.removeFromLeft(knobW * 3 + 20);
    bandDriveSlider.setBounds(bandKnobArea.removeFromLeft(knobW).withSizeKeepingCentre(knobW, knobH));
    bandDriveLabel.setBounds(bandDriveSlider.getX(), bandDriveSlider.getY() - 16, knobW, 16);

    bandNetworkZSlider.setBounds(bandKnobArea.removeFromLeft(knobW).withSizeKeepingCentre(knobW, knobH));
    bandNetworkZLabel.setBounds(bandNetworkZSlider.getX(), bandNetworkZSlider.getY() - 16, knobW, 16);

    bandGainSlider.setBounds(bandKnobArea.removeFromLeft(knobW).withSizeKeepingCentre(knobW, knobH));
    bandGainLabel.setBounds(bandGainSlider.getX(), bandGainSlider.getY() - 16, knobW, 16);

    // Diodes and Toggles
    auto diodeArea = bandArea.removeFromLeft(240).reduced(10, 10);
    diodeALabel.setBounds(diodeArea.removeFromTop(16));
    diodeACombo.setBounds(diodeArea.removeFromTop(26));
    diodeArea.removeFromTop(10);
    diodeBLabel.setBounds(diodeArea.removeFromTop(16));
    diodeBCombo.setBounds(diodeArea.removeFromTop(26));

    auto toggleArea = bandArea.reduced(10, 10);
    muteToggle.setBounds(toggleArea.removeFromTop(28));
    soloToggle.setBounds(toggleArea.removeFromTop(28));
    bypassToggle.setBounds(toggleArea.removeFromTop(28));

    bounds.removeFromTop(10);

    // Master & Crossover Bottom Bar
    auto bottomArea = bounds;
    int bKnobW = 90;
    crossoverLowSlider.setBounds(bottomArea.removeFromLeft(bKnobW).withSizeKeepingCentre(bKnobW, knobH));
    crossoverLowLabel.setBounds(crossoverLowSlider.getX(), crossoverLowSlider.getY() - 16, bKnobW, 16);

    crossoverHighSlider.setBounds(bottomArea.removeFromLeft(bKnobW).withSizeKeepingCentre(bKnobW, knobH));
    crossoverHighLabel.setBounds(crossoverHighSlider.getX(), crossoverHighSlider.getY() - 16, bKnobW, 16);

    bottomArea.removeFromLeft(40); // Spacer

    masterDriveSlider.setBounds(bottomArea.removeFromLeft(bKnobW).withSizeKeepingCentre(bKnobW, knobH));
    masterDriveLabel.setBounds(masterDriveSlider.getX(), masterDriveSlider.getY() - 16, bKnobW, 16);

    masterTrimSlider.setBounds(bottomArea.removeFromLeft(bKnobW).withSizeKeepingCentre(bKnobW, knobH));
    masterTrimLabel.setBounds(masterTrimSlider.getX(), masterTrimSlider.getY() - 16, bKnobW, 16);

    masterMixSlider.setBounds(bottomArea.removeFromLeft(bKnobW).withSizeKeepingCentre(bKnobW, knobH));
    masterMixLabel.setBounds(masterMixSlider.getX(), masterMixSlider.getY() - 16, bKnobW, 16);
}
