#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace Speka
{

class RetroLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RetroLookAndFeel();
    ~RetroLookAndFeel() override = default;

    // Palette Constants (1980s IBM 5150 / Vintage Lab Aesthetic)
    static const juce::Colour ibmBeige;
    static const juce::Colour ibmBeigeDark;
    static const juce::Colour ibmBeigeLight;
    static const juce::Colour ibmBeigeShadow;
    static const juce::Colour chassisCharcoal;
    static const juce::Colour chassisBlack;
    static const juce::Colour crtAmber;
    static const juce::Colour crtAmberDim;
    static const juce::Colour crtGreen;
    static const juce::Colour ledRedOn;
    static const juce::Colour ledRedOff;

    void drawRotarySlider (juce::Graphics& g,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& slider) override;

    void drawComboBox (juce::Graphics& g,
                       int width, int height,
                       bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override;

    void drawPopupMenuBackground (juce::Graphics& g, int width, int height) override;

    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted,
                            bool isTicked, bool hasSubMenu,
                            const juce::String& text,
                            const juce::String& shortcutKeyText,
                            const juce::Drawable* icon,
                            const juce::Colour* textColour) override;

    void drawToggleButton (juce::Graphics& g,
                            juce::ToggleButton& button,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    juce::Font getComboBoxFont (juce::ComboBox& box) override;
    juce::Font getLabelFont (juce::Label& label) override;

    juce::Label* createComboBoxTextBox (juce::ComboBox& box) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RetroLookAndFeel)
};

} // namespace Speka
