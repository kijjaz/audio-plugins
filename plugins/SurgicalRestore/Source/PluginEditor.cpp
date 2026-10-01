#include "PluginEditor.h"

SurgicalRestoreAudioProcessorEditor::SurgicalRestoreAudioProcessorEditor (SurgicalRestoreAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setLookAndFeel (&customLookAndFeel);

    // Setup rotary sliders
    auto setupKnob = [this](juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
        slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        slider.setColour (juce::Slider::textBoxTextColourId, sr_ui::CarbonGoldLookAndFeel::goldHighlight);
        addAndMakeVisible (slider);

        label.setText (text, juce::dontSendNotification);
        label.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, sr_ui::CarbonGoldLookAndFeel::textOffWhite);
        addAndMakeVisible (label);
    };

    setupKnob (clickSensitivitySlider, clickSensitivityLabel, "DE-CLICK");
    setupKnob (sideBoostSlider, sideBoostLabel, "SIDE BOOST");
    setupKnob (crackleAmountSlider, crackleAmountLabel, "DE-CRACKLE");
    setupKnob (hissReductionSlider, hissReductionLabel, "DE-HISS (dB)");
    setupKnob (harmonicShieldSlider, harmonicShieldLabel, "HARM SHIELD");

    // Presets Dropdown
    presetComboBox.addItem ("Vinyl LP (33/45 RPM Microgroove)", 1);
    presetComboBox.addItem ("Shellac 78 RPM (Archival)", 2);
    presetComboBox.addItem ("Magnetic Tape / Cassette", 3);
    presetComboBox.addItem ("Transparent Vocal Solo", 4);
    presetComboBox.setSelectedId (processorRef.getCurrentProgram() + 1, juce::dontSendNotification);
    presetComboBox.onChange = [this]()
    {
        int id = presetComboBox.getSelectedId();
        if (id >= 1 && id <= 4)
            processorRef.setCurrentProgram (id - 1);
    };
    presetComboBox.setColour (juce::ComboBox::backgroundColourId, sr_ui::CarbonGoldLookAndFeel::carbonDark);
    presetComboBox.setColour (juce::ComboBox::textColourId, sr_ui::CarbonGoldLookAndFeel::goldHighlight);
    presetComboBox.setColour (juce::ComboBox::outlineColourId, sr_ui::CarbonGoldLookAndFeel::goldAccent.withAlpha (0.4f));
    addAndMakeVisible (presetComboBox);

    // Toggles
    rumbleFilterButton.setButtonText ("RUMBLE HPF (25Hz)");
    addAndMakeVisible (rumbleFilterButton);

    azimuthAlignButton.setButtonText ("AUTO AZIMUTH (TAPE)");
    addAndMakeVisible (azimuthAlignButton);

    deltaListenButton.setButtonText ("AUDITION DELTA");
    addAndMakeVisible (deltaListenButton);

    bypassButton.setButtonText ("BYPASS");
    addAndMakeVisible (bypassButton);

    // Attachments
    clickSensAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "click_sensitivity", clickSensitivitySlider);
    sideBoostAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "side_boost", sideBoostSlider);
    crackleAmtAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "crackle_amount", crackleAmountSlider);
    hissReductAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "hiss_reduction", hissReductionSlider);
    harmShieldAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "harmonic_shield", harmonicShieldSlider);

    rumbleFilterAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "rumble_filter", rumbleFilterButton);
    azimuthAlignAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "azimuth_align", azimuthAlignButton);
    deltaListenAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "delta_listen", deltaListenButton);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "bypass", bypassButton);

    setSize (760, 380);
}

SurgicalRestoreAudioProcessorEditor::~SurgicalRestoreAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SurgicalRestoreAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Matte Carbon Background
    g.fillAll (sr_ui::CarbonGoldLookAndFeel::carbonDark);

    // Brushed metal console frame
    auto bounds = getLocalBounds().toFloat().reduced (8.0f);
    g.setColour (sr_ui::CarbonGoldLookAndFeel::carbonMatte);
    g.fillRoundedRectangle (bounds, 6.0f);

    g.setColour (sr_ui::CarbonGoldLookAndFeel::carbonSurface.brighter (0.15f));
    g.drawRoundedRectangle (bounds, 6.0f, 1.5f);

    // Header Gold Accent Bar
    auto headerBounds = bounds.removeFromTop (50.0f);
    g.setColour (sr_ui::CarbonGoldLookAndFeel::goldAccent);
    g.drawLine (headerBounds.getX() + 16.0f, headerBounds.getBottom(),
                headerBounds.getRight() - 16.0f, headerBounds.getBottom(), 1.0f);

    // Title
    g.setColour (sr_ui::CarbonGoldLookAndFeel::goldHighlight);
    g.setFont (juce::FontOptions ("Helvetica Neue", 18.0f, juce::Font::bold));
    g.drawText ("SURGICAL RESTORE", headerBounds.reduced (16.0f, 0.0f), juce::Justification::centredLeft);

    g.setColour (sr_ui::CarbonGoldLookAndFeel::textDim);
    g.setFont (juce::FontOptions (12.0f, juce::Font::plain));
    g.drawText ("NEURAL MASTERING CONSOLE • M/S & AZIMUTH", headerBounds.reduced (16.0f, 0.0f), juce::Justification::centredRight);
}

void SurgicalRestoreAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20, 16);
    auto header = area.removeFromTop (50);

    // Place Preset selector in top right corner of header
    presetComboBox.setBounds (header.getRight() - 250, header.getY() + 12, 230, 26);

    // Bottom action bar
    auto bottomBar = area.removeFromBottom (34);
    rumbleFilterButton.setBounds (bottomBar.removeFromLeft (150));
    bottomBar.removeFromLeft (12);
    azimuthAlignButton.setBounds (bottomBar.removeFromLeft (165));
    bottomBar.removeFromLeft (12);
    deltaListenButton.setBounds (bottomBar.removeFromLeft (130));
    bottomBar.removeFromLeft (12);
    bypassButton.setBounds (bottomBar.removeFromLeft (80));

    area.removeFromBottom (16);

    // 5 Knobs layout across center
    int numKnobs = 5;
    int knobWidth = area.getWidth() / numKnobs;

    auto r1 = area.removeFromLeft (knobWidth);
    clickSensitivityLabel.setBounds (r1.removeFromTop (20));
    clickSensitivitySlider.setBounds (r1.reduced (6));

    auto r2 = area.removeFromLeft (knobWidth);
    sideBoostLabel.setBounds (r2.removeFromTop (20));
    sideBoostSlider.setBounds (r2.reduced (6));

    auto r3 = area.removeFromLeft (knobWidth);
    crackleAmountLabel.setBounds (r3.removeFromTop (20));
    crackleAmountSlider.setBounds (r3.reduced (6));

    auto r4 = area.removeFromLeft (knobWidth);
    hissReductionLabel.setBounds (r4.removeFromTop (20));
    hissReductionSlider.setBounds (r4.reduced (6));

    auto r5 = area;
    harmonicShieldLabel.setBounds (r5.removeFromTop (20));
    harmonicShieldSlider.setBounds (r5.reduced (6));
}
