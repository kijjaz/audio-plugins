#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/LPCResidualEngine.h"
#include "DSP/ARInpainter.h"
#include "DSP/DeCrackleEngine.h"
#include "DSP/SpectralDeNoiser.h"
#include "DSP/RumbleFilter.h"

class SurgicalRestoreAudioProcessor : public juce::AudioProcessor
{
public:
    SurgicalRestoreAudioProcessor();
    ~SurgicalRestoreAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Surgical Restore"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Parameters
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Telemetry for UI
    std::atomic<float> clicksRepairedPerSec { 0.0f };
    std::atomic<float> noiseReductionMeter { 0.0f };

private:
    sr_dsp::RumbleFilter rumbleFilter[2];
    sr_dsp::LPCResidualEngine lpcEngine[2];
    sr_dsp::DeCrackleEngine decrackleEngine[2];
    sr_dsp::SpectralDeNoiser spectralDenoiser[2];

    int currentProgramIndex = 0;

    // Latency-compensation circular buffer for sample-accurate Delta auditioning
    static constexpr int latencySamples = 1024;
    juce::AudioBuffer<float> delayBuffer;
    int delayWritePos = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SurgicalRestoreAudioProcessor)
};
