#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

/**
 * @class CustomLookAndFeel
 * @brief Implements the "Carbon & Gold" aesthetic for Discrete808 v2.
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
  CustomLookAndFeel() {
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffb8860b)); // Gold
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffb8860b));
    setColour(juce::Slider::rotarySliderOutlineColourId,
              juce::Colour(0xff333333));
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff333333));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xffb8860b));
  }

  void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height,
                        float sliderPos, const float rotaryStartAngle,
                        const float rotaryEndAngle,
                        juce::Slider &slider) override {
    auto outline = slider.findColour(juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(juce::Slider::rotarySliderFillColourId);

    auto bounds =
        juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 3.0f;
    auto arcRadius = radius - lineW * 0.5f;

    // Dial Background
    g.setColour(outline.darker(0.2f));
    g.fillEllipse(bounds.getCentreX() - radius, bounds.getCentreY() - radius,
                  radius * 2, radius * 2);

    // Inner Glassy Circle
    juce::ColourGradient glass(juce::Colours::white.withAlpha(0.1f),
                               bounds.getCentreX(), bounds.getY(),
                               juce::Colours::black.withAlpha(0.2f),
                               bounds.getCentreX(), bounds.getBottom(), false);
    g.setGradientFill(glass);
    g.fillEllipse(bounds.reduced(4));

    // Arc Track
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                                rotaryEndAngle, true);
    g.setColour(outline);
    g.strokePath(backgroundArc,
                 juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));

    if (slider.isEnabled()) {
      juce::Path valueArc;
      valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                             arcRadius, arcRadius, 0.0f, rotaryStartAngle,
                             toAngle, true);
      g.setColour(fill);
      g.strokePath(valueArc,
                   juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));
    }

    // Thumb / Pointer
    auto thumbWidth = 4.0f;
    juce::Path p;
    p.addRectangle(-thumbWidth * 0.5f, -radius, thumbWidth, radius * 0.6f);
    p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(
        bounds.getCentreX(), bounds.getCentreY()));
    g.setColour(juce::Colours::white);
    g.fillPath(p);
  }
};
