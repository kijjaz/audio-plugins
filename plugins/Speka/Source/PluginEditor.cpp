#include "PluginProcessor.h"
#include "PluginEditor.h"

SpekaAudioProcessorEditor::SpekaAudioProcessorEditor (SpekaAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , crtView (p)
{
    setLookAndFeel (&retroLookAndFeel);

    // Add sub-components
    addAndMakeVisible (speakerView);
    addAndMakeVisible (crtView);

    // Helpers to configure rotary sliders
    auto setupSlider = [this] (juce::Slider& s, juce::Label& l, const juce::String& text, const juce::String& suffix = "") {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 16);
        s.setColour (juce::Slider::textBoxTextColourId, Speka::RetroLookAndFeel::crtAmber);
        s.setColour (juce::Slider::textBoxBackgroundColourId, Speka::RetroLookAndFeel::chassisBlack);
        s.setColour (juce::Slider::textBoxOutlineColourId, Speka::RetroLookAndFeel::ibmBeigeShadow);
        if (suffix.isNotEmpty())
            s.setTextValueSuffix (suffix);
        addAndMakeVisible (s);

        l.setText (text, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));
        l.setColour (juce::Label::textColourId, Speka::RetroLookAndFeel::chassisBlack);
        addAndMakeVisible (l);
    };

    // --- Preset Selector (Header) ---
    presetBox.setName ("retro_combo");
    presetBox.setLookAndFeel (&retroLookAndFeel);
    const auto& presets = processorRef.getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
        presetBox.addItem (juce::String (static_cast<int>(i) + 1) + ". " + presets[i].name, static_cast<int>(i) + 1);

    presetBox.setSelectedItemIndex (processorRef.getCurrentProgram(), juce::dontSendNotification);
    presetBox.onChange = [this] {
        int idx = presetBox.getSelectedItemIndex();
        if (idx >= 0)
            processorRef.loadPreset (idx);
    };
    addAndMakeVisible (presetBox);

    presetLabel.setText ("PRESET:", juce::dontSendNotification);
    presetLabel.setJustificationType (juce::Justification::centredRight);
    presetLabel.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));
    presetLabel.setColour (juce::Label::textColourId, Speka::RetroLookAndFeel::chassisBlack);
    addAndMakeVisible (presetLabel);

    // --- Section 1: Modulation & Timing ---
    modeBox.setName ("retro_combo");
    modeBox.setLookAndFeel (&retroLookAndFeel);
    modeBox.addItemList (juce::StringArray { "RealSound PWM", "Sigma-Delta PDM", "Delta Modulation", "Direct 1-Bit" }, 1);
    addAndMakeVisible (modeBox);
    modeLabel.setText ("MODULATION", juce::dontSendNotification);
    modeLabel.setJustificationType (juce::Justification::centredLeft);
    modeLabel.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));
    addAndMakeVisible (modeLabel);

    speakerBox.setName ("retro_combo");
    speakerBox.setLookAndFeel (&retroLookAndFeel);
    speakerBox.addItemList (juce::StringArray { "2.25\" Paper Cone", "12mm Piezo Disk" }, 1);
    addAndMakeVisible (speakerBox);
    speakerLabel.setText ("TRANSDUCER", juce::dontSendNotification);
    speakerLabel.setJustificationType (juce::Justification::centredLeft);
    speakerLabel.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));
    addAndMakeVisible (speakerLabel);

    setupSlider (carrierSlider, carrierLabel, "CARRIER", " Hz");

    // --- Section 2: Circuit & Drive ---
    setupSlider (driveSlider, driveLabel, "DRIVE", " dB");
    setupSlider (sagSlider, sagLabel, "SAG", "");
    setupSlider (jitterSlider, jitterLabel, "JITTER", "");

    // --- Section 3: Acoustics & Squelch ---
    setupSlider (caseResonanceSlider, caseResLabel, "CHASSIS", "");
    setupSlider (rattleSlider, rattleLabel, "RATTLE", "");
    setupSlider (whineSlider, whineLabel, "WHINE", "");

    autoGateToggle.setButtonText ("AUTO-GATE");
    addAndMakeVisible (autoGateToggle);

    setupSlider (gateThreshSlider, gateThreshLabel, "THRESHOLD", " dB");

    // --- Section 4: Output ---
    setupSlider (mixSlider, mixLabel, "MIX", " %");
    setupSlider (trimSlider, trimLabel, "TRIM", " dB");

    // Connect APVTS Attachments
    auto& apvts = processorRef.getAPVTS();
    modeAttach       = std::make_unique<ComboBoxAttachment> (apvts, "mode", modeBox);
    speakerAttach    = std::make_unique<ComboBoxAttachment> (apvts, "speakerType", speakerBox);
    carrierAttach    = std::make_unique<SliderAttachment>   (apvts, "carrierFreq", carrierSlider);
    driveAttach      = std::make_unique<SliderAttachment>   (apvts, "drive", driveSlider);
    caseResAttach    = std::make_unique<SliderAttachment>   (apvts, "caseResonance", caseResonanceSlider);
    rattleAttach     = std::make_unique<SliderAttachment>   (apvts, "rattle", rattleSlider);
    jitterAttach     = std::make_unique<SliderAttachment>   (apvts, "jitter", jitterSlider);
    sagAttach        = std::make_unique<SliderAttachment>   (apvts, "sag", sagSlider);
    whineAttach      = std::make_unique<SliderAttachment>   (apvts, "whine", whineSlider);
    autoGateAttach   = std::make_unique<ButtonAttachment>   (apvts, "autoGate", autoGateToggle);
    gateThreshAttach = std::make_unique<SliderAttachment>   (apvts, "gateThresh", gateThreshSlider);
    mixAttach        = std::make_unique<SliderAttachment>   (apvts, "mix", mixSlider);
    trimAttach       = std::make_unique<SliderAttachment>   (apvts, "trim", trimSlider);

    // Initial size
    setSize (860, 520);

    // Real-time animation timer @ 40 Hz
    startTimerHz (40);
}

