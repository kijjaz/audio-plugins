#include "SpeakerViewComponent.h"
#include "RetroLookAndFeel.h"
#include <cmath>

#if __has_include(<BinaryData.h>)
    #include <BinaryData.h>
#elif __has_include("BinaryData.h")
    #include "BinaryData.h"
#endif

namespace Speka
{

SpeakerViewComponent::SpeakerViewComponent()
{
    loadAssets();
}

void SpeakerViewComponent::loadAssets()
{
#if defined (BinaryData_speaker_cone_png) || defined (JUCE_BINARYDATA_H_INCLUDED)
    coneImage = juce::ImageCache::getFromMemory (BinaryData::speaker_cone_png, BinaryData::speaker_cone_pngSize);
    piezoImage = juce::ImageCache::getFromMemory (BinaryData::piezo_buzzer_png, BinaryData::piezo_buzzer_pngSize);
#else
    // Fallback if BinaryData is defined differently
    int size = 0;
    const char* coneData = BinaryData::getNamedResource ("speaker_cone_png", size);
    if (coneData != nullptr && size > 0)
        coneImage = juce::ImageCache::getFromMemory (coneData, size);

    const char* piezoData = BinaryData::getNamedResource ("piezo_buzzer_png", size);
    if (piezoData != nullptr && size > 0)
        piezoImage = juce::ImageCache::getFromMemory (piezoData, size);
#endif
}

void SpeakerViewComponent::setSpeakerType (int type)
{
    if (speakerType != type)
    {
        speakerType = type;
        repaint();
    }
}

void SpeakerViewComponent::setExcursion (float exc)
{
    // Low-pass smoothing for kinetic bounce
    currentExcursion = exc;
    smoothedExcursion += 0.35f * (currentExcursion - smoothedExcursion);
    repaint();
}

void SpeakerViewComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. Recessed chamber chassis bevel (sunken bay inside beige plastic)
    g.setColour (RetroLookAndFeel::ibmBeigeShadow);
    g.drawRect (bounds, 1.5f);

    auto inner = bounds.reduced (2.0f);
    g.setColour (RetroLookAndFeel::ibmBeigeDark);
    g.drawRect (inner, 1.0f);

    auto cavity = inner.reduced (3.0f);

    // Deep cavity interior gradient (dark steel cabinet backplane)
    juce::ColourGradient cavityGrad (juce::Colour (0xff1e1e24), cavity.getCentreX(), cavity.getCentreY(),
                                     juce::Colour (0xff0a0a0c), cavity.getX(), cavity.getY(), true);
    g.setGradientFill (cavityGrad);
    g.fillRect (cavity);

    // Corner Phillips mounting screws
    auto drawScrew = [&g] (float x, float y) {
        g.setColour (juce::Colour (0xff151515));
        g.fillEllipse (x - 4.5f, y - 4.5f, 9.0f, 9.0f);
        g.setColour (juce::Colour (0xff888a8f));
        g.fillEllipse (x - 3.5f, y - 3.5f, 7.0f, 7.0f);
        g.setColour (juce::Colour (0xff222222));
        g.drawLine (x - 2.5f, y, x + 2.5f, y, 1.2f);
        g.drawLine (x, y - 2.5f, x, y + 2.5f, 1.2f);
    };

    drawScrew (cavity.getX() + 8.0f, cavity.getY() + 8.0f);
    drawScrew (cavity.getRight() - 8.0f, cavity.getY() + 8.0f);
    drawScrew (cavity.getX() + 8.0f, cavity.getBottom() - 24.0f);
    drawScrew (cavity.getRight() - 8.0f, cavity.getBottom() - 24.0f);

    // Speaker bounding area
    auto speakerArea = cavity.withTrimmedBottom (22.0f).reduced (8.0f);
    float baseSize = std::min (speakerArea.getWidth(), speakerArea.getHeight());
    auto center = speakerArea.getCentre();

    // Dynamic scale expansion representing physical cone excursion forward
    float excScale = 1.0f + juce::jlimit (0.0f, 0.12f, smoothedExcursion * 0.12f);
    float currentSize = baseSize * excScale;

    auto dynamicDest = juce::Rectangle<float> (center.x - currentSize * 0.5f,
                                              center.y - currentSize * 0.5f,
                                              currentSize, currentSize);

