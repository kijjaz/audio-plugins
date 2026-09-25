#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/ScopeComponent.h"
#include "UI/TransferCurveComponent.h"

class MultibandCrossoverView;

class FriedTofuMBAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit FriedTofuMBAudioProcessorEditor(FriedTofuMBAudioProcessor&);
    ~FriedTofuMBAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int getSelectedBand() const noexcept { return selectedBand; }
    void setSelectedBand(int band);

private:
    void timerCallback() override;
    void updateBandControls();

    FriedTofuMBAudioProcessor& audioProcessor;
    FriedTofuUI::FriedTofuLookAndFeel customLookAndFeel;

    // Visualizers
    FriedTofuUI::ScopeComponent scopeComponent;
    FriedTofuUI::TransferCurveComponent transferCurveComponent;

    // Selected active band (0 = Low, 1 = Mid, 2 = High)
    int selectedBand { 1 };

    // Master Controls
    juce::Slider masterDriveSlider;
    juce::Slider masterTrimSlider;
    juce::Slider masterMixSlider;
    juce::Label masterDriveLabel;
    juce::Label masterTrimLabel;
    juce::Label masterMixLabel;

    // Crossover sliders
    juce::Slider crossoverLowSlider;
    juce::Slider crossoverHighSlider;
    juce::Label crossoverLowLabel;
    juce::Label crossoverHighLabel;

    // Band selector buttons
    juce::TextButton bandSelectButtons[3];

    // Band Controls for Selected Band
    juce::Slider bandDriveSlider;
    juce::Slider bandNetworkZSlider;
    juce::Slider bandGainSlider;
    juce::Label bandDriveLabel;
    juce::Label bandNetworkZLabel;
    juce::Label bandGainLabel;

    juce::ComboBox diodeACombo;
    juce::ComboBox diodeBCombo;
    juce::Label diodeALabel;
    juce::Label diodeBLabel;

    juce::ToggleButton muteToggle { "MUTE" };
    juce::ToggleButton soloToggle { "SOLO" };
    juce::ToggleButton bypassToggle { "BYPASS" };

    // Header labels
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterDriveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterTrimAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterMixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xoverLowAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xoverHighAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandDriveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandNetworkZAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bandGainAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> diodeAAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> diodeBAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> muteAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> soloAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FriedTofuMBAudioProcessorEditor)
};
