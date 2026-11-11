#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_formats/juce_audio_formats.h>

KindnesAudioProcessorEditor::KindnesAudioProcessorEditor(KindnesAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&lookAndFeel_);

    // 1. Oscilloscope & 1-Bit DPCM Staircase Visualizer
    addAndMakeVisible(visualizer_);

    // 2. Preset Bar
    addAndMakeVisible(presetBox_);
    const auto& presets = KindnesAudio::getFactoryPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox_.addItem(presets[i].name, static_cast<int>(i + 1));
    presetBox_.setSelectedId(audioProcessor.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.onChange = [this]() {
        int idx = presetBox_.getSelectedId() - 1;
        if (idx >= 0)
        {
            audioProcessor.setCurrentProgram(idx);
            updateDpcmPreviews();
            arpPatternEditor_.setText(audioProcessor.getArpPatternStringForCurrentSetting(), false);
        }
    };

    addAndMakeVisible(prevPresetBtn_);
    prevPresetBtn_.onClick = [this]() {
        int idx = audioProcessor.getCurrentProgram() - 1;
        if (idx >= 0)
            presetBox_.setSelectedId(idx + 1, juce::sendNotificationSync);
    };

    addAndMakeVisible(nextPresetBtn_);
    nextPresetBtn_.onClick = [this]() {
        int maxIdx = static_cast<int>(KindnesAudio::getFactoryPresets().size()) - 1;
        int idx = audioProcessor.getCurrentProgram() + 1;
        if (idx <= maxIdx)
            presetBox_.setSelectedId(idx + 1, juce::sendNotificationSync);
    };

    // 3. Tabbed Deck Navigation
    auto setupTabButton = [this](juce::TextButton& btn, DeckTab tab) {
        btn.setClickingTogglesState(false);
        btn.onClick = [this, tab]() { selectDeckTab(tab); };
        addAndMakeVisible(btn);
    };
    setupTabButton(tabSynthBtn_, DeckTab::ApuSynth);
    setupTabButton(tabDrumsBtn_, DeckTab::DrumMachine);
    setupTabButton(tabDesignerBtn_, DeckTab::DrumDesigner);
    setupTabButton(tabTrackerBtn_, DeckTab::TrackerFx);
    setupTabButton(tabMasterBtn_, DeckTab::MasterDeck);

    // --- Bay 1: Core APU & Play Mode ---
    setupComboBox(playModeBox_, playModeLabel_, "PLAY MODE", { "Classic APU", "Polyphonic", "Unison Stereo" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "playMode", playModeBox_));

    addAndMakeVisible(authenticDacToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "authenticDac", authenticDacToggle_));

    setupComboBox(pulse1DutyBox_, pulse1DutyLabel_, "P1 DUTY", { "12.5%", "25.0%", "50.0%", "75.0%", "PWM" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pulse1Duty", pulse1DutyBox_));

    setupSlider(pulse1VolSlider_, pulse1VolLabel_, "P1 VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pulse1Vol", pulse1VolSlider_));

    setupComboBox(pulse2DutyBox_, pulse2DutyLabel_, "P2 DUTY", { "12.5%", "25.0%", "50.0%", "75.0%", "PWM" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "pulse2Duty", pulse2DutyBox_));

    setupSlider(pulse2VolSlider_, pulse2VolLabel_, "P2 VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pulse2Vol", pulse2VolSlider_));

    setupSlider(triVolSlider_, triVolLabel_, "TRI VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "triVol", triVolSlider_));

    setupSlider(noiseVolSlider_, noiseVolLabel_, "NOISE VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "noiseVol", noiseVolSlider_));

    setupComboBox(noiseModeBox_, noiseModeLabel_, "NOISE LFSR", { "White (32k)", "Metallic (93)" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "noiseMode", noiseModeBox_));

    addAndMakeVisible(vrc6Toggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "vrc6Enabled", vrc6Toggle_));

    setupSlider(vrc6SawVolSlider_, vrc6SawVolLabel_, "VRC6 SAW");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "vrc6SawVol", vrc6SawVolSlider_));

    // --- Bay 2: FDS 2C33 Wavetable & FM Unit ---
    addAndMakeVisible(fdsToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "fdsEnabled", fdsToggle_));

    setupComboBox(fdsWaveBox_, fdsWaveLabel_, "FDS WAVE", {
        "Sine", "Triangle", "Sawtooth", "25% Pulse",
        "Organ", "Bell", "Formant Aah", "Heroic Fantasy"
    });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "fdsWave", fdsWaveBox_));

    setupSlider(fdsVolSlider_, fdsVolLabel_, "FDS VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fdsVol", fdsVolSlider_));

    setupSlider(fdsModSpeedSlider_, fdsModSpeedLabel_, "FM SPEED");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fdsModSpeed", fdsModSpeedSlider_));

    setupSlider(fdsModDepthSlider_, fdsModDepthLabel_, "FM DEPTH");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "fdsModDepth", fdsModDepthSlider_));

    setupComboBox(fdsModWaveBox_, fdsModWaveLabel_, "FM TABLE", { "Vibrato", "Metallic FM", "Laser Sweep" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "fdsModWave", fdsModWaveBox_));

    // --- Bay 2b: Chiptune Embellishments ---
    setupSlider(scoopSemiSlider_, scoopSemiLabel_, "PITCH SCOOP");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "scoopSemi", scoopSemiSlider_));

    setupSlider(scoopTimeSlider_, scoopTimeLabel_, "SCOOP TIME");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "scoopTime", scoopTimeSlider_));

    setupSlider(delayedVibDelaySlider_, delayedVibDelayLabel_, "VIB DELAY");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "delayedVibDelay", delayedVibDelaySlider_));

    setupSlider(delayedVibDepthSlider_, delayedVibDepthLabel_, "VIB DEPTH");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "delayedVibDepth", delayedVibDepthSlider_));

    addAndMakeVisible(dutyMorphToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "dutyMorph", dutyMorphToggle_));

    addAndMakeVisible(pseudoEchoToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "pseudoEcho", pseudoEchoToggle_));

    setupSlider(pseudoEchoDelaySlider_, pseudoEchoDelayLabel_, "ECHO TIME");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pseudoEchoDelay", pseudoEchoDelaySlider_));

    setupSlider(pseudoEchoVolSlider_, pseudoEchoVolLabel_, "ECHO VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "pseudoEchoVol", pseudoEchoVolSlider_));

    // --- Bay 3: Supercharged 1-Bit DPCM Sampler & 32-Sound Drum Machine (4 Banks x 8 Pads) ---
    setupComboBox(drumBankBox_, drumBankLabel_, "ACTIVE DRUM BANK", {
        "Bank 1: Classic 8-Bit Master Kit (01-08)",
        "Bank 2: VRC6 & FDS Synth Drums (09-16)",
        "Bank 3: Vintage 1-Bit Funk & Stabs (17-24)",
        "Bank 4: Retro Arcade Vocal & SFX (25-32)"
    });
    drumBankBox_.setSelectedId(1, juce::dontSendNotification);
    drumBankBox_.onChange = [this]() {
        int bankIdx = drumBankBox_.getSelectedId() - 1;
        if (bankIdx >= 0 && bankIdx < 4)
        {
            int firstSampleInBank = bankIdx * 8;
            dpcmSampleBox_.setSelectedId(firstSampleInBank + 1, juce::sendNotificationSync);
        }
    };

    setupComboBox(dpcmSampleBox_, dpcmSampleLabel_, "SAMPLE ROM VOICE (32)", {
        "01. Punch Bass Drum [Bank 1]", "02. Mega Crisp Snare [Bank 1]", "03. Closed Hi-Hat [Bank 1]", "04. Open Hi-Hat [Bank 1]",
        "05. 8-Bit Hand Clap [Bank 1]", "06. High Rack Tom [Bank 1]", "07. Low Floor Tom [Bank 1]", "08. Arcade Crash Cymbal [Bank 1]",
        "09. VRC6 Overdriven Kick [Bank 2]", "10. 93-Step Metallic Snare [Bank 2]", "11. Chiptune Rimshot [Bank 2]", "12. 8-Bit Arcade Cowbell [Bank 2]",
        "13. FDS 2C33 FM Simmons Tom [Bank 2]", "14. Gated Boss Battle Snare [Bank 2]", "15. Chiptune Shaker [Bank 2]", "16. FDS Metallic Ride Bell [Bank 2]",
        "17. 1-Bit Slap Bass Pop [Bank 3]", "18. 1-Bit Slap Bass Thumb [Bank 3]", "19. Orchestra Hit Major [Bank 3]", "20. Orchestra Hit Minor [Bank 3]",
        "21. Dramatic 1-Bit Timpani [Bank 3]", "22. 2A03 Deep Sub-Boom Kick [Bank 3]", "23. Slope-Overload Fuzz Snare [Bank 3]", "24. 1-Bit Vinyl Scratch [Bank 3]",
        "25. Speech: KINDNES [Bank 4]", "26. Speech: POWER UP [Bank 4]", "27. Retro Laser Zapper [Bank 4]", "28. Coin Chime B5-E6 [Bank 4]",
        "29. Jump Spring Boing [Bank 4]", "30. Boss Explosion Rumble [Bank 4]", "31. UFO Warp Siren [Bank 4]", "32. Power-Down Glitch [Bank 4]",
        "33. User Sample (Drag & Drop)"
    });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "dpcmSample", dpcmSampleBox_));
    dpcmSampleBox_.onChange = [this]() {
        updateDpcmPreviews();
    };

    addAndMakeVisible(dpcmChromaticToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "dpcmChromatic", dpcmChromaticToggle_));

    addAndMakeVisible(dpcmDrumPadsToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "dpcmDrumPads", dpcmDrumPadsToggle_));

    addAndMakeVisible(throughDpcmToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "throughDpcm", throughDpcmToggle_));

    demoBeatBtn_.setClickingTogglesState(true);
    demoBeatBtn_.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff28151A));
    demoBeatBtn_.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffB82830));
    demoBeatBtn_.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffFFD700));
    demoBeatBtn_.setColour(juce::TextButton::textColourOnId, juce::Colour(0xffFFFFFF));
    demoBeatBtn_.onClick = [this]() {
        bool on = demoBeatBtn_.getToggleState();
        if (on)
        {
            if (auto* p = audioProcessor.getAPVTS().getParameter("dpcmVol"))
            {
                if (p->getValue() < 0.2f)
                    p->setValueNotifyingHost(p->convertTo0to1(14.0f));
            }
        }
        audioProcessor.getEngine().getDPCM().setDemoBeatEnabled(on);
        demoBeatBtn_.setButtonText(on ? "STOP BEAT" : "AUTO BEAT");
    };
    addAndMakeVisible(demoBeatBtn_);

    setupSlider(dpcmDeltaStepSlider_, dpcmDeltaStepLabel_, "SLEW (GRIT)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "dpcmDeltaStep", dpcmDeltaStepSlider_));

    setupSlider(dpcmVelToSlewSlider_, dpcmVelToSlewLabel_, "VEL->SLEW");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "dpcmVelToSlew", dpcmVelToSlewSlider_));

    setupSlider(dpcmRateSlider_, dpcmRateLabel_, "RATE INDEX");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "dpcmRate", dpcmRateSlider_));

    setupSlider(dpcmVolSlider_, dpcmVolLabel_, "DPCM VOL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "dpcmVol", dpcmVolSlider_));

    static const char* kPadNames[8] = {
        "01. KICK [C]", "02. SNARE [C#]", "03. CL-HAT [D]", "04. OP-HAT [D#]",
        "05. CLAP [E]", "06. HI-TOM [F]", "07. LO-TOM [F#]", "08. CRASH [G]"
    };
    for (int i = 0; i < 8; ++i)
    {
        drumPads_[i].setButtonText(kPadNames[i]);
        drumPads_[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff221418));
        drumPads_[i].setColour(juce::TextButton::textColourOffId, juce::Colour(0xffE5B869));
        addAndMakeVisible(drumPads_[i]);
        drumPads_[i].onClick = [this, i]() {
            if (auto* p = audioProcessor.getAPVTS().getParameter("dpcmVol"))
            {
                if (p->getValue() < 0.2f)
                    p->setValueNotifyingHost(p->convertTo0to1(14.0f));
            }
            int bankBase = audioProcessor.getEngine().getActiveDrumBank() * 8;
            int sampleIdx = bankBase + i;
            audioProcessor.getEngine().triggerHybridDrumPad(sampleIdx, 1.0f);
            audioProcessor.getEngine().getDPCM().selectBuiltinSample(sampleIdx);
            dpcmSampleBox_.setSelectedId(sampleIdx + 1, juce::dontSendNotification);
            updateDpcmPreviews();
        };
    }

    // --- Bay 4: Filter, Envelope, Arp & Master ---
    setupComboBox(filterModeBox_, filterModeLabel_, "FILTER TYPE", { "LowPass", "HighPass", "BandPass", "Notch", "Bypass" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "filterMode", filterModeBox_));

    setupSlider(cutoffSlider_, cutoffLabel_, "CUTOFF");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "cutoff", cutoffSlider_));

    setupSlider(resonanceSlider_, resonanceLabel_, "RESONANCE");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "resonance", resonanceSlider_));

    addAndMakeVisible(adsrToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "useAdsr", adsrToggle_));

    setupSlider(attackSlider_, attackLabel_, "ATTACK");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "attack", attackSlider_));

    setupSlider(decaySlider_, decayLabel_, "DECAY");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "decay", decaySlider_));

    setupSlider(sustainSlider_, sustainLabel_, "SUSTAIN");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "sustain", sustainSlider_));

    setupSlider(releaseSlider_, releaseLabel_, "RELEASE");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "release", releaseSlider_));

    addAndMakeVisible(analogFilterToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "analogFilter", analogFilterToggle_));

    setupSlider(toneSlider_, toneLabel_, "TONE TILT");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "filterTone", toneSlider_));

    addAndMakeVisible(arpToggle_);
    buttonAttachments_.push_back(std::make_unique<ButtonAttachment>(audioProcessor.getAPVTS(), "arpEnabled", arpToggle_));

    setupComboBox(arpRateBox_, arpRateLabel_, "ARP SPEED", { "60Hz NTSC", "50Hz PAL", "1/16", "1/32", "1/64" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "arpRate", arpRateBox_));

    setupComboBox(arpPatternBox_, arpPatternLabel_, "ARP PATTERN",
                  { "Major", "Minor", "Octaves", "Sus4", "Fifth", "Chug", "Custom" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "arpPattern", arpPatternBox_));

    arpEditorLabel_.setFont(juce::Font(9.0f, juce::Font::bold));
    arpEditorLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff88a0b0));
    arpEditorLabel_.setText("PATTERN (STEPS & RESTS)", juce::dontSendNotification);
    addAndMakeVisible(arpEditorLabel_);

    arpPatternEditor_.setFont(juce::Font("Courier New", 12.0f, juce::Font::bold));
    arpPatternEditor_.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff121820));
    arpPatternEditor_.setColour(juce::TextEditor::textColourId, juce::Colour(0xff50e0ff));
    arpPatternEditor_.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff304050));
    arpPatternEditor_.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(0xff50e0ff));
    arpPatternEditor_.setText(audioProcessor.getArpPatternStringForCurrentSetting(), false);
    arpPatternEditor_.onReturnKey = [this]() {
        audioProcessor.setCustomArpPatternString(arpPatternEditor_.getText());
    };
    arpPatternEditor_.onFocusLost = [this]() {
        audioProcessor.setCustomArpPatternString(arpPatternEditor_.getText());
    };
    addAndMakeVisible(arpPatternEditor_);

    arpPatternBox_.onChange = [this]() {
        int id = arpPatternBox_.getSelectedId(); // 1=Major ... 7=Custom
        if (id >= 1 && id <= 6)
            arpPatternEditor_.setText(audioProcessor.getArpPatternStringForCurrentSetting(), false);
        else if (id == 7)
            arpPatternEditor_.setText(audioProcessor.getCustomArpPatternString(), false);
    };

    setupSlider(vibratoSlider_, vibratoLabel_, "VIBRATO");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "vibrato", vibratoSlider_));

    setupSlider(pwmDepthSlider_, pwmDepthLabel_, "AUTO-PWM");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "lfoPwm", pwmDepthSlider_));

    setupSlider(glideSlider_, glideLabel_, "GLIDE");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "glide", glideSlider_));

    setupSlider(detuneSlider_, detuneLabel_, "DETUNE");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "unisonDetune", detuneSlider_));

    setupSlider(spreadSlider_, spreadLabel_, "SPREAD");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "stereoSpread", spreadSlider_));

    setupSlider(masterTrimSlider_, masterTrimLabel_, "MASTER TRIM");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "masterTrim", masterTrimSlider_));

    // =========================================================================
    // Tab 3: Advanced Drum Sound Designer Setup
    // =========================================================================
    setupComboBox(drumDesignModeBox_, drumDesignModeLabel_, "DESIGN MODE", { "Easy (Macro)", "Flexible (Deep)" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumDesignMode", drumDesignModeBox_));
    drumDesignModeBox_.onChange = [this]() {
        updateDeckVisibility();
    };

    setupComboBox(drumTemplateBox_, drumTemplateLabel_, "DRUM TEMPLATE (17 PRESETS)", {
        "Custom User Drum",
        "01. Punch Kick", "02. Mega Snare", "03. Closed Hi-Hat", "04. Open Hi-Hat",
        "05. 8-Bit Hand Clap", "06. High Rack Tom", "07. Low Floor Tom", "08. Arcade Crash Cymbal",
        "09. VRC6 Overdriven Kick", "10. 93-Step Metallic Snare", "11. Chiptune Rimshot", "12. 8-Bit Arcade Cowbell",
        "13. FDS FM Simmons Tom", "14. Gated Boss Battle Snare", "15. Laser Zapper FX",
        "16. Boss Defeat Cataclysm", "17. Comic Spring Boing"
    });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumTemplate", drumTemplateBox_));
    drumTemplateBox_.onChange = [this]() {
        int idx = drumTemplateBox_.getSelectedId() - 1; // 0=Custom, 1..17=Templates
        if (idx > 0)
        {
            audioProcessor.getEngine().applyCustomDrumTemplate(idx);
            // Sync APVTS to loaded template values so UI sliders update
            const auto& p = audioProcessor.getEngine().getCustomDrumParams();
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumPitchCurve"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.pitchCurve)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumPitchTimeMs"))
                param->setValueNotifyingHost(param->convertTo0to1(p.pitchTimeMs));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumClickType"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.clickType)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumClickLevel"))
                param->setValueNotifyingHost(param->convertTo0to1(p.clickLevel));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumFmRateHz"))
                param->setValueNotifyingHost(param->convertTo0to1(p.fmRateHz));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumFmDepthHz"))
                param->setValueNotifyingHost(param->convertTo0to1(p.fmDepthHz));

            if (auto* param = audioProcessor.getAPVTS().getParameter("drumBodyStartPitch"))
                param->setValueNotifyingHost(param->convertTo0to1(p.bodyStartPitchHz));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumBodyEndPitch"))
                param->setValueNotifyingHost(param->convertTo0to1(p.bodyEndPitchHz));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumBodyDecayMs"))
                param->setValueNotifyingHost(param->convertTo0to1(p.bodyDecayMs));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumBodyWave"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.bodyWaveform)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumBodyLevel"))
                param->setValueNotifyingHost(param->convertTo0to1(p.bodyLevel));

            if (auto* param = audioProcessor.getAPVTS().getParameter("drumNoiseMode"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.noiseMode)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumNoisePeriod"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.noisePeriod)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumNoiseDecayMs"))
                param->setValueNotifyingHost(param->convertTo0to1(p.noiseDecayMs));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumNoisePattern"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.noisePattern)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumNoiseLevel"))
                param->setValueNotifyingHost(param->convertTo0to1(p.noiseLevel));

            if (auto* param = audioProcessor.getAPVTS().getParameter("drumDpcmDelta"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.dpcmDeltaStep)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumDpcmRate"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.dpcmRateIndex)));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumDpcmGrit"))
                param->setValueNotifyingHost(param->convertTo0to1(p.dpcmGrit));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumDpcmBlend"))
                param->setValueNotifyingHost(param->convertTo0to1(p.dpcmBlend));
            if (auto* param = audioProcessor.getAPVTS().getParameter("drumDpcmVoice"))
                param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.dpcmSampleVoice)));
        }
    };

    setupComboBox(drumMidiModeBox_, drumMidiModeLabel_, "MIDI PLAY MODE", {
        "Audition Only", "MIDI Note 36 (C1 Kick / D1 Snare)", "Tuned Chromatic Synth"
    });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumMidiMode", drumMidiModeBox_));

    // Audition Button
    testDrumBtn_.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffC4323E));
    testDrumBtn_.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffFFFFFF));
    testDrumBtn_.onClick = [this]() {
        audioProcessor.triggerCustomDrum(1.0f);
    };
    addAndMakeVisible(testDrumBtn_);

    // Easy Mode Macros
    setupSlider(drumPunchSlider_, drumPunchLabel_, "PUNCH (DROP)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumMacroPunch", drumPunchSlider_));
    setupSlider(drumBodyMacroSlider_, drumBodyMacroLabel_, "BODY LENGTH");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumMacroBody", drumBodyMacroSlider_));
    setupSlider(drumSnappySlider_, drumSnappyLabel_, "SNAPPY (NOISE)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumMacroSnappy", drumSnappySlider_));
    setupSlider(drumGritSlider_, drumGritLabel_, "1-BIT GRIT");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumMacroGrit", drumGritSlider_));

    // Flexible Mode Controls: Tonal Body & Pitch Engine
    setupComboBox(drumPitchCurveBox_, drumPitchCurveLabel_, "PITCH CURVE", { "Exp Drop", "Dual Click Dive", "Laser Sweep", "Pitch Scoop", "FM Chirp" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumPitchCurve", drumPitchCurveBox_));
    setupSlider(drumPitchTimeSlider_, drumPitchTimeLabel_, "SWEEP TIME (ms)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumPitchTimeMs", drumPitchTimeSlider_));

    setupSlider(drumBodyStartPitchSlider_, drumBodyStartPitchLabel_, "START FREQ (Hz)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumBodyStartPitch", drumBodyStartPitchSlider_));
    setupSlider(drumBodyEndPitchSlider_, drumBodyEndPitchLabel_, "END FREQ (Hz)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumBodyEndPitch", drumBodyEndPitchSlider_));
    setupSlider(drumBodyDecaySlider_, drumBodyDecayLabel_, "BODY DECAY (ms)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumBodyDecayMs", drumBodyDecaySlider_));
    setupComboBox(drumBodyWaveBox_, drumBodyWaveLabel_, "BODY WAVE", { "4-Bit Triangle", "Saturated Sine", "VRC6 Saw Overdrive", "25% Pulse" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumBodyWave", drumBodyWaveBox_));
    setupSlider(drumBodyLevelSlider_, drumBodyLevelLabel_, "BODY LEVEL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumBodyLevel", drumBodyLevelSlider_));

    // Flexible Mode Controls: Transient Click Layer
    setupComboBox(drumClickTypeBox_, drumClickTypeLabel_, "TRANSIENT CLICK", { "Off", "Impulse Spike", "Square Pop", "Noise Snap" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumClickType", drumClickTypeBox_));
    setupSlider(drumClickLevelSlider_, drumClickLevelLabel_, "CLICK LEVEL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumClickLevel", drumClickLevelSlider_));

    // Flexible Mode Controls: Noise
    setupComboBox(drumNoiseModeBox_, drumNoiseModeLabel_, "LFSR MODE", { "Mode 0: White (32k)", "Mode 1: Metal (93)" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumNoiseMode", drumNoiseModeBox_));
    setupSlider(drumNoisePeriodSlider_, drumNoisePeriodLabel_, "NOISE RATE (0-15)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumNoisePeriod", drumNoisePeriodSlider_));
    setupSlider(drumNoiseDecaySlider_, drumNoiseDecayLabel_, "NOISE DECAY (ms)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumNoiseDecayMs", drumNoiseDecaySlider_));
    setupComboBox(drumNoisePatternBox_, drumNoisePatternLabel_, "NOISE PATTERN", { "Single Burst", "Multi-Flam Clap", "Gated Snare", "Low-Rumble Swell", "Glitch Stutter" });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumNoisePattern", drumNoisePatternBox_));
    setupSlider(drumNoiseLevelSlider_, drumNoiseLevelLabel_, "NOISE LEVEL");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumNoiseLevel", drumNoiseLevelSlider_));

    // Flexible Mode Controls: DPCM Crusher & FM
    setupSlider(drumDpcmDeltaSlider_, drumDpcmDeltaLabel_, "DELTA SLEW (1-8)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumDpcmDelta", drumDpcmDeltaSlider_));
    setupSlider(drumDpcmRateSlider_, drumDpcmRateLabel_, "RATE (0-15)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumDpcmRate", drumDpcmRateSlider_));
    setupSlider(drumDpcmGritSlider_, drumDpcmGritLabel_, "SLOPE OVERLOAD");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumDpcmGrit", drumDpcmGritSlider_));
    setupSlider(drumDpcmBlendSlider_, drumDpcmBlendLabel_, "SYNTH/DPCM BLEND");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumDpcmBlend", drumDpcmBlendSlider_));
    setupSlider(drumFmRateSlider_, drumFmRateLabel_, "FM RATE (Hz)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumFmRateHz", drumFmRateSlider_));
    setupSlider(drumFmDepthSlider_, drumFmDepthLabel_, "FM DEPTH (Hz)");
    sliderAttachments_.push_back(std::make_unique<SliderAttachment>(audioProcessor.getAPVTS(), "drumFmDepthHz", drumFmDepthSlider_));

    setupComboBox(drumDpcmVoiceBox_, drumDpcmVoiceLabel_, "ROM SAMPLE VOICE", {
        "01. Punch Kick", "02. Crisp Snare", "03. Closed Hat", "04. Open Hat",
        "05. Hand Clap", "06. High Tom", "07. Low Tom", "08. Crash Cymbal",
        "09. Overdrive Kick", "10. Metal Snare", "11. Rimshot", "12. Cowbell",
        "13. Simmons Tom", "14. Boss Snare", "15. Shaker", "16. Ride Bell",
        "17. Bass Pop", "18. Bass Thumb", "19. Orch Hit Maj", "20. Orch Hit Min",
        "21. Timpani", "22. Sub-Boom", "23. Fuzz Snare", "24. Vinyl Scratch",
        "25. Voice KINDNES", "26. Voice POWER UP", "27. Laser Zapper", "28. Coin Chime",
        "29. Jump Spring", "30. Boss Explosion", "31. UFO Siren", "32. Glitch"
    });
    comboAttachments_.push_back(std::make_unique<ComboBoxAttachment>(audioProcessor.getAPVTS(), "drumDpcmVoice", drumDpcmVoiceBox_));

    selectDeckTab(DeckTab::ApuSynth);
    setSize(1040, 640);
    updateDpcmPreviews();
    startTimerHz(30);
}

