#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include "PluginProcessor.h"
#include "UI/SilkenTofuLookAndFeel.h"

class SilkenTofuAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    explicit SilkenTofuAudioProcessorEditor(SilkenTofuAudioProcessor&);
    ~SilkenTofuAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    void timerCallback() override;
    void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Colour arcCol);
    void bindChannelAttachments(int bankIndex);

    SilkenTofuAudioProcessor& processorRef_;
    SilkenTofuLookAndFeel lnf_;
    int boundBankIndex_ = -1;

    // Top Header & Routing Controls
    juce::ComboBox presetBox_;
    juce::ComboBox oversamplingBox_;
    juce::ComboBox modeBox_;
    juce::ComboBox crystalBox_;
    juce::Label    crystalLabel_;
    juce::ComboBox routingBox_;
    juce::ToggleButton msSoloBtn_     { "Solo M/S" };
    juce::TextButton   copyMidToSideBtn_ { "Copy M>S" };
    juce::ToggleButton bypassBtn_     { "BYP" };

    // 1. Classic Teletronix Front Panel Knobs (including flexible Stereo / M-S Link)
    juce::Slider peakReductionSlider_, gainSlider_, stereoLinkSlider_, mixSlider_, trimSlider_;
    juce::Label  peakReductionLabel_,  gainLabel_,  stereoLinkLabel_,  mixLabel_,  trimLabel_;
    juce::ToggleButton autoLevelBtn_ { "Auto-Level Link" };

    // 2. "T4-Bean" Semiconductor & Electro-Optical Physics Lab
    juce::Slider trapMemorySlider_, topologySlider_, punchHoldoffSlider_, biasGlowSlider_, cellAgeSlider_;
    juce::Label  trapMemoryLabel_,  topologyLabel_,  punchHoldoffLabel_,  biasGlowLabel_,  cellAgeLabel_;

    // 3. Oversampled Analog Tube (12AX7A + 12BH7A) & Transformer (UTC HA-100X / A-24) Stage
    juce::Slider tubeDriveSlider_, bPlusSagSlider_, millerAirSlider_;
    juce::Label  tubeDriveLabel_,  bPlusSagLabel_,  millerAirLabel_;
    juce::ToggleButton fatCathodeBtn_ { "Fat Ck (12AX7 H2)" };
    juce::ToggleButton load600Btn_    { "600 Ohm Load" };

    // 4. Sidechain & External Key Ducking (#3)
    juce::Slider r37Slider_, scHpSlider_;
    juce::Label  r37Label_,  scHpLabel_;
    juce::ToggleButton extKeyBtn_   { "Ext Key Ducking" };
    juce::ToggleButton scListenBtn_ { "SC Listen (EL)" };

    // APVTS Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ComboAttachment>  osAttach_, routingAttach_, modeAttach_, crystalAttach_;
    std::unique_ptr<SliderAttachment> stereoLinkAttach_;
    std::unique_ptr<ButtonAttachment> msSoloAttach_, bypassAttach_;
    std::unique_ptr<SliderAttachment> prAttach_, gainAttach_, mixAttach_, trimAttach_;
    std::unique_ptr<SliderAttachment> trapAttach_, topoAttach_, punchAttach_, biasAttach_, ageAttach_;
    std::unique_ptr<SliderAttachment> driveAttach_, sagAttach_, millerAttach_;
    std::unique_ptr<SliderAttachment> r37Attach_, scHpAttach_;
    std::unique_ptr<ButtonAttachment> autoLevelAttach_, fatCapAttach_, load600Attach_, extKeyAttach_, scListenAttach_;

    // Smoothed UI Telemetry for 60 FPS animation (including Dual-Channel L/M & R/S VU Needles)
    float smoothGrDb_        = 0.0f;
    float smoothInVu_        = -20.0f;
    float smoothOutVu_       = -20.0f;
    float smoothGrDbCh_[2]   = { 0.0f, 0.0f };
    float smoothInVuCh_[2]   = { -20.0f, -20.0f };
    float smoothOutVuCh_[2]  = { -20.0f, -20.0f };
    float smoothLum_         = 0.0f;
    float smoothN1_          = 0.0f;
    float smoothN2_          = 0.0f;
    float smoothBPlus_       = 275.0f;

    // Real-Time Scrolling Gain Reduction & Deep-Trap (N2) History Trace in Card 1 CRT Scope
    static constexpr int kScopeHistoryLen = 120;
    std::array<float, kScopeHistoryLen> grHistory_ {};
    std::array<float, kScopeHistoryLen> n2History_ {};
    int  historyWriteIdx_      = 0;
    bool showLiveHistoryScope_ = true;
    int  abCopyFlashFrames_    = 0;

    // 1970s Vintage Hardware Texture Assets (Loaded from BinaryData)
    juce::Image faceplateImg_;
    juce::Image vuMeterImg_;
    juce::Image t4WindowImg_;

    juce::Rectangle<int> abSwitchBounds_;
    juce::Rectangle<int> vuMeterBounds_;
    juce::Rectangle<int> vuModeSwitchBounds_;
    juce::Rectangle<int> t4CanisterBounds_;
    juce::Rectangle<int> curveScopeBounds_;
    juce::Rectangle<int> tubeScopeBounds_;
    juce::Rectangle<int> infoStripBounds_;

    // Live Hover / Drag Signal Manual Readout State
    void updateHoverReadout();
    juce::String infoCategory_ { "SIGNAL MANUAL" };
    juce::String infoTitle_    { "HOVER OR TURN ANY KNOB TO INSPECT ITS CIRCUIT ROLE & MUSICAL EFFECT" };
    juce::String infoDetail_   { "Tip: Use [A | B | A>B] to compare states, click [GR | OUT | IN] on the VU meter, or click the CRT scope to toggle live trace." };
    juce::Colour infoAccent_   { 0xffffb84d };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilkenTofuAudioProcessorEditor)
};
