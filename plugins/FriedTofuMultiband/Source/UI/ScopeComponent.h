#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

class FriedTofuMBAudioProcessor;

namespace FriedTofuUI
{

class ScopeComponent : public juce::Component, private juce::Timer
{
public:
    explicit ScopeComponent(FriedTofuMBAudioProcessor& processor);
    ~ScopeComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    FriedTofuMBAudioProcessor& audioProcessor;
    static constexpr int ScopePoints = 512;
    std::array<float, ScopePoints> displayBuffer {};
};

} // namespace FriedTofuUI
