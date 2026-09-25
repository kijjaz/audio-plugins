#include "LookAndFeel.h"

namespace FriedTofuUI
{

const juce::Colour FriedTofuLookAndFeel::bgDark         = juce::Colour::fromRGB(0x14, 0x18, 0x1a);
const juce::Colour FriedTofuLookAndFeel::panelBg        = juce::Colour::fromRGB(0x1b, 0x22, 0x24);
const juce::Colour FriedTofuLookAndFeel::panelBg2       = juce::Colour::fromRGB(0x1f, 0x27, 0x29);
const juce::Colour FriedTofuLookAndFeel::lineBorder     = juce::Colour::fromRGB(0x33, 0x40, 0x3f);
const juce::Colour FriedTofuLookAndFeel::copperGold     = juce::Colour::fromRGB(0xc9, 0x79, 0x3d);
const juce::Colour FriedTofuLookAndFeel::copperDim      = juce::Colour::fromRGB(0x7a, 0x4d, 0x2c);
const juce::Colour FriedTofuLookAndFeel::phosphorGreen  = juce::Colour::fromRGB(0x6f, 0xe6, 0xa0);
const juce::Colour FriedTofuLookAndFeel::textLight      = juce::Colour::fromRGB(0xe7, 0xe2, 0xd8);
const juce::Colour FriedTofuLookAndFeel::textMuted      = juce::Colour::fromRGB(0x7c, 0x8a, 0x87);

FriedTofuLookAndFeel::FriedTofuLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, bgDark);
    setColour(juce::Label::textColourId, textLight);
    setColour(juce::ComboBox::backgroundColourId, panelBg2);
    setColour(juce::ComboBox::textColourId, textLight);
    setColour(juce::ComboBox::outlineColourId, lineBorder);
    setColour(juce::PopupMenu::backgroundColourId, panelBg);
    setColour(juce::PopupMenu::textColourId, textLight);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, copperDim);
    setColour(juce::PopupMenu::highlightedTextColourId, textLight);
}

void FriedTofuLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPosProportional, float rotaryStartAngle,
                                            float rotaryEndAngle, juce::Slider& /*slider*/)
{
    auto radius = (float) std::min(width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width  * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Track arc
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(lineBorder);
    g.strokePath(backgroundArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active arc (Copper Gold)
    if (sliderPosProportional > 0.0f)
    {
        juce::Path activeArc;
        activeArc.addCentredArc(centreX, centreY, radius - 2.0f, radius - 2.0f, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(copperGold);
        g.strokePath(activeArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Dial body
    auto dialRadius = radius - 8.0f;
    g.setColour(panelBg2);
    g.fillEllipse(centreX - dialRadius, centreY - dialRadius, dialRadius * 2.0f, dialRadius * 2.0f);
    g.setColour(lineBorder);
    g.drawEllipse(centreX - dialRadius, centreY - dialRadius, dialRadius * 2.0f, dialRadius * 2.0f, 1.2f);

    // Indicator needle
    juce::Path p;
    auto pointerLength = dialRadius * 0.75f;
    p.addRoundedRectangle(-1.5f, -dialRadius, 3.0f, pointerLength, 1.0f);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(copperGold);
    g.fillPath(p);
}

void FriedTofuLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const juce::Slider::SliderStyle /*style*/, juce::Slider& /*slider*/)
{
    auto trackY = (float) y + (float) height * 0.5f;
    
    // Background track
    g.setColour(lineBorder);
    g.fillRoundedRectangle((float) x, trackY - 2.0f, (float) width, 4.0f, 2.0f);

    // Active track
    g.setColour(copperGold);
    g.fillRoundedRectangle((float) x, trackY - 2.0f, sliderPos - (float) x, 4.0f, 2.0f);

    // Thumb
    g.setColour(copperGold);
    g.fillRoundedRectangle(sliderPos - 6.0f, trackY - 7.0f, 12.0f, 14.0f, 2.0f);
    g.setColour(juce::Colour::fromRGB(0xff, 0xcf, 0xa0));
    g.drawRoundedRectangle(sliderPos - 6.0f, trackY - 7.0f, 12.0f, 14.0f, 2.0f, 1.0f);
}

void FriedTofuLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                       int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                       juce::ComboBox& /*box*/)
{
    auto r = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height).reduced(1.0f);
    g.setColour(panelBg2);
    g.fillRoundedRectangle(r, 2.0f);
    g.setColour(lineBorder);
    g.drawRoundedRectangle(r, 2.0f, 1.0f);

    // Arrow indicator
    juce::Path arrow;
    auto arrowX = (float) width - 16.0f;
    auto arrowY = (float) height * 0.5f - 2.0f;
    arrow.startNewSubPath(arrowX, arrowY);
    arrow.lineTo(arrowX + 8.0f, arrowY);
    arrow.lineTo(arrowX + 4.0f, arrowY + 5.0f);
    arrow.closeSubPath();
    g.setColour(textMuted);
    g.fillPath(arrow);
}

void FriedTofuLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& /*backgroundColour*/,
                                                bool shouldDrawButtonAsHighlighted,
                                                bool shouldDrawButtonAsDown)
{
    auto r = button.getLocalBounds().toFloat().reduced(0.5f);
    bool isToggled = button.getToggleState();

    if (isToggled)
    {
        g.setColour(juce::Colour::fromRGB(0x24, 0x1a, 0x12));
        g.fillRoundedRectangle(r, 2.0f);
        g.setColour(copperGold);
        g.drawRoundedRectangle(r, 2.0f, 1.2f);
    }
    else
    {
        g.setColour(shouldDrawButtonAsDown ? panelBg : (shouldDrawButtonAsHighlighted ? panelBg.brighter(0.08f) : panelBg2));
        g.fillRoundedRectangle(r, 2.0f);
        g.setColour(shouldDrawButtonAsHighlighted ? copperGold.withAlpha(0.6f) : lineBorder);
        g.drawRoundedRectangle(r, 2.0f, 1.0f);
    }
}

} // namespace FriedTofuUI
