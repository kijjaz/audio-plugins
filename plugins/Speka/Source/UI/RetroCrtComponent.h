#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include "../PluginProcessor.h"

namespace Speka
{

class RetroCrtComponent : public juce::Component
{
public:
    explicit RetroCrtComponent (SpekaAudioProcessor& proc);
    ~RetroCrtComponent() override = default;

    void updateTelemetry (int mode, float carrierHz, int speaker, bool gateActive, float excursion);
    void paint (juce::Graphics& g) override;

private:
    SpekaAudioProcessor& processor;

    int currentMode = 0;
    float currentCarrierHz = 18900.0f;
    int currentSpeaker = 0;
    bool isGateArmed = false;
    float currentExcursion = 0.0f;

    std::array<float, SpekaAudioProcessor::kCrtBufferSize> waveformBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RetroCrtComponent)
};

} // namespace Speka