KindnesAudioProcessorEditor::~KindnesAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void KindnesAudioProcessorEditor::timerCallback()
{
    static const char* kBankPadNames[4][8] = {
        { "01. KICK [C]", "02. SNARE [C#]", "03. CL-HAT [D]", "04. OP-HAT [D#]",
          "05. CLAP [E]", "06. HI-TOM [F]", "07. LO-TOM [F#]", "08. CRASH [G]" },
        { "09. VRC-KCK [C]", "10. 93-SNR [C#]", "11. RIMSHOT [D]", "12. COWBELL [D#]",
          "13. FDS-TOM [E]", "14. GATESNR [F]", "15. SHAKER [F#]", "16. RIDEBEL [G]" },
        { "17. SLAP-POP [C]", "18. SLAP-SUB [C#]", "19. HIT-MAJ [D]", "20. HIT-MIN [D#]",
          "21. TIMPANI [E]", "22. SUB-KCK [F]", "23. FUZZSNR [F#]", "24. SCRATCH [G]" },
        { "25. KINDNES [C]", "26. POWERUP [C#]", "27. LASER [D]", "28. COIN [D#]",
          "29. SPRING [E]", "30. EXPLODE [F]", "31. SIREN [F#]", "32. GLITCH [G]" }
    };

    const auto& dpcm = audioProcessor.getEngine().getDPCM();
    int bank = audioProcessor.getEngine().getActiveDrumBank();
    if (drumBankBox_.getSelectedId() != bank + 1)
        drumBankBox_.setSelectedId(bank + 1, juce::dontSendNotification);

    for (int i = 0; i < 8; ++i)
    {
        drumPads_[static_cast<size_t>(i)].setButtonText(kBankPadNames[bank][i]);
        float act = dpcm.getPadActivity(i);
        auto baseCol = juce::Colour(0xff221418);
        auto activeCol = juce::Colour(0xffC83232);
        drumPads_[static_cast<size_t>(i)].setColour(
            juce::TextButton::buttonColourId,
            baseCol.interpolatedWith(activeCol, std::clamp(act, 0.0f, 1.0f)));
        drumPads_[static_cast<size_t>(i)].setColour(
            juce::TextButton::textColourOffId,
            act > 0.25f ? juce::Colour(0xffFFFFFF) : juce::Colour(0xffE5B869));
    }
}

