#include "PluginEditor.h"

FlyByAudioProcessorEditor::FlyByAudioProcessorEditor(FlyByAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), radarComponent(p)
{
    setLookAndFeel(&customLnF);

    // Centerpiece Radar Display
    addAndMakeVisible(radarComponent);

    // Flight Mode Selector
    flightModeCombo.addItem("Manual 3D Placement", 1);
    flightModeCombo.addItem("Swoop Dive-Bomb", 2);
    flightModeCombo.addItem("Helical Orbit", 3);
    flightModeCombo.addItem("Zenith Flyover", 4);
    flightModeCombo.addItem("Hornet Flutter", 5);
    addAndMakeVisible(flightModeCombo);
    flightModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getAPVTS(), "flightMode", flightModeCombo);

    flightModeLabel.setText("TRAJECTORY MODE", juce::dontSendNotification);
    flightModeLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    flightModeLabel.setColour(juce::Label::textColourId, FlyBy::UI::FlyByLookAndFeel::goldAccent);
    flightModeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(flightModeLabel);

    // Dials setup
    setupRotary(elevationSlider, elevationLabel, "ELEVATION", " deg");
    elevationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "elevation", elevationSlider);

    setupRotary(azimuthSlider, azimuthLabel, "AZIMUTH", " deg");
    azimuthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "azimuth", azimuthSlider);

    setupRotary(distanceSlider, distanceLabel, "DISTANCE", " m");
    distanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "distance", distanceSlider);

    setupRotary(speedSlider, speedLabel, "SPEED", " Hz");
    speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "speed", speedSlider);

    setupRotary(turbulenceSlider, turbulenceLabel, "TURBULENCE", " %");
    turbulenceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "turbulence", turbulenceSlider);

    setupRotary(dopplerSlider, dopplerLabel, "DOPPLER", " x");
    dopplerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "doppler", dopplerSlider);

    setupRotary(proximitySlider, proximityLabel, "PROXIMITY", " m");
    proximityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "proximityRadius", proximitySlider);

    setupRotary(pinnaScaleSlider, pinnaScaleLabel, "PINNA SCALE", " x");
    pinnaScaleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "pinnaScale", pinnaScaleSlider);

    setupRotary(elevationStrengthSlider, elevationStrengthLabel, "EL. DEPTH", " x");
    elevationStrengthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "elevationStrength", elevationStrengthSlider);

    setupRotary(crispnessSlider, crispnessLabel, "CRISPNESS", " %");
    crispnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "crispness", crispnessSlider);

    setupRotary(groundingSlider, groundingLabel, "GROUNDING", " %");
    groundingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "grounding", groundingSlider);

    setupRotary(dryWetSlider, dryWetLabel, "MIX", " %");
    dryWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "dryWet", dryWetSlider);

    setSize(880, 520);
}

FlyByAudioProcessorEditor::~FlyByAudioProcessorEditor() {
    setLookAndFeel(nullptr);
}

void FlyByAudioProcessorEditor::setupRotary(juce::Slider& slider, juce::Label& label,
                                           const juce::String& text, const juce::String& suffix) {
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 16);
    slider.setTextValueSuffix(suffix);
    slider.setColour(juce::Slider::textBoxTextColourId, FlyBy::UI::FlyByLookAndFeel::goldGlow);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::Font(10.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, FlyBy::UI::FlyByLookAndFeel::goldAccent.withAlpha(0.85f));
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void FlyByAudioProcessorEditor::paint(juce::Graphics& g) {
    // Carbon dark textured background
    g.fillAll(FlyBy::UI::FlyByLookAndFeel::carbonDark);

    // Top Header Banner
    auto headerBounds = juce::Rectangle<int>(0, 0, getWidth(), 50);
    g.setColour(FlyBy::UI::FlyByLookAndFeel::carbonPanel);
    g.fillRect(headerBounds);
    g.setColour(FlyBy::UI::FlyByLookAndFeel::goldBezel.withAlpha(0.5f));
    g.drawHorizontalLine(50, 0, getWidth());

    // Title & Brand
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.setColour(FlyBy::UI::FlyByLookAndFeel::goldAccent);
    g.drawText("FLYBY", 20, 8, 120, 24, juce::Justification::left);

    g.setFont(juce::Font(10.0f));
    g.setColour(FlyBy::UI::FlyByLookAndFeel::textMuted);
    g.drawText("3D BINAURAL ELEVATION & TRAJECTORY ENGINE", 20, 30, 320, 14, juce::Justification::left);

    // Section group frames
    auto lowerArea = juce::Rectangle<int>(12, 330, getWidth() - 24, 175);
    g.setColour(FlyBy::UI::FlyByLookAndFeel::carbonPanel);
    g.fillRoundedRectangle(lowerArea.toFloat(), 6.0f);
    g.setColour(FlyBy::UI::FlyByLookAndFeel::carbonBorder);
    g.drawRoundedRectangle(lowerArea.toFloat(), 6.0f, 1.0f);

    // Sub-section dividers in lower panel
    int sec1W = 215;
    int sec2W = 270;
    g.drawVerticalLine(lowerArea.getX() + sec1W, lowerArea.getY() + 10, lowerArea.getBottom() - 10);
    g.drawVerticalLine(lowerArea.getX() + sec1W + sec2W, lowerArea.getY() + 10, lowerArea.getBottom() - 10);

    // Sub-section header labels
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(FlyBy::UI::FlyByLookAndFeel::goldAccent.withAlpha(0.7f));
    g.drawText("1. MANUAL COORDS", lowerArea.getX() + 12, lowerArea.getY() + 8, sec1W - 20, 14, juce::Justification::left);
    g.drawText("2. FLIGHT DYNAMICS", lowerArea.getX() + sec1W + 12, lowerArea.getY() + 8, sec2W - 20, 14, juce::Justification::left);
    g.drawText("3. ACOUSTICS, GROUNDING & MIX", lowerArea.getX() + sec1W + sec2W + 12, lowerArea.getY() + 8, 300, 14, juce::Justification::left);
}

