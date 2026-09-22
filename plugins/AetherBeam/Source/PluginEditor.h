#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/SpaceVisualizer.h"

class AetherBeamAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit AetherBeamAudioProcessorEditor(AetherBeamAudioProcessor&);
    ~AetherBeamAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updatePositionDropdown(int spaceIdx);
    void syncVisualizer();

    AetherBeamAudioProcessor& processorRef;

    // 3D Wireframe Visualizer
    SpaceVisualizer visualizer;

    // Space & Position Selectors
    juce::ComboBox spaceSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> spaceAttachment;

    juce::ComboBox positionSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> positionAttachment;

    // Quality / Performance Mode Selector
    juce::ComboBox qualitySelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> qualityAttachment;

    // Section 1: Air Dynamics
    juce::Slider betaSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> betaAttachment;
    juce::Label betaLabel;

    juce::Slider splSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> splAttachment;
    juce::Label splLabel;

    // Section 2: Material Damping & Reverb EQ
    juce::Slider decaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    juce::Label decayLabel;

    juce::Slider dampFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampFreqAttachment;
    juce::Label dampFreqLabel;

    juce::Slider hfMultSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hfMultAttachment;
    juce::Label hfMultLabel;

    juce::Slider bassMultSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassMultAttachment;
    juce::Label bassMultLabel;

    // Section 3: Master Output
    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    juce::Label mixLabel;

    int lastSpaceIndex = -1;
    int lastPosIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetherBeamAudioProcessorEditor)
};
