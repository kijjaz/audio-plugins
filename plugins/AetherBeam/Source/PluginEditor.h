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

    // Section 1: Air Dynamics
    juce::Slider betaSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> betaAttachment;
    juce::Label betaLabel;

    juce::Slider splSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> splAttachment;
    juce::Label splLabel;

    // Section 2: Material Damping & Occupancy EQ
    juce::Slider decaySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    juce::Label decayLabel;

    juce::Slider occupancySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> occupancyAttachment;
    juce::Label occupancyLabel;

    juce::Slider dampFreqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampFreqAttachment;
    juce::Label dampFreqLabel;

    juce::Slider hfMultSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hfMultAttachment;
    juce::Label hfMultLabel;

    juce::Slider bassMultSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassMultAttachment;
    juce::Label bassMultLabel;

    // Section 2B: ISO 9613-1 Microclimate & Surface Roughness
    juce::Slider tempSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tempAttachment;
    juce::Label tempLabel;

    juce::Slider humiditySlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> humidityAttachment;
    juce::Label humidityLabel;

    juce::Slider scatteringSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> scatteringAttachment;
    juce::Label scatteringLabel;

    // Section 3: Spatial Mic & Master Output
    juce::ComboBox micPatternSelector;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> micPatternAttachment;
    juce::Label micPatternLabel;

    juce::Slider stereoWidthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoWidthAttachment;
    juce::Label stereoWidthLabel;

    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    juce::Label mixLabel;

    int lastSpaceIndex = -1;
    int lastPosIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetherBeamAudioProcessorEditor)
};
