#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

SilkenTofuAudioProcessorEditor::SilkenTofuAudioProcessorEditor(SilkenTofuAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef_(p)
{
    setLookAndFeel(&lnf_);

    // Load Cropped 1970s Vintage Hardware Assets from BinaryData
    faceplateImg_ = juce::ImageCache::getFromMemory(BinaryData::chassis_faceplate_png, BinaryData::chassis_faceplate_pngSize);
    vuMeterImg_   = juce::ImageCache::getFromMemory(BinaryData::vu_meter_vintage_png,  BinaryData::vu_meter_vintage_pngSize);
    t4WindowImg_  = juce::ImageCache::getFromMemory(BinaryData::t4b_xray_window_png,   BinaryData::t4b_xray_window_pngSize);

    // 1. Top Header & Routing ComboBoxes
    const auto& presets = SilkenTofuDSP::getFactoryPresets();
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
        presetBox_.addItem(presets[static_cast<size_t>(i)].name, i + 1);
    presetBox_.setSelectedId(processorRef_.getCurrentProgram() + 1, juce::dontSendNotification);
    presetBox_.onChange = [this]()
    {
        const int idx = presetBox_.getSelectedId() - 1;
        if (idx >= 0)
            processorRef_.setCurrentProgram(idx);
    };
    addAndMakeVisible(presetBox_);

    modeBox_.addItemList({ "Comp (3:1)", "Limit (inf:1)" }, 1);
    addAndMakeVisible(modeBox_);

    crystalBox_.addItemList({ "Vintage CdS (LA-2A)", "CdSe Vactrol (LA-3A)", "Fast Silicon (FET-Opto)" }, 1);
    addAndMakeVisible(crystalBox_);

    crystalLabel_.setText("T4B PHOTOCELL CRYSTAL", juce::dontSendNotification);
    crystalLabel_.setJustificationType(juce::Justification::centred);
    crystalLabel_.setFont(juce::FontOptions(10.2f, juce::Font::bold));
    crystalLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff1e1711));
    addAndMakeVisible(crystalLabel_);

    oversamplingBox_.addItemList({ "1x (Off)", "2x OS", "4x OS", "8x OS" }, 1);
    addAndMakeVisible(oversamplingBox_);

    routingBox_.addItemList({ "Stereo (L/R)", "M/S: Edit MID", "M/S: Edit SIDE" }, 1);
    addAndMakeVisible(routingBox_);

    copyMidToSideBtn_.setColour(juce::TextButton::buttonColourId,  juce::Colour(0xff221d18));
    copyMidToSideBtn_.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffe6c275));
    copyMidToSideBtn_.onClick = [this]()
    {
        processorRef_.copyMidToSide();
        repaint();
    };
    addAndMakeVisible(copyMidToSideBtn_);
    addAndMakeVisible(msSoloBtn_);
    addAndMakeVisible(bypassBtn_);

    // 2. Knobs Setup (High-Contrast Warm 1970s Tungsten/Amber/Phosphor Accents + Units)
    const juce::Colour colGreen(0xff52d669);
    const juce::Colour colAmber(0xffff9d2e);
    const juce::Colour colRed(0xffe8563a);
    const juce::Colour colGold(0xfff0a830);
    const juce::Colour colCyan(0xff4de8ff);

    setupKnob(peakReductionSlider_, peakReductionLabel_, "PEAK REDUCTION", colGreen);
    setupKnob(gainSlider_,          gainLabel_,          "MAKEUP GAIN",    colAmber);
    setupKnob(stereoLinkSlider_,    stereoLinkLabel_,    "STEREO LINK",    colCyan);
    setupKnob(mixSlider_,           mixLabel_,           "DRY / WET MIX",  colGold);
    setupKnob(trimSlider_,          trimLabel_,          "MASTER TRIM",    colGold);

    setupKnob(trapMemorySlider_,    trapMemoryLabel_,    "TRAP MEMORY",    colGreen);
    setupKnob(topologySlider_,      topologyLabel_,      "FB / FF MORPH",  colGreen);
    setupKnob(punchHoldoffSlider_,  punchHoldoffLabel_,  "PUNCH HOLDOFF",  colGreen);
    setupKnob(biasGlowSlider_,      biasGlowLabel_,      "BIAS PRE-WARM",  colAmber);
    setupKnob(cellAgeSlider_,       cellAgeLabel_,       "CELL AGE (50Y)", colAmber);

    setupKnob(tubeDriveSlider_,     tubeDriveLabel_,     "TUBE & IRON",    colRed);
    setupKnob(bPlusSagSlider_,      bPlusSagLabel_,      "B+ RAIL SAG",    colRed);
    setupKnob(millerAirSlider_,     millerAirLabel_,     "MILLER AIR",     colAmber);

    setupKnob(r37Slider_,           r37Label_,           "R37 EMPHASIS",   colGold);
    setupKnob(scHpSlider_,          scHpLabel_,          "SC WARM-CUT",    colGold);

    // 3. Toggle Buttons
    for (auto* btn : { &autoLevelBtn_, &fatCathodeBtn_, &load600Btn_, &extKeyBtn_, &scListenBtn_ })
        addAndMakeVisible(*btn);

    // 4. Global APVTS Attachments + Dynamic Channel Bank Binding (0 = Stereo/MID, 1 = SIDE)
    auto& apvts = processorRef_.getAPVTS();
    osAttach_         = std::make_unique<ComboAttachment>(apvts,  "oversampling",   oversamplingBox_);
    routingAttach_    = std::make_unique<ComboAttachment>(apvts,  "channelRouting", routingBox_);
    stereoLinkAttach_ = std::make_unique<SliderAttachment>(apvts, "stereoLink",     stereoLinkSlider_);
    msSoloAttach_     = std::make_unique<ButtonAttachment>(apvts, "msSolo",         msSoloBtn_);
    bypassAttach_     = std::make_unique<ButtonAttachment>(apvts, "bypass",         bypassBtn_);

    const int initialBank = (processorRef_.getChannelRoutingMode() == 2) ? 1 : 0;
    bindChannelAttachments(initialBank);

    // Add clean unit suffixes to readouts
    gainSlider_.setTextValueSuffix(" dB");
    stereoLinkSlider_.setTextValueSuffix(" %");
    mixSlider_.setTextValueSuffix(" %");
    trimSlider_.setTextValueSuffix(" dB");
    trapMemorySlider_.setTextValueSuffix(" %");
    topologySlider_.setTextValueSuffix(" %");
    punchHoldoffSlider_.setTextValueSuffix(" ms");
    biasGlowSlider_.setTextValueSuffix(" %");
    cellAgeSlider_.setTextValueSuffix(" %");
    tubeDriveSlider_.setTextValueSuffix(" dB");
    bPlusSagSlider_.setTextValueSuffix(" %");
    millerAirSlider_.setTextValueSuffix(" %");
    r37Slider_.setTextValueSuffix(" %");
    scHpSlider_.setTextValueSuffix(" Hz");

    setSize(980, 620);
    startTimerHz(60);
}

SilkenTofuAudioProcessorEditor::~SilkenTofuAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void SilkenTofuAudioProcessorEditor::bindChannelAttachments(int bankIndex)
{
    boundBankIndex_ = std::clamp(bankIndex, 0, 1);
    const juce::String s = (boundBankIndex_ == 1) ? "_side" : "";
    auto& apvts = processorRef_.getAPVTS();

    // Reset existing channel attachments before rebinding to the active bank
    modeAttach_.reset();
    crystalAttach_.reset();
    prAttach_.reset();
    gainAttach_.reset();
    mixAttach_.reset();
    trimAttach_.reset();
    trapAttach_.reset();
    topoAttach_.reset();
    punchAttach_.reset();
    biasAttach_.reset();
    ageAttach_.reset();
    driveAttach_.reset();
    sagAttach_.reset();
    millerAttach_.reset();
    r37Attach_.reset();
    scHpAttach_.reset();
    autoLevelAttach_.reset();
    fatCapAttach_.reset();
    load600Attach_.reset();
    extKeyAttach_.reset();
    scListenAttach_.reset();

    modeAttach_      = std::make_unique<ComboAttachment>(apvts,  "mode"          + s, modeBox_);
    crystalAttach_   = std::make_unique<ComboAttachment>(apvts,  "crystalType"   + s, crystalBox_);

    prAttach_        = std::make_unique<SliderAttachment>(apvts, "peakReduction" + s, peakReductionSlider_);
    gainAttach_      = std::make_unique<SliderAttachment>(apvts, "gainDb"        + s, gainSlider_);
    mixAttach_       = std::make_unique<SliderAttachment>(apvts, "mix"           + s, mixSlider_);
    trimAttach_      = std::make_unique<SliderAttachment>(apvts, "trimDb"        + s, trimSlider_);

    trapAttach_      = std::make_unique<SliderAttachment>(apvts, "trapMemory"    + s, trapMemorySlider_);
    topoAttach_      = std::make_unique<SliderAttachment>(apvts, "topologyMorph" + s, topologySlider_);
    punchAttach_     = std::make_unique<SliderAttachment>(apvts, "punchHoldoff"  + s, punchHoldoffSlider_);
    biasAttach_      = std::make_unique<SliderAttachment>(apvts, "biasGlow"      + s, biasGlowSlider_);
    ageAttach_       = std::make_unique<SliderAttachment>(apvts, "cellAge"       + s, cellAgeSlider_);

    driveAttach_     = std::make_unique<SliderAttachment>(apvts, "tubeIronDrive" + s, tubeDriveSlider_);
    sagAttach_       = std::make_unique<SliderAttachment>(apvts, "bPlusSag"      + s, bPlusSagSlider_);
    millerAttach_    = std::make_unique<SliderAttachment>(apvts, "millerAir"     + s, millerAirSlider_);

    r37Attach_       = std::make_unique<SliderAttachment>(apvts, "r37Emphasis"   + s, r37Slider_);
    scHpAttach_      = std::make_unique<SliderAttachment>(apvts, "scHighPass"    + s, scHpSlider_);

    autoLevelAttach_ = std::make_unique<ButtonAttachment>(apvts, "autoLevel"     + s, autoLevelBtn_);
    fatCapAttach_    = std::make_unique<ButtonAttachment>(apvts, "fatCathodeCap" + s, fatCathodeBtn_);
    load600Attach_   = std::make_unique<ButtonAttachment>(apvts, "load600Ohm"    + s, load600Btn_);
    extKeyAttach_    = std::make_unique<ButtonAttachment>(apvts, "extSidechain"  + s, extKeyBtn_);
    scListenAttach_  = std::make_unique<ButtonAttachment>(apvts, "scListen"      + s, scListenBtn_);

    const int routeMode = processorRef_.getChannelRoutingMode();
    stereoLinkLabel_.setText((routeMode > 0) ? "M/S OPTIC LINK" : "STEREO LINK", juce::dontSendNotification);
    msSoloBtn_.setButtonText((routeMode == 2) ? "Solo Side" : "Solo Mid");
    msSoloBtn_.setEnabled(routeMode > 0);
}

