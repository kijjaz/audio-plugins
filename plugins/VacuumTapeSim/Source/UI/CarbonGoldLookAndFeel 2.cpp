#include "CarbonGoldLookAndFeel.h"

namespace vts
{

const juce::Colour CarbonGoldLookAndFeel::carbonMatte = juce::Colour(0xff2b2d30);
const juce::Colour CarbonGoldLookAndFeel::carbonDark = juce::Colour(0xff1e1f22);
const juce::Colour CarbonGoldLookAndFeel::goldAccent = juce::Colour(0xffd4af37);
const juce::Colour CarbonGoldLookAndFeel::goldHighlight = juce::Colour(0xfff3e5ab);
const juce::Colour CarbonGoldLookAndFeel::textOffWhite = juce::Colour(0xffe0e0e0);

CarbonGoldLookAndFeel::CarbonGoldLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, goldAccent);
    setColour(juce::Slider::rotarySliderOutlineColourId, carbonDark);
    setColour(juce::Slider::thumbColourId, goldHighlight);
    
    setColour(juce::Label::textColourId, textOffWhite);
    setColour(juce::ComboBox::backgroundColourId, carbonDark);
    setColour(juce::ComboBox::textColourId, goldAccent);
    setColour(juce::ComboBox::outlineColourId, goldAccent.withAlpha(0.3f));
    
    setColour(juce::PopupMenu::backgroundColourId, carbonDark);
    setColour(juce::PopupMenu::textColourId, textOffWhite);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, goldAccent.withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId, goldHighlight);
}

void CarbonGoldLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                              float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                                              juce::Slider& slider)
{
    auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
    auto centreX = (float) x + (float) width  * 0.5f;
    auto centreY = (float) y + (float) height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Track Background (Dark)
    g.setColour (carbonDark);
    g.fillEllipse (rx, ry, rw, rw);

    // Track Outline (Subtle Gold)
    g.setColour (goldAccent.withAlpha(0.2f));
    g.drawEllipse (rx, ry, rw, rw, 1.5f);
    
    // Active Track Arc
    juce::Path activeArc;
    activeArc.addCentredArc (centreX, centreY, radius - 1.0f, radius - 1.0f, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (goldAccent);
    g.strokePath (activeArc, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Pointer
    juce::Path p;
    auto pointerLength = radius * 0.6f;
    auto pointerThickness = 2.0f;
    p.addRectangle (-pointerThickness * 0.5f, -radius + 3.0f, pointerThickness, pointerLength);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    
    g.setColour (goldHighlight);
    g.fillPath (p);
}

void CarbonGoldLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    
    g.setColour(carbonDark);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    if (button.getToggleState())
    {
        g.setColour(goldAccent);
        g.drawRoundedRectangle(bounds, 4.0f, 1.5f);
        
        g.setColour(goldHighlight);
        g.setFont(12.0f);
        g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, false);
    }
    else
    {
        g.setColour(goldAccent.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        g.setColour(textOffWhite.withAlpha(0.5f));
        g.setFont(12.0f);
        g.drawText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, false);
    }
}

void CarbonGoldLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                          int buttonX, int buttonY, int buttonW, int buttonH, 
                                          juce::ComboBox& box)
{
    auto cornerSize = 3.0f;
    juce::Rectangle<int> boxBounds (0, 0, width, height);
    
    g.setColour (carbonDark);
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (goldAccent.withAlpha(0.5f));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
    
    juce::Path path;
    path.startNewSubPath ((float) buttonX + 3.0f, (float) buttonY + (float) buttonH * 0.35f);
    path.lineTo ((float) buttonX + (float) buttonW * 0.5f, (float) buttonY + (float) buttonH * 0.65f);
    path.lineTo ((float) buttonX + (float) buttonW - 3.0f, (float) buttonY + (float) buttonH * 0.35f);

    g.setColour (goldAccent);
    g.strokePath (path, juce::PathStrokeType (1.5f));
}

juce::Typeface::Ptr CarbonGoldLookAndFeel::getTypefaceForFont (const juce::Font& f)
{
    // In a real plugin we would load a custom TrueType font from BinaryData here.
    // For now, we fallback to default sans-serif.
    return juce::LookAndFeel_V4::getTypefaceForFont (f);
}

