#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "UI/KindnesLookAndFeel.h"
#include "UI/APUVisualizer.h"

class KindnesAudioProcessorEditor : public juce::AudioProcessorEditor,
                                    public juce::FileDragAndDropTarget,
                                    private juce::Timer
{
public:
    explicit KindnesAudioProcessorEditor(KindnesAudioProcessor&);
    ~KindnesAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    void pushScopeSamples(const float* samples, int numSamples);
    void updateDpcmPreviews();

    // File drag and drop for DPCM sample loading
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

private:
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void setupComboBox(juce::ComboBox& box, juce::Label& label, const juce::String& text, const juce::StringArray& items);

    KindnesAudioProcessor& audioProcessor;
    KindnesUI::KindnesLookAndFeel lookAndFeel_;

    KindnesUI::APUVisualizer visualizer_;

    // Presets Bar
    juce::ComboBox presetBox_;
    juce::TextButton prevPresetBtn_{ "<" };
    juce::TextButton nextPresetBtn_{ ">" };

    // --- Bay 1: Core APU & Play Mode ---
    juce::ComboBox playModeBox_;
    juce::Label playModeLabel_;
    juce::ToggleButton authenticDacToggle_{ "Non-linear DAC" };

    juce::ComboBox pulse1DutyBox_;
    juce::Label pulse1DutyLabel_;
    juce::Slider pulse1VolSlider_;
    juce::Label pulse1VolLabel_;

    juce::ComboBox pulse2DutyBox_;
    juce::Label pulse2DutyLabel_;
    juce::Slider pulse2VolSlider_;
    juce::Label pulse2VolLabel_;

    juce::Slider triVolSlider_;
    juce::Label triVolLabel_;

    juce::Slider noiseVolSlider_;
    juce::Label noiseVolLabel_;
    juce::ComboBox noiseModeBox_;
    juce::Label noiseModeLabel_;

    juce::ToggleButton vrc6Toggle_{ "VRC6 Saw" };
    juce::Slider vrc6SawVolSlider_;
    juce::Label vrc6SawVolLabel_;

    // --- Bay 2: FDS 2C33 Wavetable & FM Unit ---
    juce::ToggleButton fdsToggle_{ "FDS 2C33 FM" };
    juce::ComboBox fdsWaveBox_;
    juce::Label fdsWaveLabel_;
    juce::Slider fdsVolSlider_;
    juce::Label fdsVolLabel_;
    juce::Slider fdsModSpeedSlider_;
    juce::Label fdsModSpeedLabel_;
    juce::Slider fdsModDepthSlider_;
    juce::Label fdsModDepthLabel_;
    juce::ComboBox fdsModWaveBox_;
    juce::Label fdsModWaveLabel_;

    // --- Bay 2b: Chiptune Embellishments ---
    juce::Slider scoopSemiSlider_;
    juce::Label scoopSemiLabel_;
    juce::Slider scoopTimeSlider_;
    juce::Label scoopTimeLabel_;
    juce::Slider delayedVibDelaySlider_;
    juce::Label delayedVibDelayLabel_;
    juce::Slider delayedVibDepthSlider_;
    juce::Label delayedVibDepthLabel_;
    juce::ToggleButton dutyMorphToggle_{ "Duty Morph" };
    juce::ToggleButton pseudoEchoToggle_{ "Pseudo Echo" };
    juce::Slider pseudoEchoDelaySlider_;
    juce::Label pseudoEchoDelayLabel_;
    juce::Slider pseudoEchoVolSlider_;
    juce::Label pseudoEchoVolLabel_;

    // --- Bay 3: Supercharged 1-Bit DPCM Sampler & Drum Pads ---
    juce::ComboBox dpcmSampleBox_;
    juce::Label dpcmSampleLabel_;
    juce::ToggleButton dpcmChromaticToggle_{ "Pitched Sample (Keys)" };
    juce::ToggleButton dpcmDrumPadsToggle_{ "MIDI Drum Kit (C-G)" };
    juce::ToggleButton throughDpcmToggle_{ "Live Input Crusher" };
    juce::TextButton demoBeatBtn_{ "AUTO BEAT" };
    juce::Slider dpcmDeltaStepSlider_;
    juce::Label dpcmDeltaStepLabel_;
    juce::Slider dpcmVelToSlewSlider_;
    juce::Label dpcmVelToSlewLabel_;
    juce::Slider dpcmRateSlider_;
    juce::Label dpcmRateLabel_;
    juce::Slider dpcmVolSlider_;
    juce::Label dpcmVolLabel_;
    std::array<juce::TextButton, 8> drumPads_;

    // --- Bay 4: Filter & Envelope ---
    juce::ComboBox filterModeBox_;
    juce::Label filterModeLabel_;
    juce::Slider cutoffSlider_;
    juce::Label cutoffLabel_;
    juce::Slider resonanceSlider_;
    juce::Label resonanceLabel_;

    juce::ToggleButton adsrToggle_{ "ADSR Envelope" };
    juce::Slider attackSlider_;
    juce::Label attackLabel_;
    juce::Slider decaySlider_;
    juce::Label decayLabel_;
    juce::Slider sustainSlider_;
    juce::Label sustainLabel_;
    juce::Slider releaseSlider_;
    juce::Label releaseLabel_;

    juce::ToggleButton analogFilterToggle_{ "Motherboard Filter" };
    juce::Slider toneSlider_;
    juce::Label toneLabel_;

    // --- Modulation, Arp & Master ---
    juce::ToggleButton arpToggle_{ "Arpeggiator" };
    juce::ComboBox arpRateBox_;
    juce::Label arpRateLabel_;
    juce::ComboBox arpPatternBox_;
    juce::Label arpPatternLabel_;
    juce::TextEditor arpPatternEditor_;
    juce::Label arpEditorLabel_;

    juce::Slider vibratoSlider_;
    juce::Label vibratoLabel_;
    juce::Slider pwmDepthSlider_;
    juce::Label pwmDepthLabel_;
    juce::Slider glideSlider_;
    juce::Label glideLabel_;

    juce::Slider detuneSlider_;
    juce::Label detuneLabel_;
    juce::Slider spreadSlider_;
    juce::Label spreadLabel_;
    juce::Slider masterTrimSlider_;
    juce::Label masterTrimLabel_;

    // APVTS Attachments
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::vector<std::unique_ptr<ComboBoxAttachment>> comboAttachments_;
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments_;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KindnesAudioProcessorEditor)
};
