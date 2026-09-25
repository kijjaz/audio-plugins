#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

class FriedTofuAudioProcessor;

namespace FriedTofuUI
{

class ScopeComponent : public juce::Component, private juce::Timer
{
public:
    explicit ScopeComponent(FriedTofuAudioProcessor& processor);
    ~ScopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    FriedTofuAudioProcessor& audioProcessor;
    static constexpr int ScopePoints = 512;
    std::array<float, ScopePoints> displayBuffer {};
};

} // namespace FriedTofuUI
