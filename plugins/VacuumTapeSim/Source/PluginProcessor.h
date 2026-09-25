#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/TapeDSP.h"

class VacuumTapeSimAudioProcessor  : public juce::AudioProcessor
{
public:
    VacuumTapeSimAudioProcessor();
    ~VacuumTapeSimAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;
    
    // DSP Objects (one for Left, one for Right)
    vts::TapeDSP tapeDSP[2];

    struct Preset
    {
        juce::String name;
        float drive;
        float sag;
        float ips;
        float wow_flutter;
        float bias;
        float asymmetry;
        int eq_mode;
        bool auto_gain;
        float output;
        float mix;
        float hiss;
    };

    const std::vector<Preset>& getPresets() const { return presets; }
    void loadPreset(int index);

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    int currentProgram = 0;
    std::vector<Preset> presets;
    void initPresets();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VacuumTapeSimAudioProcessor)
};
