#include "PluginEditor.h"
#include "DSP/DiodeModels.h"

FriedTofuAudioProcessorEditor::FriedTofuAudioProcessorEditor(FriedTofuAudioProcessor& p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      scopeComponent(p),
      transferCurveComponent(p)
{
    setLookAndFeel(&customLookAndFeel);

    // Setup Visualizers
    addAndMakeVisible(scopeComponent);
    addAndMakeVisible(transferCurveComponent);

    // Helper lambda for configuring sliders
    auto setupRotary = [this](juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        slider.setTextValueSuffix(suffix);
        slider.setColour(juce::Slider::textBoxTextColourId, FriedTofuUI::FriedTofuLookAndFeel::textLight);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(11.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::textMuted);
        addAndMakeVisible(label);
    };

    setupRotary(driveSlider,    driveLabel,    "DRIVE",     " dB");
    setupRotary(networkZSlider, networkZLabel, "NETWORK Z", " Ω");
    setupRotary(toneSlider,     toneLabel,     "TONE",      " Hz");
    setupRotary(mixSlider,      mixLabel,      "MIX",       " %");
    setupRotary(trimSlider,     trimLabel,     "TRIM",      " dB");

    // Diode Combos
    for (int i = 0; i < FriedTofuDSP::NumDiodeTypes; ++i)
    {
        diodeACombo.addItem(FriedTofuDSP::AvailableDiodes[i].name, i + 1);
        diodeBCombo.addItem(FriedTofuDSP::AvailableDiodes[i].name, i + 1);
    }
    addAndMakeVisible(diodeACombo);
    addAndMakeVisible(diodeBCombo);

    diodeALabel.setText("PATH A (+)", juce::dontSendNotification);
    diodeALabel.setFont(juce::Font(10.5f, juce::Font::bold));
    diodeALabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    addAndMakeVisible(diodeALabel);

    diodeBLabel.setText("PATH B (-)", juce::dontSendNotification);
    diodeBLabel.setFont(juce::Font(10.5f, juce::Font::bold));
    diodeBLabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    addAndMakeVisible(diodeBLabel);

    // Oversample Combo
    oversampleCombo.addItem("1x (Raw)", 1);
    oversampleCombo.addItem("2x Oversample", 2);
    oversampleCombo.addItem("4x (Crisp Clean)", 3);
    addAndMakeVisible(oversampleCombo);

    oversampleLabel.setText("OVERSAMPLE", juce::dontSendNotification);
    oversampleLabel.setFont(juce::Font(10.5f, juce::Font::bold));
    oversampleLabel.setColour(juce::Label::textColourId, FriedTofuUI::FriedTofuLookAndFeel::textMuted);
    addAndMakeVisible(oversampleLabel);

    // Presets
    auto setupPresetBtn = [this](juce::TextButton& btn)
    {
        btn.setColour(juce::TextButton::textColourOffId, FriedTofuUI::FriedTofuLookAndFeel::textLight);
        addAndMakeVisible(btn);
    };
    setupPresetBtn(presetGlueSym);
    setupPresetBtn(presetMojoSym);
    setupPresetBtn(presetAsym);
    setupPresetBtn(presetGerm);

    presetGlueSym.onClick = [this] { applyPreset(1, 1, 6.0f, 1000.0f); };    // Ge + Ge, 1k
    presetMojoSym.onClick = [this] { applyPreset(0, 0, 22.0f, 2000.0f); };   // Si + Si, 2k
    presetAsym.onClick    = [this] { applyPreset(0, 1, 16.0f, 1600.0f); };   // Si + Ge, 1.6k
    presetGerm.onClick    = [this] { applyPreset(1, 1, 30.0f, 630.0f); };    // Ge + Ge, 630 Ohm, Drive 30dB

    // APVTS Attachments
    auto& apvts = audioProcessor.getAPVTS();
    driveAttach      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "drive", driveSlider);
    networkZAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "networkZ", networkZSlider);
    toneAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "tone", toneSlider);
    mixAttach        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
    trimAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "trim", trimSlider);
    diodeAAttach     = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "diodeA", diodeACombo);
    diodeBAttach     = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "diodeB", diodeBCombo);
    oversampleAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "oversample", oversampleCombo);

    setSize(780, 560);
}

FriedTofuAudioProcessorEditor::~FriedTofuAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void FriedTofuAudioProcessorEditor::applyPreset(int diodeA, int diodeB, float drive, float networkZ)
{
    auto& apvts = audioProcessor.getAPVTS();
    if (auto* param = apvts.getParameter("diodeA"))
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(diodeA)));
    if (auto* param = apvts.getParameter("diodeB"))
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(diodeB)));
    if (auto* param = apvts.getParameter("drive"))
        param->setValueNotifyingHost(param->convertTo0to1(drive));
    if (auto* param = apvts.getParameter("networkZ"))
        param->setValueNotifyingHost(param->convertTo0to1(networkZ));
}

void FriedTofuAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Matte Carbon Background
    g.fillAll(FriedTofuUI::FriedTofuLookAndFeel::bgDark);

    // Header Panel
    auto headerRect = bounds.removeFromTop(68.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::panelBg);
    g.fillRect(headerRect);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::lineBorder);
    g.drawHorizontalLine(static_cast<int>(headerRect.getBottom()), 0.0f, bounds.getWidth());

    // Title: "FRIED TOFU"
    g.setFont(juce::Font(22.0f, juce::Font::bold));
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    g.drawText("FRIED TOFU", 24, 12, 200, 26, juce::Justification::left);

    // Subtitle
    g.setFont(juce::Font(11.0f));
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::textMuted);
    g.drawText("WDF Wave Digital Filter Diode Clipper · Newton-Raphson Engine", 24, 38, 480, 18, juce::Justification::left);

    // Status Badge
    juce::Rectangle<float> badgeRect(bounds.getWidth() - 140.0f, 22.0f, 116.0f, 24.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::panelBg2);
    g.fillRoundedRectangle(badgeRect, 2.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::phosphorGreen.withAlpha(0.6f));
    g.drawRoundedRectangle(badgeRect, 2.0f, 1.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::phosphorGreen);
    g.setFont(juce::Font(10.5f, juce::Font::bold));
    g.drawText("4x OVERSAMPLE", badgeRect, juce::Justification::centred);

    // Control section background panel
    auto controlsPanel = juce::Rectangle<float>(18.0f, 110.0f, bounds.getWidth() - 36.0f, 184.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::panelBg);
    g.fillRoundedRectangle(controlsPanel, 3.0f);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::lineBorder);
    g.drawRoundedRectangle(controlsPanel, 3.0f, 1.0f);

    // Visualizers section headers
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::phosphorGreen);
    g.drawText("SCOPE  (WET OUTPUT)", 22, 304, 250, 18, juce::Justification::left);
    g.setColour(FriedTofuUI::FriedTofuLookAndFeel::copperGold);
    g.drawText("STATIC TRANSFER CURVE  (V_OUT vs V_IN)", 406, 304, 340, 18, juce::Justification::left);
}

void FriedTofuAudioProcessorEditor::resized()
{
    // Preset buttons row
    const int presetY = 76;
    const int btnW = 120;
    const int btnH = 24;
    const int btnGap = 8;
    int curX = 22;

    presetGlueSym.setBounds(curX, presetY, btnW, btnH); curX += btnW + btnGap;
    presetMojoSym.setBounds(curX, presetY, btnW, btnH); curX += btnW + btnGap;
    presetAsym.setBounds(curX, presetY, btnW + 20, btnH); curX += btnW + 20 + btnGap;
    presetGerm.setBounds(curX, presetY, btnW, btnH);

    // Controls Row 1: Knobs & Selectors inside controlsPanel (Y: 110..294)
    const int knobY = 120;
    const int knobW = 86;
    const int knobH = 92;

    driveLabel.setBounds(30, knobY, knobW, 16);
    driveSlider.setBounds(30, knobY + 16, knobW, knobH);

    networkZLabel.setBounds(126, knobY, knobW, 16);
    networkZSlider.setBounds(126, knobY + 16, knobW, knobH);

    // Diode selectors in the middle
    diodeALabel.setBounds(230, knobY + 10, 140, 16);
    diodeACombo.setBounds(230, knobY + 28, 140, 24);

    diodeBLabel.setBounds(230, knobY + 58, 140, 16);
    diodeBCombo.setBounds(230, knobY + 76, 140, 24);

    oversampleLabel.setBounds(230, knobY + 106, 140, 16);
    oversampleCombo.setBounds(230, knobY + 124, 140, 24);

    // Output knobs on the right
    toneLabel.setBounds(390, knobY, knobW, 16);
    toneSlider.setBounds(390, knobY + 16, knobW, knobH);

    mixLabel.setBounds(486, knobY, knobW, 16);
    mixSlider.setBounds(486, knobY + 16, knobW, knobH);

    trimLabel.setBounds(582, knobY, knobW, 16);
    trimSlider.setBounds(582, knobY + 16, knobW, knobH);

    // Visualizers in bottom half
    const int visY = 326;
    const int visH = 210;
    const int visW = (getWidth() - 54) / 2;

    scopeComponent.setBounds(20, visY, visW, visH);
    transferCurveComponent.setBounds(20 + visW + 14, visY, visW, visH);
}