void SilkenTofuAudioProcessorEditor::setupKnob(juce::Slider& slider, juce::Label& label,
                                               const juce::String& text, juce::Colour arcCol)
{
    slider.setLookAndFeel(&lnf_);
    addAndMakeVisible(slider);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setColour(juce::Slider::rotarySliderFillColourId,    arcCol);
    slider.setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colour(0xff181512));
    slider.setColour(juce::Slider::textBoxTextColourId,         juce::Colour(0xffffd27d));
    slider.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colour(0xff6e5d47));
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 68, 17);
    slider.lookAndFeelChanged();

    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xff1e1711));
    addAndMakeVisible(label);
}

void SilkenTofuAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    const auto pt = event.getPosition();

    // 1. Top Header [ A | B | A>B ] Hardware Comparison Switch
    if (abSwitchBounds_.contains(pt))
    {
        const float relX = static_cast<float>(pt.x - abSwitchBounds_.getX()) /
                           static_cast<float>(std::max(1, abSwitchBounds_.getWidth()));
        if (relX < 0.30f)
        {
            processorRef_.setActiveABSlot(0);
        }
        else if (relX < 0.60f)
        {
            processorRef_.setActiveABSlot(1);
        }
        else
        {
            processorRef_.copyActiveSlotToOther();
            abCopyFlashFrames_ = 18;
        }
        repaint(abSwitchBounds_.expanded(4));
        return;
    }

    // 2. VU Meter [ GR | OUT | IN ] Switch or Glass Bezel Click
    if (vuModeSwitchBounds_.contains(pt))
    {
        const float relX = static_cast<float>(pt.x - vuModeSwitchBounds_.getX()) /
                           static_cast<float>(std::max(1, vuModeSwitchBounds_.getWidth()));
        const int clickedMode = std::clamp(static_cast<int>(relX * 3.0f), 0, 2);
        processorRef_.setVuMode(clickedMode);
        repaint(vuMeterBounds_.expanded(4));
        return;
    }
    if (vuMeterBounds_.contains(pt))
    {
        const int nextMode = (processorRef_.getVuMode() + 1) % 3;
        processorRef_.setVuMode(nextMode);
        repaint(vuMeterBounds_.expanded(4));
        return;
    }

    // 3. Card 1 CRT Scope Click: Toggle Live Scrolling GR/N2 History Trace Overlay
    if (curveScopeBounds_.contains(pt))
    {
        showLiveHistoryScope_ = !showLiveHistoryScope_;
        repaint(curveScopeBounds_.expanded(2));
        return;
    }

    AudioProcessorEditor::mouseDown(event);
}

void SilkenTofuAudioProcessorEditor::timerCallback()
{
    if (abCopyFlashFrames_ > 0)
    {
        --abCopyFlashFrames_;
        repaint(abSwitchBounds_.expanded(2));
    }

    const int expectedPresetId = processorRef_.getCurrentProgram() + 1;
    if (presetBox_.getSelectedId() != expectedPresetId)
        presetBox_.setSelectedId(expectedPresetId, juce::dontSendNotification);

    const int routeMode = processorRef_.getChannelRoutingMode();
    const int desiredBank = (routeMode == 2) ? 1 : 0;
    if (desiredBank != boundBankIndex_)
    {
        bindChannelAttachments(desiredBank);
        repaint();
    }
    else
    {
        stereoLinkLabel_.setText((routeMode > 0) ? "M/S OPTIC LINK" : "STEREO LINK", juce::dontSendNotification);
        msSoloBtn_.setButtonText((routeMode == 2) ? "Solo Side" : "Solo Mid");
        msSoloBtn_.setEnabled(routeMode > 0);
    }

    // Smooth both channels (0 = Left/Mid, 1 = Right/Side) for Dual-Needle VU Metering
    for (int ch = 0; ch < 2; ++ch)
    {
        const float tGr    = processorRef_.getGainReductionDb(ch);
        const float tInVu  = std::clamp(processorRef_.getInputPeakDb(ch)  + 18.0f, -20.0f, 3.5f);
        const float tOutVu = std::clamp(processorRef_.getOutputPeakDb(ch) + 18.0f, -20.0f, 3.5f);
        smoothGrDbCh_[ch]  += 0.28f * (tGr    - smoothGrDbCh_[ch]);
        smoothInVuCh_[ch]  += 0.25f * (tInVu  - smoothInVuCh_[ch]);
        smoothOutVuCh_[ch] += 0.25f * (tOutVu - smoothOutVuCh_[ch]);
    }

    const int activeCh      = desiredBank;
    const float targetLum   = processorRef_.getElLuminescence(activeCh);
    const float targetN1    = processorRef_.getShallowTrapN1(activeCh);
    const float targetN2    = processorRef_.getDeepTrapN2(activeCh);
    const float targetBPlus = processorRef_.getBPlusVolts(activeCh);

    smoothGrDb_  = smoothGrDbCh_[activeCh];
    smoothInVu_  = smoothInVuCh_[activeCh];
    smoothOutVu_ = smoothOutVuCh_[activeCh];
    smoothLum_   += 0.30f * (targetLum   - smoothLum_);
    smoothN1_    += 0.28f * (targetN1    - smoothN1_);
    smoothN2_    += 0.22f * (targetN2    - smoothN2_);
    smoothBPlus_ += 0.25f * (targetBPlus - smoothBPlus_);

    // Push latest Gain Reduction magnitude (0..20 dB) & N2 Deep-Trap state into scrolling CRT buffer
    grHistory_[static_cast<size_t>(historyWriteIdx_)] = std::clamp(-smoothGrDb_, 0.0f, 20.0f) / 20.0f;
    n2History_[static_cast<size_t>(historyWriteIdx_)] = std::clamp(smoothN2_ * 1.45f, 0.0f, 1.0f);
    historyWriteIdx_ = (historyWriteIdx_ + 1) % kScopeHistoryLen;

    updateHoverReadout();

    repaint(vuMeterBounds_.expanded(4));
    repaint(t4CanisterBounds_.expanded(4));
    repaint(curveScopeBounds_.expanded(2));
    repaint(tubeScopeBounds_.expanded(2));
    repaint(infoStripBounds_.expanded(2));
}

