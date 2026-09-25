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
    setupSlider (wowSlider, wowLabel, "WOW", "wow", wowAttachment);
    setupSlider (flutterSlider, flutterLabel, "FLUTTER", "flutter", flutterAttachment);
    setupSlider (biasSlider, biasLabel, "BIAS", "bias", biasAttachment);
    setupSlider (asymSlider, asymLabel, "ASYMMETRY", "asymmetry", asymAttachment);
    setupSlider (hissSlider, hissLabel, "TAPE HISS", "hiss", hissAttachment);
    setupSlider (mixSlider, mixLabel, "MIX %", "mix", mixAttachment);
    setupSlider (outputSlider, outputLabel, "OUTPUT", "output", outputAttachment);

    // Setup Auto Gain Button
    autoGainButton.setButtonText ("AUTO GAIN");
    autoGainButton.setColour (juce::ToggleButton::textColourId, vts::CarbonGoldLookAndFeel::goldAccent);
    autoGainButton.setColour (juce::ToggleButton::tickColourId, vts::CarbonGoldLookAndFeel::goldAccent);
    addAndMakeVisible (autoGainButton);
    autoGainAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts, "auto_gain", autoGainButton);

    // Setup Combo Box
    eqBox.addItemList ({"NAB", "CCIR"}, 1);
    addAndMakeVisible (eqBox);
    eqLabel.setText ("EQ CURVE", juce::dontSendNotification);
    eqLabel.setJustificationType (juce::Justification::centred);
    eqLabel.setColour(juce::Label::textColourId, vts::CarbonGoldLookAndFeel::textOffWhite);
    addAndMakeVisible (eqLabel);
    
    eqAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "eq_mode", eqBox);
    
    // Setup Preset Selector
    const auto& presets = audioProcessor.getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        presetBox.addItem (presets[i].name, static_cast<int>(i + 1));
    }
    presetBox.setSelectedId (audioProcessor.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox.onChange = [this]()
    {
        int selected = presetBox.getSelectedId() - 1;
        if (selected >= 0)
        {
            audioProcessor.setCurrentProgram(selected);
        }
    };
    addAndMakeVisible (presetBox);

    presetLabel.setText ("PRESET", juce::dontSendNotification);
    presetLabel.setJustificationType (juce::Justification::centredRight);
    presetLabel.setColour (juce::Label::textColourId, vts::CarbonGoldLookAndFeel::goldAccent);
    presetLabel.setFont (juce::Font(13.0f, juce::Font::bold));
    addAndMakeVisible (presetLabel);

    analysisPanel = std::make_unique<vts::AnalysisPanelComponent>(&audioProcessor.tapeDSP[0]);
    addAndMakeVisible(analysisPanel.get());

    setSize (900, 800);
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
    g.setFont (juce::Font (22.0f, juce::Font::bold));
    g.drawText ("VACUUM TAPE SIM", 50, 0, 300, 60, juce::Justification::centredLeft, true);
    
    // Subtle separator line
    g.setColour(vts::CarbonGoldLookAndFeel::goldAccent.withAlpha(0.2f));
    g.drawLine(0, 60, getWidth(), 60, 2.0f);
    
    // Draw Tape Reels
    juce::Rectangle<float> leftReel (50, 320, 140, 140);
    juce::Rectangle<float> rightReel (getWidth() - 190, 320, 140, 140);
    vts::CarbonGoldLookAndFeel::drawTapeReel(g, leftReel, tapeRotation);
    vts::CarbonGoldLookAndFeel::drawTapeReel(g, rightReel, tapeRotation);
    
    // Draw Vacuum Tube (Center Top)
    juce::Rectangle<float> tubeBounds (getWidth() / 2 - 40, 310, 80, 130);
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
    
    if (analysisPanel) analysisPanel->repaint();

    // Keep preset dropdown in sync with host program changes
    int curProg = audioProcessor.getCurrentProgram();
    if (presetBox.getSelectedId() != curProg + 1)
    {
        presetBox.setSelectedId(curProg + 1, juce::dontSendNotification);
    }

    repaint();
}

void VacuumTapeSimAudioProcessorEditor::resized()
{
    // Preset dropdown in header (top right)
    presetLabel.setBounds(getWidth() - 370, 16, 70, 28);
    presetBox.setBounds(getWidth() - 290, 16, 240, 28);

    if (analysisPanel)
        analysisPanel->setBounds(50, 75, getWidth() - 100, 220);
    
    // Layout 10 knobs in 2 rows of 5:
    // Row 1: DRIVE, SAG, IPS, WOW, FLUTTER
    // Row 2: BIAS, ASYMMETRY, TAPE HISS, MIX %, OUTPUT
    int knobSize = 78;
    int marginX = 28;
    int marginY = 18;
    
    int totalRowWidth = 5 * knobSize + 4 * marginX; // 5 * 78 + 4 * 28 = 390 + 112 = 502px
    int startX = (getWidth() - totalRowWidth) / 2;
    int startY = 490;
    
    auto positionComponent = [&](juce::Component& comp, juce::Label& label, int col, int row)
    {
        int x = startX + col * (knobSize + marginX);
        int y = startY + row * (knobSize + marginY + 22);
        
        comp.setBounds(x, y + 20, knobSize, knobSize);
        label.setBounds(x - 10, y, knobSize + 20, 18);
    };

    // Row 1: Dynamics, Speed & Modulations
    positionComponent(driveSlider, driveLabel, 0, 0);
    positionComponent(sagSlider, sagLabel, 1, 0);
    positionComponent(ipsSlider, ipsLabel, 2, 0);
    positionComponent(wowSlider, wowLabel, 3, 0);
    positionComponent(flutterSlider, flutterLabel, 4, 0);

    // Row 2: Magnetics, Color & Output
    positionComponent(biasSlider, biasLabel, 0, 1);
    positionComponent(asymSlider, asymLabel, 1, 1);
    positionComponent(hissSlider, hissLabel, 2, 1);
    positionComponent(mixSlider, mixLabel, 3, 1);
    positionComponent(outputSlider, outputLabel, 4, 1);
    
    // Controls strip between tubes/reels and knobs
    int sideControlsY = 445;
    autoGainButton.setBounds(startX, sideControlsY, 130, 28);
    
    int eqRightX = startX + totalRowWidth - 110;
    eqLabel.setBounds(eqRightX, sideControlsY - 18, 110, 18);
    eqBox.setBounds(eqRightX, sideControlsY, 110, 28);
}
