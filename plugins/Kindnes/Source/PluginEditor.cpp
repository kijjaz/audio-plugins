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
        "Organ", "Bell", "Formant Aah", "Zelda Title"
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
    setupComboBox(dpcmSampleBox_, dpcmSampleLabel_, "DRUM BANK & SAMPLE ROM (32)", {
        "01. Sunsoft Punch Kick [Bank 1]", "02. Mega Crisp Snare [Bank 1]", "03. Closed Hi-Hat [Bank 1]", "04. Open Hi-Hat [Bank 1]",
        "05. 8-Bit Hand Clap [Bank 1]", "06. High Rack Tom [Bank 1]", "07. Low Floor Tom [Bank 1]", "08. Arcade Crash Cymbal [Bank 1]",
        "09. VRC6 Overdriven Kick [Bank 2]", "10. 93-Step Metallic Snare [Bank 2]", "11. Chiptune Rimshot [Bank 2]", "12. 8-Bit Arcade Cowbell [Bank 2]",
        "13. FDS 2C33 FM Simmons Tom [Bank 2]", "14. Gated Boss Battle Snare [Bank 2]", "15. Chiptune Shaker [Bank 2]", "16. FDS Metallic Ride Bell [Bank 2]",
        "17. 1-Bit Slap Bass Pop [Bank 3]", "18. 1-Bit Slap Bass Thumb [Bank 3]", "19. Orchestra Hit Major [Bank 3]", "20. Orchestra Hit Minor [Bank 3]",
        "21. Konami 1-Bit Timpani [Bank 3]", "22. 2A03 Deep Sub-Boom Kick [Bank 3]", "23. Slope-Overload Fuzz Snare [Bank 3]", "24. 1-Bit Vinyl Scratch [Bank 3]",
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

    setSize(1040, 890);
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
    g.drawText("v0.0.2 • RICOH 2A03 APU, FDS 2C33 FM & VRC6 SYNTHESIZER", 125, 14, 480, 18, juce::Justification::left, false);

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

    // Draw the 5 main rack cards
    drawCard(juce::Rectangle<float>(12, 196, 1016, 134), "BAY 1: RICOH 2A03 APU OSCILLATOR CHANNELS & VRC6 EXPANSION");
    drawCard(juce::Rectangle<float>(12, 336, 498, 140), "BAY 2A: FAMICOM DISK SYSTEM (FDS 2C33 FM)");
    drawCard(juce::Rectangle<float>(518, 336, 510, 140), "BAY 2B: CHIPTUNE EMBELLISHMENTS & FX");
    drawCard(juce::Rectangle<float>(12, 482, 1016, 178), "BAY 3: 8-PAD NES DRUM MACHINE & 1-BIT DPCM SAMPLER (CLICK PADS, AUTO BEAT, OR PLAY C-G)");
    drawCard(juce::Rectangle<float>(12, 666, 1016, 198), "BAY 4: SVF ANALOG FILTER, ADSR, ARPEGGIATOR & MASTER");

    // Drag & Drop Hint at the bottom
    g.setFont(juce::Font(9.5f, juce::Font::italic));
    g.setColour(juce::Colour(0x60D0D2D6));
    g.drawText("DRAG & DROP ANY AUDIO FILE (.WAV / .AIF / .MP3) TO AUTO-ENCODE 1-BIT DELTA MODULATION",
               0, getHeight() - 20, getWidth(), 16, juce::Justification::centred, false);
}

void KindnesAudioProcessorEditor::resized()
{
    // Preset selector bar
    prevPresetBtn_.setBounds(16, 48, 28, 26);
    presetBox_.setBounds(48, 48, 944, 26);
    nextPresetBtn_.setBounds(996, 48, 28, 26);

    // Dual Visualizer (Oscilloscope + 1-Bit DPCM Staircase)
    visualizer_.setBounds(14, 78, 1012, 112);

    // =========================================================================
    // --- BAY 1: RICOH 2A03 APU CHANNELS & VRC6 (y = 196, h = 134) ---
    // =========================================================================
    // Strip 0: Pulse 1
    pulse1DutyLabel_.setBounds(20, 218, 125, 13);
    pulse1DutyBox_.setBounds(24, 233, 117, 20);
    pulse1VolLabel_.setBounds(20, 258, 125, 13);
    pulse1VolSlider_.setBounds(20, 271, 125, 52);

    // Strip 1: Pulse 2
    pulse2DutyLabel_.setBounds(155, 218, 125, 13);
    pulse2DutyBox_.setBounds(159, 233, 117, 20);
    pulse2VolLabel_.setBounds(155, 258, 125, 13);
    pulse2VolSlider_.setBounds(155, 271, 125, 52);

    // Strip 2: Auto-PWM & Triangle
    pwmDepthLabel_.setBounds(290, 218, 65, 13);
    pwmDepthSlider_.setBounds(290, 233, 65, 90);
    triVolLabel_.setBounds(360, 218, 65, 13);
    triVolSlider_.setBounds(360, 233, 65, 90);

    // Strip 3: Noise LFSR
    noiseModeLabel_.setBounds(435, 218, 125, 13);
    noiseModeBox_.setBounds(439, 233, 117, 20);
    noiseVolLabel_.setBounds(435, 258, 125, 13);
    noiseVolSlider_.setBounds(435, 271, 125, 52);

    // Strip 4: VRC6 Saw Expansion
    vrc6Toggle_.setBounds(575, 231, 120, 22);
    vrc6SawVolLabel_.setBounds(575, 258, 120, 13);
    vrc6SawVolSlider_.setBounds(575, 271, 120, 52);

    // Strip 5: Glide & Stereo Spread
    glideLabel_.setBounds(710, 218, 68, 13);
    glideSlider_.setBounds(710, 233, 68, 90);
    spreadLabel_.setBounds(784, 218, 68, 13);
    spreadSlider_.setBounds(784, 233, 68, 90);

    // Strip 6: Play Mode & Non-Linear DAC
    playModeLabel_.setBounds(862, 218, 155, 13);
    playModeBox_.setBounds(866, 233, 147, 20);
    authenticDacToggle_.setBounds(866, 268, 147, 24);

    // =========================================================================
    // --- BAY 2A: FDS 2C33 WAVETABLE & FM (y = 336, h = 140) ---
    // =========================================================================
    fdsToggle_.setBounds(20, 358, 120, 22);
    fdsWaveLabel_.setBounds(150, 356, 160, 13);
    fdsWaveBox_.setBounds(150, 370, 160, 20);
    fdsModWaveLabel_.setBounds(320, 356, 175, 13);
    fdsModWaveBox_.setBounds(320, 370, 175, 20);

    fdsVolLabel_.setBounds(30, 396, 140, 13);
    fdsVolSlider_.setBounds(30, 410, 140, 60);

    fdsModSpeedLabel_.setBounds(185, 396, 140, 13);
    fdsModSpeedSlider_.setBounds(185, 410, 140, 60);

    fdsModDepthLabel_.setBounds(340, 396, 140, 13);
    fdsModDepthSlider_.setBounds(340, 410, 140, 60);

    // =========================================================================
    // --- BAY 2B: CHIPTUNE EMBELLISHMENTS & FX (y = 336, h = 140) ---
    // =========================================================================
    scoopSemiLabel_.setBounds(526, 358, 68, 13);
    scoopSemiSlider_.setBounds(526, 372, 68, 66);
    scoopTimeLabel_.setBounds(600, 358, 68, 13);
    scoopTimeSlider_.setBounds(600, 372, 68, 66);

    delayedVibDelayLabel_.setBounds(678, 358, 70, 13);
    delayedVibDelaySlider_.setBounds(678, 372, 70, 66);
    delayedVibDepthLabel_.setBounds(754, 358, 70, 13);
    delayedVibDepthSlider_.setBounds(754, 372, 70, 66);

    dutyMorphToggle_.setBounds(834, 358, 185, 20);
    pseudoEchoToggle_.setBounds(834, 380, 185, 20);

    pseudoEchoDelayLabel_.setBounds(834, 404, 88, 13);
    pseudoEchoDelaySlider_.setBounds(834, 418, 88, 52);
    pseudoEchoVolLabel_.setBounds(928, 404, 88, 13);
    pseudoEchoVolSlider_.setBounds(928, 418, 88, 52);

    // =========================================================================
    // --- BAY 3: 1-BIT DPCM SAMPLER & DRUM MACHINE (y = 482, h = 178) ---
    // =========================================================================
    dpcmSampleLabel_.setBounds(20, 504, 210, 13);
    dpcmSampleBox_.setBounds(20, 518, 210, 22);

    dpcmChromaticToggle_.setBounds(240, 504, 155, 20);
    dpcmDrumPadsToggle_.setBounds(240, 526, 155, 20);

    demoBeatBtn_.setBounds(402, 504, 180, 22);
    throughDpcmToggle_.setBounds(402, 528, 180, 20);

    dpcmDeltaStepLabel_.setBounds(600, 504, 95, 13);
    dpcmDeltaStepSlider_.setBounds(600, 518, 95, 58);

    dpcmVelToSlewLabel_.setBounds(705, 504, 95, 13);
    dpcmVelToSlewSlider_.setBounds(705, 518, 95, 58);

    dpcmRateLabel_.setBounds(810, 504, 95, 13);
    dpcmRateSlider_.setBounds(810, 518, 95, 58);

    dpcmVolLabel_.setBounds(915, 504, 95, 13);
    dpcmVolSlider_.setBounds(915, 518, 95, 58);

    // 8 Tactile Drum Machine Pads
    const int padStartX = 20;
    const int padY = 590;
    const int padW = 120;
    const int padH = 40;
    const int padGap = 6;
    for (int i = 0; i < 8; ++i)
    {
        drumPads_[i].setBounds(padStartX + i * (padW + padGap), padY, padW, padH);
    }

    // =========================================================================
    // --- BAY 4: FILTER, ADSR, ARPEGGIATOR & MASTER (y = 666, h = 198) ---
    // =========================================================================
    // 1. Resonant SVF & Tone Tilt
    filterModeLabel_.setBounds(20, 688, 105, 13);
    filterModeBox_.setBounds(20, 702, 105, 20);
    analogFilterToggle_.setBounds(135, 700, 135, 24);

    cutoffLabel_.setBounds(20, 734, 75, 13);
    cutoffSlider_.setBounds(20, 748, 75, 76);
    resonanceLabel_.setBounds(102, 734, 75, 13);
    resonanceSlider_.setBounds(102, 748, 75, 76);
    toneLabel_.setBounds(184, 734, 75, 13);
    toneSlider_.setBounds(184, 748, 75, 76);

    // 2. ADSR Envelope
    adsrToggle_.setBounds(275, 700, 140, 24);
    attackLabel_.setBounds(275, 734, 62, 13);
    attackSlider_.setBounds(275, 748, 62, 76);
    decayLabel_.setBounds(342, 734, 62, 13);
    decaySlider_.setBounds(342, 748, 62, 76);
    sustainLabel_.setBounds(409, 734, 62, 13);
    sustainSlider_.setBounds(409, 748, 62, 76);
    releaseLabel_.setBounds(476, 734, 62, 13);
    releaseSlider_.setBounds(476, 748, 62, 76);

    // 3. Arpeggiator & Modulation
    arpToggle_.setBounds(552, 690, 110, 20);
    arpRateLabel_.setBounds(668, 688, 115, 13);
    arpRateBox_.setBounds(668, 702, 115, 20);

    arpPatternLabel_.setBounds(552, 714, 110, 13);
    arpPatternBox_.setBounds(552, 728, 110, 20);

    arpEditorLabel_.setBounds(552, 752, 110, 13);
    arpPatternEditor_.setBounds(552, 768, 110, 24);

    vibratoLabel_.setBounds(668, 734, 60, 13);
    vibratoSlider_.setBounds(668, 748, 60, 76);
    detuneLabel_.setBounds(732, 734, 60, 13);
    detuneSlider_.setBounds(732, 748, 60, 76);

    // 4. Master Output Trim
    masterTrimLabel_.setBounds(840, 700, 160, 14);
    masterTrimSlider_.setBounds(840, 718, 160, 100);
}

