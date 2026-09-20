#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

namespace FriedTofuUI
{

class FriedTofuLookAndFeel : public juce::LookAndFeel_V4
{
public:
    FriedTofuLookAndFeel();
    ~FriedTofuLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override;

    // Palette Colors
    static const juce::Colour bgDark;
    static const juce::Colour panelBg;
    static const juce::Colour panelBg2;
    static const juce::Colour lineBorder;
    static const juce::Colour copperGold;
    static const juce::Colour copperDim;
    static const juce::Colour phosphorGreen;
    static const juce::Colour textLight;
    static const juce::Colour textMuted;
};

} // namespace FriedTofuUI
