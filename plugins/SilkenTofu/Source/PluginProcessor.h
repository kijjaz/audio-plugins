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

    // Live UI Telemetry Accessors (Thread-safe atomics, channel 0 = Left/Mid, channel 1 = Right/Side)
    float getGainReductionDb(int ch = 0) const { return telemetryGrDb_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getElLuminescence(int ch = 0) const  { return telemetryLum_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getShallowTrapN1(int ch = 0) const   { return telemetryN1_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getDeepTrapN2(int ch = 0) const      { return telemetryN2_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getBPlusVolts(int ch = 0) const      { return telemetryBPlus_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getInputPeakDb(int ch = 0) const     { return telemetryInDb_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }
    float getOutputPeakDb(int ch = 0) const    { return telemetryOutDb_[std::clamp(ch, 0, 1)].load(std::memory_order_relaxed); }

    int getVuMode() const;
    void setVuMode(int mode);

    // 0 = L/R Stereo, 1 = M/S (Editing Mid), 2 = M/S (Editing Side)
    int getChannelRoutingMode() const;
    void setChannelRoutingMode(int mode);
    void copyMidToSide();

    // A/B State Comparison Slots (0 = Slot A, 1 = Slot B)
    int getActiveABSlot() const { return activeABSlot_; }
    void setActiveABSlot(int slot);
    void copyActiveSlotToOther();

    SilkenTofuDSP::Parameters readCurrentParameters() const { return readChannelParameters(0); }
    SilkenTofuDSP::Parameters readChannelParameters(int channelIndex) const;
    void applyPreset(int presetIndex);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState apvts_;
    int currentPresetIndex_ = 0;
    int activeABSlot_ = 0;
    int abPresetIndex_[2] { 0, 0 };
    juce::ValueTree abStates_[2];
    double currentSampleRate_ = 48000.0;
    int lastReportedLatency_ = -1;

    // Stereo / Mid-Side Dual-Canister T4B + Tube/Transformer Engines (0 = Left/Mid, 1 = Right/Side)
    SilkenTofuDSP::ChannelEngine engines_[2];
    SilkenTofuDSP::SmoothedParam smStereoLink_;

    // Selectable Polyphase Oversamplers (index 0 = 1x, 1 = 2x, 2 = 4x, 3 = 8x)
    std::array<std::unique_ptr<juce::dsp::Oversampling<float>>, 4> oversamplers_;
    juce::AudioBuffer<float> t4bGainBuffer_;
    juce::AudioBuffer<float> scMonitorBuffer_;

    // Atomic UI Telemetry per channel (0 = Left/Mid, 1 = Right/Side)
    std::atomic<float> telemetryGrDb_[2]  { { 0.0f },   { 0.0f } };
    std::atomic<float> telemetryLum_[2]   { { 0.0f },   { 0.0f } };
    std::atomic<float> telemetryN1_[2]    { { 0.0f },   { 0.0f } };
    std::atomic<float> telemetryN2_[2]    { { 0.0f },   { 0.0f } };
    std::atomic<float> telemetryBPlus_[2] { { 275.0f }, { 275.0f } };
    std::atomic<float> telemetryInDb_[2]  { { -60.0f }, { -60.0f } };
    std::atomic<float> telemetryOutDb_[2] { { -60.0f }, { -60.0f } };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SilkenTofuAudioProcessor)
};
