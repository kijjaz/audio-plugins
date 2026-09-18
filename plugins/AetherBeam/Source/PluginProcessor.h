#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include <mutex>
#include "DSP/NonlinearWaveguideArray.h"
#include "DSP/BeamSeededFDN.h"
#include "DSP/CleverTailDetector.h"
#include "DSP/AcousticDefinitions.h"
#include "DSP/AcousticDatabase.h"

class AetherBeamAudioProcessor : public juce::AudioProcessor
{
public:
    AetherBeamAudioProcessor();
    ~AetherBeamAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "AetherBeam Reverb"; }
    const juce::String getProducer() const { return "Kijjaz"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 15.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    void switchSpaceAndPosition(int spaceIdx, int posIdx);
    void setCustomCoordinates(float srcXNorm, float srcYNorm, float lisXNorm, float lisYNorm);

    int getCurrentSpaceIndex() const { return currentSpaceIndex; }
    int getCurrentPositionIndex() const { return currentPositionIndex; }

    AetherAcoustics::Vec3 getCurrentSourcePos() const;
    AetherAcoustics::Vec3 getCurrentListenerPos() const;
    std::vector<AetherAcoustics::RaySegment> getCurrentRays() const;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    NonlinearWaveguideArray waveguideArray;
    BeamSeededFDN fdn;
    CleverTailDetector tailDetector;

    std::atomic<int> currentSpaceIndex{ 0 };
    std::atomic<int> currentPositionIndex{ 0 };

    std::atomic<float> targetSourceXNorm{ 0.5f };
    std::atomic<float> targetSourceYNorm{ 0.35f };
    std::atomic<float> targetListenerXNorm{ 0.5f };
    std::atomic<float> targetListenerYNorm{ 0.75f };

    std::atomic<bool> coordinatesDirty{ false };

    mutable std::mutex rayMutex;
    AetherAcoustics::Vec3 currentSourcePos{ 0.0f, 2.0f, 1.2f };
    AetherAcoustics::Vec3 currentListenerPos{ 0.0f, 8.0f, 1.7f };
    std::vector<AetherAcoustics::RaySegment> currentRays;

    float currentRt60 = 4.2f;
    float currentVolume = 57.0f;
    float currentArea = 92.0f;

    void updateAcousticPaths(const std::vector<AetherAcoustics::RaySegment>& rays);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AetherBeamAudioProcessor)
};
