#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/KindnesEngine.h"
#include "Audio/KindnesPresets.h"

class KindnesAudioProcessor : public juce::AudioProcessor
{
public:
    KindnesAudioProcessor();
    ~KindnesAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts_; }
    KindnesDSP::KindnesEngine& getEngine() { return engine_; }

    void loadPreset(int index);

    void setCustomArpPatternString(const juce::String& pattern);
    juce::String getCustomArpPatternString() const;
    juce::String getArpPatternStringForCurrentSetting() const;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateEngineParameters();

    juce::AudioProcessorValueTreeState apvts_;
    KindnesDSP::KindnesEngine engine_;

    int currentProgram_{ 0 };
    juce::String customArpPattern_{ "0, 4, 7, 12" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KindnesAudioProcessor)
};
