#include "PluginEditor.h"

AetherBeamAudioProcessorEditor::AetherBeamAudioProcessorEditor(AetherBeamAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1000, 750);

    // 1. Add 3D Wireframe Visualizer
    addAndMakeVisible(visualizer);

    // While user is dragging: update audio processor atomics only (zero DAW host notification spam)
    visualizer.onNodesMoved = [this](float sXN, float sYN, float lXN, float lYN)
    {
        processorRef.setCustomCoordinates(sXN, sYN, lXN, lYN);
    };

    // When mouse is released: notify DAW host (Ableton Live) once of new position parameters
    visualizer.onDragEnded = [this](float sXN, float sYN, float lXN, float lYN)
    {
        positionSelector.setSelectedItemIndex(4, juce::dontSendNotification);

        if (auto* param = processorRef.getAPVTS().getParameter("position"))
            param->setValueNotifyingHost(1.0f);

        if (auto* param = processorRef.getAPVTS().getParameter("sourceX"))
            param->setValueNotifyingHost(sXN);
        if (auto* param = processorRef.getAPVTS().getParameter("sourceY"))
            param->setValueNotifyingHost(sYN);
        if (auto* param = processorRef.getAPVTS().getParameter("listenerX"))
            param->setValueNotifyingHost(lXN);
        if (auto* param = processorRef.getAPVTS().getParameter("listenerY"))
            param->setValueNotifyingHost(lYN);
    };

    // 2. Space ComboBox
    const auto& spaces = AetherAcoustics::AcousticDatabase::getSpaces();
    juce::StringArray spaceNames;
    for (const auto& s : spaces)
        spaceNames.add(s.title);

    spaceSelector.addItemList(spaceNames, 1);
    addAndMakeVisible(spaceSelector);
    spaceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.getAPVTS(), "space", spaceSelector);

    // 3. Position ComboBox
    addAndMakeVisible(positionSelector);
    positionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.getAPVTS(), "position", positionSelector);

    spaceSelector.onChange = [this]()
    {
        int sIdx = spaceSelector.getSelectedItemIndex();
        if (sIdx >= 0)
        {
            updatePositionDropdown(sIdx);
            int pIdx = positionSelector.getSelectedItemIndex();
            if (pIdx < 0) pIdx = 0;
            processorRef.switchSpaceAndPosition(sIdx, pIdx);
            syncVisualizer();
            lastSpaceIndex = sIdx;
            lastPosIndex = pIdx;
        }
    };

    positionSelector.onChange = [this]()
    {
        int sIdx = spaceSelector.getSelectedItemIndex();
        int pIdx = positionSelector.getSelectedItemIndex();
        if (sIdx >= 0 && pIdx >= 0)
        {
            processorRef.switchSpaceAndPosition(sIdx, pIdx);
            syncVisualizer();
            lastPosIndex = pIdx;
        }
    };

    // Initial sync
    int initSpace = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("space")->load());
    int initPos = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("position")->load());
    updatePositionDropdown(initSpace);
    processorRef.switchSpaceAndPosition(initSpace, initPos);
    syncVisualizer();
    lastSpaceIndex = initSpace;
    lastPosIndex = initPos;

    // 4. Knobs setup
    auto setupRotary = [this](juce::Slider& slider, juce::Label& label, const juce::String& text,
                             std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attach,
                             const juce::String& paramId)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(11.5f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
        addAndMakeVisible(label);

        attach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processorRef.getAPVTS(), paramId, slider);
    };

    // Air Dynamics
    setupRotary(betaSlider, betaLabel, "Air Beta", betaAttachment, "airNonlinearity");
    setupRotary(splSlider, splLabel, "Drive SPL", splAttachment, "driveSPL");

    // Material Damping & Occupancy EQ
    setupRotary(decaySlider, decayLabel, "RT60 Scale", decayAttachment, "decayScale");
    setupRotary(occupancySlider, occupancyLabel, "Occupancy", occupancyAttachment, "occupancy");
    setupRotary(dampFreqSlider, dampFreqLabel, "HF Damping", dampFreqAttachment, "dampFreq");
    setupRotary(hfMultSlider, hfMultLabel, "HF Mult", hfMultAttachment, "hfDecayMult");
    setupRotary(bassMultSlider, bassMultLabel, "Bass Mult", bassMultAttachment, "bassDecayMult");

    // Spatial Mic & Output
    micPatternSelector.addItem("Binaural HRTF", 1);
    micPatternSelector.addItem("ORTF Cardioid", 2);
    micPatternSelector.addItem("Blumlein Fig-8", 3);
    micPatternSelector.addItem("Omni Pair", 4);
    addAndMakeVisible(micPatternSelector);

    micPatternLabel.setText("Mic Pattern", juce::dontSendNotification);
    micPatternLabel.setJustificationType(juce::Justification::centred);
    micPatternLabel.setFont(juce::Font(11.0f, juce::Font::bold));
    micPatternLabel.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
    addAndMakeVisible(micPatternLabel);

    micPatternAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.getAPVTS(), "micPattern", micPatternSelector);

    setupRotary(stereoWidthSlider, stereoWidthLabel, "Stereo Width", stereoWidthAttachment, "stereoWidth");
    setupRotary(mixSlider, mixLabel, "Dry / Wet", mixAttachment, "mix");

    // Poll at 25Hz to handle DAW host automation smoothly
    startTimerHz(25);
}