    // 2. Draw Transducer Image or Vector Fallback
    bool drewImage = false;
    if (speakerType == 0) // Paper Cone
    {
        if (coneImage.isValid())
        {
            // Transducer drop shadow
            g.setColour (juce::Colour (0x70000000));
            g.fillEllipse (dynamicDest.translated (0.0f, 4.0f));

            g.drawImage (coneImage, dynamicDest,
                         juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
            drewImage = true;
        }
    }
    else // Piezo Buzzer
    {
        if (piezoImage.isValid())
        {
            g.setColour (juce::Colour (0x60000000));
            g.fillEllipse (dynamicDest.translated (0.0f, 3.0f));

            g.drawImage (piezoImage, dynamicDest,
                         juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
            drewImage = true;
        }
    }

    if (!drewImage)
    {
        // High-fidelity procedural vector fallback
        float r = currentSize * 0.46f;
        if (speakerType == 0) // Paper Cone
        {
            // Outer basket ring
            g.setColour (juce::Colour (0xff333333));
            g.fillEllipse (center.x - r, center.y - r, r * 2.0f, r * 2.0f);

            // Paper cone ribs
            juce::ColourGradient coneGrad (juce::Colour (0xff252320), center.x, center.y,
                                           juce::Colour (0xff4a453f), center.x - r, center.y - r, true);
            g.setGradientFill (coneGrad);
            g.fillEllipse (center.x - r * 0.9f, center.y - r * 0.9f, r * 1.8f, r * 1.8f);

            // Dust cap
            float capR = r * 0.35f;
            g.setColour (juce::Colour (0xff181715));
            g.fillEllipse (center.x - capR, center.y - capR, capR * 2.0f, capR * 2.0f);
        }
        else // Piezo
        {
            // Brass outer disc
            g.setColour (juce::Colour (0xffb8974a));
            g.fillEllipse (center.x - r, center.y - r, r * 2.0f, r * 2.0f);

            // Ceramic inner disc
            float pieR = r * 0.72f;
            g.setColour (juce::Colour (0xffe8e9ea));
            g.fillEllipse (center.x - pieR, center.y - pieR, pieR * 2.0f, pieR * 2.0f);
        }
    }

    // 3. Iconic IBM PC Horizontal Ventilation Grille Slats (3D molded plastic)
    // Sits in front of the speaker inside the bezel!
    const int numSlats = 14;
    float slatStep = speakerArea.getHeight() / (float)numSlats;
    float slatThickness = 3.5f;

    for (int i = 0; i < numSlats; ++i)
    {
        float y = speakerArea.getY() + (float)i * slatStep + 2.0f;
        auto slatRect = juce::Rectangle<float> (speakerArea.getX() + 6.0f, y, speakerArea.getWidth() - 12.0f, slatThickness);

        // Dark cavity gap between slats is already transparent so cone is visible!
        // Slat cast shadow
        g.setColour (juce::Colour (0x66000000));
        g.drawHorizontalLine (juce::roundToInt (y + slatThickness), slatRect.getX(), slatRect.getRight());

        // Bevel highlight on front edge of plastic slat
        juce::ColourGradient slatGrad (RetroLookAndFeel::ibmBeigeDark.withAlpha (0.85f), slatRect.getX(), y,
                                       RetroLookAndFeel::ibmBeige.withAlpha (0.85f), slatRect.getRight(), y + slatThickness, false);
        g.setGradientFill (slatGrad);
        g.fillRoundedRectangle (slatRect, 1.0f);

        // Thin specular highlight
        g.setColour (RetroLookAndFeel::ibmBeigeLight.withAlpha (0.45f));
        g.drawHorizontalLine (juce::roundToInt (y), slatRect.getX(), slatRect.getRight());
    }

    // 4. Transducer Model Stamped Legend at bottom
    auto legendArea = cavity.removeFromBottom (20.0f);
    g.setColour (RetroLookAndFeel::ibmBeigeDark);
    g.setFont (juce::Font ("Courier New", 10.0f, juce::Font::bold));

    juce::String modelStr = (speakerType == 0)
        ? "TRANSDUCER: 2.25\" ALNICO PAPER CONE [8-OHM 0.5W]"
        : "TRANSDUCER: 12mm PIEZO BUZZER [4.0kHz RESONANCE]";

    g.drawText (modelStr, legendArea, juce::Justification::centred, false);
}

} // namespace Speka