void CarbonGoldLookAndFeel::drawTapeReel (juce::Graphics& g, juce::Rectangle<float> bounds, float rotationAngle)
{
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float radius = bounds.getWidth() * 0.45f;
    
    // Save state for rotation
    juce::Graphics::ScopedSaveState state(g);
    g.addTransform(juce::AffineTransform::rotation(rotationAngle, cx, cy));

    // Outer rim
    g.setColour(carbonDark.brighter(0.05f));
    g.fillEllipse(cx - radius, cy - radius, radius * 2, radius * 2);
    
    g.setColour(goldAccent);
    g.drawEllipse(cx - radius, cy - radius, radius * 2, radius * 2, 2.0f);
    
    // Tape wound around the hub
    float tapeRadius = radius * 0.8f;
    g.setColour(carbonDark.darker(0.5f));
    g.fillEllipse(cx - tapeRadius, cy - tapeRadius, tapeRadius * 2, tapeRadius * 2);
    
    // Concentric tape lines
    g.setColour(carbonMatte.brighter(0.1f));
    for (int i = 0; i < 15; ++i)
    {
        float r = tapeRadius - (i * 2.5f);
        if (r > radius * 0.35f)
            g.drawEllipse(cx - r, cy - r, r * 2, r * 2, 0.5f);
    }
    
    // Spokes (5 spokes)
    g.setColour(goldAccent);
    for (int i = 0; i < 5; ++i)
    {
        juce::Path spoke;
        spoke.addPieSegment(cx - radius, cy - radius, radius * 2, radius * 2, 
                            -0.2f, 0.2f, 0.35f);
        
        juce::AffineTransform t = juce::AffineTransform::rotation(i * juce::MathConstants<float>::twoPi / 5.0f, cx, cy);
        spoke.applyTransform(t);
        
        g.fillPath(spoke);
        
        // Inner cutout for spoke
        juce::Path cutout;
        cutout.addPieSegment(cx - radius * 0.9f, cy - radius * 0.9f, radius * 1.8f, radius * 1.8f, 
                             -0.12f, 0.12f, 0.4f);
        cutout.applyTransform(t);
        g.setColour(carbonDark.brighter(0.05f));
        g.fillPath(cutout);
        g.setColour(goldAccent);
    }
    
    // Inner Hub
    float hubRadius = radius * 0.35f;
    g.setColour(goldHighlight);
    g.fillEllipse(cx - hubRadius, cy - hubRadius, hubRadius * 2, hubRadius * 2);
    
    // Center Hole and Screws
    g.setColour(carbonDark);
    g.fillEllipse(cx - hubRadius * 0.3f, cy - hubRadius * 0.3f, hubRadius * 0.6f, hubRadius * 0.6f);
    for (int i = 0; i < 3; ++i)
    {
        float sx = cx + std::sin(i * juce::MathConstants<float>::twoPi / 3.0f) * hubRadius * 0.6f;
        float sy = cy - std::cos(i * juce::MathConstants<float>::twoPi / 3.0f) * hubRadius * 0.6f;
        g.fillEllipse(sx - 3.0f, sy - 3.0f, 6.0f, 6.0f);
    }
}

void CarbonGoldLookAndFeel::drawVacuumTube (juce::Graphics& g, juce::Rectangle<float> bounds, float glowAmount)
{
    float cx = bounds.getCentreX();
    float cy = bounds.getCentreY();
    float w = bounds.getWidth() * 0.6f;
    float h = bounds.getHeight() * 0.9f;
    float x = cx - w / 2.0f;
    float y = cy - h / 2.0f;
    
    // Glow effect (rendered behind)
    if (glowAmount > 0.01f)
    {
        juce::ColourGradient glow(juce::Colour(0xffff9900).withAlpha(glowAmount * 0.8f), cx, cy,
                                  juce::Colours::transparentBlack, cx, cy - h * 0.8f, true);
        g.setGradientFill(glow);
        g.fillEllipse(cx - h, cy - h, h * 2, h * 2);
    }
    
    // Glass Bulb Envelope
    juce::Path bulb;
    bulb.startNewSubPath(x, y + h * 0.2f);
    bulb.lineTo(x, y + h * 0.9f);
    bulb.quadraticTo(x, y + h, x + w * 0.2f, y + h);
    bulb.lineTo(x + w * 0.8f, y + h);
    bulb.quadraticTo(x + w, y + h, x + w, y + h * 0.9f);
    bulb.lineTo(x + w, y + h * 0.2f);
    // Top dome and tip
    bulb.cubicTo(x + w, y - h * 0.1f, x + w * 0.6f, y, cx + w * 0.1f, y - h * 0.1f);
    bulb.lineTo(cx, y - h * 0.2f); // tip
    bulb.lineTo(cx - w * 0.1f, y - h * 0.1f);
    bulb.cubicTo(x + w * 0.4f, y, x, y - h * 0.1f, x, y + h * 0.2f);
    
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.fillPath(bulb);
    g.setColour(textOffWhite.withAlpha(0.3f));
    g.strokePath(bulb, juce::PathStrokeType(1.5f));
    
    // Internal Plates
    float px = cx - w * 0.35f;
    float py = cy - h * 0.3f;
    float pw = w * 0.7f;
    float ph = h * 0.5f;
    
    g.setColour(carbonDark.brighter(0.2f));
    g.fillRect(px, py, pw, ph);
    g.setColour(goldAccent.withAlpha(0.5f));
    g.drawRect(px, py, pw, ph, 1.0f);
    
    // Grid Lines inside plate
    for (int i = 1; i < 4; ++i)
    {
        float gx = px + (pw * i / 4.0f);
        g.drawLine(gx, py, gx, py + ph, 1.0f);
    }
    for (int i = 1; i < 6; ++i)
    {
        float gy = py + (ph * i / 6.0f);
        g.drawLine(px, gy, px + pw, gy, 1.0f);
    }
    
    // Heater filament (glows hot)
    g.setColour(juce::Colour(0xffffdd55).withAlpha(0.2f + glowAmount * 0.8f));
    g.drawLine(cx, py + h * 0.1f, cx, py + ph - h * 0.1f, 2.0f + glowAmount * 3.0f);
    
    // Pins
    g.setColour(goldAccent.withAlpha(0.7f));
    for (int i = 0; i < 7; ++i)
    {
        float pinX = x + w * 0.2f + (w * 0.6f * i / 6.0f);
        g.drawLine(pinX, y + h, pinX, y + h + h * 0.15f, 2.0f);
    }
}

} // namespace vts
