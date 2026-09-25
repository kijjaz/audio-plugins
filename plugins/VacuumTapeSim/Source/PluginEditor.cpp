#include "PluginProcessor.h"
#include "PluginEditor.h"

VacuumTapeSimAudioProcessorEditor::VacuumTapeSimAudioProcessorEditor (VacuumTapeSimAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel (&customLookAndFeel);

    // Setup function for sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& paramId, std::unique_ptr<SliderAttachment>& attachment)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        slider.setColour(juce::Slider::textBoxTextColourId, vts::CarbonGoldLookAndFeel::textOffWhite);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible (slider);

        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        label.setColour(juce::Label::textColourId, vts::CarbonGoldLookAndFeel::textOffWhite);
        addAndMakeVisible (label);

        attachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, paramId, slider);
    };

    setupSlider (driveSlider, driveLabel, "DRIVE", "drive", driveAttachment);
    setupSlider (sagSlider, sagLabel, "SAG", "sag", sagAttachment);
    setupSlider (ipsSlider, ipsLabel, "IPS", "ips", ipsAttachment);
    setupSlider (wowSlider, wowLabel, "WOW/FLUTTER", "wow_flutter", wowAttachment);
    setupSlider (biasSlider, biasLabel, "BIAS", "bias", biasAttachment);
    setupSlider (asymSlider, asymLabel, "ASYMMETRY", "asymmetry", asymAttachment);

    // Setup Combo Box
    eqBox.addItemList ({"NAB", "CCIR"}, 1);
    addAndMakeVisible (eqBox);
    eqLabel.setText ("EQ CURVE", juce::dontSendNotification);
    eqLabel.setJustificationType (juce::Justification::centred);
    eqLabel.setColour(juce::Label::textColourId, vts::CarbonGoldLookAndFeel::textOffWhite);
    addAndMakeVisible (eqLabel);
    
    eqAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "eq_mode", eqBox);

    setSize (800, 500);
    startTimerHz(30); // 30 FPS animation
}

VacuumTapeSimAudioProcessorEditor::~VacuumTapeSimAudioProcessorEditor()
{
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

void VacuumTapeSimAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background
    g.fillAll (vts::CarbonGoldLookAndFeel::carbonMatte);
    
    // Header
    g.setColour(vts::CarbonGoldLookAndFeel::carbonDark);
    g.fillRect(0, 0, getWidth(), 60);

    g.setColour (vts::CarbonGoldLookAndFeel::goldAccent);
    g.setFont (juce::Font (24.0f, juce::Font::bold));
    g.drawText ("VACUUM TAPE SIM", 0, 0, getWidth(), 60, juce::Justification::centred, true);
    
    // Subtle separator line
    g.setColour(vts::CarbonGoldLookAndFeel::goldAccent.withAlpha(0.2f));
    g.drawLine(0, 60, getWidth(), 60, 2.0f);
    
    // Draw Tape Reels
    juce::Rectangle<float> leftReel (50, 100, 150, 150);
    juce::Rectangle<float> rightReel (getWidth() - 200, 100, 150, 150);
    vts::CarbonGoldLookAndFeel::drawTapeReel(g, leftReel, tapeRotation);
    vts::CarbonGoldLookAndFeel::drawTapeReel(g, rightReel, tapeRotation);
    
    // Draw Vacuum Tube (Center Top)
    juce::Rectangle<float> tubeBounds (getWidth() / 2 - 40, 70, 80, 140);
    vts::CarbonGoldLookAndFeel::drawVacuumTube(g, tubeBounds, currentGlow);
}

void VacuumTapeSimAudioProcessorEditor::timerCallback()
{
    // Update rotation based on IPS parameter
    float ips = audioProcessor.apvts.getRawParameterValue("ips")->load();
    tapeRotation += (ips * 0.005f); 
    if (tapeRotation >= juce::MathConstants<float>::twoPi)
        tapeRotation -= juce::MathConstants<float>::twoPi;
        
    // Update glow based on left channel v_state (simplified proxy for sag)
    float v_state = audioProcessor.tapeDSP[0].v_state; 
    // Combine drive and sag for the visual glow!
    float drive = audioProcessor.apvts.getRawParameterValue("drive")->load();
    currentGlow = (drive * 0.1f) + (1.0f - v_state); // v_state drops when sagging
    if (currentGlow > 1.0f) currentGlow = 1.0f;
    if (currentGlow < 0.0f) currentGlow = 0.0f;

    repaint();
}

void VacuumTapeSimAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(60); // Header area
    
    // Create a 2x4 grid layout below the visual assets
    int knobSize = 90;
    int marginX = 20;
    int marginY = 30;
    
    int startX = (getWidth() - (4 * knobSize + 3 * marginX)) / 2;
    int startY = 250; // Below the tube and reels
    
    auto positionComponent = [&](juce::Component& comp, juce::Label& label, int col, int row)
    {
        int x = startX + col * (knobSize + marginX);
        int y = startY + row * (knobSize + marginY + 20); // +20 for label space
        
        comp.setBounds(x, y + 20, knobSize, knobSize);
        label.setBounds(x, y, knobSize, 20);
    };

    // Row 1 (Input/Tape core)
    positionComponent(driveSlider, driveLabel, 0, 0);
    positionComponent(sagSlider, sagLabel, 1, 0);
    positionComponent(ipsSlider, ipsLabel, 2, 0);
    positionComponent(wowSlider, wowLabel, 3, 0);

    // Row 2 (Advanced parameters)
    positionComponent(biasSlider, biasLabel, 0, 1);
    positionComponent(asymSlider, asymLabel, 1, 1);
    
    // ComboBox requires different layout
    int x = startX + 2 * (knobSize + marginX);
    int y = startY + 1 * (knobSize + marginY + 20);
    eqBox.setBounds(x, y + 50, knobSize, 30);
    eqLabel.setBounds(x, y + 30, knobSize, 20);
}