void KindnesAudioProcessorEditor::pushScopeSamples(const float* samples, int numSamples)
{
    visualizer_.pushSamples(samples, numSamples);
}

void KindnesAudioProcessorEditor::selectDeckTab(DeckTab tab)
{
    activeTab_ = tab;

    auto updateTabStyle = [this](juce::TextButton& btn, bool active) {
        if (active)
        {
            btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffA62432));     // Famicom Crimson
            btn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffFFFFFF));
        }
        else
        {
            btn.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff1C1E23));     // Dark Slate
            btn.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffA0A4B0));
        }
    };

    updateTabStyle(tabSynthBtn_, activeTab_ == DeckTab::ApuSynth);
    updateTabStyle(tabDrumsBtn_, activeTab_ == DeckTab::DrumMachine);
    updateTabStyle(tabDesignerBtn_, activeTab_ == DeckTab::DrumDesigner);
    updateTabStyle(tabTrackerBtn_, activeTab_ == DeckTab::TrackerFx);
    updateTabStyle(tabMasterBtn_, activeTab_ == DeckTab::MasterDeck);

    updateDeckVisibility();
    repaint();
}

void KindnesAudioProcessorEditor::updateDeckVisibility()
{
    const bool isSynth = (activeTab_ == DeckTab::ApuSynth);
    const bool isDrums = (activeTab_ == DeckTab::DrumMachine);
    const bool isDesigner = (activeTab_ == DeckTab::DrumDesigner);
    const bool isTracker = (activeTab_ == DeckTab::TrackerFx);
    const bool isMaster = (activeTab_ == DeckTab::MasterDeck);

    // In DrumDesigner tab: check if Easy or Flexible mode
    const bool isDesignerEasy = isDesigner && (drumDesignModeBox_.getSelectedId() <= 1);
    const bool isDesignerFlex = isDesigner && (drumDesignModeBox_.getSelectedId() == 2);

    // --- Tab 1: APU Synth & Expansion (Pulse 1, Pulse 2, Tri, Noise, VRC6, FDS) ---
    pulse1DutyBox_.setVisible(isSynth);
    pulse1DutyLabel_.setVisible(isSynth);
    pulse1VolSlider_.setVisible(isSynth);
    pulse1VolLabel_.setVisible(isSynth);

    pulse2DutyBox_.setVisible(isSynth);
    pulse2DutyLabel_.setVisible(isSynth);
    pulse2VolSlider_.setVisible(isSynth);
    pulse2VolLabel_.setVisible(isSynth);

    pwmDepthSlider_.setVisible(isSynth);
    pwmDepthLabel_.setVisible(isSynth);

    triVolSlider_.setVisible(isSynth);
    triVolLabel_.setVisible(isSynth);

    noiseModeBox_.setVisible(isSynth);
    noiseModeLabel_.setVisible(isSynth);
    noiseVolSlider_.setVisible(isSynth);
    noiseVolLabel_.setVisible(isSynth);

    vrc6Toggle_.setVisible(isSynth);
    vrc6SawVolSlider_.setVisible(isSynth);
    vrc6SawVolLabel_.setVisible(isSynth);

    fdsToggle_.setVisible(isSynth);
    fdsWaveBox_.setVisible(isSynth);
    fdsWaveLabel_.setVisible(isSynth);
    fdsVolSlider_.setVisible(isSynth);
    fdsVolLabel_.setVisible(isSynth);
    fdsModSpeedSlider_.setVisible(isSynth);
    fdsModSpeedLabel_.setVisible(isSynth);
    fdsModDepthSlider_.setVisible(isSynth);
    fdsModDepthLabel_.setVisible(isSynth);
    fdsModWaveBox_.setVisible(isSynth);
    fdsModWaveLabel_.setVisible(isSynth);

    // --- Tab 2: Drum Machine & DPCM (32 Voices, 4 Banks, 8 Large Pads) ---
    drumBankLabel_.setVisible(isDrums);
    drumBankBox_.setVisible(isDrums);
    dpcmSampleLabel_.setVisible(isDrums);
    dpcmSampleBox_.setVisible(isDrums);
    dpcmChromaticToggle_.setVisible(isDrums);
    dpcmDrumPadsToggle_.setVisible(isDrums);
    demoBeatBtn_.setVisible(isDrums);
    throughDpcmToggle_.setVisible(isDrums);
    dpcmDeltaStepLabel_.setVisible(isDrums);
    dpcmDeltaStepSlider_.setVisible(isDrums);
    dpcmVelToSlewLabel_.setVisible(isDrums);
    dpcmVelToSlewSlider_.setVisible(isDrums);
    dpcmRateLabel_.setVisible(isDrums);
    dpcmRateSlider_.setVisible(isDrums);
    dpcmVolLabel_.setVisible(isDrums);
    dpcmVolSlider_.setVisible(isDrums);
    for (auto& pad : drumPads_)
        pad.setVisible(isDrums);

    // --- Tab 3: Drum Sound Designer ---
    drumDesignModeLabel_.setVisible(isDesigner);
    drumDesignModeBox_.setVisible(isDesigner);
    drumTemplateLabel_.setVisible(isDesigner);
    drumTemplateBox_.setVisible(isDesigner);
    drumMidiModeLabel_.setVisible(isDesigner);
    drumMidiModeBox_.setVisible(isDesigner);
    testDrumBtn_.setVisible(isDesigner);

    // Easy Macros
    drumPunchLabel_.setVisible(isDesignerEasy);
    drumPunchSlider_.setVisible(isDesignerEasy);
    drumBodyMacroLabel_.setVisible(isDesignerEasy);
    drumBodyMacroSlider_.setVisible(isDesignerEasy);
    drumSnappyLabel_.setVisible(isDesignerEasy);
    drumSnappySlider_.setVisible(isDesignerEasy);
    drumGritLabel_.setVisible(isDesignerEasy);
    drumGritSlider_.setVisible(isDesignerEasy);

    // Flexible / Deep Controls: Body & Pitch Engine
    drumPitchCurveLabel_.setVisible(isDesignerFlex);
    drumPitchCurveBox_.setVisible(isDesignerFlex);
    drumPitchTimeLabel_.setVisible(isDesignerFlex);
    drumPitchTimeSlider_.setVisible(isDesignerFlex);
    drumBodyStartPitchLabel_.setVisible(isDesignerFlex);
    drumBodyStartPitchSlider_.setVisible(isDesignerFlex);
    drumBodyEndPitchLabel_.setVisible(isDesignerFlex);
    drumBodyEndPitchSlider_.setVisible(isDesignerFlex);
    drumBodyDecayLabel_.setVisible(isDesignerFlex);
    drumBodyDecaySlider_.setVisible(isDesignerFlex);
    drumBodyWaveLabel_.setVisible(isDesignerFlex);
    drumBodyWaveBox_.setVisible(isDesignerFlex);
    drumBodyLevelLabel_.setVisible(isDesignerFlex);
    drumBodyLevelSlider_.setVisible(isDesignerFlex);

    // Flexible / Deep Controls: Noise & Transient Click
    drumClickTypeLabel_.setVisible(isDesignerFlex);
    drumClickTypeBox_.setVisible(isDesignerFlex);
    drumClickLevelLabel_.setVisible(isDesignerFlex);
    drumClickLevelSlider_.setVisible(isDesignerFlex);
    drumNoiseModeLabel_.setVisible(isDesignerFlex);
    drumNoiseModeBox_.setVisible(isDesignerFlex);
    drumNoisePeriodLabel_.setVisible(isDesignerFlex);
    drumNoisePeriodSlider_.setVisible(isDesignerFlex);
    drumNoiseDecayLabel_.setVisible(isDesignerFlex);
    drumNoiseDecaySlider_.setVisible(isDesignerFlex);
    drumNoisePatternLabel_.setVisible(isDesignerFlex);
    drumNoisePatternBox_.setVisible(isDesignerFlex);
    drumNoiseLevelLabel_.setVisible(isDesignerFlex);
    drumNoiseLevelSlider_.setVisible(isDesignerFlex);

    // Flexible / Deep Controls: DPCM & FM
    drumDpcmDeltaLabel_.setVisible(isDesignerFlex);
    drumDpcmDeltaSlider_.setVisible(isDesignerFlex);
    drumDpcmRateLabel_.setVisible(isDesignerFlex);
    drumDpcmRateSlider_.setVisible(isDesignerFlex);
    drumDpcmGritLabel_.setVisible(isDesignerFlex);
    drumDpcmGritSlider_.setVisible(isDesignerFlex);
    drumDpcmBlendLabel_.setVisible(isDesignerFlex);
    drumDpcmBlendSlider_.setVisible(isDesignerFlex);
    drumFmRateLabel_.setVisible(isDesignerFlex);
    drumFmRateSlider_.setVisible(isDesignerFlex);
    drumFmDepthLabel_.setVisible(isDesignerFlex);
    drumFmDepthSlider_.setVisible(isDesignerFlex);
    drumDpcmVoiceLabel_.setVisible(isDesignerFlex);
    drumDpcmVoiceBox_.setVisible(isDesignerFlex);

    // --- Tab 4: Tracker Arp & Chiptune Embellishments ---
    arpToggle_.setVisible(isTracker);
    arpRateLabel_.setVisible(isTracker);
    arpRateBox_.setVisible(isTracker);
    arpPatternLabel_.setVisible(isTracker);
    arpPatternBox_.setVisible(isTracker);
    arpEditorLabel_.setVisible(isTracker);
    arpPatternEditor_.setVisible(isTracker);

    scoopSemiLabel_.setVisible(isTracker);
    scoopSemiSlider_.setVisible(isTracker);
    scoopTimeLabel_.setVisible(isTracker);
    scoopTimeSlider_.setVisible(isTracker);

    delayedVibDelayLabel_.setVisible(isTracker);
    delayedVibDelaySlider_.setVisible(isTracker);
    delayedVibDepthLabel_.setVisible(isTracker);
    delayedVibDepthSlider_.setVisible(isTracker);

    dutyMorphToggle_.setVisible(isTracker);
    pseudoEchoToggle_.setVisible(isTracker);
    pseudoEchoDelayLabel_.setVisible(isTracker);
    pseudoEchoDelaySlider_.setVisible(isTracker);
    pseudoEchoVolLabel_.setVisible(isTracker);
    pseudoEchoVolSlider_.setVisible(isTracker);

    // --- Tab 4: Master, SVF Filter, ADSR & Output ---
    playModeLabel_.setVisible(isMaster);
    playModeBox_.setVisible(isMaster);
    authenticDacToggle_.setVisible(isMaster);

    filterModeLabel_.setVisible(isMaster);
    filterModeBox_.setVisible(isMaster);
    analogFilterToggle_.setVisible(isMaster);
    cutoffLabel_.setVisible(isMaster);
    cutoffSlider_.setVisible(isMaster);
    resonanceLabel_.setVisible(isMaster);
    resonanceSlider_.setVisible(isMaster);
    toneLabel_.setVisible(isMaster);
    toneSlider_.setVisible(isMaster);

    adsrToggle_.setVisible(isMaster);
    attackLabel_.setVisible(isMaster);
    attackSlider_.setVisible(isMaster);
    decayLabel_.setVisible(isMaster);
    decaySlider_.setVisible(isMaster);
    sustainLabel_.setVisible(isMaster);
    sustainSlider_.setVisible(isMaster);
    releaseLabel_.setVisible(isMaster);
    releaseSlider_.setVisible(isMaster);

    glideLabel_.setVisible(isMaster);
    glideSlider_.setVisible(isMaster);
    vibratoLabel_.setVisible(isMaster);
    vibratoSlider_.setVisible(isMaster);
    detuneLabel_.setVisible(isMaster);
    detuneSlider_.setVisible(isMaster);
    spreadLabel_.setVisible(isMaster);
    spreadSlider_.setVisible(isMaster);
    masterTrimLabel_.setVisible(isMaster);
    masterTrimSlider_.setVisible(isMaster);
}

