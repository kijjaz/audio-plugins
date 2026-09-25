#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "DSP/AnthropometricModel.h"
#include "DSP/FlightPathGenerator.h"

class FlyByAudioProcessor : public juce::AudioProcessor {
public:
    FlyByAudioProcessor();
    ~FlyByAudioProcessor() override;

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

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Live coordinates for GUI radar rendering (thread-safe atomics)
    struct LiveFlightState {
        float posX = 0.0f;
        float posY = 1.0f;
        float posZ = 0.0f;
        float azimuthDeg = 0.0f;
        float elevationDeg = 0.0f;
        float distanceMeters = 1.0f;
    };
    LiveFlightState getLiveFlightState() const;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    FlyBy::DSP::AnthropometricModel spatialModel;
    FlyBy::DSP::FlightPathGenerator flightPathGen;

    std::atomic<float> currentX { 0.0f };
    std::atomic<float> currentY { 1.0f };
    std::atomic<float> currentZ { 0.0f };
    std::atomic<float> currentAz { 0.0f };
    std::atomic<float> currentEl { 0.0f };
    std::atomic<float> currentDist { 1.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlyByAudioProcessor)
};
