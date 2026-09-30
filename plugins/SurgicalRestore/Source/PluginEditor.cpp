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
        label.setFont (juce::Font (13.0f, juce::Font::bold));
        label.setJustificationType (juce::Justification::centred);
        label.setColour (juce::Label::textColourId, sr_ui::CarbonGoldLookAndFeel::textOffWhite);
        addAndMakeVisible (label);
    };

    setupKnob (clickSensitivitySlider, clickSensitivityLabel, "DE-CLICK");
    setupKnob (crackleAmountSlider, crackleAmountLabel, "DE-CRACKLE");
    setupKnob (hissReductionSlider, hissReductionLabel, "DE-HISS (dB)");
    setupKnob (harmonicShieldSlider, harmonicShieldLabel, "HARM SHIELD");

    // Toggles
    deltaListenButton.setButtonText ("AUDITION DELTA");
    addAndMakeVisible (deltaListenButton);

    bypassButton.setButtonText ("BYPASS");
    addAndMakeVisible (bypassButton);

    // Attachments
    clickSensAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "click_sensitivity", clickSensitivitySlider);
    crackleAmtAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "crackle_amount", crackleAmountSlider);
    hissReductAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "hiss_reduction", hissReductionSlider);
    harmShieldAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processorRef.apvts, "harmonic_shield", harmonicShieldSlider);

    deltaListenAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "delta_listen", deltaListenButton);
    bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processorRef.apvts, "bypass", bypassButton);

    setSize (640, 360);
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
    g.setFont (juce::Font ("Helvetica Neue", 18.0f, juce::Font::bold));
    g.drawText ("SURGICAL RESTORE", headerBounds.reduced (16.0f, 0.0f), juce::Justification::centredLeft);

    g.setColour (sr_ui::CarbonGoldLookAndFeel::textDim);
    g.setFont (juce::Font (12.0f, juce::Font::plain));
    g.drawText ("NEURAL MASTERING CONSOLE • ARA 2 / VST3", headerBounds.reduced (16.0f, 0.0f), juce::Justification::centredRight);
}

void SurgicalRestoreAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (24, 20);
    area.removeFromTop (50); // Header

    // Bottom action bar
    auto bottomBar = area.removeFromBottom (36);
    deltaListenButton.setBounds (bottomBar.removeFromLeft (140));
    bottomBar.removeFromLeft (16);
    bypassButton.setBounds (bottomBar.removeFromLeft (90));

    area.removeFromBottom (20);

    // 4 Knobs layout across center
    int numKnobs = 4;
    int knobWidth = area.getWidth() / numKnobs;

    auto r1 = area.removeFromLeft (knobWidth);
    clickSensitivityLabel.setBounds (r1.removeFromTop (20));
    clickSensitivitySlider.setBounds (r1.reduced (10));

    auto r2 = area.removeFromLeft (knobWidth);
    crackleAmountLabel.setBounds (r2.removeFromTop (20));
    crackleAmountSlider.setBounds (r2.reduced (10));

    auto r3 = area.removeFromLeft (knobWidth);
    hissReductionLabel.setBounds (r3.removeFromTop (20));
    hissReductionSlider.setBounds (r3.reduced (10));

    auto r4 = area;
    harmonicShieldLabel.setBounds (r4.removeFromTop (20));
    harmonicShieldSlider.setBounds (r4.reduced (10));
}