void KindnesAudioProcessorEditor::updateDpcmPreviews()
{
    const auto& dpcm = audioProcessor.getEngine().getDPCM();
    visualizer_.setDpcmPreviews(dpcm.getOriginalPcmWaveform(), dpcm.getReconstructedWaveform());
}

void KindnesAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 13);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(9.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffC4C7CE));
    addAndMakeVisible(label);
}

void KindnesAudioProcessorEditor::setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items)
{
    for (int i = 0; i < items.size(); ++i)
        box.addItem(items[i], i + 1);
    addAndMakeVisible(box);

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font(9.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xffC4C7CE));
    addAndMakeVisible(label);
}

bool KindnesAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (const auto& f : files)
    {
        if (f.endsWithIgnoreCase(".wav") || f.endsWithIgnoreCase(".aif") || f.endsWithIgnoreCase(".aiff") || f.endsWithIgnoreCase(".mp3"))
            return true;
    }
    return false;
}

void KindnesAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.isEmpty())
        return;

    juce::File file(files[0]);
    juce::AudioFormatManager formatMgr;
    formatMgr.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatMgr.createReaderFor(file));
    if (reader != nullptr)
    {
        juce::AudioBuffer<float> buffer(1, static_cast<int>(reader->lengthInSamples));
        reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, false);
        audioProcessor.getEngine().loadDPCMFile(buffer.getReadPointer(0), buffer.getNumSamples(), static_cast<float>(reader->sampleRate));
        dpcmSampleBox_.setSelectedId(33, juce::dontSendNotification);
        updateDpcmPreviews();
    }
}

void KindnesAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. Matte Carbon Chassis Background
    g.setColour(juce::Colour(0xff121316));
    g.fillAll();

    juce::ColourGradient grad(juce::Colour(0xff16171C), 0, 0, juce::Colour(0xff0C0D10), 0, bounds.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRect(bounds);

    // 2. Vintage Famicom Crimson Header Bar
    auto headerBounds = bounds.removeFromTop(44.0f);
    g.setColour(juce::Colour(0xff8C1D24));
    g.fillRect(headerBounds);

    g.setColour(juce::Colour(0xffFFD700));
    g.fillRect(0.0f, 42.0f, bounds.getWidth(), 2.0f);

    // Title Badge
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xffFFFFFF));
    g.drawText("KINDNES", 16, 6, 120, 32, juce::Justification::left, false);

    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.setColour(juce::Colour(0xffE5B869));
    g.drawText("v0.0.4 • 2A03 APU, 2C33 FM & VRC6 EXPANSION SYNTHESIZER", 125, 14, 480, 18, juce::Justification::left, false);

    g.setFont(juce::Font(9.5f, juce::Font::plain));
    g.setColour(juce::Colour(0x90FFFFFF));
    g.drawText("96kHz HIGH-FIDELITY • BIT-ACCURATE DPCM", getWidth() - 280, 14, 260, 18, juce::Justification::right, false);

    // Helper for hardware rack cards
    auto drawCard = [&](juce::Rectangle<float> cardArea, const juce::String& title)
    {
        g.setColour(juce::Colour(0xff17181D));
        g.fillRoundedRectangle(cardArea, 4.0f);
        g.setColour(juce::Colour(0xff272A34));
        g.drawRoundedRectangle(cardArea, 4.0f, 1.0f);

        // Header band inside card
        auto titleArea = cardArea.removeFromTop(20.0f);
        g.setColour(juce::Colour(0x18000000));
        g.fillRect(titleArea);

        g.setFont(juce::Font(9.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xffE5B869));
        g.drawText(title, titleArea.reduced(8.0f, 0.0f), juce::Justification::centredLeft, false);
    };

    // Draw active deck cards based on selected tab
    if (activeTab_ == DeckTab::ApuSynth)
    {
        drawCard(juce::Rectangle<float>(14, 230, 600, 380), "2A03 APU CHANNELS (PULSE 1, PULSE 2, TRIANGLE, NOISE)");
        drawCard(juce::Rectangle<float>(624, 230, 402, 380), "CARTRIDGE EXPANSIONS (VRC6 SAWTOOTH & 2C33 FM)");
    }
    else if (activeTab_ == DeckTab::DrumMachine)
    {
        drawCard(juce::Rectangle<float>(14, 230, 1012, 114), "1-BIT DPCM HARDWARE SAMPLER & BANK SELECT (32 BUILT-IN VOICES)");
        drawCard(juce::Rectangle<float>(14, 354, 1012, 256), "8-PAD HYBRID CHIPTUNE DRUM MACHINE (PUNCH DIVE + NOISE TRANSIENT + 1-BIT DPCM)");
    }
    else if (activeTab_ == DeckTab::DrumDesigner)
    {
        const bool isEasy = (drumDesignModeBox_.getSelectedId() <= 1);
        if (isEasy)
        {
            drawCard(juce::Rectangle<float>(14, 230, 1012, 110), "ADVANCED DRUM ARCHITECTURE & INSTANT TEMPLATES");
            drawCard(juce::Rectangle<float>(14, 350, 1012, 260), "EASY MACRO SCULPTING (PUNCH, BODY, SNAPPY, 1-BIT GRIT)");
        }
        else
        {
            drawCard(juce::Rectangle<float>(14, 230, 1012, 70), "SURGICAL PARAMETERS & LIVE REAL-TIME AUDITION");
            drawCard(juce::Rectangle<float>(14, 308, 324, 302), "1. TONAL BODY & MULTI-CURVE PITCH ENGINE");
            drawCard(juce::Rectangle<float>(348, 308, 324, 302), "2. LFSR NOISE & TRANSIENT CLICK");
            drawCard(juce::Rectangle<float>(682, 308, 344, 302), "3. 1-BIT DPCM DELTA CRUSHER & FM");
        }
    }
    else if (activeTab_ == DeckTab::TrackerFx)
    {
        drawCard(juce::Rectangle<float>(14, 230, 498, 380), "60Hz / 50Hz FRAME SEQUENCER ARPEGGIATOR");
        drawCard(juce::Rectangle<float>(522, 230, 504, 380), "TRACKER CHIPTUNE EMBELLISHMENTS & PSEUDO-ECHO");
    }
    else if (activeTab_ == DeckTab::MasterDeck)
    {
        drawCard(juce::Rectangle<float>(14, 230, 498, 380), "SVF RESONANT FILTER & ANALOG MOTHERBOARD STAGE");
        drawCard(juce::Rectangle<float>(522, 230, 504, 380), "ADSR ENVELOPE, STEREO UNISON & OUTPUT STAGE");
    }

    // Drag & Drop Hint at the bottom
    g.setFont(juce::Font(9.5f, juce::Font::italic));
    g.setColour(juce::Colour(0x60D0D2D6));
    g.drawText("DRAG & DROP ANY AUDIO FILE (.WAV / .AIF / .MP3) TO AUTO-ENCODE 1-BIT DELTA MODULATION",
               0, getHeight() - 20, getWidth(), 16, juce::Justification::centred, false);
}

