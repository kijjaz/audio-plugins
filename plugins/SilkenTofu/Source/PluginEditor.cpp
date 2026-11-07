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

    // 1. Top Header ComboBoxes
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

    modeBox_.addItemList({ "Compress (~3:1)", "Limit (~inf:1)" }, 1);
    addAndMakeVisible(modeBox_);

    crystalBox_.addItemList({ "Vintage CdS (LA-2A)", "CdSe Vactrol (LA-3A)", "Fast Silicon (FET-Opto)" }, 1);
    addAndMakeVisible(crystalBox_);

    crystalLabel_.setText("T4B PHOTOCELL CRYSTAL", juce::dontSendNotification);
    crystalLabel_.setJustificationType(juce::Justification::centred);
    crystalLabel_.setFont(juce::FontOptions(10.2f, juce::Font::bold));
    crystalLabel_.setColour(juce::Label::textColourId, juce::Colour(0xff1e1711));
    addAndMakeVisible(crystalLabel_);

    oversamplingBox_.addItemList({ "1x (Off)", "2x Polyphase", "4x Polyphase", "8x Polyphase" }, 1);
    addAndMakeVisible(oversamplingBox_);

    // 2. Knobs Setup (High-Contrast Warm 1970s Tungsten/Amber/Phosphor Accents + Units)
    const juce::Colour colGreen(0xff52d669);
    const juce::Colour colAmber(0xffff9d2e);
    const juce::Colour colRed(0xffe8563a);
    const juce::Colour colGold(0xfff0a830);

    setupKnob(peakReductionSlider_, peakReductionLabel_, "PEAK REDUCTION", colGreen);
    setupKnob(gainSlider_,          gainLabel_,          "MAKEUP GAIN",    colAmber);
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

    // 4. APVTS Attachments
    auto& apvts = processorRef_.getAPVTS();
    modeAttach_      = std::make_unique<ComboAttachment>(apvts, "mode",          modeBox_);
    crystalAttach_   = std::make_unique<ComboAttachment>(apvts, "crystalType",   crystalBox_);
    osAttach_        = std::make_unique<ComboAttachment>(apvts, "oversampling",  oversamplingBox_);

    prAttach_        = std::make_unique<SliderAttachment>(apvts, "peakReduction", peakReductionSlider_);
    gainAttach_      = std::make_unique<SliderAttachment>(apvts, "gainDb",        gainSlider_);
    mixAttach_       = std::make_unique<SliderAttachment>(apvts, "mix",           mixSlider_);
    trimAttach_      = std::make_unique<SliderAttachment>(apvts, "trimDb",        trimSlider_);

    trapAttach_      = std::make_unique<SliderAttachment>(apvts, "trapMemory",    trapMemorySlider_);
    topoAttach_      = std::make_unique<SliderAttachment>(apvts, "topologyMorph", topologySlider_);
    punchAttach_     = std::make_unique<SliderAttachment>(apvts, "punchHoldoff",  punchHoldoffSlider_);
    biasAttach_      = std::make_unique<SliderAttachment>(apvts, "biasGlow",      biasGlowSlider_);
    ageAttach_       = std::make_unique<SliderAttachment>(apvts, "cellAge",       cellAgeSlider_);

    driveAttach_     = std::make_unique<SliderAttachment>(apvts, "tubeIronDrive", tubeDriveSlider_);
    sagAttach_       = std::make_unique<SliderAttachment>(apvts, "bPlusSag",      bPlusSagSlider_);
    millerAttach_    = std::make_unique<SliderAttachment>(apvts, "millerAir",     millerAirSlider_);

    r37Attach_       = std::make_unique<SliderAttachment>(apvts, "r37Emphasis",   r37Slider_);
    scHpAttach_      = std::make_unique<SliderAttachment>(apvts, "scHighPass",    scHpSlider_);

    autoLevelAttach_ = std::make_unique<ButtonAttachment>(apvts, "autoLevel",     autoLevelBtn_);
    fatCapAttach_    = std::make_unique<ButtonAttachment>(apvts, "fatCathodeCap", fatCathodeBtn_);
    load600Attach_   = std::make_unique<ButtonAttachment>(apvts, "load600Ohm",    load600Btn_);
    extKeyAttach_    = std::make_unique<ButtonAttachment>(apvts, "extSidechain",  extKeyBtn_);
    scListenAttach_  = std::make_unique<ButtonAttachment>(apvts, "scListen",      scListenBtn_);

    // Add clean unit suffixes to readouts
    gainSlider_.setTextValueSuffix(" dB");
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
    label.setFont(juce::FontOptions(10.2f, juce::Font::bold));
    label.setColour(juce::Label::textColourId, juce::Colour(0xff1e1711));
    addAndMakeVisible(label);
}

