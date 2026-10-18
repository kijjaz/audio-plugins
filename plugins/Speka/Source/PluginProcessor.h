#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>

#include "DSP/SpekaEngine.h"

class SpekaAudioProcessor : public juce::AudioProcessor
{
public:
    SpekaAudioProcessor();
    ~SpekaAudioProcessor() override;

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

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Visualizer & Telemetry getters for GUI
    float getConeExcursion() const { return currentExcursion.load(std::memory_order_relaxed); }
    bool isGateActive() const { return gateActive.load(std::memory_order_relaxed); }
    float getEffectiveBits() const { return effectiveBits.load(std::memory_order_relaxed); }
    float getTimerTicks() const { return timerTicks.load(std::memory_order_relaxed); }
    
    // Waveform buffer for Amber CRT oscilloscope (512 points)
    static constexpr int kCrtBufferSize = 512;
    void getCrtWaveform(std::array<float, kCrtBufferSize>& dest) const;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Factory Presets with Calibrated Loudness
    struct Preset
    {
        const char* name;
        int mode;
        float carrierFreq;
        int speaker;
        float driveDb;
        float caseRes;
        float rattle;
        float jitter;
        float sag;
        float whine;
        bool autoGate;
        float gateThreshDb;
        float trimDb;
    };

    static const std::vector<Preset>& getPresets();
    void loadPreset (int index);

private:
    juce::AudioProcessorValueTreeState apvts;
    int currentProgram = 0;

    // Stereo physical engines
    std::array<Speka::SpekaEngine, 2> engines;

    // Parameter pointers for low-latency block access
    std::atomic<float>* modeParam = nullptr;
    std::atomic<float>* carrierFreqParam = nullptr;
    std::atomic<float>* speakerTypeParam = nullptr;
    std::atomic<float>* driveParam = nullptr;
    std::atomic<float>* caseResonanceParam = nullptr;
    std::atomic<float>* rattleParam = nullptr;
    std::atomic<float>* jitterParam = nullptr;
    std::atomic<float>* sagParam = nullptr;
    std::atomic<float>* whineParam = nullptr;
    std::atomic<float>* autoGateParam = nullptr;
    std::atomic<float>* gateThreshParam = nullptr;
    std::atomic<float>* mixParam = nullptr;
    std::atomic<float>* trimParam = nullptr;

    // Telemetry state for GUI
    std::atomic<float> currentExcursion { 0.0f };
    std::atomic<bool> gateActive { false };
    std::atomic<float> effectiveBits { 6.0f };
    std::atomic<float> timerTicks { 63.0f };

    // Circular buffer for oscilloscope
    std::array<float, kCrtBufferSize> crtBuffer;
    std::atomic<int> crtBufferIndex { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpekaAudioProcessor)
};
