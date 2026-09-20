#include "TransferCurveComponent.h"
#include "../PluginProcessor.h"
#include "../DSP/WDFDiodeClipper.h"
#include "LookAndFeel.h"

namespace FriedTofuUI
{

TransferCurveComponent::TransferCurveComponent(FriedTofuAudioProcessor& processor)
    : audioProcessor(processor)
{
    vInPoints.resize(NumPoints);
    vOutPoints.resize(NumPoints);

    updateCurveData();
    startTimerHz(15); // Check for parameter changes at 15 Hz
}

TransferCurveComponent::~TransferCurveComponent()
{
    stopTimer();
}

void TransferCurveComponent::timerCallback()
{
    int currentDiodeA = audioProcessor.getDiodeIndexA();
    int currentDiodeB = audioProcessor.getDiodeIndexB();
    float currentZ    = audioProcessor.getNetworkZOhms();

    if (currentDiodeA != lastDiodeA || currentDiodeB != lastDiodeB || std::abs(currentZ - lastNetworkZ) > 1.0f)
    {
        updateCurveData();
        repaint();
    }
}

void TransferCurveComponent::updateCurveData()
{
    lastDiodeA    = audioProcessor.getDiodeIndexA();
    lastDiodeB    = audioProcessor.getDiodeIndexB();
    lastNetworkZ  = audioProcessor.getNetworkZOhms();

    const auto& paramA = FriedTofuDSP::AvailableDiodes[std::clamp(lastDiodeA, 0, FriedTofuDSP::NumDiodeTypes - 1)];
    const auto& paramB = FriedTofuDSP::AvailableDiodes[std::clamp(lastDiodeB, 0, FriedTofuDSP::NumDiodeTypes - 1)];

    double warm = 0.0;
    for (int i = 0; i < NumPoints; ++i)
    {
        // Vin swept from -3.0V to +3.0V
        double vIn = -3.0 + 6.0 * static_cast<double>(i) / static_cast<double>(NumPoints - 1);
        double vOut = FriedTofuDSP::WDFDiodeClipper::solveStatic(
            vIn, lastNetworkZ, paramA.Is, paramA.N, paramB.Is, paramB.N, warm);
        warm = vOut;

        vInPoints[static_cast<size_t>(i)]  = static_cast<float>(vIn);
        vOutPoints[static_cast<size_t>(i)] = static_cast<float>(vOut);
    }
}

void TransferCurveComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Dark screen background
    g.setColour(juce::Colour::fromRGB(0x0e, 0x12, 0x13));
    g.fillRoundedRectangle(bounds, 2.0f);

    // Border
    g.setColour(FriedTofuLookAndFeel::lineBorder);
    g.drawRoundedRectangle(bounds, 2.0f, 1.0f);

    const float w = bounds.getWidth();
    const float h = bounds.getHeight();

    // Center crosshairs (0V In, 0V Out)
    g.setColour(FriedTofuLookAndFeel::lineBorder.withAlpha(0.6f));
    g.drawHorizontalLine(static_cast<int>(h * 0.5f), 0.0f, w);
    g.drawVerticalLine(static_cast<int>(w * 0.5f), 0.0f, h);

    // Diagonal unity line (dashed)
    juce::Path unityPath;
    unityPath.startNewSubPath(0.0f, h);
    unityPath.lineTo(w, 0.0f);
    
    const float dashes[] = { 4.0f, 4.0f };
    juce::Path dashedUnityPath;
    juce::PathStrokeType dashedStroke(1.0f);
    dashedStroke.createDashedStroke(dashedUnityPath, unityPath, dashes, 2);
    g.setColour(FriedTofuLookAndFeel::lineBorder.brighter(0.2f));
    g.strokePath(dashedUnityPath, dashedStroke);

    // Transfer curve path (Vout vs Vin)
    // Domain: -3V to +3V, Range: -3V to +3V
    juce::Path curvePath;
    for (int i = 0; i < NumPoints; ++i)
    {
        float x = w * static_cast<float>(i) / static_cast<float>(NumPoints - 1);
        // Normalize -3..+3 to 0..h (inverted Y)
        float normY = (vOutPoints[static_cast<size_t>(i)] + 3.0f) / 6.0f;
        float y = h * (1.0f - std::clamp(normY, 0.0f, 1.0f));

        if (i == 0)
            curvePath.startNewSubPath(x, y);
        else
            curvePath.lineTo(x, y);
    }

    // Glow
    g.setColour(FriedTofuLookAndFeel::copperGold.withAlpha(0.25f));
    g.strokePath(curvePath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Sharp line
    g.setColour(FriedTofuLookAndFeel::copperGold);
    g.strokePath(curvePath, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void TransferCurveComponent::resized()
{
}

} // namespace FriedTofuUI
