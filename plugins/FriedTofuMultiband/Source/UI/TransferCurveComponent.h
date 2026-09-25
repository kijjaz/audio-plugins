#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class FriedTofuMBAudioProcessor;
class FriedTofuMBAudioProcessorEditor;

namespace FriedTofuUI
{

class TransferCurveComponent : public juce::Component, private juce::Timer
{
public:
    explicit TransferCurveComponent(FriedTofuMBAudioProcessor& processor, FriedTofuMBAudioProcessorEditor& editor);
    ~TransferCurveComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateCurveData();

    FriedTofuMBAudioProcessor& audioProcessor;
    FriedTofuMBAudioProcessorEditor& editor;

    static constexpr int NumPoints = 128;
    std::vector<float> vInPoints;
    std::vector<float> vOutPoints;

    int lastDiodeA { -1 };
    int lastDiodeB { -1 };
    float lastNetworkZ { -1.0f };
    int lastBand { -1 };
};

} // namespace FriedTofuUI
