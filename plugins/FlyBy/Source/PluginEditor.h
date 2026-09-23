#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/FlyByLookAndFeel.h"
#include "UI/Radar3DComponent.h"

class FlyByAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    FlyByAudioProcessorEditor(FlyByAudioProcessor&);
    ~FlyByAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FlyByAudioProcessor& audioProcessor;
    FlyBy::UI::FlyByLookAndFeel customLnF;

    // Centerpiece Radar Display
    FlyBy::UI::Radar3DComponent radarComponent;

    // Flight Mode Selector
    juce::ComboBox flightModeCombo;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> flightModeAttachment;
    juce::Label flightModeLabel;

    // Trajectory Dials
    juce::Slider speedSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    juce::Label speedLabel;

    juce::Slider turbulenceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> turbulenceAttachment;
    juce::Label turbulenceLabel;

    juce::Slider dopplerSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dopplerAttachment;
    juce::Label dopplerLabel;

    juce::Slider proximitySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> proximityAttachment;
    juce::Label proximityLabel;

    // Spatial Placement Dials
    juce::Slider elevationSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> elevationAttachment;
    juce::Label elevationLabel;

    juce::Slider azimuthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> azimuthAttachment;
    juce::Label azimuthLabel;

    juce::Slider distanceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distanceAttachment;
    juce::Label distanceLabel;

    // Anthropometry & Physical Acoustics Tuning Dials
    juce::Slider pinnaScaleSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pinnaScaleAttachment;
    juce::Label pinnaScaleLabel;

    juce::Slider elevationStrengthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> elevationStrengthAttachment;
    juce::Label elevationStrengthLabel;

    juce::Slider crispnessSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crispnessAttachment;
    juce::Label crispnessLabel;

    juce::Slider groundingSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> groundingAttachment;
    juce::Label groundingLabel;

    juce::Slider dryWetSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dryWetAttachment;
    juce::Label dryWetLabel;

    void setupRotary(juce::Slider& slider, juce::Label& label, const juce::String& text,
                     const juce::String& suffix = "");

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlyByAudioProcessorEditor)
};