void FlyByAudioProcessorEditor::resized() {
    // Mode selector top-right
    flightModeLabel.setBounds(getWidth() - 360, 10, 140, 30);
    flightModeCombo.setBounds(getWidth() - 210, 12, 190, 26);

    // Centerpiece Radar Display
    radarComponent.setBounds(12, 58, getWidth() - 24, 260);

    // Lower Controls Panel (Y: 330, H: 175)
    int startY = 356;
    int knobW = 62;
    int knobH = 68;
    int labelH = 14;

    // Section 1: Manual Coordinates (Azimuth, Elevation, Distance)
    int sec1X = 20;
    azimuthSlider.setBounds(sec1X, startY, knobW, knobH);
    azimuthLabel.setBounds(sec1X, startY + knobH, knobW, labelH);

    elevationSlider.setBounds(sec1X + 68, startY, knobW, knobH);
    elevationLabel.setBounds(sec1X + 68, startY + knobH, knobW, labelH);

    distanceSlider.setBounds(sec1X + 136, startY, knobW, knobH);
    distanceLabel.setBounds(sec1X + 136, startY + knobH, knobW, labelH);

    // Section 2: Flight Dynamics (Speed, Turbulence, Doppler, Proximity)
    int sec2X = 240;
    speedSlider.setBounds(sec2X, startY, knobW, knobH);
    speedLabel.setBounds(sec2X, startY + knobH, knobW, labelH);

    turbulenceSlider.setBounds(sec2X + 64, startY, knobW, knobH);
    turbulenceLabel.setBounds(sec2X + 64, startY + knobH, knobW, labelH);

    dopplerSlider.setBounds(sec2X + 128, startY, knobW, knobH);
    dopplerLabel.setBounds(sec2X + 128, startY + knobH, knobW, labelH);

    proximitySlider.setBounds(sec2X + 192, startY, knobW, knobH);
    proximityLabel.setBounds(sec2X + 192, startY + knobH, knobW, labelH);

    // Section 3: Acoustics & Grounding & Mix (Pinna Scale, Elevation Depth, Crispness, Grounding, Mix)
    int sec3X = 512;
    pinnaScaleSlider.setBounds(sec3X, startY, knobW, knobH);
    pinnaScaleLabel.setBounds(sec3X, startY + knobH, knobW, labelH);

    elevationStrengthSlider.setBounds(sec3X + 66, startY, knobW, knobH);
    elevationStrengthLabel.setBounds(sec3X + 66, startY + knobH, knobW, labelH);

    crispnessSlider.setBounds(sec3X + 132, startY, knobW, knobH);
    crispnessLabel.setBounds(sec3X + 132, startY + knobH, knobW, labelH);

    groundingSlider.setBounds(sec3X + 198, startY, knobW, knobH);
    groundingLabel.setBounds(sec3X + 198, startY + knobH, knobW, labelH);

    dryWetSlider.setBounds(sec3X + 264, startY, knobW, knobH);
    dryWetLabel.setBounds(sec3X + 264, startY + knobH, knobW, labelH);
}