AetherBeamAudioProcessorEditor::~AetherBeamAudioProcessorEditor()
{
    stopTimer();
}

void AetherBeamAudioProcessorEditor::syncVisualizer()
{
    const auto& spaces = AetherAcoustics::AcousticDatabase::getSpaces();
    int sIdx = processorRef.getCurrentSpaceIndex();
    if (sIdx >= 0 && sIdx < static_cast<int>(spaces.size()))
    {
        visualizer.setSpaceData(&spaces[static_cast<size_t>(sIdx)],
                                processorRef.getCurrentSourcePos(),
                                processorRef.getCurrentListenerPos(),
                                processorRef.getCurrentRays());
        visualizer.setAcousticTelemetry(processorRef.getCurrentMicPattern(),
                                        processorRef.getCurrentStereoWidth(),
                                        processorRef.getCurrentOccupancy());
    }
}

void AetherBeamAudioProcessorEditor::updatePositionDropdown(int spaceIdx)
{
    const auto& spaces = AetherAcoustics::AcousticDatabase::getSpaces();
    if (spaceIdx < 0 || spaceIdx >= static_cast<int>(spaces.size())) return;

    juce::StringArray posNames;
    for (const auto& pos : spaces[static_cast<size_t>(spaceIdx)].positions)
        posNames.add(pos.name);
    posNames.add("Custom Interactive Position");

    int currentPosIdx = positionSelector.getSelectedItemIndex();
    positionSelector.clear(juce::dontSendNotification);
    positionSelector.addItemList(posNames, 1);
    
    if (currentPosIdx >= 0 && currentPosIdx < posNames.size())
        positionSelector.setSelectedItemIndex(currentPosIdx, juce::dontSendNotification);
    else
        positionSelector.setSelectedItemIndex(0, juce::dontSendNotification);
}

void AetherBeamAudioProcessorEditor::timerCallback()
{
    int currentSpace = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("space")->load());
    int currentPos = static_cast<int>(processorRef.getAPVTS().getRawParameterValue("position")->load());

    if (currentSpace != lastSpaceIndex)
    {
        updatePositionDropdown(currentSpace);
        syncVisualizer();
        lastSpaceIndex = currentSpace;
        lastPosIndex = currentPos;
    }
    else if (currentPos != lastPosIndex)
    {
        syncVisualizer();
        lastPosIndex = currentPos;
    }
    else
    {
        visualizer.setAcousticTelemetry(processorRef.getCurrentMicPattern(),
                                        processorRef.getCurrentStereoWidth(),
                                        processorRef.getCurrentOccupancy());
    }
}

void AetherBeamAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff07090e));

    // Top title header
    g.setColour(juce::Colour(0xff0f172a));
    g.fillRect(0, 0, getWidth(), 64);

    g.setColour(juce::Colour(0xfff59e0b));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("AETHERBEAM REVERB", 24, 12, 280, 24, juce::Justification::left);

    g.setColour(juce::Colour(0xff38bdf8));
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.drawText("by Kijjaz", 220, 15, 100, 20, juce::Justification::left);

    g.setColour(juce::Colour(0xff94a3b8));
    g.setFont(juce::Font(11.0f, juce::Font::plain));
    g.drawText("Real-Time 3D Beam-Traced Acoustic Waveguide | 3-Band Material Damping EQ", 24, 35, 600, 18, juce::Justification::left);

    g.setColour(juce::Colour(0x22ffffff));
    g.drawHorizontalLine(64, 0.0f, static_cast<float>(getWidth()));

    // Bottom control deck
    juce::Rectangle<int> controlCardBounds(16, 516, getWidth() - 32, 218);
    g.setColour(juce::Colour(0xff0b1120));
    g.fillRoundedRectangle(controlCardBounds.toFloat(), 8.0f);
    g.setColour(juce::Colour(0x3338bdf8));
    g.drawRoundedRectangle(controlCardBounds.toFloat(), 8.0f, 1.0f);

    // Section Headers in Control Card
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    
    // Group 1: Air Dynamics
    g.setColour(juce::Colour(0xfff59e0b));
    g.drawText("AIR DYNAMICS", 28, 524, 180, 14, juce::Justification::left);

    // Group 2: Material & Occupancy EQ
    g.setColour(juce::Colour(0xff38bdf8));
    g.drawText("MATERIAL ABSORPTION & OCCUPANCY EQ", 218, 524, 460, 14, juce::Justification::left);

    // Group 3: Spatial Mic & Output
    g.setColour(juce::Colour(0xff10b981));
    g.drawText("SPATIAL MIC & OUTPUT", 698, 524, 230, 14, juce::Justification::left);

    // Subtle group dividing vertical lines
    g.setColour(juce::Colour(0x2238bdf8));
    g.drawVerticalLine(208, 522.0f, 726.0f);
    g.drawVerticalLine(688, 522.0f, 726.0f);
}

void AetherBeamAudioProcessorEditor::resized()
{
    spaceSelector.setBounds(16, 74, 450, 32);
    positionSelector.setBounds(480, 74, getWidth() - 496, 32);

    visualizer.setBounds(16, 114, getWidth() - 32, 390);

    int knobY = 548;
    int knobW = 86;
    int knobH = 95;

    // Group 1: Air Dynamics (2 knobs)
    betaSlider.setBounds(24, knobY, knobW, knobH);
    betaLabel.setBounds(16, knobY + knobH, knobW + 16, 18);

    splSlider.setBounds(116, knobY, knobW, knobH);
    splLabel.setBounds(108, knobY + knobH, knobW + 16, 18);

    // Group 2: Material Damping & Occupancy EQ (5 knobs)
    decaySlider.setBounds(216, knobY, knobW, knobH);
    decayLabel.setBounds(208, knobY + knobH, knobW + 16, 18);

    occupancySlider.setBounds(308, knobY, knobW, knobH);
    occupancyLabel.setBounds(300, knobY + knobH, knobW + 16, 18);

    dampFreqSlider.setBounds(400, knobY, knobW, knobH);
    dampFreqLabel.setBounds(392, knobY + knobH, knobW + 16, 18);

    hfMultSlider.setBounds(492, knobY, knobW, knobH);
    hfMultLabel.setBounds(484, knobY + knobH, knobW + 16, 18);

    bassMultSlider.setBounds(584, knobY, knobW, knobH);
    bassMultLabel.setBounds(576, knobY + knobH, knobW + 16, 18);

    // Group 3: Spatial Mic & Master Output
    micPatternSelector.setBounds(692, knobY + 18, 116, 28);
    micPatternLabel.setBounds(692, knobY + 48, 116, 18);

    stereoWidthSlider.setBounds(814, knobY, knobW, knobH);
    stereoWidthLabel.setBounds(806, knobY + knobH, knobW + 16, 18);

    mixSlider.setBounds(894, knobY, knobW, knobH);
    mixLabel.setBounds(886, knobY + knobH, knobW + 16, 18);
}