SpekaAudioProcessorEditor::~SpekaAudioProcessorEditor()
{
    stopTimer();
    presetBox.setLookAndFeel (nullptr);
    modeBox.setLookAndFeel (nullptr);
    speakerBox.setLookAndFeel (nullptr);
    setLookAndFeel (nullptr);
}

void SpekaAudioProcessorEditor::timerCallback()
{
    float excursion = processorRef.getConeExcursion();
    bool gateActive = processorRef.isGateActive();
    int mode = modeBox.getSelectedItemIndex();
    float carrierHz = static_cast<float> (carrierSlider.getValue());
    int speaker = speakerBox.getSelectedItemIndex();

    // Sync preset box if host/DAW changed program
    if (presetBox.getSelectedItemIndex() != processorRef.getCurrentProgram())
        presetBox.setSelectedItemIndex (processorRef.getCurrentProgram(), juce::dontSendNotification);

    speakerView.setSpeakerType (speaker);
    speakerView.setExcursion (excursion);

    crtView.updateTelemetry (mode, carrierHz, speaker, gateActive, excursion);

    // Repaint chassis for red jewel LED activity
    repaint (getWidth() - 60, 10, 45, 45);
}

void SpekaAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. IBM 5150 Beige Molded Plastic Chassis
    g.setColour (Speka::RetroLookAndFeel::ibmBeige);
    g.fillRect (bounds);

    // Subtle plastic texture / bevel edges
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeLight);
    g.drawHorizontalLine (0, 0.0f, bounds.getWidth());
    g.drawVerticalLine (0, 0.0f, bounds.getHeight());

    g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
    g.drawHorizontalLine (juce::roundToInt (bounds.getHeight() - 1.0f), 0.0f, bounds.getWidth());
    g.drawVerticalLine (juce::roundToInt (bounds.getWidth() - 1.0f), 0.0f, bounds.getHeight());

    // 2. Vintage Header Panel
    auto headerArea = bounds.removeFromTop (56.0f).reduced (12.0f, 6.0f);

    // Debossed "SPEKA" Vintage Logo
    g.setFont (juce::Font ("Courier New", 26.0f, juce::Font::bold));

    // Logo drop-shadow (debossed impression into plastic)
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeLight);
    g.drawText ("SPEKA", headerArea.translated (1.0f, 1.0f), juce::Justification::topLeft, false);
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
    g.drawText ("SPEKA", headerArea.translated (-1.0f, -1.0f), juce::Justification::topLeft, false);
    g.setColour (Speka::RetroLookAndFeel::chassisBlack);
    g.drawText ("SPEKA", headerArea, juce::Justification::topLeft, false);

    // Subtitle
    g.setFont (juce::Font ("Courier New", 9.5f, juce::Font::bold));
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
    g.drawText ("PIT-8253 1-BIT TRANSDUCER",
                headerArea.translated (96.0f, 7.0f), juce::Justification::topLeft, false);

    // Model Badge (vintage brushed metal plate) with version
    auto badgeArea = juce::Rectangle<float> (bounds.getWidth() - 190.0f, 12.0f, 142.0f, 28.0f);
    g.setColour (juce::Colour (0xff32353a));
    g.fillRect (badgeArea);
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
    g.drawRect (badgeArea, 1.0f);

    g.setColour (Speka::RetroLookAndFeel::crtAmber);
    g.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));
    g.drawText ("IBM PC 5150 v0.1.0", badgeArea, juce::Justification::centred, false);

    // Red Jeweled Power & Signal LED
    float ledX = bounds.getWidth() - 32.0f;
    float ledY = 26.0f;
    float ledR = 8.0f;

    // Metal bezel ring
    g.setColour (juce::Colour (0xff222428));
    g.fillEllipse (ledX - ledR - 2.0f, ledY - ledR - 2.0f, (ledR + 2.0f) * 2.0f, (ledR + 2.0f) * 2.0f);
    g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
    g.drawEllipse (ledX - ledR - 2.0f, ledY - ledR - 2.0f, (ledR + 2.0f) * 2.0f, (ledR + 2.0f) * 2.0f, 1.0f);

    bool isLit = processorRef.isGateActive();
    if (isLit)
    {
        // Radiating glow
        g.setColour (Speka::RetroLookAndFeel::ledRedOn.withAlpha (0.4f));
        g.fillEllipse (ledX - ledR - 6.0f, ledY - ledR - 6.0f, (ledR + 6.0f) * 2.0f, (ledR + 6.0f) * 2.0f);

        // Core jewel
        juce::ColourGradient ledGrad (juce::Colour (0xffff6666), ledX - 2.0f, ledY - 2.0f,
                                      Speka::RetroLookAndFeel::ledRedOn, ledX + 4.0f, ledY + 4.0f, false);
        g.setGradientFill (ledGrad);
        g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);
    }
    else
    {
        g.setColour (Speka::RetroLookAndFeel::ledRedOff);
        g.fillEllipse (ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);
    }

    // 3. Lower Control Bays (recessed beige panels with etched separator lines)
    float controlY = 308.0f;
    float bayH = 200.0f;

    auto drawBay = [&g, controlY, bayH] (float x, float w, const juce::String& title) {
        auto bayRect = juce::Rectangle<float> (x, controlY, w, bayH);
        g.setColour (Speka::RetroLookAndFeel::ibmBeigeLight.withAlpha (0.4f));
        g.drawRoundedRectangle (bayRect.translated (1.0f, 1.0f), 3.0f, 1.0f);

        g.setColour (Speka::RetroLookAndFeel::ibmBeigeShadow);
        g.drawRoundedRectangle (bayRect, 3.0f, 1.0f);

        // Header tab
        auto tabRect = juce::Rectangle<float> (x + 8.0f, controlY - 8.0f, w - 16.0f, 16.0f);
        g.setColour (Speka::RetroLookAndFeel::ibmBeige);
        g.fillRect (tabRect);

        g.setColour (Speka::RetroLookAndFeel::chassisBlack);
        g.setFont (juce::Font ("Courier New", 9.5f, juce::Font::bold));
        g.drawText (title, tabRect, juce::Justification::centred, false);
    };

    drawBay (14.0f,  190.0f, "1. TIMING & TRANSDUCER");
    drawBay (212.0f, 198.0f, "2. CIRCUIT & DRIVE");
    drawBay (418.0f, 226.0f, "3. ACOUSTICS & SQUELCH");
    drawBay (652.0f, 194.0f, "4. MASTER OUTPUT");
}

