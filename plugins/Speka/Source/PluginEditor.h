#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <memory>

#include "PluginProcessor.h"
#include "UI/RetroLookAndFeel.h"
#include "UI/SpeakerViewComponent.h"
#include "UI/RetroCrtComponent.h"

class SpekaAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer
{
public:
    explicit SpekaAudioProcessorEditor (SpekaAudioProcessor&);
    ~SpekaAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    SpekaAudioProcessor& processorRef;
    Speka::RetroLookAndFeel retroLookAndFeel;

    // Sub-components
    Speka::SpeakerViewComponent speakerView;
    Speka::RetroCrtComponent crtView;

    // ComboBoxes
    juce::ComboBox presetBox;
    juce::ComboBox modeBox;
    juce::ComboBox speakerBox;

    juce::Label presetLabel;

    // Sliders
    juce::Slider carrierSlider;
    juce::Slider driveSlider;
    juce::Slider caseResonanceSlider;
    juce::Slider rattleSlider;
    juce::Slider jitterSlider;
    juce::Slider sagSlider;
    juce::Slider whineSlider;
    juce::Slider gateThreshSlider;
    juce::Slider mixSlider;
    juce::Slider trimSlider;

    // Button
    juce::ToggleButton autoGateToggle;

    // Labels
    juce::Label modeLabel;
    juce::Label speakerLabel;
    juce::Label carrierLabel;
    juce::Label driveLabel;
    juce::Label caseResLabel;
    juce::Label rattleLabel;
    juce::Label jitterLabel;
    juce::Label sagLabel;
    juce::Label whineLabel;
    juce::Label autoGateLabel;
    juce::Label gateThreshLabel;
    juce::Label mixLabel;
    juce::Label trimLabel;

    // Section group headers
    juce::Label sectionModLabel;
    juce::Label sectionPhysLabel;
    juce::Label sectionCircuitLabel;
    juce::Label sectionOutLabel;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ComboBoxAttachment> modeAttach;
    std::unique_ptr<ComboBoxAttachment> speakerAttach;
    std::unique_ptr<SliderAttachment> carrierAttach;
    std::unique_ptr<SliderAttachment> driveAttach;
    std::unique_ptr<SliderAttachment> caseResAttach;
    std::unique_ptr<SliderAttachment> rattleAttach;
    std::unique_ptr<SliderAttachment> jitterAttach;
    std::unique_ptr<SliderAttachment> sagAttach;
    std::unique_ptr<SliderAttachment> whineAttach;
    std::unique_ptr<ButtonAttachment> autoGateAttach;
    std::unique_ptr<SliderAttachment> gateThreshAttach;
    std::unique_ptr<SliderAttachment> mixAttach;
    std::unique_ptr<SliderAttachment> trimAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpekaAudioProcessorEditor)
};