void SilkenTofuAudioProcessorEditor::updateHoverReadout()
{
    const auto setInfo = [this](const juce::String& cat, const juce::String& title,
                                const juce::String& detail, juce::Colour accent)
    {
        infoCategory_ = cat;
        infoTitle_    = title;
        infoDetail_   = detail;
        infoAccent_   = accent;
    };

    const juce::Colour colGreen(0xff7aff89);
    const juce::Colour colAmber(0xffffb84d);
    const juce::Colour colRed(0xffff7a59);
    const juce::Colour colBrass(0xffe6c275);
    const juce::Colour colCyan(0xff4de8ff);

    const int routeMode = processorRef_.getChannelRoutingMode();
    const juce::String chPrefix = (routeMode == 1) ? "[MID] " : (routeMode == 2) ? "[SIDE] " : "";

    auto active = [](const juce::Component& c, const juce::Label* lbl = nullptr)
    {
        return c.isMouseOverOrDragging(true) || (lbl != nullptr && lbl->isMouseOver(true));
    };

    // 1. Front Panel Knobs, Stereo Link, Routing, Solo M/S & Auto-Level
    if (active(peakReductionSlider_, &peakReductionLabel_))
    {
        setInfo("FRONT PANEL",
                chPrefix + "PEAK REDUCTION [" + juce::String(peakReductionSlider_.getValue(), 1) + "]  -  Sidechain Drive into T4B Electroluminescent Panel",
                "LEFT (0): No attenuation   |   RIGHT (60-100): Exponential 100k pot throw for deep -20 dB+ optical compression",
                colGreen);
        return;
    }
    if (active(gainSlider_, &gainLabel_))
    {
        setInfo("FRONT PANEL",
                chPrefix + "MAKEUP GAIN [" + juce::String(gainSlider_.getValue(), 1) + " dB]  -  12AX7A / 12BH7A Output Line Amplifier Gain",
                "LEFT (-12 dB): Attenuates hot signals   |   RIGHT (+24 dB): Boosts post-T4B output level",
                colAmber);
        return;
    }
    if (active(stereoLinkSlider_, &stereoLinkLabel_))
    {
        setInfo("STEREO & M/S",
                "STEREO / M-S OPTICAL LINK [" + juce::String(stereoLinkSlider_.getValue(), 1) + " %]  -  Dual T4B Cell Gain-Reduction Coupling",
                "LEFT (0%): Unlinked Dual Mono or Independent Mid/Side   |   80%: 1965 Pin-6 Link   |   RIGHT (100%): Locked Stereo/M-S GR",
                colCyan);
        return;
    }
    if (active(routingBox_))
    {
        setInfo("STEREO & M/S",
                "CHANNEL ROUTING & BANK [" + routingBox_.getText() + "]  -  Stereo L/R vs. Independent Mid-Side Processing",
                "Stereo (L/R): Linked controls   |   M/S Edit MID: Knobs & meters sculpt Center/Sum   |   M/S Edit SIDE: Knobs & meters sculpt Stereo Width",
                colCyan);
        return;
    }
    if (active(msSoloBtn_))
    {
        const juce::String state = msSoloBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("STEREO & M/S",
                "SOLO ACTIVE M/S CHANNEL [" + state + "]  -  Audition Mid (Center) or Side (Stereo Difference) in Isolation",
                "Requires M/S Mode: Solos the currently selected MID or SIDE channel so you can dial in compression & saturation by ear",
                colCyan);
        return;
    }
    if (active(copyMidToSideBtn_))
    {
        setInfo("STEREO & M/S",
                "COPY MID -> SIDE  -  Clone All 17 MID Channel Parameters into the SIDE Channel Bank",
                "Click to copy your current MID compressor, tube, and sidechain settings into the SIDE channel as a balanced starting point",
                colCyan);
        return;
    }
    if (active(bypassBtn_))
    {
        const juce::String state = bypassBtn_.getToggleState() ? "BYPASSED" : "ACTIVE";
        setInfo("TOP BAR",
                "ZERO-LATENCY-JUMP SOFT BYPASS [" + state + "]  -  Phase-Coherent Oversampled Dry/Wet Crossfade",
                "Crossfades inside the polyphase oversampler delay line so DAW Plugin Delay Compensation (PDC) never glitches or clicks",
                colAmber);
        return;
    }
    if (active(mixSlider_, &mixLabel_))
    {
        setInfo("FRONT PANEL",
                chPrefix + "DRY / WET MIX [" + juce::String(mixSlider_.getValue(), 1) + " %]  -  Phase-Locked Parallel Optical Compression Blend",
                "LEFT (0%): 100% uncompressed dry input   |   RIGHT (100%): 100% compressed T4B + tube wet signal (0.0 deg phase lock)",
                colBrass);
        return;
    }
    if (active(trimSlider_, &trimLabel_))
    {
        setInfo("FRONT PANEL",
                chPrefix + "MASTER TRIM [" + juce::String(trimSlider_.getValue(), 1) + " dB]  -  Clean Post-Transformer Output Fader (Unity = 0.0 dB)",
                "LEFT (-12 dB): Clean headroom trim   |   RIGHT (+12 dB): Clean output boost without altering tube saturation",
                colBrass);
        return;
    }
    if (active(autoLevelBtn_))
    {
        const juce::String state = autoLevelBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("FRONT PANEL",
                chPrefix + "AUTO-LEVEL LINK [" + state + "]  -  Automatic Loudness-Compensated Makeup Gain",
                "OFF: Manual Makeup Gain only   |   ON: Automatically makes up volume lost as you raise Peak Reduction",
                colAmber);
        return;
    }

    // 2. Card 1: T4-Bean Semiconductor & Electro-Optical Lab
    if (active(trapMemorySlider_, &trapMemoryLabel_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "TRAP MEMORY [" + juce::String(trapMemorySlider_.getValue(), 1) + " %]  -  Deep-Trap (N2) Multi-Second Optical Release Tail",
                "LEFT (-50%): Reverse optical bloom swell   |   0%: Fast single-stage release   |   RIGHT (200%): Classic 15s LA-2A hold",
                colGreen);
        return;
    }
    if (active(topologySlider_, &topologyLabel_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "FB / FF MORPH [" + juce::String(topologySlider_.getValue(), 1) + " %]  -  Sidechain Tap Topology (Feedback to Feed-Forward)",
                "LEFT (0%): Classic LA-2A Feedback (smooth ~3:1)   |   100%: Hard Feed-Forward   |   RIGHT (125%): Over-compression ducking",
                colGreen);
        return;
    }
    if (active(punchHoldoffSlider_, &punchHoldoffLabel_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "PUNCH HOLDOFF [" + juce::String(punchHoldoffSlider_.getValue(), 1) + " ms]  -  Pre-EL Optical Attack Transient Delay",
                "LEFT (0 ms): Clamps transients immediately   |   RIGHT (30 ms): Lets drum & vocal attacks punch through before T4B reacts",
                colGreen);
        return;
    }
    if (active(biasGlowSlider_, &biasGlowLabel_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "BIAS PRE-WARM [" + juce::String(biasGlowSlider_.getValue(), 1) + " %]  -  Continuous Sub-Threshold EL Panel Bias Voltage",
                "LEFT (0%): Classic 19V Destriau dead-zone knee   |   RIGHT (100%): Pre-glows EL panel for instant zero-threshold compression",
                colAmber);
        return;
    }
    if (active(cellAgeSlider_, &cellAgeLabel_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "CELL AGE (50Y) [" + juce::String(cellAgeSlider_.getValue(), 1) + " %]  -  50-Year CdS Crystal Lattice Fatigue & Drift",
                "LEFT (0%): 1965 factory-fresh T4B wafer   |   RIGHT (100%): Aged 1970s cell with warmer knee & slower dark recovery",
                colAmber);
        return;
    }

    // 3. Card 2: Oversampled 12AX7A / 12BH7A & UTC Iron
    if (active(tubeDriveSlider_, &tubeDriveLabel_))
    {
        setInfo("TUBE & IRON",
                chPrefix + "TUBE & IRON DRIVE [" + juce::String(tubeDriveSlider_.getValue(), 1) + " dB]  -  12AX7A Triode + 12BH7A WCF + UTC Iron Saturation",
                "LEFT (-6 dB): Ultra-clean linear headroom   |   RIGHT (+18 dB): Rich 2nd/3rd harmonic tube warmth & closed-loop sidechain bloom",
                colRed);
        return;
    }
    if (active(bPlusSagSlider_, &bPlusSagLabel_))
    {
        setInfo("TUBE & IRON",
                chPrefix + "B+ RAIL SAG [" + juce::String(bPlusSagSlider_.getValue(), 1) + " %]  -  275V High-Voltage Power Supply Droop (R29 / C7A Cap)",
                "LEFT (0%): Stiff regulated 275V supply   |   RIGHT (100%): Heavy compression droops B+ voltage & softens 6AQ5 EL drive",
                colRed);
        return;
    }
    if (active(millerAirSlider_, &millerAirLabel_))
    {
        setInfo("TUBE & IRON",
                chPrefix + "MILLER AIR [" + juce::String(millerAirSlider_.getValue(), 1) + " %]  -  Gain-Dependent 12AX7A Miller Capacitance HF Tilt",
                "LEFT (-100%): Vintage high-frequency rolloff under heavy GR   |   RIGHT (+100%): Restores & boosts silky top-end air",
                colAmber);
        return;
    }
    if (active(fatCathodeBtn_))
    {
        const juce::String state = fatCathodeBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("TUBE & IRON",
                chPrefix + "FAT Ck (12AX7 H2) [" + state + "]  -  V1 Cathode Bypass Capacitor Asymmetry Mod",
                "OFF: Standard vintage cathode bias   |   ON: Increases grid-conduction bias shift for thicker even-order (2nd) harmonics",
                colRed);
        return;
    }
    if (active(load600Btn_))
    {
        const juce::String state = load600Btn_.getToggleState() ? "ON" : "OFF";
        setInfo("TUBE & IRON",
                chPrefix + "600 OHM LOAD [" + state + "]  -  UTC A-24 Output Transformer Termination Load",
                "OFF: Modern 10k bridging load (open headroom)   |   ON: Vintage 600-Ohm line termination (earlier asymmetrical clipping)",
                colRed);
        return;
    }

    // 4. Card 3: R37 Sidechain & Ducking
    if (active(r37Slider_, &r37Label_))
    {
        setInfo("SIDECHAIN",
                chPrefix + "R37 EMPHASIS [" + juce::String(r37Slider_.getValue(), 1) + " %]  -  Back-Panel R37 Sidechain High-Frequency Pre-Emphasis",
                "LEFT (0%): Flat broadband sidechain   |   RIGHT (100%): Boosts highs into sidechain so sibilance & harshness duck harder",
                colBrass);
        return;
    }
    if (active(scHpSlider_, &scHpLabel_))
    {
        setInfo("SIDECHAIN",
                chPrefix + "SC WARM-CUT [" + juce::String(scHpSlider_.getValue(), 0) + " Hz]  -  Sidechain Bass High-Pass Filter (20 Hz - 350 Hz)",
                "LEFT (20 Hz): Full sub-bass triggers compression   |   RIGHT (350 Hz): Ignores kick & bass so low end stays uncompressed",
                colBrass);
        return;
    }
    if (active(extKeyBtn_))
    {
        const juce::String state = extKeyBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("SIDECHAIN",
                chPrefix + "EXT KEY DUCKING [" + state + "]  -  External Sidechain Input Optical Keying",
                "OFF: Compresses using main input   |   ON: Uses DAW Aux/Sidechain bus to trigger T4B optical ducking",
                colBrass);
        return;
    }
    if (active(scListenBtn_))
    {
        const juce::String state = scListenBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("SIDECHAIN",
                chPrefix + "SC LISTEN (EL) [" + state + "]  -  Sidechain Filter & Emphasis Audition Monitor",
                "OFF: Normal main audio output   |   ON: Solos the filtered R37 / Warm-Cut sidechain signal driving the T4B panel",
                colBrass);
        return;
    }

    // 5. Top Bar Dropdowns & Crystal Box
    if (active(presetBox_))
    {
        setInfo("TOP BAR",
                "FACTORY PRESET [" + presetBox_.getText() + "]  -  Calibrated 0.0 dB Unity-Trim Studio Setups",
                "Selects between 9 calibrated studio presets including Mid-Side Mastering Sculptor, Vocal Silk, 600-Ohm Bass, and Drum Bus",
                colAmber);
        return;
    }
    if (active(modeBox_))
    {
        setInfo("TOP BAR",
                chPrefix + "COMPRESS / LIMIT MODE [" + modeBox_.getText() + "]  -  Front-Panel Ratio & Threshold Switch",
                "Compress (~3:1): Gentle musical optical knee   |   Limit (~inf:1): Lowers EL threshold for +4 to +8 dB deeper clamping",
                colAmber);
        return;
    }
    if (active(crystalBox_))
    {
        setInfo("T4-BEAN LAB",
                chPrefix + "PHOTOCELL CRYSTAL [" + crystalBox_.getText() + "]  -  T4B Semiconductor Wafer Chemistry",
                "CdS (LA-2A): Silky 40ms release   |   CdSe (LA-3A): Punchy 18ms snap   |   Silicon (FET-Opto): Ultra-fast 4ms transient grab",
                colGreen);
        return;
    }
    if (active(oversamplingBox_))
    {
        setInfo("TOP BAR",
                "POLYPHASE OVERSAMPLING [" + oversamplingBox_.getText() + "]  -  Anti-Aliasing Filter for Tube & Iron Harmonics",
                "1x: Zero-latency CPU saver   |   2x / 4x / 8x: Eliminates harmonic foldback aliasing when driving Tube & Iron hard",
                colAmber);
        return;
    }

    // 6. Custom Visual Displays (A/B Switch, VU Meter, X-Ray Window, Two-Trap Scope)
    const auto mousePt = getMouseXYRelative();
    if (abSwitchBounds_.contains(mousePt))
    {
        const int slot = processorRef_.getActiveABSlot();
        const juce::String slotName = (slot == 0) ? "SLOT A" : "SLOT B";
        setInfo("A / B COMPARE",
                "A / B STATE COMPARISON [" + slotName + " ACTIVE]  -  Instant Parameter Snapshot Compare",
                "Click [A] or [B] to switch between two full plugin states (including Mid & Side banks); click [A>B] / [B>A] to clone",
                colAmber);
        return;
    }
    if (vuMeterBounds_.contains(mousePt))
    {
        const int vuMode = processorRef_.getVuMode();
        const juce::String modeName = (vuMode == 0) ? "GAIN REDUCTION (GR)"
                                    : (vuMode == 1) ? "OUTPUT VU (0 VU = -18 dBFS)"
                                                    : "INPUT VU (0 VU = -18 dBFS)";
        setInfo("VU METER",
                chPrefix + "DUAL-NEEDLE ILLUMINATED VU METER [" + modeName + "]  -  Click [GR | OUT | IN] to Switch Mode",
                "Solid needle tracks active channel (Mid/L or Side/R); translucent ghost needle tracks companion channel when M/S or Link < 99%",
                colAmber);
        return;
    }
    if (t4CanisterBounds_.contains(mousePt))
    {
        setInfo("TELEMETRY",
                chPrefix + "T4-BEAN & TUBE X-RAY WINDOW  -  Live Semiconductor Traps & 275V B+ Supply Telemetry",
                "N1 TRAP: Fast shallow carrier density   |   N2 MEMORY: Slow multi-second trap charge   |   B+ RAIL: Live tube plate supply voltage",
                colGreen);
        return;
    }
    if (curveScopeBounds_.contains(mousePt))
    {
        setInfo("TELEMETRY",
                chPrefix + "TWO-TRAP CRT SCOPE  -  Click Scope to Toggle Live Scrolling GR + N2 History Trace",
                "Displays theoretical Two-Trap release trajectory + real-time scrolling Gain Reduction (solid) and Deep-Trap N2 memory (amber)",
                colGreen);
        return;
    }
}

void SilkenTofuAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const int routeMode = processorRef_.getChannelRoutingMode();
    const int vuMode    = processorRef_.getVuMode();

    // 1. Draw Photorealistic 1970s Worn Brushed Aluminum Rack Faceplate
    if (faceplateImg_.isValid())
    {
        g.drawImage(faceplateImg_, bounds, juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        juce::ColourGradient chassisGrad(juce::Colour(0xffdfd5c3), 0.0f, 0.0f,
                                         juce::Colour(0xffbfb29c), 0.0f, bounds.getHeight(), false);
        g.setGradientFill(chassisGrad);
        g.fillRect(bounds);
    }

    // 2. Top 1970s Wrinkle-Finish Black Enamel Header Bar with Worn Brass Trim (inset inside x = 54..926)
    auto headerRect = juce::Rectangle<float>(54.0f, 6.0f, bounds.getWidth() - 108.0f, 46.0f);
    g.setColour(juce::Colours::black.withAlpha(0.45f));
    g.fillRoundedRectangle(headerRect.translated(0.0f, 2.5f), 5.0f);

    juce::ColourGradient headerGrad(juce::Colour(0xff231f1c), headerRect.getX(), headerRect.getY(),
                                    juce::Colour(0xff14110f), headerRect.getX(), headerRect.getBottom(), false);
    g.setGradientFill(headerGrad);
    g.fillRoundedRectangle(headerRect, 5.0f);

    // Riveted Brass Nameplate on Left of Header (x = 62..376)
    auto badgeRect = juce::Rectangle<float>(62.0f, 11.0f, 314.0f, 36.0f);
    juce::ColourGradient brassGrad(juce::Colour(0xffd2b173), badgeRect.getX(), badgeRect.getY(),
                                   juce::Colour(0xff94733b), badgeRect.getRight(), badgeRect.getBottom(), false);
    g.setGradientFill(brassGrad);
    g.fillRoundedRectangle(badgeRect, 3.0f);
    g.setColour(juce::Colour(0xff2e2210));
    g.drawRoundedRectangle(badgeRect, 3.0f, 1.4f);

    for (auto rPt : { juce::Point<float>(badgeRect.getX() + 5.0f, badgeRect.getY() + 5.0f),
                      juce::Point<float>(badgeRect.getRight() - 5.0f, badgeRect.getY() + 5.0f),
                      juce::Point<float>(badgeRect.getX() + 5.0f, badgeRect.getBottom() - 5.0f),
                      juce::Point<float>(badgeRect.getRight() - 5.0f, badgeRect.getBottom() - 5.0f) })
    {
        g.setColour(juce::Colour(0xff382a14));
        g.fillEllipse(rPt.x - 2.0f, rPt.y - 2.0f, 4.0f, 4.0f);
    }

    g.setColour(juce::Colour(0xff171108));
    g.setFont(juce::FontOptions(16.8f, juce::Font::bold));
    g.drawText("SILKEN TOFU   LA-2A / T4B", 71, 12, 298, 20, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff362813));
    g.setFont(juce::FontOptions(8.7f, juce::Font::bold));
    g.drawText("SER. NO. 1974-0420  |  ELECTRO-OPTICAL LEVELER", 71, 29, 298, 14, juce::Justification::centredLeft);

    // 2B. Top Header [ A | B | A>B ] Hardware Comparison Switch (x = 565..651)
    if (!abSwitchBounds_.isEmpty())
    {
        auto ab = abSwitchBounds_.toFloat().reduced(0.5f, 1.0f);
        g.setColour(juce::Colour(0xff1b1815));
        g.fillRoundedRectangle(ab, 4.0f);
        g.setColour(juce::Colour(0xff8c7a62));
        g.drawRoundedRectangle(ab, 4.0f, 1.1f);

        const int activeSlot = processorRef_.getActiveABSlot();
        const float wA = ab.getWidth() * 0.29f;
        const float wB = ab.getWidth() * 0.29f;
        const float wC = ab.getWidth() - wA - wB;

        auto rectA = juce::Rectangle<float>(ab.getX(),          ab.getY(), wA, ab.getHeight());
        auto rectB = juce::Rectangle<float>(ab.getX() + wA,     ab.getY(), wB, ab.getHeight());
        auto rectC = juce::Rectangle<float>(ab.getX() + wA + wB, ab.getY(), wC, ab.getHeight());

        auto drawSeg = [&](juce::Rectangle<float> r, const juce::String& txt, bool highlighted, juce::Colour hiCol)
        {
            if (highlighted)
            {
                g.setColour(hiCol.withAlpha(0.26f));
                g.fillRoundedRectangle(r.reduced(2.0f), 2.5f);
                g.setColour(hiCol);
            }
            else
            {
                g.setColour(juce::Colour(0xffbfb29c));
            }
            g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
            g.drawText(txt, r.toNearestInt(), juce::Justification::centred);
        };

        drawSeg(rectA, "A", activeSlot == 0, juce::Colour(0xffffd27d));
        drawSeg(rectB, "B", activeSlot == 1, juce::Colour(0xff4de8ff));
        const juce::String copyTxt = (abCopyFlashFrames_ > 0) ? "OK!" : (activeSlot == 0 ? "A>B" : "B>A");
        drawSeg(rectC, copyTxt, abCopyFlashFrames_ > 0, juce::Colour(0xff7aff89));

        g.setColour(juce::Colour(0x558c7a62));
        g.drawVerticalLine(static_cast<int>(rectB.getX()), ab.getY() + 3.0f, ab.getBottom() - 3.0f);
        g.drawVerticalLine(static_cast<int>(rectC.getX()), ab.getY() + 3.0f, ab.getBottom() - 3.0f);
    }

    // 3. Draw Tightly Cropped 1970s Bakelite VU Meter + Dual-Channel Needles + Clickable [GR | OUT | IN] Switch
    if (!vuMeterBounds_.isEmpty())
    {
        auto vu = vuMeterBounds_.toFloat();
        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.fillRoundedRectangle(vu.translated(2.0f, 4.0f), 10.0f);

        if (vuMeterImg_.isValid())
        {
            juce::Graphics::ScopedSaveState clipState(g);
            juce::Path bezelClip;
            bezelClip.addRoundedRectangle(vu, 12.0f);
            g.reduceClipRegion(bezelClip);

            const int sw = vuMeterImg_.getWidth();
            const int sh = vuMeterImg_.getHeight();
            g.drawImage(vuMeterImg_,
                        static_cast<int>(vu.getX()), static_cast<int>(vu.getY()),
                        static_cast<int>(vu.getWidth()), static_cast<int>(vu.getHeight()),
                        22, 18, sw - 44, sh - 36);
        }
        g.setColour(juce::Colour(0xff141210));
        g.drawRoundedRectangle(vu, 12.0f, 2.4f);

        auto computeChannelNeedleNorm = [&](int ch) -> float
        {
            if (vuMode == 0)
            {
                const float grClamped = std::clamp(smoothGrDbCh_[ch], -20.0f, 0.0f);
                return std::pow((grClamped + 20.0f) / 20.0f, 1.38f);
            }
            const float vuVal = (vuMode == 1) ? smoothOutVuCh_[ch] : smoothInVuCh_[ch];
            const float vuClamped = std::clamp(vuVal, -20.0f, 3.0f);
            if (vuClamped <= 0.0f)
                return std::pow((vuClamped + 20.0f) / 20.0f, 1.38f);
            return 1.0f + (vuClamped / 3.0f) * 0.22f;
        };

        const int primaryCh   = (routeMode == 2) ? 1 : 0;
        const int secondaryCh = 1 - primaryCh;
        const float primaryNorm   = computeChannelNeedleNorm(primaryCh);
        const float secondaryNorm = computeChannelNeedleNorm(secondaryCh);
        const bool showDualNeedle = (routeMode > 0) || (stereoLinkSlider_.getValue() < 99.0);

        juce::String readoutStr;
        if (vuMode == 0)
        {
            readoutStr = juce::String(smoothGrDbCh_[primaryCh], 1) + " dB";
        }
        else
        {
            const float vuVal = (vuMode == 1) ? smoothOutVuCh_[primaryCh] : smoothInVuCh_[primaryCh];
            const juce::String signPrefix = (vuVal >= 0.0f) ? "+" : "";
            readoutStr = signPrefix + juce::String(vuVal, 1) + " VU";
        }

        auto glassWindow = juce::Rectangle<float>(vu.getX() + vu.getWidth() * 0.10f,
                                                  vu.getY() + vu.getHeight() * 0.14f,
                                                  vu.getWidth() * 0.80f,
                                                  vu.getHeight() * 0.645f);
        {
            juce::Graphics::ScopedSaveState state(g);
            g.reduceClipRegion(glassWindow.toNearestInt());

            const auto pivot = juce::Point<float>(vu.getCentreX(), vu.getBottom() + vu.getHeight() * 0.12f);
            const float needleLen = vu.getHeight() * 0.88f;

            // Optional Companion Channel Ghost Needle (Cyan for Side/Right, Warm Gold for Mid/Left)
            if (showDualNeedle)
            {
                const float ghostAng = -0.72f + secondaryNorm * 0.955f;
                const float gx = pivot.x + std::sin(ghostAng) * needleLen;
                const float gy = pivot.y - std::cos(ghostAng) * needleLen;
                const juce::Colour ghostCol = (secondaryCh == 1) ? juce::Colour(0xb824b8d6)
                                                                 : juce::Colour(0xb8e09b2d);
                g.setColour(ghostCol);
                g.drawLine(pivot.x, pivot.y, gx, gy, 1.7f);

                juce::Path ghostSpade;
                ghostSpade.addTriangle(0.0f, -needleLen * 0.95f, -2.6f, -needleLen * 0.84f, 2.6f, -needleLen * 0.84f);
                g.fillPath(ghostSpade, juce::AffineTransform::rotation(ghostAng).translated(pivot.x, pivot.y));
            }

            // Primary Channel Physical Needle
            const float needleAng = -0.72f + primaryNorm * 0.955f;
            const float nx = pivot.x + std::sin(needleAng) * needleLen;
            const float ny = pivot.y - std::cos(needleAng) * needleLen;

            g.setColour(juce::Colours::black.withAlpha(0.18f));
            g.drawLine(pivot.x + 2.0f, pivot.y + 1.5f, nx + 2.2f, ny + 1.8f, 2.2f);

            g.setColour(juce::Colour(0xff181411));
            g.drawLine(pivot.x, pivot.y, nx, ny, 2.1f);

            juce::Path spade;
            spade.addTriangle(0.0f, -needleLen * 0.96f, -3.2f, -needleLen * 0.84f, 3.2f, -needleLen * 0.84f);
            g.fillPath(spade, juce::AffineTransform::rotation(needleAng).translated(pivot.x, pivot.y));
        }

        // Bottom-Left Clickable 3-Way VU Mode Selector Pill: [ GR | OUT | IN ]
        auto swRect = vuModeSwitchBounds_.toFloat();
        g.setColour(juce::Colour(0xe614110e));
        g.fillRoundedRectangle(swRect, 3.0f);
        g.setColour(juce::Colour(0xff8c7248));
        g.drawRoundedRectangle(swRect, 3.0f, 1.0f);

        const char* modeLabels[3] = { "GR", "OUT", "IN" };
        const float segW = swRect.getWidth() / 3.0f;
        for (int m = 0; m < 3; ++m)
        {
            auto seg = juce::Rectangle<float>(swRect.getX() + static_cast<float>(m) * segW, swRect.getY(), segW, swRect.getHeight());
            if (m == vuMode)
            {
                g.setColour(juce::Colour(0xffffb84d).withAlpha(0.28f));
                g.fillRoundedRectangle(seg.reduced(1.5f), 2.0f);
                g.setColour(juce::Colour(0xffffd27d));
            }
            else
            {
                g.setColour(juce::Colour(0xff9e8f7a));
            }
            g.setFont(juce::FontOptions(9.2f, juce::Font::bold));
            g.drawText(modeLabels[m], seg.toNearestInt(), juce::Justification::centred);
        }

        // If Mid/Side mode is active, show active channel badge [MID] or [SIDE] (+ SOLO status) at top-center of VU bezel
        if (routeMode > 0)
        {
            const bool isSide = (routeMode == 2);
            const bool isSolo = msSoloBtn_.getToggleState();
            const juce::Colour badgeCol = isSide ? juce::Colour(0xff4de8ff) : juce::Colour(0xffffb84d);
            auto msPill = juce::Rectangle<float>(vu.getCentreX() - 52.0f, vu.getY() + 5.0f, 104.0f, 14.0f);
            g.setColour(juce::Colour(0xd914110e));
            g.fillRoundedRectangle(msPill, 3.0f);
            g.setColour(badgeCol.withAlpha(0.80f));
            g.drawRoundedRectangle(msPill, 3.0f, 1.0f);
            g.setColour(badgeCol);
            g.setFont(juce::FontOptions(8.6f, juce::Font::bold));
            const juce::String badgeTxt = isSide ? (isSolo ? "SIDE [SOLO]" : "SIDE + MID GHOST")
                                                 : (isSolo ? "MID [SOLO]"  : "MID + SIDE GHOST");
            g.drawText(badgeTxt, msPill.toNearestInt(), juce::Justification::centred);
        }

        // Recessed Amber Readout Pill at Bottom-Right of Bezel
        auto pill = juce::Rectangle<float>(vu.getRight() - 80.0f, vu.getBottom() - 21.0f, 70.0f, 16.0f);
        g.setColour(juce::Colour(0xe614110e));
        g.fillRoundedRectangle(pill, 3.0f);
        g.setColour(juce::Colour(0xff8c7248));
        g.drawRoundedRectangle(pill, 3.0f, 1.0f);
        g.setColour(juce::Colour(0xffffb84d));
        g.setFont(juce::FontOptions(10.2f, juce::Font::bold));
        g.drawText(readoutStr, pill.toNearestInt(), juce::Justification::centred);
    }

    // 4. Draw Tightly Cropped 1970s "T4-Bean" & 12AX7/12BH7A Tube Inspection Window + Dynamic Glows
    if (!t4CanisterBounds_.isEmpty())
    {
        auto can = t4CanisterBounds_.toFloat();
        g.setColour(juce::Colours::black.withAlpha(0.55f));
        g.fillRoundedRectangle(can.translated(2.0f, 4.0f), 10.0f);

        if (t4WindowImg_.isValid())
        {
            juce::Graphics::ScopedSaveState clipState(g);
            juce::Path canClip;
            canClip.addRoundedRectangle(can, 10.0f);
            g.reduceClipRegion(canClip);

            const int sw = t4WindowImg_.getWidth();
            const int sh = t4WindowImg_.getHeight();
            g.drawImage(t4WindowImg_,
                        static_cast<int>(can.getX()), static_cast<int>(can.getY()),
                        static_cast<int>(can.getWidth()), static_cast<int>(can.getHeight()),
                        6, 6, sw - 12, sh - 12);
        }
        g.setColour(juce::Colour(0xff141210));
        g.drawRoundedRectangle(can, 10.0f, 2.2f);

        // A. Left & Center Vacuum Tubes (12AX7 & 12BH7A) Filament Glow reacts to B+ Rail Voltage & Tube Drive
        const float driveNorm = std::clamp((static_cast<float>(tubeDriveSlider_.getValue()) + 6.0f) / 24.0f, 0.0f, 1.0f);
        const float bPlusNorm = std::clamp((smoothBPlus_ - 215.0f) / 60.0f, 0.25f, 1.1f);
        const float tubeAlpha = std::clamp((0.22f + 0.55f * driveNorm) * bPlusNorm, 0.10f, 0.85f);

        const juce::Point<float> tube1Pos(can.getX() + can.getWidth() * 0.24f, can.getY() + can.getHeight() * 0.56f);
        const juce::Point<float> tube2Pos(can.getX() + can.getWidth() * 0.43f, can.getY() + can.getHeight() * 0.56f);
        for (auto tPt : { tube1Pos, tube2Pos })
        {
            juce::ColourGradient filamentGlow(juce::Colour(0xffff9e3b).withAlpha(tubeAlpha), tPt.x, tPt.y,
                                              juce::Colour(0x00ff4500), tPt.x + 36.0f, tPt.y + 36.0f, true);
            g.setGradientFill(filamentGlow);
            g.fillEllipse(tPt.x - 36.0f, tPt.y - 36.0f, 72.0f, 72.0f);
        }

        // B. Right "T4-BEAN" Canister Phosphor Seam Glow reacts to live Destriau Luminescence L(t)
        const int crystalChoice = crystalBox_.getSelectedId() - 1;
        juce::Colour phosphorCol = (crystalChoice == 0) ? juce::Colour(0xff7aff89)
                                 : (crystalChoice == 1) ? juce::Colour(0xffffb84d)
                                                        : juce::Colour(0xff4de8ff);
        const float elAlpha = std::clamp(smoothLum_ * 0.85f, 0.05f, 0.90f);
        const juce::Point<float> t4Pos(can.getX() + can.getWidth() * 0.67f, can.getY() + can.getHeight() * 0.50f);
        juce::ColourGradient t4Glow(phosphorCol.withAlpha(elAlpha), t4Pos.x, t4Pos.y,
                                    phosphorCol.withAlpha(0.0f), t4Pos.x + 56.0f, t4Pos.y + 56.0f, true);
        g.setGradientFill(t4Glow);
        g.fillEllipse(t4Pos.x - 56.0f, t4Pos.y - 56.0f, 112.0f, 112.0f);

        // C. Bottom Telemetry Strip Inside Window (Integer B+ Volts, zero decimal clutter!)
        auto strip = juce::Rectangle<float>(can.getX() + 12.0f, can.getBottom() - 22.0f, can.getWidth() - 24.0f, 17.0f);
        g.setColour(juce::Colour(0xe6101214));
        g.fillRoundedRectangle(strip, 3.0f);
        g.setColour(juce::Colour(0xff5e6670));
        g.drawRoundedRectangle(strip, 3.0f, 1.0f);

        g.setColour(phosphorCol);
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        const int n1Pct = static_cast<int>(std::min(100.0f, smoothN1_ * 150.0f));
        const int n2Pct = static_cast<int>(std::min(100.0f, smoothN2_ * 160.0f));
        const int bPlusInt = static_cast<int>(std::round(smoothBPlus_));
        g.drawText("N1 TRAP: " + juce::String(n1Pct) + "%   |   N2 MEMORY: " + juce::String(n2Pct) +
                   "%   |   B+ RAIL: " + juce::String(bPlusInt) + " V",
                   strip.toNearestInt(), juce::Justification::centred);
    }

    // 4B. Recessed Semiconductor Spec Pill Below Crystal Selector (aligns bottom with Link/Mix/Trim readout boxes)
    {
        const int crystalChoice = crystalBox_.getSelectedId() - 1;
        const juce::Colour specCol = (crystalChoice == 0) ? juce::Colour(0xff7aff89)
                                   : (crystalChoice == 1) ? juce::Colour(0xffffb84d)
                                                          : juce::Colour(0xff4de8ff);
        const juce::String specText = (crystalChoice == 0) ? "TAU-1: 40 ms   |   MEMORY: 4.5 s"
                                    : (crystalChoice == 1) ? "TAU-1: 18 ms   |   MEMORY: 1.8 s"
                                                           : "TAU-1: 4 ms    |   MEMORY: 0.4 s";
        auto specPill = juce::Rectangle<float>(206.0f, 266.0f, 214.0f, 22.0f);
        g.setColour(juce::Colour(0xff181512));
        g.fillRoundedRectangle(specPill, 3.5f);
        g.setColour(juce::Colour(0xff6e5d47));
        g.drawRoundedRectangle(specPill, 3.5f, 1.0f);
        g.setColour(specCol);
        g.setFont(juce::FontOptions(9.8f, juce::Font::bold));
        g.drawText(specText, specPill.toNearestInt(), juce::Justification::centred);
    }

    // 5. Section Sub-Panels (1970s Engraved Anodized Sub-Plates with Corner Screws & Edge Wear)
    auto drawVintageSubplate = [&](juce::Rectangle<float> r, const juce::String& title, juce::Colour badgeCol)
    {
        g.setColour(juce::Colours::black.withAlpha(0.28f));
        g.fillRoundedRectangle(r.translated(1.5f, 2.5f), 6.0f);

        juce::ColourGradient plateGrad(juce::Colour(0x99d8ccb8), r.getX(), r.getY(),
                                       juce::Colour(0xaaafa089), r.getX(), r.getBottom(), false);
        g.setGradientFill(plateGrad);
        g.fillRoundedRectangle(r, 6.0f);

        g.setColour(juce::Colour(0xff4a4137));
        g.drawRoundedRectangle(r, 6.0f, 1.6f);
        g.setColour(juce::Colour(0x55fff7e8));
        g.drawRoundedRectangle(r.reduced(1.5f), 5.0f, 0.8f);

        auto banner = r.removeFromTop(23.0f);
        g.setColour(juce::Colour(0xff221e1a));
        g.fillRoundedRectangle(banner.getX(), banner.getY(), banner.getWidth(), 23.0f, 5.0f);
        g.setColour(badgeCol);
        g.fillRect(banner.getX() + 10.0f, banner.getY() + 6.0f, 4.0f, 11.0f);

        g.setColour(juce::Colour(0xffefe4ce));
        g.setFont(juce::FontOptions(10.2f, juce::Font::bold));
        g.drawText(title, banner.reduced(18.0f, 0.0f).toNearestInt(), juce::Justification::centredLeft);

        if (routeMode > 0)
        {
            const bool isSide = (routeMode == 2);
            const juce::Colour tagCol = isSide ? juce::Colour(0xff4de8ff) : juce::Colour(0xffffb84d);
            auto tagRect = juce::Rectangle<float>(banner.getRight() - 38.0f, banner.getY() + 4.5f, 32.0f, 14.0f);
            g.setColour(tagCol.withAlpha(0.18f));
            g.fillRoundedRectangle(tagRect, 2.5f);
            g.setColour(tagCol.withAlpha(0.75f));
            g.drawRoundedRectangle(tagRect, 2.5f, 0.9f);
            g.setColour(tagCol);
            g.setFont(juce::FontOptions(8.6f, juce::Font::bold));
            g.drawText(isSide ? "SIDE" : "MID", tagRect.toNearestInt(), juce::Justification::centred);
        }

        for (float sx : { r.getX() + 8.0f, r.getRight() - 8.0f })
        {
            const float sy = r.getBottom() - 8.0f;
            g.setColour(juce::Colour(0xff6b6052));
            g.fillEllipse(sx - 3.5f, sy - 3.5f, 7.0f, 7.0f);
            g.setColour(juce::Colour(0xff241e18));
            g.drawLine(sx - 2.2f, sy - 1.5f, sx + 2.2f, sy + 1.5f, 1.1f);
        }
    };

    const float botY = 304.0f;
    const float botH = 244.0f;
    const juce::String bankTag = (routeMode == 1) ? " [MID]" : (routeMode == 2) ? " [SIDE]" : "";
    drawVintageSubplate({ 54.0f,  botY, 415.0f, botH }, "T4-BEAN SEMICONDUCTOR & ELECTRO-OPTICAL LAB", juce::Colour(0xff68c97a));
    drawVintageSubplate({ 478.0f, botY, 258.0f, botH }, "12AX7A / 12BH7A & UTC IRON",                  juce::Colour(0xffe08a2e));
    drawVintageSubplate({ 745.0f, botY, 181.0f, botH }, "R37 SIDECHAIN LAB",                           juce::Colour(0xffc99e4c));

    // 6. Draw Live 1970s Amber/Phosphor CRT Scope inside Bottom of Card 1 (Two-Trap Release + Scrolling Live GR/N2 Trace!)
    if (!curveScopeBounds_.isEmpty())
    {
        auto sc = curveScopeBounds_.toFloat();
        g.setColour(juce::Colour(0xff141613));
        g.fillRoundedRectangle(sc, 5.0f);
        g.setColour(juce::Colour(0xff5a5244));
        g.drawRoundedRectangle(sc, 5.0f, 1.4f);

        // Subtle CRT graticule grid
        g.setColour(juce::Colour(0x227aff89));
        for (int i = 1; i < 4; ++i)
        {
            const float gx = sc.getX() + sc.getWidth() * (static_cast<float>(i) * 0.25f);
            g.drawVerticalLine(static_cast<int>(gx), sc.getY() + 2.0f, sc.getBottom() - 2.0f);
        }
        g.drawHorizontalLine(static_cast<int>(sc.getCentreY()), sc.getX() + 2.0f, sc.getRight() - 2.0f);

        const int crystalChoice = crystalBox_.getSelectedId() - 1;
        juce::Colour traceCol = (crystalChoice == 0) ? juce::Colour(0xff7aff89)
                              : (crystalChoice == 1) ? juce::Colour(0xffffb84d)
                                                     : juce::Colour(0xff4de8ff);

        // A. Theoretical Two-Trap Release Curve (dim reference when Live History is overlaid, bright when soloed)
        const float memNorm = static_cast<float>(trapMemorySlider_.getValue()) * 0.01f;
        const float speed   = (crystalChoice == 0) ? 1.0f : (crystalChoice == 1) ? 2.2f : 5.5f;

        juce::Path releasePath;
        const int numPts = 64;
        for (int i = 0; i < numPts; ++i)
        {
            const float tNorm = static_cast<float>(i) / static_cast<float>(numPts - 1);
            const float fastDecay = std::exp(-tNorm * 8.0f * speed);
            const float deepTail  = 0.42f * memNorm * std::exp(-tNorm * 1.1f * speed);
            const float yNorm     = std::clamp(0.18f + 0.65f * (0.58f * fastDecay + deepTail), 0.06f, 0.94f);

            const float px = sc.getX() + 8.0f + tNorm * (sc.getWidth() - 16.0f);
            const float py = sc.getBottom() - 6.0f - yNorm * (sc.getHeight() - 12.0f);
            if (i == 0)
                releasePath.startNewSubPath(px, py);
            else
                releasePath.lineTo(px, py);
        }

        const float modelAlpha = showLiveHistoryScope_ ? 0.42f : 1.0f;
        g.setColour(traceCol.withAlpha(0.22f * modelAlpha));
        g.strokePath(releasePath, juce::PathStrokeType(4.2f));
        g.setColour(traceCol.withAlpha(modelAlpha));
        g.strokePath(releasePath, juce::PathStrokeType(1.6f));

        // B. Real-Time Scrolling Gain-Reduction & Deep-Trap (N2) History Trace
        if (showLiveHistoryScope_)
        {
            juce::Path grFillPath;
            juce::Path grLinePath;
            juce::Path n2LinePath;

            const float x0 = sc.getX() + 8.0f;
            const float w  = sc.getWidth() - 16.0f;
            const float yTop = sc.getY() + 16.0f;
            const float h  = sc.getHeight() - 22.0f;

            grFillPath.startNewSubPath(x0, yTop);
            for (int i = 0; i < kScopeHistoryLen; ++i)
            {
                const int ringIdx = (historyWriteIdx_ + i) % kScopeHistoryLen;
                const float u     = static_cast<float>(i) / static_cast<float>(kScopeHistoryLen - 1);
                const float px    = x0 + u * w;

                // Gain reduction deflects downward from the top reference line (like an optical GR strip-chart)
                const float grVal = grHistory_[static_cast<size_t>(ringIdx)];
                const float pyGr  = yTop + grVal * h;

                // Deep-trap N2 memory rises from the bottom baseline
                const float n2Val = n2History_[static_cast<size_t>(ringIdx)];
                const float pyN2  = (sc.getBottom() - 6.0f) - n2Val * (h * 0.85f);

                grFillPath.lineTo(px, pyGr);
                if (i == 0)
                {
                    grLinePath.startNewSubPath(px, pyGr);
                    n2LinePath.startNewSubPath(px, pyN2);
                }
                else
                {
                    grLinePath.lineTo(px, pyGr);
                    n2LinePath.lineTo(px, pyN2);
                }
            }
            grFillPath.lineTo(x0 + w, yTop);
            grFillPath.closeSubPath();

            g.setColour(traceCol.withAlpha(0.14f));
            g.fillPath(grFillPath);
            g.setColour(traceCol.withAlpha(0.92f));
            g.strokePath(grLinePath, juce::PathStrokeType(1.7f));

            g.setColour(juce::Colour(0xffffb84d).withAlpha(0.80f));
            g.strokePath(n2LinePath, juce::PathStrokeType(1.3f));
        }

        // Live operating point dot moving with current N2 charge
        const float dotX = sc.getX() + 12.0f + std::clamp(smoothN2_ * 1.4f, 0.0f, 0.90f) * (sc.getWidth() - 24.0f);
        const float dotY = sc.getCentreY();
        g.setColour(juce::Colours::white);
        g.fillEllipse(dotX - 3.5f, dotY - 3.5f, 7.0f, 7.0f);

        g.setColour(juce::Colour(0xffb8c4b8));
        g.setFont(juce::FontOptions(9.2f, juce::Font::bold));
        const juce::String scopeTitle = showLiveHistoryScope_
            ? ("TWO-TRAP CURVE + LIVE GR/N2" + bankTag)
            : ("TWO-TRAP RELEASE MODEL" + bankTag);
        g.drawText(scopeTitle, sc.reduced(8.0f, 4.0f).toNearestInt(), juce::Justification::topRight);
    }

    // 6B. Draw Live Tube & Iron Harmonic Transfer Curve & B+ Sag Scope inside Bottom of Card 2!
    if (!tubeScopeBounds_.isEmpty())
    {
        auto ts = tubeScopeBounds_.toFloat();
        g.setColour(juce::Colour(0xff161311));
        g.fillRoundedRectangle(ts, 4.0f);
        g.setColour(juce::Colour(0xff5e5143));
        g.drawRoundedRectangle(ts, 4.0f, 1.2f);

        const float driveNorm = std::clamp((static_cast<float>(tubeDriveSlider_.getValue()) + 6.0f) / 24.0f, 0.0f, 1.0f);
        const float sagPct    = std::clamp((275.0f - smoothBPlus_) / 45.0f, 0.0f, 1.0f);
        const bool  is600     = load600Btn_.getToggleState();
        const bool  isFat     = fatCathodeBtn_.getToggleState();

        juce::Path xferPath;
        const int nPts = 40;
        const float waveW = ts.getWidth() * 0.48f;
        for (int i = 0; i < nPts; ++i)
        {
            const float u = static_cast<float>(i) / static_cast<float>(nPts - 1);
            const float xIn = (u * 2.0f - 1.0f) * (0.75f + 2.2f * driveNorm);
            const float posLim = is600 ? 0.82f : 1.0f;
            const float negLim = (is600 ? 0.65f : 0.85f) * (isFat ? 0.85f : 1.0f);
            const float ySat = (xIn >= 0.0f) ? (posLim * std::tanh(xIn / posLim))
                                             : (negLim * std::tanh(xIn / negLim));
            const float px = ts.getX() + 8.0f + u * (waveW - 12.0f);
            const float py = ts.getCentreY() - ySat * (ts.getHeight() * 0.38f);
            if (i == 0)
                xferPath.startNewSubPath(px, py);
            else
                xferPath.lineTo(px, py);
        }
        g.setColour(juce::Colour(0xffff7a45));
        g.strokePath(xferPath, juce::PathStrokeType(1.8f));

        auto rightBox = juce::Rectangle<float>(ts.getX() + waveW + 4.0f, ts.getY() + 3.0f,
                                               ts.getWidth() - waveW - 10.0f, ts.getHeight() - 6.0f);
        g.setColour(juce::Colour(0xffffb84d));
        g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
        const juce::String modeStr = juce::String(isFat ? "H2 FAT " : "H2/H3 ") + (is600 ? "| 600 OHM" : "| 10K LOAD");
        g.drawText(modeStr, rightBox.removeFromTop(14.0f).toNearestInt(), juce::Justification::centredRight);

        g.setColour(juce::Colour(0xffc9bfb0));
        g.setFont(juce::FontOptions(9.0f, juce::Font::plain));
        const int sagV = static_cast<int>(std::round(smoothBPlus_));
        g.drawText("B+ RAIL: " + juce::String(sagV) + "V (SAG " + juce::String( static_cast<int>(sagPct * 100.0f) ) + "%)",
                   rightBox.toNearestInt(), juce::Justification::centredRight);
    }

    // 7. Bottom Recessed 1970s Amber LED / Signal Manual Inspector Strip (#1)
    if (!infoStripBounds_.isEmpty())
    {
        auto ir = infoStripBounds_.toFloat();

        g.setColour(juce::Colours::black.withAlpha(0.38f));
        g.fillRoundedRectangle(ir.translated(0.0f, 2.0f), 5.0f);

        juce::ColourGradient stripGrad(juce::Colour(0xff191613), ir.getX(), ir.getY(),
                                       juce::Colour(0xff100e0c), ir.getX(), ir.getBottom(), false);
        g.setGradientFill(stripGrad);
        g.fillRoundedRectangle(ir, 5.0f);

        g.setColour(juce::Colour(0xff6b5a42));
        g.drawRoundedRectangle(ir, 5.0f, 1.4f);

        // Left Category Pill Badge
        auto catRect = juce::Rectangle<float>(ir.getX() + 10.0f, ir.getY() + 9.0f, 112.0f, ir.getHeight() - 18.0f);
        g.setColour(infoAccent_.withAlpha(0.18f));
        g.fillRoundedRectangle(catRect, 4.0f);
        g.setColour(infoAccent_.withAlpha(0.75f));
        g.drawRoundedRectangle(catRect, 4.0f, 1.1f);

        g.setColour(infoAccent_);
        g.setFont(juce::FontOptions(10.2f, juce::Font::bold));
        g.drawText(infoCategory_, catRect.toNearestInt(), juce::Justification::centred);

        const int textX = static_cast<int>(catRect.getRight() + 14.0f);
        const int textW = static_cast<int>(ir.getRight() - 14.0f) - textX;

        g.setColour(infoAccent_);
        g.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        g.drawText(infoTitle_, textX, static_cast<int>(ir.getY()) + 7, textW, 18, juce::Justification::centredLeft);

        g.setColour(juce::Colour(0xffdfd6c3));
        g.setFont(juce::FontOptions(11.0f, juce::Font::plain));
        g.drawText(infoDetail_, textX, static_cast<int>(ir.getY()) + 26, textW, 18, juce::Justification::centredLeft);
    }
}

