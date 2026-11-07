#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
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

private:
    void timerCallback() override;
    void setupKnob(juce::Slider& slider, juce::Label& label, const juce::String& text, juce::Colour arcCol);

    SilkenTofuAudioProcessor& processorRef_;
    SilkenTofuLookAndFeel lnf_;

    // Top Header Controls
    juce::ComboBox presetBox_;
    juce::ComboBox oversamplingBox_;
    juce::ComboBox modeBox_;
    juce::ComboBox crystalBox_;
    juce::Label    crystalLabel_;

    // 1. Classic Teletronix Front Panel Knobs
    juce::Slider peakReductionSlider_, gainSlider_, mixSlider_, trimSlider_;
    juce::Label  peakReductionLabel_,  gainLabel_,  mixLabel_,  trimLabel_;
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

    std::unique_ptr<ComboAttachment>  osAttach_, modeAttach_, crystalAttach_;
    std::unique_ptr<SliderAttachment> prAttach_, gainAttach_, mixAttach_, trimAttach_;
    std::unique_ptr<SliderAttachment> trapAttach_, topoAttach_, punchAttach_, biasAttach_, ageAttach_;
    std::unique_ptr<SliderAttachment> driveAttach_, sagAttach_, millerAttach_;
    std::unique_ptr<SliderAttachment> r37Attach_, scHpAttach_;
    std::unique_ptr<ButtonAttachment> autoLevelAttach_, fatCapAttach_, load600Attach_, extKeyAttach_, scListenAttach_;

    // Smoothed UI Telemetry for 60 FPS animation
    float smoothGrDb_  = 0.0f;
    float smoothLum_   = 0.0f;
    float smoothN1_    = 0.0f;
    float smoothN2_    = 0.0f;
    float smoothBPlus_ = 275.0f;

    // 1970s Vintage Hardware Texture Assets (Loaded from BinaryData)
    juce::Image faceplateImg_;
    juce::Image vuMeterImg_;
    juce::Image t4WindowImg_;

    juce::Rectangle<int> vuMeterBounds_;
    juce::Rectangle<int> t4CanisterBounds_;
    juce::Rectangle<int> curveScopeBounds_;
    juce::Rectangle<int> tubeScopeBounds_;
    juce::Rectangle<int> infoStripBounds_;

    // Live Hover / Drag Signal Manual Readout State
    void updateHoverReadout();
    juce::String infoCategory_ { "SIGNAL MANUAL" };
    juce::String infoTitle_    { "HOVER OR TURN ANY KNOB TO INSPECT ITS CIRCUIT ROLE & MUSICAL EFFECT" };
    juce::String infoDetail_   { "Tip: For heavier compression, raise Peak Reduction (60-100), switch to Limit (~inf:1), or push FB/FF Morph right." };
    juce::Colour infoAccent_   { 0xffffb84d };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilkenTofuAudioProcessorEditor)
};

