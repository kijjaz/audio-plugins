#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "DSP/WDFDiodeClipper.h"

class FriedTofuAudioProcessor : public juce::AudioProcessor
{
public:
    FriedTofuAudioProcessor();
    ~FriedTofuAudioProcessor() override;

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

    // Diode models info
    int getDiodeIndexA() const;
    int getDiodeIndexB() const;
    float getNetworkZOhms() const;
    float getDriveLinear() const;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP components per stereo channel
    FriedTofuDSP::WDFDiodeClipper diodeClippers[2];
    
    // 4x Oversampling engine
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler4x;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampler2x;

    // Post-clipping Tone filter (1-pole or 2nd order LPF)
    juce::dsp::IIR::Filter<float> toneFilters[2];

    // Lock-free scope buffer
    juce::AbstractFifo scopeFifo { ScopeBufferSize };
    std::array<float, ScopeBufferSize> scopeBuffer {};

    // Cached parameter atomic values
    std::atomic<float>* driveParam       { nullptr };
    std::atomic<float>* networkZParam    { nullptr };
    std::atomic<float>* diodeAParam      { nullptr };
    std::atomic<float>* diodeBParam      { nullptr };
    std::atomic<float>* toneParam        { nullptr };
    std::atomic<float>* mixParam         { nullptr };
    std::atomic<float>* trimParam        { nullptr };
    std::atomic<float>* oversampleParam  { nullptr };

    double currentSampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FriedTofuAudioProcessor)
};
