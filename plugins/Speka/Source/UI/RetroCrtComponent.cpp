#include "RetroCrtComponent.h"
#include "RetroLookAndFeel.h"
#include <cmath>

namespace Speka
{

RetroCrtComponent::RetroCrtComponent (SpekaAudioProcessor& proc)
    : processor (proc)
{
    waveformBuffer.fill (0.0f);
}

void RetroCrtComponent::updateTelemetry (int mode, float carrierHz, int speaker, bool gateActive, float excursion)
{
    currentMode = mode;
    currentCarrierHz = carrierHz;
    currentSpeaker = speaker;
    isGateArmed = gateActive;
    currentExcursion = excursion;

    processor.getCrtWaveform (waveformBuffer);
    repaint();
}

void RetroCrtComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. Molded dark CRT plastic monitor bezel
    g.setColour (RetroLookAndFeel::ibmBeigeShadow);
    g.drawRoundedRectangle (bounds, 8.0f, 2.0f);

    auto screenHousing = bounds.reduced (3.0f);
    g.setColour (juce::Colour (0xff141517));
    g.fillRoundedRectangle (screenHousing, 6.0f);

    auto screen = screenHousing.reduced (6.0f);

    // Deep curved Cathode Ray Tube phosphor glass gradient
    juce::Colour phosphorDark (0xff120901);
    juce::Colour phosphorMid (0xff241302);
    juce::Colour phosphorGlow (0xff381d03);

    juce::ColourGradient screenGrad (phosphorMid, screen.getCentreX(), screen.getCentreY(),
                                     phosphorDark, screen.getX(), screen.getY(), true);
    g.setGradientFill (screenGrad);
    g.fillRoundedRectangle (screen, 4.0f);

    // Subtle glass tube curvature highlight at top
    g.setColour (juce::Colour (0x1affffff));
    g.fillRoundedRectangle (screen.removeFromTop (screen.getHeight() * 0.18f), 4.0f);

    // 2. CRT Oscilloscope Reticle / Graticule
    auto crtArea = screen.reduced (4.0f);
    g.setColour (RetroLookAndFeel::crtAmberDim.withAlpha (0.12f));

    // Horizontal grid lines
    const int numDivH = 6;
    for (int i = 1; i < numDivH; ++i)
    {
        float y = crtArea.getY() + (crtArea.getHeight() * (float)i / (float)numDivH);
        g.drawHorizontalLine (juce::roundToInt (y), crtArea.getX(), crtArea.getRight());
    }

    // Vertical grid lines
    const int numDivV = 8;
    for (int i = 1; i < numDivV; ++i)
    {
        float x = crtArea.getX() + (crtArea.getWidth() * (float)i / (float)numDivV);
        g.drawVerticalLine (juce::roundToInt (x), crtArea.getY(), crtArea.getBottom());
    }

    // Center crosshair axis
    float midY = crtArea.getCentreY();
    g.setColour (RetroLookAndFeel::crtAmberDim.withAlpha (0.28f));
    g.drawHorizontalLine (juce::roundToInt (midY), crtArea.getX(), crtArea.getRight());

    // 3. Oscilloscope Waveform Trace (Dual pass: wide amber bloom + sharp bright core)
    juce::Path wavePath;
    float stepX = crtArea.getWidth() / (float)SpekaAudioProcessor::kCrtBufferSize;
    float traceScale = crtArea.getHeight() * 0.42f;

    for (int i = 0; i < SpekaAudioProcessor::kCrtBufferSize; ++i)
    {
        float x = crtArea.getX() + (float)i * stepX;
        float y = midY - (waveformBuffer[(size_t)i] * traceScale);
        y = juce::jlimit (crtArea.getY(), crtArea.getBottom(), y);

        if (i == 0)
            wavePath.startNewSubPath (x, y);
        else
            wavePath.lineTo (x, y);
    }

