#include "CarbonGoldLookAndFeel.h"

namespace sr_ui
{

const juce::Colour CarbonGoldLookAndFeel::carbonMatte   = juce::Colour (0xff141416);
const juce::Colour CarbonGoldLookAndFeel::carbonDark    = juce::Colour (0xff0a0a0c);
const juce::Colour CarbonGoldLookAndFeel::carbonSurface = juce::Colour (0xff1f2024);
const juce::Colour CarbonGoldLookAndFeel::goldAccent    = juce::Colour (0xffd4af37);
const juce::Colour CarbonGoldLookAndFeel::goldHighlight = juce::Colour (0xfff3e5ab);
const juce::Colour CarbonGoldLookAndFeel::amberWarning  = juce::Colour (0xffffa000);
const juce::Colour CarbonGoldLookAndFeel::textOffWhite  = juce::Colour (0xffe8e8e8);
const juce::Colour CarbonGoldLookAndFeel::textDim       = juce::Colour (0xff8a8a92);

CarbonGoldLookAndFeel::CarbonGoldLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, carbonDark);
    setColour (juce::Label::textColourId, textOffWhite);
}

void CarbonGoldLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (8.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto centre = bounds.getCentre();

    // Outer Bezel
    g.setColour (carbonSurface);
    g.fillEllipse (bounds);

    g.setColour (carbonDark);
    g.drawEllipse (bounds, 1.5f);

    // Inner Disc
    auto innerBounds = bounds.reduced (4.0f);
    juce::ColourGradient grad (carbonSurface.brighter (0.1f), centre.x, innerBounds.getY(),
                               carbonDark, centre.x, innerBounds.getBottom(), false);
    g.setGradientFill (grad);
    g.fillEllipse (innerBounds);

    // Gold Progress Arc
    juce::Path arc;
    arc.addCentredArc (centre.x, centre.y, radius - 2.0f, radius - 2.0f,
                       0.0f, rotaryStartAngle, toAngle, true);
    g.setColour (goldAccent);
    g.strokePath (arc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Pointer Dot / Notch
    auto pointerLength = radius * 0.75f;
    juce::Point<float> pointerPos (centre.x + pointerLength * std::sin (toAngle),
                                   centre.y - pointerLength * std::cos (toAngle));
    g.setColour (goldHighlight);
    g.fillEllipse (pointerPos.x - 2.5f, pointerPos.y - 2.5f, 5.0f, 5.0f);
}

void CarbonGoldLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    bool isOn = button.getToggleState();

    g.setColour (isOn ? goldAccent.withAlpha (0.15f) : carbonSurface);
    g.fillRoundedRectangle (bounds, 4.0f);

    g.setColour (isOn ? goldAccent : carbonSurface.brighter (0.2f));
    g.drawRoundedRectangle (bounds, 4.0f, 1.2f);

    g.setColour (isOn ? goldHighlight : textDim);
    g.setFont (12.0f);
    g.drawFittedText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
}

} // namespace sr_ui
