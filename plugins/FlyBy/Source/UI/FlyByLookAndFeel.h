#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace FlyBy::UI {

class FlyByLookAndFeel : public juce::LookAndFeel_V4 {
public:
    FlyByLookAndFeel();
    ~FlyByLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH,
                      juce::ComboBox& box) override;

    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    static const juce::Colour carbonDark;
    static const juce::Colour carbonPanel;
    static const juce::Colour carbonBorder;
    static const juce::Colour goldBezel;
    static const juce::Colour goldAccent;
    static const juce::Colour goldGlow;
    static const juce::Colour radarCyan;
    static const juce::Colour radarAmber;
    static const juce::Colour textMuted;
};

} // namespace FlyBy::UI