    // Pass 1: Wide phosphor persistence glow
    g.setColour (RetroLookAndFeel::crtAmber.withAlpha (0.20f));
    g.strokePath (wavePath, juce::PathStrokeType (4.0f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));

    // Pass 2: Intense amber beam
    g.setColour (RetroLookAndFeel::crtAmber.withAlpha (0.75f));
    g.strokePath (wavePath, juce::PathStrokeType (1.8f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));

    // Pass 3: White-hot phosphor core
    g.setColour (juce::Colour (0xfffff2b3));
    g.strokePath (wavePath, juce::PathStrokeType (0.9f));

    // 4. CRT Horizontal Scanlines (classic interlaced phosphor scan)
    g.setColour (juce::Colour (0x33000000));
    for (float y = screen.getY(); y < screen.getBottom(); y += 2.5f)
    {
        g.drawHorizontalLine (juce::roundToInt (y), screen.getX(), screen.getRight());
    }

    // 5. Authentic BIOS / DOS 8253 Real-Time Diagnostic Telemetry
    g.setFont (juce::Font ("Courier New", 10.5f, juce::Font::bold));

    // Header line
    auto topArea = screen.withTrimmedLeft (8.0f).withTrimmedTop (6.0f).withHeight (14.0f);
    g.setColour (RetroLookAndFeel::crtAmber);
    g.drawText ("SPEKA v0.1.0 - PIT-8253 TELEMETRY [1.193182 MHz]", topArea, juce::Justification::left, false);

    // Mode name
    const char* modeNames[] = { "REALSOUND PWM", "SIGMA-DELTA PDM", "DELTAMOD", "DIRECT 1-BIT" };
    juce::String modeStr = (currentMode >= 0 && currentMode < 4) ? modeNames[currentMode] : "UNKNOWN";

    float ticks = std::max (2.0f, std::floor (1193182.0f / std::max (100.0f, currentCarrierHz)));
    float bits = std::log2 (ticks);

    auto row1 = screen.withTrimmedLeft (8.0f).withTrimmedTop (22.0f).withHeight (14.0f);
    g.setColour (RetroLookAndFeel::crtAmberDim);
    g.drawText ("MODE    : " + modeStr, row1, juce::Justification::left, false);

    auto row2 = screen.withTrimmedLeft (8.0f).withTrimmedTop (36.0f).withHeight (14.0f);
    juce::String timerInfo = juce::String::formatted ("TIMER 2 : %.1f Hz [%.0f TICKS / %.1f BITS]",
                                                      currentCarrierHz, ticks, bits);
    g.drawText (timerInfo, row2, juce::Justification::left, false);

    auto row3 = screen.withTrimmedLeft (8.0f).withTrimmedTop (50.0f).withHeight (14.0f);
    juce::String gateStr = isGateArmed ? "PORT 61h: [VOICE ENABLED / ON]" : "PORT 61h: [SQUELCHED / SILENT]";
    g.setColour (isGateArmed ? RetroLookAndFeel::crtAmber : RetroLookAndFeel::crtAmberDim.withAlpha (0.6f));
    g.drawText (gateStr, row3, juce::Justification::left, false);

    // Excursion status bar at bottom
    auto bottomArea = screen.withTrimmedLeft (8.0f).withTrimmedRight (8.0f).withTrimmedBottom (6.0f).removeFromBottom (14.0f);
    int numBlocks = 12;
    int filledBlocks = juce::roundToInt (juce::jlimit (0.0f, 1.0f, currentExcursion) * (float)numBlocks);
    juce::String bar = "[";
    for (int b = 0; b < numBlocks; ++b)
        bar += (b < filledBlocks) ? "=" : " ";
    bar += "]";

    float excMm = currentExcursion * 0.8f; // ~0.8mm max excursion
    juce::String excStr = juce::String::formatted ("CONE X: %s +/-%.2f mm", bar.toRawUTF8(), excMm);
    g.setColour (RetroLookAndFeel::crtAmber);
    g.drawText (excStr, bottomArea, juce::Justification::left, false);
}

} // namespace Speka
