#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>

class SilkenTofuLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SilkenTofuLookAndFeel()
    {
        setColour(juce::Slider::textBoxTextColourId,       juce::Colour(0xffffd27d));
        setColour(juce::Slider::textBoxOutlineColourId,    juce::Colour(0xff5e5243));
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff1c1815));
        setColour(juce::ComboBox::backgroundColourId,      juce::Colour(0xff1e1a17));
        setColour(juce::ComboBox::textColourId,            juce::Colour(0xfff5e8c8));
        setColour(juce::ComboBox::outlineColourId,         juce::Colour(0xff8c7a62));
        setColour(juce::PopupMenu::backgroundColourId,     juce::Colour(0xff1b1815));
        setColour(juce::PopupMenu::textColourId,           juce::Colour(0xfff5e8c8));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff524432));
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = juce::LookAndFeel_V4::createSliderTextBox(slider);
        l->setFont(juce::FontOptions(11.0f, juce::Font::bold));
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::backgroundColourId, juce::Colour(0xff1b1714));
        l->setColour(juce::Label::textColourId,       juce::Colour(0xffffd480));
        l->setColour(juce::Label::outlineColourId,    juce::Colour(0xff6b5d4d));
        return l;
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle,
                          const float rotaryEndAngle, juce::Slider& slider) override
    {
        const auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(2.0f);
        const auto radius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        const auto centre = bounds.getCentre();
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        const bool isLargeBakelite = (radius > 34.0f);

        // 1. Vintage Silkscreened Dial Ticks & Worn Halo around knob
        const float wearR = radius * 0.90f;
        juce::ColourGradient wearHalo(juce::Colour(0x281e160c), centre.x, centre.y,
                                      juce::Colour(0x001e160c), centre.x + wearR, centre.y + wearR, true);
        g.setGradientFill(wearHalo);
        g.fillEllipse(centre.x - wearR, centre.y - wearR, wearR * 2.0f, wearR * 2.0f);

        // 2. Deep 1970s Hardware Drop Shadow (drawn BEFORE ticks and value arc so shadow never covers the right-side arc!)
        const float knobR = radius * 0.71f;
        g.setColour(juce::Colours::black.withAlpha(0.50f));
        g.fillEllipse(centre.x - knobR + 2.0f, centre.y - knobR + 3.2f, knobR * 2.0f, knobR * 2.0f);

        const int numTicks = isLargeBakelite ? 11 : 9;
        for (int i = 0; i < numTicks; ++i)
        {
            const float t = static_cast<float>(i) / static_cast<float>(numTicks - 1);
            const float tickAngle = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
            const float rIn  = radius * 0.80f;
            const float rOut = radius * 0.96f;
            const float sx = std::sin(tickAngle);
            const float cy = -std::cos(tickAngle);

            g.setColour(juce::Colour(0xff1e1914).withAlpha((i % 2 == 0) ? 0.90f : 0.65f));
            g.drawLine(centre.x + sx * rIn, centre.y + cy * rIn,
                       centre.x + sx * rOut, centre.y + cy * rOut,
                       (i == 0 || i == numTicks / 2 || i == numTicks - 1) ? 2.2f : 1.3f);
        }

        // 3. Recessed Track Groove + Bright Warm Backlit Value Arc (drawn over shadow, never occluded)
        juce::Path trackArc;
        trackArc.addCentredArc(centre.x, centre.y, radius * 0.85f, radius * 0.85f,
                               0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(0xff1c1712).withAlpha(0.55f));
        g.strokePath(trackArc, juce::PathStrokeType(3.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path activeArc;
        activeArc.addCentredArc(centre.x, centre.y, radius * 0.85f, radius * 0.85f,
                                0.0f, rotaryStartAngle, angle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        g.strokePath(activeArc, juce::PathStrokeType(2.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 4. Scalloped / Knurled 1970s Bakelite Skirt
        juce::ColourGradient bodyGrad(juce::Colour(0xff38322d), centre.x - knobR * 0.55f, centre.y - knobR * 0.65f,
                                      juce::Colour(0xff120f0d), centre.x + knobR * 0.65f, centre.y + knobR * 0.75f, true);
        g.setGradientFill(bodyGrad);
        g.fillEllipse(centre.x - knobR, centre.y - knobR, knobR * 2.0f, knobR * 2.0f);

        const int numFlutes = isLargeBakelite ? 18 : 12;
        for (int i = 0; i < numFlutes; ++i)
        {
            const float fAng = angle + static_cast<float>(i) * (juce::MathConstants<float>::twoPi / static_cast<float>(numFlutes));
            const float fx1 = centre.x + std::sin(fAng) * (knobR * 0.64f);
            const float fy1 = centre.y - std::cos(fAng) * (knobR * 0.64f);
            const float fx2 = centre.x + std::sin(fAng) * (knobR * 0.98f);
            const float fy2 = centre.y - std::cos(fAng) * (knobR * 0.98f);

            g.setColour(juce::Colour(0xff090706).withAlpha(0.75f));
            g.drawLine(fx1, fy1, fx2, fy2, isLargeBakelite ? 2.2f : 1.5f);

            const float hAng = fAng + 0.05f;
            g.setColour(juce::Colour(0xff786d60).withAlpha(0.35f));
            g.drawLine(centre.x + std::sin(hAng) * (knobR * 0.70f),
                       centre.y - std::cos(hAng) * (knobR * 0.70f),
                       centre.x + std::sin(hAng) * (knobR * 0.98f),
                       centre.y - std::cos(hAng) * (knobR * 0.98f), 1.0f);
        }

        g.setColour(juce::Colour(0xff6e6356).withAlpha(0.60f));
        g.drawEllipse(centre.x - knobR, centre.y - knobR, knobR * 2.0f, knobR * 2.0f, 1.3f);

        // 5. Tarnished Spun-Brass / Aged Aluminum Center Insert Cap
        const float capR = knobR * 0.54f;
        juce::ColourGradient capGrad(juce::Colour(0xffdfd3bc), centre.x - capR * 0.8f, centre.y - capR * 0.8f,
                                     juce::Colour(0xff8c7d67), centre.x + capR * 0.9f, centre.y + capR * 0.9f, false);
        g.setGradientFill(capGrad);
        g.fillEllipse(centre.x - capR, centre.y - capR, capR * 2.0f, capR * 2.0f);

        g.setColour(juce::Colour(0xff3b3228).withAlpha(0.45f));
        g.drawEllipse(centre.x - capR, centre.y - capR, capR * 2.0f, capR * 2.0f, 1.2f);
        g.setColour(juce::Colour(0xfffff6e5).withAlpha(0.30f));
        g.drawEllipse(centre.x - capR * 0.65f, centre.y - capR * 0.65f, capR * 1.3f, capR * 1.3f, 0.8f);

        // 6. Aged Ivory / Yellowed Enamel Pointer Line
        juce::Path pointer;
        const float pLen = knobR * 0.94f;
        const float pIn  = capR * 0.22f;
        pointer.addRoundedRectangle(-1.9f, -pLen, 3.8f, pLen - pIn, 1.4f);
        g.setColour(juce::Colours::black.withAlpha(0.45f));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x + 1.0f, centre.y + 1.2f));
        g.setColour(juce::Colour(0xfff5e6c4));
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centre.x, centre.y));
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/) override
    {
        auto b = button.getLocalBounds().toFloat().reduced(1.5f);
        const bool on = button.getToggleState();

        g.setColour(juce::Colours::black.withAlpha(0.48f));
        g.fillRoundedRectangle(b.translated(0.0f, 2.0f), 4.0f);

        juce::ColourGradient bezelGrad(juce::Colour(0xff2c2824), b.getX(), b.getY(),
                                       juce::Colour(0xff171412), b.getX(), b.getBottom(), false);
        if (shouldDrawButtonAsHighlighted)
            bezelGrad = juce::ColourGradient(juce::Colour(0xff38332e), b.getX(), b.getY(),
                                             juce::Colour(0xff1f1b18), b.getX(), b.getBottom(), false);
        g.setGradientFill(bezelGrad);
        g.fillRoundedRectangle(b, 4.0f);

        g.setColour(on ? juce::Colour(0xffd49b4b) : juce::Colour(0xff6e6459));
        g.drawRoundedRectangle(b, 4.0f, 1.3f);

        // Faceted 1970s Amber Jewel Pilot Lamp on the left
        const float ledR = 4.2f;
        const float ledX = b.getX() + 10.5f;
        const float ledY = b.getCentreY();

        g.setColour(juce::Colour(0xff8c8275));
        g.fillEllipse(ledX - ledR - 1.2f, ledY - ledR - 1.2f, (ledR + 1.2f) * 2.0f, (ledR + 1.2f) * 2.0f);

        if (on)
        {
            juce::ColourGradient lampGlow(juce::Colour(0xffffdf70), ledX, ledY,
                                          juce::Colour(0xffd9531e), ledX + ledR, ledY + ledR, true);
            g.setGradientFill(lampGlow);
            g.fillEllipse(ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);

            g.setColour(juce::Colour(0x55ffae42));
            g.fillEllipse(ledX - ledR * 1.8f, ledY - ledR * 1.8f, ledR * 3.6f, ledR * 3.6f);
        }
        else
        {
            g.setColour(juce::Colour(0xff362218));
            g.fillEllipse(ledX - ledR, ledY - ledR, ledR * 2.0f, ledR * 2.0f);
        }

        g.setColour(on ? juce::Colour(0xfffff2d6) : juce::Colour(0xffdcd0bc));
        g.setFont(juce::FontOptions(10.2f, juce::Font::bold));
        b.removeFromLeft(19.0f);
        b.removeFromRight(3.0f);
        g.drawFittedText(button.getButtonText(), b.toNearestInt(), juce::Justification::centred, 1);
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& /*backgroundColour*/,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        auto b = button.getLocalBounds().toFloat().reduced(1.5f);
        g.setColour(juce::Colours::black.withAlpha(0.45f));
        g.fillRoundedRectangle(b.translated(0.0f, 1.8f), 4.0f);

        const juce::Colour topCol = shouldDrawButtonAsDown        ? juce::Colour(0xff191613)
                                  : shouldDrawButtonAsHighlighted ? juce::Colour(0xff38322c)
                                                                  : juce::Colour(0xff2a2520);
        const juce::Colour botCol = shouldDrawButtonAsDown        ? juce::Colour(0xff120f0d)
                                                                  : juce::Colour(0xff171411);
        juce::ColourGradient grad(topCol, b.getX(), b.getY(), botCol, b.getX(), b.getBottom(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(b, 4.0f);

        g.setColour(shouldDrawButtonAsHighlighted ? juce::Colour(0xffb59663) : juce::Colour(0xff786852));
        g.drawRoundedRectangle(b, 4.0f, 1.2f);
    }

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) override
    {
        g.setFont(juce::FontOptions(9.8f, juce::Font::bold));
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds().reduced(4, 1),
                         juce::Justification::centred, 1);
    }
};
