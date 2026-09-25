#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

namespace vts
{

class CarbonGoldLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CarbonGoldLookAndFeel();
    ~CarbonGoldLookAndFeel() override = default;

    // Custom slider drawing (Minimalist Vector Knobs)
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                           juce::Slider& slider) override;

    // Custom toggle/switch drawing
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // Custom combo box drawing (for NAB/CCIR)
    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH, 
                       juce::ComboBox& box) override;

    // Procedural UI Assets
    static void drawTapeReel (juce::Graphics& g, juce::Rectangle<float> bounds, float rotationAngle);
    static void drawVacuumTube (juce::Graphics& g, juce::Rectangle<float> bounds, float glowAmount);

    // Font definitions
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font& f) override;

    // Color Palette
    static const juce::Colour carbonMatte;
    static const juce::Colour carbonDark;
    static const juce::Colour goldAccent;
    static const juce::Colour goldHighlight;
    static const juce::Colour textOffWhite;
};

} // namespace vts