void SilkenTofuAudioProcessorEditor::timerCallback()
{
    const float targetGr    = processorRef_.getGainReductionDb();
    const float targetLum   = processorRef_.getElLuminescence();
    const float targetN1    = processorRef_.getShallowTrapN1();
    const float targetN2    = processorRef_.getDeepTrapN2();
    const float targetBPlus = processorRef_.getBPlusVolts();

    smoothGrDb_  += 0.28f * (targetGr    - smoothGrDb_);
    smoothLum_   += 0.30f * (targetLum   - smoothLum_);
    smoothN1_    += 0.28f * (targetN1    - smoothN1_);
    smoothN2_    += 0.22f * (targetN2    - smoothN2_);
    smoothBPlus_ += 0.25f * (targetBPlus - smoothBPlus_);

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

    auto active = [](const juce::Component& c, const juce::Label* lbl = nullptr)
    {
        return c.isMouseOverOrDragging(true) || (lbl != nullptr && lbl->isMouseOver(true));
    };

    // 1. Front Panel Knobs & Auto-Level
    if (active(peakReductionSlider_, &peakReductionLabel_))
    {
        setInfo("FRONT PANEL",
                "PEAK REDUCTION [" + juce::String(peakReductionSlider_.getValue(), 1) + "]  -  Sidechain Drive into T4B Electroluminescent Panel",
                "LEFT (0): No attenuation   |   RIGHT (60-100): Exponential 100k pot throw for deep -20 dB+ optical compression",
                colGreen);
        return;
    }
    if (active(gainSlider_, &gainLabel_))
    {
        setInfo("FRONT PANEL",
                "MAKEUP GAIN [" + juce::String(gainSlider_.getValue(), 1) + " dB]  -  12AX7A / 12BH7A Output Line Amplifier Gain",
                "LEFT (-6 dB): Attenuates hot signals   |   RIGHT (+24 dB): Boosts post-T4B output level",
                colAmber);
        return;
    }
    if (active(mixSlider_, &mixLabel_))
    {
        setInfo("FRONT PANEL",
                "DRY / WET MIX [" + juce::String(mixSlider_.getValue(), 1) + " %]  -  Parallel New York Optical Compression Blend",
                "LEFT (0%): 100% uncompressed dry input   |   RIGHT (100%): 100% compressed T4B + tube wet signal",
                colBrass);
        return;
    }
    if (active(trimSlider_, &trimLabel_))
    {
        setInfo("FRONT PANEL",
                "MASTER TRIM [" + juce::String(trimSlider_.getValue(), 1) + " dB]  -  Clean Post-Transformer Digital Output Fader (Unity = 0.0 dB)",
                "LEFT (-18 dB): Clean headroom trim   |   RIGHT (+18 dB): Clean output boost without altering tube saturation",
                colBrass);
        return;
    }
    if (active(autoLevelBtn_))
    {
        const juce::String state = autoLevelBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("FRONT PANEL",
                "AUTO-LEVEL LINK [" + state + "]  -  Automatic Loudness-Compensated Makeup Gain",
                "OFF: Manual Makeup Gain only   |   ON: Automatically makes up volume lost as you raise Peak Reduction",
                colAmber);
        return;
    }

    // 2. Card 1: T4-Bean Semiconductor & Electro-Optical Lab
    if (active(trapMemorySlider_, &trapMemoryLabel_))
    {
        setInfo("T4-BEAN LAB",
                "TRAP MEMORY [" + juce::String(trapMemorySlider_.getValue(), 1) + " %]  -  Deep-Trap (N2) Multi-Second Optical Release Tail",
                "LEFT (-50%): Reverse optical bloom swell   |   0%: Fast single-stage release   |   RIGHT (200%): Classic 15s LA-2A hold",
                colGreen);
        return;
    }
    if (active(topologySlider_, &topologyLabel_))
    {
        setInfo("T4-BEAN LAB",
                "FB / FF MORPH [" + juce::String(topologySlider_.getValue(), 1) + " %]  -  Sidechain Tap Topology (Feedback to Feed-Forward)",
                "LEFT (0%): Classic LA-2A Feedback (smooth ~3:1)   |   100%: Hard Feed-Forward   |   RIGHT (125%): Over-compression ducking",
                colGreen);
        return;
    }
    if (active(punchHoldoffSlider_, &punchHoldoffLabel_))
    {
        setInfo("T4-BEAN LAB",
                "PUNCH HOLDOFF [" + juce::String(punchHoldoffSlider_.getValue(), 1) + " ms]  -  Pre-EL Optical Attack Transient Delay",
                "LEFT (0 ms): Clamps transients immediately   |   RIGHT (30 ms): Lets drum & vocal attacks punch through before T4B reacts",
                colGreen);
        return;
    }
    if (active(biasGlowSlider_, &biasGlowLabel_))
    {
        setInfo("T4-BEAN LAB",
                "BIAS PRE-WARM [" + juce::String(biasGlowSlider_.getValue(), 1) + " %]  -  Continuous Sub-Threshold EL Panel Bias Voltage",
                "LEFT (0%): Classic 22V Destriau dead-zone knee   |   RIGHT (100%): Pre-glows EL panel for instant zero-threshold compression",
                colAmber);
        return;
    }
    if (active(cellAgeSlider_, &cellAgeLabel_))
    {
        setInfo("T4-BEAN LAB",
                "CELL AGE (50Y) [" + juce::String(cellAgeSlider_.getValue(), 1) + " %]  -  50-Year CdS Crystal Lattice Fatigue & Drift",
                "LEFT (0%): 1965 factory-fresh T4B wafer   |   RIGHT (100%): Aged 1970s cell with warmer knee & slower dark recovery",
                colAmber);
        return;
    }

    // 3. Card 2: Oversampled 12AX7A / 12BH7A & UTC Iron
    if (active(tubeDriveSlider_, &tubeDriveLabel_))
    {
        setInfo("TUBE & IRON",
                "TUBE & IRON DRIVE [" + juce::String(tubeDriveSlider_.getValue(), 1) + " dB]  -  12AX7A Triode + 12BH7A WCF + UTC Iron Saturation",
                "LEFT (-6 dB): Ultra-clean linear headroom   |   RIGHT (+18 dB): Rich 2nd/3rd harmonic tube warmth & transformer core saturation",
                colRed);
        return;
    }
    if (active(bPlusSagSlider_, &bPlusSagLabel_))
    {
        setInfo("TUBE & IRON",
                "B+ RAIL SAG [" + juce::String(bPlusSagSlider_.getValue(), 1) + " %]  -  275V High-Voltage Power Supply Droop (R29 / C7A Cap)",
                "LEFT (0%): Stiff regulated 275V supply   |   RIGHT (100%): Heavy compression droops B+ voltage for spongy tube bloom",
                colRed);
        return;
    }
    if (active(millerAirSlider_, &millerAirLabel_))
    {
        setInfo("TUBE & IRON",
                "MILLER AIR [" + juce::String(millerAirSlider_.getValue(), 1) + " %]  -  Gain-Dependent 12AX7A Miller Capacitance HF Tilt",
                "LEFT (-100%): Vintage high-frequency rolloff under heavy GR   |   RIGHT (+100%): Restores & boosts silky top-end air",
                colAmber);
        return;
    }
    if (active(fatCathodeBtn_))
    {
        const juce::String state = fatCathodeBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("TUBE & IRON",
                "FAT Ck (12AX7 H2) [" + state + "]  -  V1 Cathode Bypass Capacitor Asymmetry Mod",
                "OFF: Standard vintage cathode bias   |   ON: Increases grid-conduction bias shift for thicker even-order (2nd) harmonics",
                colRed);
        return;
    }
    if (active(load600Btn_))
    {
        const juce::String state = load600Btn_.getToggleState() ? "ON" : "OFF";
        setInfo("TUBE & IRON",
                "600 OHM LOAD [" + state + "]  -  UTC A-24 Output Transformer Termination Load",
                "OFF: Modern 10k bridging load (open headroom)   |   ON: Vintage 600-Ohm line termination (earlier asymmetrical clipping)",
                colRed);
        return;
    }

    // 4. Card 3: R37 Sidechain & Ducking
    if (active(r37Slider_, &r37Label_))
    {
        setInfo("SIDECHAIN",
                "R37 EMPHASIS [" + juce::String(r37Slider_.getValue(), 1) + " %]  -  Back-Panel R37 Sidechain High-Frequency Pre-Emphasis",
                "LEFT (0%): Flat broadband sidechain   |   RIGHT (100%): Boosts highs into sidechain so sibilance & harshness duck harder",
                colBrass);
        return;
    }
    if (active(scHpSlider_, &scHpLabel_))
    {
        setInfo("SIDECHAIN",
                "SC WARM-CUT [" + juce::String(scHpSlider_.getValue(), 0) + " Hz]  -  Sidechain Bass High-Pass Filter (20 Hz - 400 Hz)",
                "LEFT (20 Hz): Full sub-bass triggers compression   |   RIGHT (400 Hz): Ignores kick & bass so low end stays uncompressed",
                colBrass);
        return;
    }
    if (active(extKeyBtn_))
    {
        const juce::String state = extKeyBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("SIDECHAIN",
                "EXT KEY DUCKING [" + state + "]  -  External Sidechain Input Optical Keying",
                "OFF: Compresses using main input   |   ON: Uses DAW Aux/Sidechain bus to trigger T4B optical ducking",
                colBrass);
        return;
    }
    if (active(scListenBtn_))
    {
        const juce::String state = scListenBtn_.getToggleState() ? "ON" : "OFF";
        setInfo("SIDECHAIN",
                "SC LISTEN (EL) [" + state + "]  -  Sidechain Filter & Emphasis Audition Monitor",
                "OFF: Normal main audio output   |   ON: Solos the filtered R37 / Warm-Cut sidechain signal driving the T4B panel",
                colBrass);
        return;
    }

    // 5. Top Bar Dropdowns & Crystal Box
    if (active(presetBox_))
    {
        setInfo("TOP BAR",
                "FACTORY PRESET [" + presetBox_.getText() + "]  -  Calibrated 0.0 dB Unity-Trim Studio Setups",
                "Selects between 8 calibrated studio presets for vocal silk, 600-Ohm bass iron, drum bus, FET-opto limiting, and mastering",
                colAmber);
        return;
    }
    if (active(modeBox_))
    {
        setInfo("TOP BAR",
                "COMPRESS / LIMIT MODE [" + modeBox_.getText() + "]  -  Front-Panel Ratio & Threshold Switch",
                "Compress (~3:1): Gentle musical optical knee   |   Limit (~inf:1): Lowers EL threshold for +4 to +8 dB deeper clamping",
                colAmber);
        return;
    }
    if (active(crystalBox_))
    {
        setInfo("T4-BEAN LAB",
                "PHOTOCELL CRYSTAL [" + crystalBox_.getText() + "]  -  T4B Semiconductor Wafer Chemistry",
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

    // 6. Custom Visual Displays (VU Meter, X-Ray Window, Two-Trap Scope)
    const auto mousePt = getMouseXYRelative();
    if (vuMeterBounds_.contains(mousePt))
    {
        setInfo("TELEMETRY",
                "ILLUMINATED VU METER [" + juce::String(smoothGrDb_, 1) + " dB]  -  Live T4B Optical Gain Reduction Readout",
                "Need more compression? Raise Peak Reduction (60-100), switch Mode to Limit (~inf:1), or push FB/FF Morph above 50%",
                colAmber);
        return;
    }
    if (t4CanisterBounds_.contains(mousePt))
    {
        setInfo("TELEMETRY",
                "T4-BEAN & TUBE X-RAY WINDOW  -  Live Semiconductor Traps & 275V B+ Supply Telemetry",
                "N1 TRAP: Fast shallow carrier density   |   N2 MEMORY: Slow multi-second trap charge   |   B+ RAIL: Live tube plate supply voltage",
                colGreen);
        return;
    }
    if (curveScopeBounds_.contains(mousePt))
    {
        setInfo("TELEMETRY",
                "TWO-TRAP RELEASE TRAJECTORY CRT  -  Live Optical Recovery Curve Visualizer",
                "Shows fast initial recovery (tau1) + slow memory tail (tau2); white dot tracks live N2 deep-trap charge state",
                colGreen);
        return;
    }
}

void SilkenTofuAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

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

    // Riveted Brass Nameplate on Left of Header (Pure ASCII, zero truncation)
    auto badgeRect = juce::Rectangle<float>(62.0f, 11.0f, 382.0f, 36.0f);
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
    g.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    g.drawText("SILKEN TOFU   MODEL LA-2A / T4B", 72, 12, 362, 20, juce::Justification::centredLeft);

    g.setColour(juce::Colour(0xff362813));
    g.setFont(juce::FontOptions(9.0f, juce::Font::bold));
    g.drawText("SER. NO. 1974-0420  |  ELECTRO-OPTICAL LEVELING AMPLIFIER", 72, 29, 362, 14, juce::Justification::centredLeft);

    // 3. Draw Tightly Cropped 1970s Bakelite VU Meter + Dynamic Physical Needle
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

        // Dynamic Physical Blued-Steel Needle Pivoting Inside the Cropped Glass Opening
        auto glassWindow = juce::Rectangle<float>(vu.getX() + vu.getWidth() * 0.10f,
                                                  vu.getY() + vu.getHeight() * 0.14f,
                                                  vu.getWidth() * 0.80f,
                                                  vu.getHeight() * 0.645f);
        {
            juce::Graphics::ScopedSaveState state(g);
            g.reduceClipRegion(glassWindow.toNearestInt());

            const auto pivot = juce::Point<float>(vu.getCentreX(), vu.getBottom() + vu.getHeight() * 0.12f);
            const float needleLen = vu.getHeight() * 0.88f;

            const float grClamped  = std::clamp(smoothGrDb_, -20.0f, 0.0f);
            const float needleNorm = std::pow((grClamped + 20.0f) / 20.0f, 1.38f);
            // -0.72 rad = -20 VU, +0.235 rad = dead-on 0 VU mark
            const float needleAng  = -0.72f + needleNorm * 0.955f;

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

        // Recessed Amber Readout Pill at Bottom-Right of Bezel
        auto pill = juce::Rectangle<float>(vu.getRight() - 78.0f, vu.getBottom() - 21.0f, 68.0f, 16.0f);
        g.setColour(juce::Colour(0xe614110e));
        g.fillRoundedRectangle(pill, 3.0f);
        g.setColour(juce::Colour(0xff8c7248));
        g.drawRoundedRectangle(pill, 3.0f, 1.0f);
        g.setColour(juce::Colour(0xffffb84d));
        g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        g.drawText(juce::String(smoothGrDb_, 1) + " dB", pill.toNearestInt(), juce::Justification::centred);
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

    // 4B. Recessed Semiconductor Spec Pill Below Crystal Selector (aligns bottom with Mix/Trim readout boxes)
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
        g.setFont(juce::FontOptions(10.5f, juce::Font::bold));
        g.drawText(title, banner.reduced(18.0f, 0.0f).toNearestInt(), juce::Justification::centredLeft);

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
    drawVintageSubplate({ 54.0f,  botY, 415.0f, botH }, "T4-BEAN SEMICONDUCTOR & ELECTRO-OPTICAL LAB", juce::Colour(0xff68c97a));
    drawVintageSubplate({ 478.0f, botY, 258.0f, botH }, "OVERSAMPLED 12AX7A / 12BH7A & UTC IRON",      juce::Colour(0xffe08a2e));
    drawVintageSubplate({ 745.0f, botY, 181.0f, botH }, "R37 SIDECHAIN & DUCKING",                     juce::Colour(0xffc99e4c));

    // 6. Draw Live 1970s Amber CRT / Destriau Knee & Two-Trap Memory Scope inside Bottom of Card 1!
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

        // Plot live Two-Trap Release Curve based on current Crystal + Trap Memory + Cell Age knobs!
        const int crystalChoice = crystalBox_.getSelectedId() - 1;
        juce::Colour traceCol = (crystalChoice == 0) ? juce::Colour(0xff7aff89)
                              : (crystalChoice == 1) ? juce::Colour(0xffffb84d)
                                                     : juce::Colour(0xff4de8ff);

        const float memNorm = static_cast<float>(trapMemorySlider_.getValue()) * 0.01f;
        const float speed   = (crystalChoice == 0) ? 1.0f : (crystalChoice == 1) ? 2.2f : 5.5f;

        juce::Path releasePath;
        const int numPts = 64;
        for (int i = 0; i < numPts; ++i)
        {
            const float tNorm = static_cast<float>(i) / static_cast<float>(numPts - 1);
            // Fast shallow-trap drop + deep-trap memory tail (or negative bloom!)
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

        g.setColour(traceCol.withAlpha(0.28f));
        g.strokePath(releasePath, juce::PathStrokeType(4.5f));
        g.setColour(traceCol);
        g.strokePath(releasePath, juce::PathStrokeType(1.8f));

        // Live operating point dot moving with current N1/N2 charge
        const float dotX = sc.getX() + 12.0f + std::clamp(smoothN2_ * 1.4f, 0.0f, 0.90f) * (sc.getWidth() - 24.0f);
        const float dotY = sc.getCentreY();
        g.setColour(juce::Colours::white);
        g.fillEllipse(dotX - 3.5f, dotY - 3.5f, 7.0f, 7.0f);

        g.setColour(juce::Colour(0xffb8c4b8));
        g.setFont(juce::FontOptions(9.5f, juce::Font::bold));
        g.drawText("TWO-TRAP RELEASE TRAJECTORY", sc.reduced(8.0f, 4.0f).toNearestInt(), juce::Justification::topRight);
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

        // Plot live asymmetric 12AX7A + 12BH7A WCF saturation transfer curve
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

        // Right side of Card 2 scope: Live H2/H3 Harmonic & B+ Rail Status Readout
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

        // Right 2-Line Readout: Line 1 = Control Name + Value + Role, Line 2 = Left vs Right Action Guide
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
    presetBox_.setBounds(450, 15, 202, 28);
    modeBox_.setBounds(658, 15, 130, 28);
    oversamplingBox_.setBounds(794, 15, 124, 28);

    // 2. Top Row: Classic Teletronix LA-2A Front Panel
    // Left: Peak Reduction (Large Scalloped Bakelite Knob — inset at x = 54)
    peakReductionLabel_.setBounds(54, 60, 142, 15);
    peakReductionSlider_.setBounds(54, 75, 142, 156);
    autoLevelBtn_.setBounds(58, 242, 134, 28);

    // Center-Left & Center-Right: Symmetrical 1970s VU Meter & T4-Bean Tube Inspection Windows
    vuMeterBounds_    = { 206, 60, 276, 142 };
    t4CanisterBounds_ = { 496, 60, 276, 142 };

    // Center Row Under VU & Canister (y = 212..294): Labeled Crystal Selector + Balanced Mix & Trim Knobs
    crystalLabel_.setBounds(206, 214, 214, 14);
    crystalBox_.setBounds(206, 232, 214, 28);

    mixLabel_.setBounds(452, 212, 118, 14);
    mixSlider_.setBounds(452, 226, 118, 72);

    trimLabel_.setBounds(608, 212, 118, 14);
    trimSlider_.setBounds(608, 226, 118, 72);

    // Right: Makeup Gain (Large Scalloped Bakelite Knob — inset ending at x = 924)
    gainLabel_.setBounds(782, 60, 142, 15);
    gainSlider_.setBounds(782, 75, 142, 156);

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

    // Bottom of Card 1: Live Two-Trap Release Trajectory CRT Scope (y = 458..532)
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
