#include "FlyByLookAndFeel.h"

namespace FlyBy::UI {

const juce::Colour FlyByLookAndFeel::carbonDark   = juce::Colour::fromRGB(18, 19, 22);
const juce::Colour FlyByLookAndFeel::carbonPanel  = juce::Colour::fromRGB(26, 28, 33);
const juce::Colour FlyByLookAndFeel::carbonBorder = juce::Colour::fromRGB(45, 48, 56);
const juce::Colour FlyByLookAndFeel::goldBezel    = juce::Colour::fromRGB(180, 142, 58);
const juce::Colour FlyByLookAndFeel::goldAccent   = juce::Colour::fromRGB(224, 185, 88);
const juce::Colour FlyByLookAndFeel::goldGlow     = juce::Colour::fromRGB(255, 220, 130);
const juce::Colour FlyByLookAndFeel::radarCyan    = juce::Colour::fromRGB(64, 224, 208);
const juce::Colour FlyByLookAndFeel::radarAmber   = juce::Colour::fromRGB(255, 175, 40);
const juce::Colour FlyByLookAndFeel::textMuted    = juce::Colour::fromRGB(150, 155, 168);

FlyByLookAndFeel::FlyByLookAndFeel() {
    setColour(juce::ResizableWindow::backgroundColourId, carbonDark);
    setColour(juce::ComboBox::backgroundColourId, carbonPanel);
    setColour(juce::ComboBox::textColourId, goldAccent);
    setColour(juce::ComboBox::outlineColourId, goldBezel.withAlpha(0.5f));
    setColour(juce::ComboBox::arrowColourId, goldAccent);
    setColour(juce::PopupMenu::backgroundColourId, carbonPanel);
    setColour(juce::PopupMenu::textColourId, juce::Colours::white);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, goldBezel.withAlpha(0.3f));
    setColour(juce::PopupMenu::highlightedTextColourId, goldGlow);
}

void FlyByLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPosProportional, float rotaryStartAngle,
                                       float rotaryEndAngle, juce::Slider& slider) {
    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(4.0f);
    auto radius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto center = bounds.getCentre();

    // Outer recessed shadow
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillEllipse(center.x - radius, center.y - radius + 2.0f, radius * 2.0f, radius * 2.0f);

    // Outer bezel ring (brushed gold/bronze)
    juce::ColourGradient bezelGrad(goldBezel.darker(0.3f), center.x - radius, center.y - radius,
                                   goldAccent, center.x + radius, center.y + radius, false);
    g.setGradientFill(bezelGrad);
    g.drawEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 2.0f);

    // Inner dial body (carbon slate)
    auto innerRadius = radius - 3.5f;
    juce::ColourGradient dialGrad(carbonPanel.brighter(0.15f), center.x - innerRadius * 0.5f, center.y - innerRadius * 0.5f,
                                  carbonDark.darker(0.4f), center.x + innerRadius, center.y + innerRadius, true);
    g.setGradientFill(dialGrad);
    g.fillEllipse(center.x - innerRadius, center.y - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

    // Arc track for current value
    float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    
    // Background arc track
    juce::Path bgArc;
    bgArc.addCentredArc(center.x, center.y, innerRadius - 2.5f, innerRadius - 2.5f, 0.0f,
                        rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(carbonBorder);
    g.strokePath(bgArc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Active value arc track (gold glow)
    juce::Path activeArc;
    activeArc.addCentredArc(center.x, center.y, innerRadius - 2.5f, innerRadius - 2.5f, 0.0f,
                           rotaryStartAngle, currentAngle, true);
    g.setColour(goldAccent);
    g.strokePath(activeArc, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Dial needle pointer (gold indicator)
    juce::Path needle;
    float needleLen = innerRadius * 0.72f;
    needle.startNewSubPath(center.x, center.y);
    needle.lineTo(center.x + needleLen * std::sin(currentAngle),
                  center.y - needleLen * std::cos(currentAngle));
    g.setColour(goldGlow);
    g.strokePath(needle, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Center pivot pip
    g.setColour(goldAccent);
    g.fillEllipse(center.x - 2.5f, center.y - 2.5f, 5.0f, 5.0f);
}

void FlyByLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   juce::ComboBox& /*box*/) {
    auto bounds = juce::Rectangle<float>(0, 0, width, height).reduced(1.0f);
    g.setColour(carbonPanel);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(goldBezel.withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.2f);

    // Chevron arrow
    float arrowX = width - 18.0f;
    float arrowY = height * 0.5f - 2.0f;
    juce::Path arrow;
    arrow.startNewSubPath(arrowX, arrowY);
    arrow.lineTo(arrowX + 5.0f, arrowY + 5.0f);
    arrow.lineTo(arrowX + 10.0f, arrowY);
    g.setColour(goldAccent);
    g.strokePath(arrow, juce::PathStrokeType(1.5f));
}

void FlyByLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    g.fillAll(label.findColour(juce::Label::backgroundColourId));
    if (!label.isBeingEdited()) {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(label.getFont());
        g.setFont(font);
        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                         juce::jmax(1, (int)((float)textArea.getHeight() / font.getHeight())),
                         label.getMinimumHorizontalScale());
    }
}

} // namespace FlyBy::UI