void SpekaAudioProcessorEditor::resized()
{
    // Header preset selector
    presetLabel.setBounds (360, 15, 64, 24);
    presetBox.setBounds (430, 15, 230, 24);

    // Upper row: Speaker bay (left) and CRT oscilloscope (right)
    speakerView.setBounds (14, 58, 386, 240);
    crtView.setBounds (408, 58, 438, 240);

    // Lower Bay 1: Timing & Transducer
    modeLabel.setBounds (24, 320, 170, 14);
    modeBox.setBounds (24, 336, 170, 22);

    speakerLabel.setBounds (24, 364, 170, 14);
    speakerBox.setBounds (24, 380, 170, 22);

    carrierLabel.setBounds (64, 412, 90, 14);
    carrierSlider.setBounds (64, 426, 90, 74);

    // Lower Bay 2: Circuit & Drive
    driveLabel.setBounds (222, 330, 80, 14);
    driveSlider.setBounds (222, 344, 80, 74);

    sagLabel.setBounds (318, 330, 80, 14);
    sagSlider.setBounds (318, 344, 80, 74);

    jitterLabel.setBounds (270, 420, 80, 14);
    jitterSlider.setBounds (270, 434, 80, 70);

    // Lower Bay 3: Acoustics & Squelch
    caseResLabel.setBounds (428, 330, 68, 14);
    caseResonanceSlider.setBounds (428, 344, 68, 70);

    rattleLabel.setBounds (498, 330, 68, 14);
    rattleSlider.setBounds (498, 344, 68, 70);

    whineLabel.setBounds (568, 330, 68, 14);
    whineSlider.setBounds (568, 344, 68, 70);

    autoGateToggle.setBounds (432, 434, 110, 26);
    gateThreshLabel.setBounds (550, 420, 84, 14);
    gateThreshSlider.setBounds (550, 434, 84, 70);

    // Lower Bay 4: Master Output
    mixLabel.setBounds (664, 344, 80, 14);
    mixSlider.setBounds (664, 358, 80, 74);

    trimLabel.setBounds (754, 344, 80, 14);
    trimSlider.setBounds (754, 358, 80, 74);
}
