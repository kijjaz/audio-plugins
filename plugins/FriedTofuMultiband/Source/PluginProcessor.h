#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/MultibandEngine.h"

class FriedTofuMBAudioProcessor : public juce::AudioProcessor
{
public:
    FriedTofuMBAudioProcessor();
    ~FriedTofuMBAudioProcessor() override;

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

    // APVTS
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    
    // Oscilloscope FIFO interface for UI
    static constexpr int ScopeBufferSize = 1024;
    void getScopeSamples(float* destBuffer, int numSamples);

    // Inspector helpers for current active band
    int getBandDiodeA(int band) const;
    int getBandDiodeB(int band) const;
    float getBandNetworkZ(int band) const;
    float getBandDriveDb(int band) const;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    FriedTofuDSP::MultibandEngine mbEngine;

    // Dry delay line to match linear-phase crossover latency for Dry/Wet Mix
    std::vector<std::vector<float>> dryDelayBuffer;
    size_t dryDelayIndex { 0 };

    // Lock-free scope buffer
    juce::AbstractFifo scopeFifo { ScopeBufferSize };
    std::array<float, ScopeBufferSize> scopeBuffer {};

    // Parameter pointers
    std::atomic<float>* masterDriveParam   { nullptr };
    std::atomic<float>* masterTrimParam    { nullptr };
    std::atomic<float>* masterMixParam     { nullptr };
    std::atomic<float>* crossoverLowParam  { nullptr };
    std::atomic<float>* crossoverHighParam { nullptr };

    struct BandParams
    {
        std::atomic<float>* drive    { nullptr };
        std::atomic<float>* networkZ { nullptr };
        std::atomic<float>* diodeA   { nullptr };
        std::atomic<float>* diodeB   { nullptr };
        std::atomic<float>* gain     { nullptr };
        std::atomic<float>* mute     { nullptr };
        std::atomic<float>* solo     { nullptr };
        std::atomic<float>* bypass   { nullptr };
    };

    std::array<BandParams, 3> bandParams;

    double currentSampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FriedTofuMBAudioProcessor)
};
