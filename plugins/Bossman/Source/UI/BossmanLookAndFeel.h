#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include <cmath>

namespace bossman
{

class BossmanLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BossmanLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, goldAccent);
        setColour(juce::Slider::rotarySliderOutlineColourId, carbonDark);
        setColour(juce::Slider::thumbColourId, goldHighlight);
        
        setColour(juce::Label::textColourId, textOffWhite);
        setColour(juce::ComboBox::backgroundColourId, carbonDark);
        setColour(juce::ComboBox::textColourId, goldAccent);
        setColour(juce::ComboBox::outlineColourId, goldAccent.withAlpha(0.4f));
        
        setColour(juce::PopupMenu::backgroundColourId, carbonDark);
        setColour(juce::PopupMenu::textColourId, textOffWhite);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, goldAccent.withAlpha(0.25f));
        setColour(juce::PopupMenu::highlightedTextColourId, goldHighlight);
    }

    ~BossmanLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(4.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto cx = bounds.getCentreX();
        auto cy = bounds.getCentreY();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Recessed Outer Shadow
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.fillEllipse(cx - radius, cy - radius + 2.0f, radius * 2.0f, radius * 2.0f);

        // 2. 3D Metallic Brass/Gold Bevel Base
        juce::ColourGradient baseGrad(goldAccent.darker(0.6f), cx - radius * 0.4f, cy - radius * 0.4f,
                                      carbonDark.darker(0.8f), cx + radius, cy + radius, true);
        g.setGradientFill(baseGrad);
        g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

        // 3. Knurled Metal Rim Ridge
        g.setColour(goldAccent.withAlpha(0.35f));
        g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        // 4. Inner Dark Bakelite / Anodized Aluminum Cap
        float innerRadius = radius * 0.78f;
        juce::ColourGradient capGrad(juce::Colour(0xff2a2926), cx - innerRadius * 0.3f, cy - innerRadius * 0.3f,
                                     juce::Colour(0xff121110), cx + innerRadius * 0.8f, cy + innerRadius * 0.8f, true);
        g.setGradientFill(capGrad);
        g.fillEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.drawEllipse(cx - innerRadius, cy - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.0f);

        // 5. Active Value Arc Track
        juce::Path activeArc;
        activeArc.addCentredArc(cx, cy, radius - 1.5f, radius - 1.5f, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(goldAccent);
        g.strokePath(activeArc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 6. Vintage Pointer Line with Specular Core
        juce::Path pointer;
        float pLength = innerRadius * 0.85f;
        pointer.addRoundedRectangle(-1.5f, -innerRadius + 2.0f, 3.0f, pLength, 1.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(cx, cy));
        
        g.setColour(goldHighlight);
        g.fillPath(pointer);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, 
                      juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
        
        // Recessed Carbon panel fill
        g.setColour(carbonDark);
        g.fillRoundedRectangle(bounds, 4.0f);

        // Gold border
        g.setColour(goldAccent.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

        // Down Arrow
        juce::Path arrow;
        float ax = (float)buttonX + (float)buttonW * 0.4f;
        float ay = (float)buttonY + (float)buttonH * 0.45f;
        arrow.startNewSubPath(ax - 4.0f, ay - 2.0f);
        arrow.lineTo(ax, ay + 3.0f);
        arrow.lineTo(ax + 4.0f, ay - 2.0f);

        g.setColour(goldAccent);
        g.strokePath(arrow, juce::PathStrokeType(1.5f));
    }

    static void draw12AX7Tube(juce::Graphics& g, juce::Rectangle<float> bounds, float glowAmount)
    {
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY();
        float w = bounds.getWidth() * 0.55f;
        float h = bounds.getHeight() * 0.90f;
        float x = cx - w * 0.5f;
        float y = cy - h * 0.5f;

        // 1. Incandescent Filament Ambient Glow
        float glowClamp = juce::jlimit(0.0f, 1.0f, glowAmount);
        juce::Colour coreGlow = juce::Colour(0xffff941a).withAlpha(0.20f + 0.65f * glowClamp);

        juce::ColourGradient glow(coreGlow, cx, cy + h * 0.05f, juce::Colours::transparentBlack, cx, cy - h * 0.65f, true);
        g.setGradientFill(glow);
        g.fillEllipse(cx - w * 1.4f, cy - h * 0.5f, w * 2.8f, h * 1.4f);

        // 2. Glass Bulb Envelope (Curved vintage vacuum envelope)
        juce::Path bulb;
        bulb.startNewSubPath(x, y + h * 0.25f);
        bulb.lineTo(x, y + h * 0.88f);
        bulb.quadraticTo(x, y + h, x + w * 0.2f, y + h);
        bulb.lineTo(x + w * 0.8f, y + h);
        bulb.quadraticTo(x + w, y + h, x + w, y + h * 0.88f);
        bulb.lineTo(x + w, y + h * 0.25f);
        bulb.cubicTo(x + w, y - h * 0.06f, x + w * 0.6f, y, cx + w * 0.08f, y - h * 0.06f);
        bulb.lineTo(cx, y - h * 0.12f); // Glass seal tip
        bulb.lineTo(cx - w * 0.08f, y - h * 0.06f);
        bulb.cubicTo(x + w * 0.4f, y, x, y - h * 0.06f, x, y + h * 0.25f);

        // Glass volume subtle reflection
        g.setColour(juce::Colours::white.withAlpha(0.04f));
        g.fillPath(bulb);
        g.setColour(juce::Colour(0xffc0c5cc).withAlpha(0.35f));
        g.strokePath(bulb, juce::PathStrokeType(1.2f));

        // 3. Anode Grey Metal Plates (12AX7 Dual Triode Plates)
        float plateW = w * 0.60f;
        float plateH = h * 0.48f;
        float plateX = cx - plateW * 0.5f;
        float plateY = cy - plateH * 0.45f;

        g.setColour(juce::Colour(0xff22201d));
        g.fillRoundedRectangle(plateX, plateY, plateW, plateH, 2.0f);
        g.setColour(goldAccent.withAlpha(0.4f));
        g.drawRoundedRectangle(plateX, plateY, plateW, plateH, 2.0f, 1.0f);

        // Grid wing slots
        g.setColour(juce::Colour(0xff121110));
        for (int i = 1; i <= 3; ++i)
        {
            float slotY = plateY + (plateH * i / 4.0f);
            g.fillRect(plateX + 3.0f, slotY - 1.0f, plateW - 6.0f, 2.0f);
        }

        // 4. Hot Tungsten Heater Wire (Glowing center line)
        float wireThickness = 2.0f + 2.0f * glowClamp;
        g.setColour(juce::Colour(0xfffff4b8).withAlpha(0.4f + 0.6f * glowClamp));
        g.drawLine(cx, plateY + 4.0f, cx, plateY + plateH - 4.0f, wireThickness);

        // 5. Specular Glass Highlight Arch
        juce::Path specArc;
        specArc.startNewSubPath(x + 3.0f, y + h * 0.3f);
        specArc.quadraticTo(x + 3.0f, y + 4.0f, cx - w * 0.1f, y + 2.0f);
        g.setColour(juce::Colours::white.withAlpha(0.25f));
        g.strokePath(specArc, juce::PathStrokeType(1.2f));

        // 6. Base Octal/Noval Pins
        g.setColour(goldAccent.withAlpha(0.8f));
        for (int i = 0; i < 5; ++i)
        {
            float px = x + w * 0.22f + (w * 0.56f * i / 4.0f);
            g.drawLine(px, y + h, px, y + h + h * 0.12f, 1.8f);
        }
    }

    // Color Palette
    static inline const juce::Colour carbonDark    { 0xff141517 };
    static inline const juce::Colour carbonMatte   { 0xff1e2023 };
    static inline const juce::Colour carbonPanel   { 0xff25282c };
    static inline const juce::Colour goldAccent    { 0xffd4af37 };
    static inline const juce::Colour goldHighlight { 0xfff6e7b2 };
    static inline const juce::Colour textOffWhite  { 0xffeaebee };
    static inline const juce::Colour tweedGold     { 0xffc49a45 };
};

} // namespace bossman
