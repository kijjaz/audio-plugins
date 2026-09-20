#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FriedTofuAudioProcessor;

namespace FriedTofuUI
{

class TransferCurveComponent : public juce::Component, private juce::Timer
{
public:
    explicit TransferCurveComponent(FriedTofuAudioProcessor& processor);
    ~TransferCurveComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateCurveData();

    FriedTofuAudioProcessor& audioProcessor;

    static constexpr int NumPoints = 128;
    std::vector<float> vInPoints;
    std::vector<float> vOutPoints;

    int lastDiodeA { -1 };
    int lastDiodeB { -1 };
    float lastNetworkZ { -1.0f };
};

} // namespace FriedTofuUI
