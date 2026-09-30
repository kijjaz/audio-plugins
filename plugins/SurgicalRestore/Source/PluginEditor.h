#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/CarbonGoldLookAndFeel.h"

class SurgicalRestoreAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SurgicalRestoreAudioProcessorEditor (SurgicalRestoreAudioProcessor&);
    ~SurgicalRestoreAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SurgicalRestoreAudioProcessor& processorRef;
    sr_ui::CarbonGoldLookAndFeel customLookAndFeel;

    // Controls
    juce::Slider clickSensitivitySlider;
    juce::Slider crackleAmountSlider;
    juce::Slider hissReductionSlider;
    juce::Slider harmonicShieldSlider;

    juce::Label clickSensitivityLabel;
    juce::Label crackleAmountLabel;
    juce::Label hissReductionLabel;
    juce::Label harmonicShieldLabel;

    juce::ToggleButton deltaListenButton;
    juce::ToggleButton bypassButton;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clickSensAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> crackleAmtAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hissReductAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> harmShieldAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> deltaListenAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurgicalRestoreAudioProcessorEditor)
};