void SilkenTofuAudioProcessorEditor::resized()
{
    // 1. Header Bar Controls (inset inside the 1970s faceplate x = 54..926, with seam at 43 and 936)
    presetBox_.setBounds(382, 15, 178, 28);
    abSwitchBounds_ = { 565, 15, 86, 28 };
    modeBox_.setBounds(656, 15, 108, 28);
    oversamplingBox_.setBounds(769, 15, 88, 28);
    bypassBtn_.setBounds(862, 15, 58, 28);

    // 2. Top Row: Classic Teletronix LA-2A Front Panel
    // Left: Peak Reduction (Large Scalloped Bakelite Knob — inset at x = 54)
    peakReductionLabel_.setBounds(54, 60, 142, 15);
    peakReductionSlider_.setBounds(54, 75, 142, 156);
    autoLevelBtn_.setBounds(58, 242, 134, 28);

    // Center-Left & Center-Right: Symmetrical 1970s VU Meter & T4-Bean Tube Inspection Windows
    vuMeterBounds_      = { 206, 60, 276, 142 };
    vuModeSwitchBounds_ = { 216, 181, 98,  16 };
    t4CanisterBounds_   = { 496, 60, 276, 142 };

    // Center Row Under VU & Canister (y = 212..298): Labeled Crystal Selector + Stereo Link, Mix & Trim Knobs
    crystalLabel_.setBounds(206, 214, 214, 14);
    crystalBox_.setBounds(206, 232, 214, 28);

    stereoLinkLabel_.setBounds(434, 212, 108, 14);
    stereoLinkSlider_.setBounds(434, 226, 108, 72);

    mixLabel_.setBounds(548, 212, 108, 14);
    mixSlider_.setBounds(548, 226, 108, 72);

    trimLabel_.setBounds(662, 212, 108, 14);
    trimSlider_.setBounds(662, 226, 108, 72);

    // Right: Makeup Gain + Symmetrical Stereo / Mid-Side Routing, Solo M/S & Copy M>S Controls
    gainLabel_.setBounds(782, 60, 142, 15);
    gainSlider_.setBounds(782, 75, 142, 156);
    routingBox_.setBounds(782, 236, 140, 25);
    msSoloBtn_.setBounds(782, 265, 69, 23);
    copyMidToSideBtn_.setBounds(853, 265, 69, 23);

    // 3. Bottom Row Sub-Plates (y = 304 .. 548, x = 54 .. 926)
    const int knobW = 78;
    const int knobH = 96;
    const int c1Y1  = 334;
    const int c1Y2  = 462;

    // Card 1: T4-Bean Semiconductor & Electro-Optical Lab (x = 54..469)
    trapMemoryLabel_.setBounds(58,    c1Y1, knobW, 15);
    trapMemorySlider_.setBounds(58,   c1Y1 + 15, knobW, knobH);

    topologyLabel_.setBounds(139,     c1Y1, knobW, 15);
    topologySlider_.setBounds(139,    c1Y1 + 15, knobW, knobH);

    punchHoldoffLabel_.setBounds(220, c1Y1, knobW, 15);
    punchHoldoffSlider_.setBounds(220, c1Y1 + 15, knobW, knobH);

    biasGlowLabel_.setBounds(301,     c1Y1, knobW, 15);
    biasGlowSlider_.setBounds(301,    c1Y1 + 15, knobW, knobH);

    cellAgeLabel_.setBounds(382,      c1Y1, knobW, 15);
    cellAgeSlider_.setBounds(382,     c1Y1 + 15, knobW, knobH);

    // Bottom of Card 1: Live Two-Trap Release Trajectory + Scrolling History CRT Scope (y = 458..532)
    curveScopeBounds_ = { 66, c1Y2 - 4, 391, 74 };

    // Card 2: Oversampled 12AX7A / 12BH7A & UTC Iron (x = 478..736)
    tubeDriveLabel_.setBounds(486,   c1Y1, knobW, 15);
    tubeDriveSlider_.setBounds(486,  c1Y1 + 15, knobW, knobH);

    bPlusSagLabel_.setBounds(568,    c1Y1, knobW, 15);
    bPlusSagSlider_.setBounds(568,   c1Y1 + 15, knobW, knobH);

    millerAirLabel_.setBounds(650,   c1Y1, knobW, 15);
    millerAirSlider_.setBounds(650,  c1Y1 + 15, knobW, knobH);

    fatCathodeBtn_.setBounds(488, c1Y2 - 4, 116, 28);
    load600Btn_.setBounds(610,    c1Y2 - 4, 116, 28);
    tubeScopeBounds_ = { 488, c1Y2 + 30, 238, 40 };

    // Card 3: R37 Sidechain & External Key Ducking (#3) (x = 745..926)
    r37Label_.setBounds(754,   c1Y1, knobW, 15);
    r37Slider_.setBounds(754,  c1Y1 + 15, knobW, knobH);

    scHpLabel_.setBounds(838,  c1Y1, knobW, 15);
    scHpSlider_.setBounds(838, c1Y1 + 15, knobW, knobH);

    extKeyBtn_.setBounds(756,   c1Y2 - 4,  158, 32);
    scListenBtn_.setBounds(756, c1Y2 + 36, 158, 32);

    // 4. Bottom Recessed 1970s Amber LED / Signal Manual Inspector Strip (x = 54..926)
    infoStripBounds_ = { 54, 556, 872, 52 };
}
