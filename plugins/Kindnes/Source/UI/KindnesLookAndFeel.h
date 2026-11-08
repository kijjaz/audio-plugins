#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace KindnesUI
{

/**
 * Kindnes Custom LookAndFeel.
 * 
 * Fuses the repository's "Carbon & Gold" design system with
 * Japanese Famicom dark crimson (#8C1D24) and warm gold accents (#E5B869).
 */
class KindnesLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KindnesLookAndFeel()
    {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff121316));
        setColour(juce::Slider::thumbColourId, juce::Colour(0xffE5B869));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffA62432));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff22252A));
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff18191D));
        setColour(juce::ComboBox::textColourId, juce::Colour(0xffE5B869));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff3A3E48));
        setColour(juce::TextButton::buttonColourId, juce::Colour(0xff22252A));
        setColour(juce::TextButton::textColourOnId, juce::Colour(0xffE5B869));
        setColour(juce::TextButton::textColourOffId, juce::Colour(0xffD0D2D6));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(6.0f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        auto centre = bounds.getCentre();

        // 1. Outer Dark Chassis Ring
        g.setColour(juce::Colour(0xff1A1C20));
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

        // 2. Beveled Rim
        g.setColour(juce::Colour(0xff2E323B));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        // 3. Track Arc (Background)
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(centre.x, centre.y, radius * 0.82f, radius * 0.82f,
                                    0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff22252C));
        g.strokePath(backgroundArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 4. Value Arc (Famicom Crimson & Gold)
        if (sliderPosProportional > 0.001f)
        {
            juce::Path valueArc;
            valueArc.addCentredArc(centre.x, centre.y, radius * 0.82f, radius * 0.82f,
                                   0.0f, rotaryStartAngle, toAngle, true);
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(valueArc, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }

        // 5. Knob Cap Body
        auto knobRadius = radius * 0.68f;
        juce::ColourGradient knobGrad(juce::Colour(0xff2A2D35), centre.x - knobRadius, centre.y - knobRadius,
                                     juce::Colour(0xff141518), centre.x + knobRadius, centre.y + knobRadius, false);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        g.setColour(juce::Colour(0xff3D424E));
        g.drawEllipse(centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.0f);

        // 6. Pointer Needle (Brushed Gold)
        juce::Path pointer;
        auto pointerLength = knobRadius * 0.75f;
        auto pointerThickness = 2.5f;
        pointer.addRoundedRectangle(-pointerThickness * 0.5f, -knobRadius + 2.0f, pointerThickness, pointerLength, 1.0f);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(centre.x, centre.y));

        g.setColour(juce::Colour(0xffE5B869));
        g.fillPath(pointer);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto toggleArea = bounds.removeFromLeft(16.0f).withSizeKeepingCentre(14.0f, 14.0f);

        // Hardware LED switch indicator
        g.setColour(juce::Colour(0xff18191D));
        g.fillRoundedRectangle(toggleArea, 3.0f);
        g.setColour(button.getToggleState() ? juce::Colour(0xffE5B869) : juce::Colour(0xff3A3E48));
        g.drawRoundedRectangle(toggleArea, 3.0f, 1.2f);

        if (button.getToggleState())
        {
            auto inner = toggleArea.reduced(2.5f);
            g.setColour(juce::Colour(0xffE5B869));
            g.fillRoundedRectangle(inner, 1.5f);
        }

        // Text
        g.setColour(button.getToggleState() ? juce::Colour(0xffFFFFFF) : juce::Colour(0xffA0A4B0));
        g.setFont(juce::Font(10.0f, juce::Font::bold));
        g.drawText(button.getButtonText(), bounds.reduced(4.0f, 0.0f), juce::Justification::centredLeft, true);
    }
};

} // namespace KindnesUI
