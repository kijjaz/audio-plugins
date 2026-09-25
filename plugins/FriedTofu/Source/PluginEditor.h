#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/ScopeComponent.h"
#include "UI/TransferCurveComponent.h"

class FriedTofuAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit FriedTofuAudioProcessorEditor(FriedTofuAudioProcessor&);
    ~FriedTofuAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    FriedTofuAudioProcessor& audioProcessor;
    FriedTofuUI::FriedTofuLookAndFeel customLookAndFeel;

    // Visualizers
    FriedTofuUI::ScopeComponent scopeComponent;
    FriedTofuUI::TransferCurveComponent transferCurveComponent;

    // Sliders & Labels
    juce::Slider driveSlider;
    juce::Label driveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;

    juce::Slider networkZSlider;
    juce::Label networkZLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> networkZAttach;

    juce::ComboBox diodeACombo;
    juce::Label diodeALabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> diodeAAttach;

    juce::ComboBox diodeBCombo;
    juce::Label diodeBLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> diodeBAttach;

    juce::Slider toneSlider;
    juce::Label toneLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttach;

    juce::Slider mixSlider;
    juce::Label mixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach;

    juce::Slider trimSlider;
    juce::Label trimLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trimAttach;

    juce::ComboBox oversampleCombo;
    juce::Label oversampleLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> oversampleAttach;

    // Quick Presets
    juce::TextButton presetGlueSym { "Glue - SYM" };
    juce::TextButton presetMojoSym { "Mojo - SYM" };
    juce::TextButton presetAsym    { "Glue/Mojo - ASYM" };
    juce::TextButton presetGerm    { "Germanium Fuzz" };

    void applyPreset(int diodeA, int diodeB, float drive, float networkZ);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FriedTofuAudioProcessorEditor)
};
