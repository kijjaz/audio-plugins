#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/CarbonGoldLookAndFeel.h"
#include "UI/AnalysisPanelComponent.h"

class VacuumTapeSimAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    VacuumTapeSimAudioProcessorEditor (VacuumTapeSimAudioProcessor&);
    ~VacuumTapeSimAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    VacuumTapeSimAudioProcessor& audioProcessor;
    vts::CarbonGoldLookAndFeel customLookAndFeel;

    float tapeRotation = 0.0f;
    float currentGlow = 0.0f;
    
    std::unique_ptr<vts::AnalysisPanelComponent> analysisPanel;

    // Sliders
    juce::Slider driveSlider;
    juce::Slider sagSlider;
    juce::Slider ipsSlider;
    juce::Slider wowSlider;
    juce::Slider flutterSlider;
    juce::Slider biasSlider;
    juce::Slider asymSlider;
    juce::Slider outputSlider;
    juce::Slider mixSlider;
    juce::Slider hissSlider;

    // Toggle & Combo Box
    juce::ToggleButton autoGainButton;
    juce::ComboBox eqBox;
    juce::ComboBox presetBox;

    // Labels
    juce::Label presetLabel;
    juce::Label driveLabel;
    juce::Label sagLabel;
    juce::Label ipsLabel;
    juce::Label wowLabel;
    juce::Label flutterLabel;
    juce::Label biasLabel;
    juce::Label asymLabel;
    juce::Label eqLabel;
    juce::Label outputLabel;
    juce::Label mixLabel;
    juce::Label hissLabel;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> sagAttachment;
    std::unique_ptr<SliderAttachment> ipsAttachment;
    std::unique_ptr<SliderAttachment> wowAttachment;
    std::unique_ptr<SliderAttachment> flutterAttachment;
    std::unique_ptr<SliderAttachment> biasAttachment;
    std::unique_ptr<SliderAttachment> asymAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> hissAttachment;
    std::unique_ptr<ButtonAttachment> autoGainAttachment;
    std::unique_ptr<ComboBoxAttachment> eqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VacuumTapeSimAudioProcessorEditor)
};