void KindnesAudioProcessorEditor::resized()
{
    // 1. Preset Selector Bar
    prevPresetBtn_.setBounds(14, 48, 28, 24);
    presetBox_.setBounds(46, 48, 948, 24);
    nextPresetBtn_.setBounds(998, 48, 28, 24);

    // 2. Dual Oscilloscope & 1-Bit DPCM Staircase Visualizer
    visualizer_.setBounds(14, 76, 1012, 108);

    // 3. Tabbed Deck Navigation Bar (y = 190, h = 32)
    const int tabY = 190;
    const int tabH = 32;
    const int tabGap = 8;
    const int tabW = (1012 - (4 * tabGap)) / 5; // ~196px each
    tabSynthBtn_.setBounds(14 + 0 * (tabW + tabGap), tabY, tabW, tabH);
    tabDrumsBtn_.setBounds(14 + 1 * (tabW + tabGap), tabY, tabW, tabH);
    tabDesignerBtn_.setBounds(14 + 2 * (tabW + tabGap), tabY, tabW, tabH);
    tabTrackerBtn_.setBounds(14 + 3 * (tabW + tabGap), tabY, tabW, tabH);
    tabMasterBtn_.setBounds(14 + 4 * (tabW + tabGap), tabY, tabW, tabH);

    // =========================================================================
    // --- TAB 1: APU SYNTH & EXPANSIONS (y = 230, h = 380) ---
    // =========================================================================
    // Left Card (x = 14..614): 2A03 APU Channels
    // Row 1: Pulse 1 & Pulse 2
    pulse1DutyLabel_.setBounds(24, 256, 130, 13);
    pulse1DutyBox_.setBounds(24, 271, 130, 22);
    pulse1VolLabel_.setBounds(24, 298, 130, 13);
    pulse1VolSlider_.setBounds(24, 312, 130, 78);

    pulse2DutyLabel_.setBounds(168, 256, 130, 13);
    pulse2DutyBox_.setBounds(168, 271, 130, 22);
    pulse2VolLabel_.setBounds(168, 298, 130, 13);
    pulse2VolSlider_.setBounds(168, 312, 130, 78);

    pwmDepthLabel_.setBounds(312, 298, 120, 13);
    pwmDepthSlider_.setBounds(312, 312, 120, 78);

    // Row 2: Triangle, Noise LFSR
    triVolLabel_.setBounds(24, 410, 130, 13);
    triVolSlider_.setBounds(24, 426, 130, 80);

    noiseModeLabel_.setBounds(168, 410, 130, 13);
    noiseModeBox_.setBounds(168, 426, 130, 22);
    noiseVolLabel_.setBounds(168, 454, 130, 13);
    noiseVolSlider_.setBounds(168, 468, 130, 80);

    // Right Card (x = 624..1026): VRC6 Saw & FDS 2C33 FM
    vrc6Toggle_.setBounds(636, 258, 160, 22);
    vrc6SawVolLabel_.setBounds(636, 298, 160, 13);
    vrc6SawVolSlider_.setBounds(636, 312, 160, 80);

    fdsToggle_.setBounds(636, 404, 160, 22);
    fdsWaveLabel_.setBounds(636, 430, 175, 13);
    fdsWaveBox_.setBounds(636, 444, 175, 22);
    fdsModWaveLabel_.setBounds(826, 430, 185, 13);
    fdsModWaveBox_.setBounds(826, 444, 185, 22);

    fdsVolLabel_.setBounds(636, 474, 115, 13);
    fdsVolSlider_.setBounds(636, 488, 115, 78);
    fdsModSpeedLabel_.setBounds(766, 474, 115, 13);
    fdsModSpeedSlider_.setBounds(766, 488, 115, 78);
    fdsModDepthLabel_.setBounds(896, 474, 115, 13);
    fdsModDepthSlider_.setBounds(896, 488, 115, 78);

    // =========================================================================
    // --- TAB 2: DRUM MACHINE & DPCM (y = 230, h = 380) ---
    // =========================================================================
    // Top Card (y = 230, h = 114): Bank Selector & DPCM Controls
    drumBankLabel_.setBounds(24, 256, 250, 13);
    drumBankBox_.setBounds(24, 271, 250, 24);

    dpcmSampleLabel_.setBounds(290, 256, 230, 13);
    dpcmSampleBox_.setBounds(290, 271, 230, 24);

    dpcmChromaticToggle_.setBounds(535, 256, 170, 20);
    dpcmDrumPadsToggle_.setBounds(535, 278, 170, 20);

    demoBeatBtn_.setBounds(715, 256, 150, 24);
    throughDpcmToggle_.setBounds(715, 282, 150, 20);

    dpcmDeltaStepLabel_.setBounds(24, 304, 115, 13);
    dpcmDeltaStepSlider_.setBounds(24, 318, 115, 24); // slider with text

    dpcmVelToSlewLabel_.setBounds(155, 304, 115, 13);
    dpcmVelToSlewSlider_.setBounds(155, 318, 115, 24);

    dpcmRateLabel_.setBounds(286, 304, 115, 13);
    dpcmRateSlider_.setBounds(286, 318, 115, 24);

    dpcmVolLabel_.setBounds(417, 304, 100, 13);
    dpcmVolSlider_.setBounds(417, 318, 100, 24);

    // Bottom Card (y = 354, h = 256): 8 Large Tactile Drum Pads (2 rows x 4 cols)
    const int gridStartX = 26;
    const int gridStartY = 384;
    const int padWidth = 234;
    const int padHeight = 98;
    const int padSpacingX = 14;
    const int padSpacingY = 14;

    for (int i = 0; i < 8; ++i)
    {
        int col = i % 4;
        int row = i / 4;
        drumPads_[static_cast<size_t>(i)].setBounds(
            gridStartX + col * (padWidth + padSpacingX),
            gridStartY + row * (padHeight + padSpacingY),
            padWidth,
            padHeight);
    }

    // =========================================================================
    // --- TAB 3: ADVANCED DRUM SOUND DESIGNER (y = 230, h = 380) ---
    // =========================================================================
    const bool isDesignerEasyMode = (drumDesignModeBox_.getSelectedId() <= 1);
    if (isDesignerEasyMode)
    {
        // Top Card: Architecture selector, MIDI Mode & Audition Pad
        drumDesignModeLabel_.setBounds(24, 254, 150, 13);
        drumDesignModeBox_.setBounds(24, 269, 150, 24);

        drumTemplateLabel_.setBounds(184, 254, 220, 13);
        drumTemplateBox_.setBounds(184, 269, 220, 24);

        drumMidiModeLabel_.setBounds(414, 254, 220, 13);
        drumMidiModeBox_.setBounds(414, 269, 220, 24);

        testDrumBtn_.setBounds(648, 256, 362, 44);

        // Bottom Card: 4 Large Intuitive Macros
        const int mY = 380;
        const int mW = 210;
        drumPunchLabel_.setBounds(36, mY, mW, 15);
        drumPunchSlider_.setBounds(36, mY + 18, mW, 170);

        drumBodyMacroLabel_.setBounds(284, mY, mW, 15);
        drumBodyMacroSlider_.setBounds(284, mY + 18, mW, 170);

        drumSnappyLabel_.setBounds(532, mY, mW, 15);
        drumSnappySlider_.setBounds(532, mY + 18, mW, 170);

        drumGritLabel_.setBounds(780, mY, mW, 15);
        drumGritSlider_.setBounds(780, mY + 18, mW, 170);
    }
    else
    {
        // Flexible Mode
        // Top Header
        drumDesignModeLabel_.setBounds(24, 244, 150, 13);
        drumDesignModeBox_.setBounds(24, 258, 150, 24);

        drumTemplateLabel_.setBounds(184, 244, 220, 13);
        drumTemplateBox_.setBounds(184, 258, 220, 24);

        drumMidiModeLabel_.setBounds(414, 244, 220, 13);
        drumMidiModeBox_.setBounds(414, 258, 220, 24);

        testDrumBtn_.setBounds(648, 246, 362, 38);

        // Card 1: Tonal Body & Multi-Curve Pitch Engine (x = 14, w = 324)
        drumPitchCurveLabel_.setBounds(24, 326, 140, 13);
        drumPitchCurveBox_.setBounds(24, 340, 140, 22);
        drumPitchTimeLabel_.setBounds(178, 326, 140, 13);
        drumPitchTimeSlider_.setBounds(178, 340, 140, 22);

        drumBodyStartPitchLabel_.setBounds(24, 370, 140, 13);
        drumBodyStartPitchSlider_.setBounds(24, 384, 140, 68);
        drumBodyEndPitchLabel_.setBounds(178, 370, 140, 13);
        drumBodyEndPitchSlider_.setBounds(178, 384, 140, 68);

        drumBodyDecayLabel_.setBounds(24, 458, 140, 13);
        drumBodyDecaySlider_.setBounds(24, 472, 140, 68);
        drumBodyLevelLabel_.setBounds(178, 458, 140, 13);
        drumBodyLevelSlider_.setBounds(178, 472, 140, 68);

        drumBodyWaveLabel_.setBounds(24, 546, 294, 13);
        drumBodyWaveBox_.setBounds(24, 560, 294, 24);

        // Card 2: LFSR Noise & Transient Click (x = 348, w = 324)
        drumClickTypeLabel_.setBounds(358, 326, 140, 13);
        drumClickTypeBox_.setBounds(358, 340, 140, 22);
        drumClickLevelLabel_.setBounds(512, 326, 140, 13);
        drumClickLevelSlider_.setBounds(512, 340, 140, 22);

        drumNoiseModeLabel_.setBounds(358, 370, 140, 13);
        drumNoiseModeBox_.setBounds(358, 384, 140, 24);
        drumNoisePeriodLabel_.setBounds(512, 370, 140, 13);
        drumNoisePeriodSlider_.setBounds(512, 384, 140, 68);

        drumNoiseDecayLabel_.setBounds(358, 458, 140, 13);
        drumNoiseDecaySlider_.setBounds(358, 472, 140, 68);
        drumNoiseLevelLabel_.setBounds(512, 458, 140, 13);
        drumNoiseLevelSlider_.setBounds(512, 472, 140, 68);

        drumNoisePatternLabel_.setBounds(358, 546, 294, 13);
        drumNoisePatternBox_.setBounds(358, 560, 294, 24);

        // Card 3: 1-Bit DPCM Delta Crusher & FM (x = 682, w = 344)
        drumFmRateLabel_.setBounds(692, 326, 150, 13);
        drumFmRateSlider_.setBounds(692, 340, 150, 22);
        drumFmDepthLabel_.setBounds(856, 326, 150, 13);
        drumFmDepthSlider_.setBounds(856, 340, 150, 22);

        drumDpcmDeltaLabel_.setBounds(692, 370, 150, 13);
        drumDpcmDeltaSlider_.setBounds(692, 384, 150, 68);
        drumDpcmRateLabel_.setBounds(856, 370, 150, 13);
        drumDpcmRateSlider_.setBounds(856, 384, 150, 68);

        drumDpcmGritLabel_.setBounds(692, 458, 150, 13);
        drumDpcmGritSlider_.setBounds(692, 472, 150, 68);
        drumDpcmBlendLabel_.setBounds(856, 458, 150, 13);
        drumDpcmBlendSlider_.setBounds(856, 472, 150, 68);

        drumDpcmVoiceLabel_.setBounds(692, 546, 314, 13);
        drumDpcmVoiceBox_.setBounds(692, 560, 314, 24);
    }

    // =========================================================================
    // --- TAB 4: TRACKER ARP & CHIPTUNE EMBELLISHMENTS (y = 230, h = 380) ---
    // =========================================================================
    // Left Card (y = 230, w = 498): 60Hz/50Hz Frame Sequencer Arp
    arpToggle_.setBounds(24, 256, 140, 22);
    arpRateLabel_.setBounds(180, 256, 140, 13);
    arpRateBox_.setBounds(180, 271, 140, 22);

    arpPatternLabel_.setBounds(24, 300, 140, 13);
    arpPatternBox_.setBounds(24, 315, 140, 22);

    arpEditorLabel_.setBounds(24, 350, 460, 13);
    arpPatternEditor_.setBounds(24, 368, 460, 36);

    // Right Card (y = 230, w = 504): Pitch Scoop, Delayed Vibrato, Echo
    scoopSemiLabel_.setBounds(536, 256, 105, 13);
    scoopSemiSlider_.setBounds(536, 271, 105, 78);
    scoopTimeLabel_.setBounds(656, 256, 105, 13);
    scoopTimeSlider_.setBounds(656, 271, 105, 78);

    delayedVibDelayLabel_.setBounds(776, 256, 105, 13);
    delayedVibDelaySlider_.setBounds(776, 271, 105, 78);
    delayedVibDepthLabel_.setBounds(896, 256, 105, 13);
    delayedVibDepthSlider_.setBounds(896, 271, 105, 78);

    dutyMorphToggle_.setBounds(536, 370, 200, 22);
    pseudoEchoToggle_.setBounds(750, 370, 200, 22);

    pseudoEchoDelayLabel_.setBounds(536, 404, 150, 13);
    pseudoEchoDelaySlider_.setBounds(536, 418, 150, 78);
    pseudoEchoVolLabel_.setBounds(710, 404, 150, 13);
    pseudoEchoVolSlider_.setBounds(710, 418, 150, 78);

    // =========================================================================
    // --- TAB 4: MASTER, SVF FILTER, ADSR & OUTPUT (y = 230, h = 380) ---
    // =========================================================================
    // Left Card (y = 230, w = 498): SVF Filter & Motherboard
    filterModeLabel_.setBounds(24, 256, 140, 13);
    filterModeBox_.setBounds(24, 271, 140, 22);
    analogFilterToggle_.setBounds(180, 270, 160, 24);

    cutoffLabel_.setBounds(24, 308, 100, 13);
    cutoffSlider_.setBounds(24, 322, 100, 80);
    resonanceLabel_.setBounds(138, 308, 100, 13);
    resonanceSlider_.setBounds(138, 322, 100, 80);
    toneLabel_.setBounds(252, 308, 100, 13);
    toneSlider_.setBounds(252, 322, 100, 80);

    playModeLabel_.setBounds(24, 420, 140, 13);
    playModeBox_.setBounds(24, 435, 140, 22);
    authenticDacToggle_.setBounds(180, 434, 160, 24);

    glideLabel_.setBounds(24, 478, 100, 13);
    glideSlider_.setBounds(24, 492, 100, 80);
    vibratoLabel_.setBounds(138, 478, 100, 13);
    vibratoSlider_.setBounds(138, 492, 100, 80);

    // Right Card (y = 230, w = 504): ADSR & Master Output
    adsrToggle_.setBounds(536, 256, 160, 22);
    attackLabel_.setBounds(536, 290, 75, 13);
    attackSlider_.setBounds(536, 304, 75, 80);
    decayLabel_.setBounds(622, 290, 75, 13);
    decaySlider_.setBounds(622, 304, 75, 80);
    sustainLabel_.setBounds(708, 290, 75, 13);
    sustainSlider_.setBounds(708, 304, 75, 80);
    releaseLabel_.setBounds(794, 290, 75, 13);
    releaseSlider_.setBounds(794, 304, 75, 80);

    detuneLabel_.setBounds(536, 400, 110, 13);
    detuneSlider_.setBounds(536, 414, 110, 80);
    spreadLabel_.setBounds(660, 400, 110, 13);
    spreadSlider_.setBounds(660, 414, 110, 80);

    masterTrimLabel_.setBounds(810, 396, 175, 14);
    masterTrimSlider_.setBounds(810, 414, 175, 100);
}

