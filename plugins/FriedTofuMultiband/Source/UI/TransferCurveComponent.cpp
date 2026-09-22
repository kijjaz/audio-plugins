#include "TransferCurveComponent.h"
#include "../PluginProcessor.h"
#include "../PluginEditor.h"
#include "../DSP/WDFDiodeClipper.h"
#include "../DSP/DiodeModels.h"
#include "LookAndFeel.h"

namespace FriedTofuUI
{

TransferCurveComponent::TransferCurveComponent(FriedTofuMBAudioProcessor& p, FriedTofuMBAudioProcessorEditor& ed)
    : audioProcessor(p), editor(ed)
{
    vInPoints.resize(NumPoints);
    vOutPoints.resize(NumPoints);

    for (int i = 0; i < NumPoints; ++i)
    {
        float norm = static_cast<float>(i) / (NumPoints - 1);
        vInPoints[static_cast<size_t>(i)] = (norm * 2.0f - 1.0f) * 4.0f; // -4V to +4V
    }

    startTimerHz(20);
    updateCurveData();
}

TransferCurveComponent::~TransferCurveComponent()
{
    stopTimer();
}

void TransferCurveComponent::timerCallback()
{
    int currentBand = editor.getSelectedBand();
    int diodeA = audioProcessor.getBandDiodeA(currentBand);
    int diodeB = audioProcessor.getBandDiodeB(currentBand);
    float z    = audioProcessor.getBandNetworkZ(currentBand);

    if (diodeA != lastDiodeA || diodeB != lastDiodeB || std::abs(z - lastNetworkZ) > 1.0f || currentBand != lastBand)
    {
        lastDiodeA = diodeA;
        lastDiodeB = diodeB;
        lastNetworkZ = z;
        lastBand = currentBand;
        updateCurveData();
        repaint();
    }
}

void TransferCurveComponent::updateCurveData()
{
    int currentBand = editor.getSelectedBand();
    int diodeA = audioProcessor.getBandDiodeA(currentBand);
    int diodeB = audioProcessor.getBandDiodeB(currentBand);
    double z   = static_cast<double>(audioProcessor.getBandNetworkZ(currentBand));

    auto paramsA = FriedTofuDSP::AvailableDiodes[std::clamp(diodeA, 0, FriedTofuDSP::NumDiodeTypes - 1)];
    auto paramsB = FriedTofuDSP::AvailableDiodes[std::clamp(diodeB, 0, FriedTofuDSP::NumDiodeTypes - 1)];

    double warm = 0.0;
    for (int i = 0; i < NumPoints; ++i)
    {
        double vin = static_cast<double>(vInPoints[static_cast<size_t>(i)]);
        double vout = FriedTofuDSP::WDFDiodeClipper::solveStatic(vin, z, paramsA.Is, paramsA.N, paramsB.Is, paramsB.N, warm);
        vOutPoints[static_cast<size_t>(i)] = static_cast<float>(vout);
        warm = vout;
    }
}

void TransferCurveComponent::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    // Dark screen background
    g.setColour(juce::Colour(0xff0d1117));
    g.fillRoundedRectangle(bounds, 6.0f);

    // Subtle grid lines
    g.setColour(juce::Colour(0x2230363d));
    const float midX = bounds.getCentreX();
    const float midY = bounds.getCentreY();
    g.drawHorizontalLine(static_cast<int>(midY), bounds.getX(), bounds.getRight());
    g.drawVerticalLine(static_cast<int>(midX), bounds.getY(), bounds.getBottom());

    // Linear reference diagonal y = x
    g.setColour(juce::Colour(0x33ffffff));
    g.drawLine(bounds.getX(), bounds.getBottom(), bounds.getRight(), bounds.getY(), 1.0f);

    // Warm copper gold transfer curve
    g.setColour(FriedTofuLookAndFeel::copperGold);
    juce::Path curvePath;

    const float maxVin = 4.0f;
    const float maxVout = 4.0f;

    bool first = true;
    for (int i = 0; i < NumPoints; ++i)
    {
        float inNorm  = (vInPoints[static_cast<size_t>(i)] / maxVin) * 0.5f + 0.5f;
        float outNorm = (vOutPoints[static_cast<size_t>(i)] / maxVout) * 0.5f + 0.5f;

        float x = bounds.getX() + inNorm * bounds.getWidth();
        float y = bounds.getBottom() - outNorm * bounds.getHeight();

        if (first)
        {
            curvePath.startNewSubPath(x, y);
            first = false;
        }
        else
        {
            curvePath.lineTo(x, y);
        }
    }

    g.strokePath(curvePath, juce::PathStrokeType(2.0f));

    // Outer amber border
    g.setColour(juce::Colour(0x44d29922));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    // Watermark tag
    g.setColour(FriedTofuLookAndFeel::textMuted.withAlpha(0.6f));
    g.setFont(juce::Font(9.5f, juce::Font::bold));
    juce::String bandNames[3] = { "LOW BAND", "MID BAND", "HIGH BAND" };
    g.drawText(bandNames[std::clamp(editor.getSelectedBand(), 0, 2)] + " VOUT vs VIN", bounds.reduced(6.0f), juce::Justification::topLeft, false);
}

void TransferCurveComponent::resized()
{
}

} // namespace FriedTofuUI
