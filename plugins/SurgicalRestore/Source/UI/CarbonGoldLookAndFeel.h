#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace sr_ui
{

class CarbonGoldLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CarbonGoldLookAndFeel();
    ~CarbonGoldLookAndFeel() override = default;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                           juce::Slider& slider) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, 
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    // Palette
    static const juce::Colour carbonMatte;
    static const juce::Colour carbonDark;
    static const juce::Colour carbonSurface;
    static const juce::Colour goldAccent;
    static const juce::Colour goldHighlight;
    static const juce::Colour amberWarning;
    static const juce::Colour textOffWhite;
    static const juce::Colour textDim;
};

} // namespace sr_ui
