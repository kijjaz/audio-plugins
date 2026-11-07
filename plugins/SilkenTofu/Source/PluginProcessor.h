#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <array>
#include <memory>
#include "DSP/SilkenTofuDSP.h"

class SilkenTofuAudioProcessor : public juce::AudioProcessor
{
public:
    SilkenTofuAudioProcessor();
    ~SilkenTofuAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

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

    // Live UI Telemetry Accessors (Thread-safe atomics)
    float getGainReductionDb() const { return telemetryGrDb_.load(std::memory_order_relaxed); }
    float getElLuminescence() const  { return telemetryLum_.load(std::memory_order_relaxed); }
    float getShallowTrapN1() const   { return telemetryN1_.load(std::memory_order_relaxed); }
    float getDeepTrapN2() const      { return telemetryN2_.load(std::memory_order_relaxed); }
    float getBPlusVolts() const      { return telemetryBPlus_.load(std::memory_order_relaxed); }
    float getInputPeakDb() const     { return telemetryInDb_.load(std::memory_order_relaxed); }
    float getOutputPeakDb() const    { return telemetryOutDb_.load(std::memory_order_relaxed); }

    SilkenTofuDSP::Parameters readCurrentParameters() const;
    void applyPreset(int presetIndex);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState apvts_;
    int currentPresetIndex_ = 0;
    double currentSampleRate_ = 48000.0;

    // Stereo / Dual-Canister T4B + Tube/Transformer Engines
    SilkenTofuDSP::ChannelEngine engines_[2];

    // Selectable Polyphase Oversamplers (index 0 = 1x, 1 = 2x, 2 = 4x, 3 = 8x)
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 4> oversamplers_;
    juce::AudioBuffer<float> dryBuffer_;
    juce::AudioBuffer<float> t4bGainBuffer_;
    juce::AudioBuffer<float> scMonitorBuffer_;

    // Atomic UI Telemetry
    std::atomic<float> telemetryGrDb_  { 0.0f };
    std::atomic<float> telemetryLum_   { 0.0f };
    std::atomic<float> telemetryN1_    { 0.0f };
    std::atomic<float> telemetryN2_    { 0.0f };
    std::atomic<float> telemetryBPlus_ { 275.0f };
    std::atomic<float> telemetryInDb_  { -60.0f };
    std::atomic<float> telemetryOutDb_ { -60.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilkenTofuAudioProcessor)
};

