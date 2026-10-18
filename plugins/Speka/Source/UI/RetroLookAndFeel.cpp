#include "RetroLookAndFeel.h"
#include <cmath>

namespace Speka
{

const juce::Colour RetroLookAndFeel::ibmBeige        = juce::Colour (0xffd5cdba);
const juce::Colour RetroLookAndFeel::ibmBeigeDark    = juce::Colour (0xffb5ab95);
const juce::Colour RetroLookAndFeel::ibmBeigeLight   = juce::Colour (0xffeae4d4);
const juce::Colour RetroLookAndFeel::ibmBeigeShadow  = juce::Colour (0xff736954);
const juce::Colour RetroLookAndFeel::chassisCharcoal = juce::Colour (0xff1f2124);
const juce::Colour RetroLookAndFeel::chassisBlack    = juce::Colour (0xff101114);
const juce::Colour RetroLookAndFeel::crtAmber        = juce::Colour (0xffffb01a);
const juce::Colour RetroLookAndFeel::crtAmberDim     = juce::Colour (0x99ffb01a);
const juce::Colour RetroLookAndFeel::crtGreen        = juce::Colour (0xff33ff55);
const juce::Colour RetroLookAndFeel::ledRedOn        = juce::Colour (0xffff2222);
const juce::Colour RetroLookAndFeel::ledRedOff       = juce::Colour (0xff4d0f0f);

RetroLookAndFeel::RetroLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, ibmBeige);
    setColour (juce::Label::textColourId, chassisBlack);
    setColour (juce::ComboBox::backgroundColourId, chassisCharcoal);
    setColour (juce::ComboBox::textColourId, crtAmber);
    setColour (juce::ComboBox::outlineColourId, ibmBeigeShadow);
    setColour (juce::PopupMenu::backgroundColourId, chassisCharcoal);
    setColour (juce::PopupMenu::textColourId, ibmBeigeLight);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xff383c42));
    setColour (juce::PopupMenu::highlightedTextColourId, crtAmber);
}

void RetroLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                         int x, int y, int width, int height,
                                         float sliderPosProportional,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<float> ((float)x, (float)y, (float)width, (float)height);
    float size = std::min (bounds.getWidth(), bounds.getHeight());
    auto center = bounds.getCentre();
    float radius = (size * 0.5f) - 4.0f;

    if (radius < 5.0f)
        return;

    // 1. Cast shadow below knob
    g.setColour (juce::Colour (0x40000000));
    g.fillEllipse (center.x - radius, center.y - radius + 3.0f, radius * 2.0f, radius * 2.0f);

    // 2. Outer knurled/ribbed skirt (classic 1980s ribbed plastic)
    float skirtRadius = radius;
    juce::ColourGradient skirtGrad (ibmBeigeDark, center.x - skirtRadius, center.y - skirtRadius,
                                    ibmBeigeShadow, center.x + skirtRadius, center.y + skirtRadius, false);
    g.setGradientFill (skirtGrad);
    g.fillEllipse (center.x - skirtRadius, center.y - skirtRadius, skirtRadius * 2.0f, skirtRadius * 2.0f);

    // Rib notches around perimeter
    const int numRibs = 24;
    for (int i = 0; i < numRibs; ++i)
    {
        float angle = (float)i * (juce::MathConstants<float>::twoPi / (float)numRibs);
        float x1 = center.x + std::cos (angle) * (skirtRadius - 3.5f);
        float y1 = center.y + std::sin (angle) * (skirtRadius - 3.5f);
        float x2 = center.x + std::cos (angle) * skirtRadius;
        float y2 = center.y + std::sin (angle) * skirtRadius;

        g.setColour (ibmBeigeShadow.withAlpha (0.6f));
        g.drawLine (x1, y1, x2, y2, 1.2f);

        // Highlight beside notch
        float ha = angle + 0.04f;
        float hx1 = center.x + std::cos (ha) * (skirtRadius - 3.5f);
        float hy1 = center.y + std::sin (ha) * (skirtRadius - 3.5f);
        float hx2 = center.x + std::cos (ha) * skirtRadius;
        float hy2 = center.y + std::sin (ha) * skirtRadius;
        g.setColour (ibmBeigeLight.withAlpha (0.4f));
        g.drawLine (hx1, hy1, hx2, hy2, 0.8f);
    }

    // 3. Inner raised dial face
    float faceRadius = radius * 0.78f;
    juce::ColourGradient faceGrad (ibmBeigeLight, center.x - faceRadius * 0.6f, center.y - faceRadius * 0.6f,
                                   ibmBeige, center.x + faceRadius * 0.6f, center.y + faceRadius * 0.6f, false);
    g.setGradientFill (faceGrad);
    g.fillEllipse (center.x - faceRadius, center.y - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f);

    // Bevel ring around face
    g.setColour (ibmBeigeDark);
    g.drawEllipse (center.x - faceRadius, center.y - faceRadius, faceRadius * 2.0f, faceRadius * 2.0f, 1.5f);

    // 4. Pointer indicator notch / line
    float currentAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    float pStartX = center.x + std::sin (currentAngle) * (faceRadius * 0.25f);
    float pStartY = center.y - std::cos (currentAngle) * (faceRadius * 0.25f);
    float pEndX   = center.x + std::sin (currentAngle) * (faceRadius * 0.88f);
    float pEndY   = center.y - std::cos (currentAngle) * (faceRadius * 0.88f);

    // Shadow of indicator
    g.setColour (juce::Colour (0x55000000));
    g.drawLine (pStartX + 1.0f, pStartY + 1.0f, pEndX + 1.0f, pEndY + 1.0f, 2.5f);

    // Debossed indicator notch (dark charcoal or vintage burnt red indicator line)
    g.setColour (chassisBlack);
    g.drawLine (pStartX, pStartY, pEndX, pEndY, 2.4f);

    // Center indentation dimple
    float dimpleRadius = faceRadius * 0.15f;
    g.setColour (ibmBeigeShadow.withAlpha (0.4f));
    g.fillEllipse (center.x - dimpleRadius, center.y - dimpleRadius, dimpleRadius * 2.0f, dimpleRadius * 2.0f);
}

void RetroLookAndFeel::drawComboBox (juce::Graphics& g,
                                     int width, int height,
                                     bool /*isButtonDown*/,
                                     int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                     juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float)width, (float)height);

    // Recessed 3D bezel (sunken slot into chassis)
    g.setColour (ibmBeigeShadow);
    g.drawRect (bounds, 1.0f);

    g.setColour (chassisCharcoal);
    g.fillRect (bounds.reduced (1.0f));

    // Subtle drop shadow inside upper/left edge
    g.setColour (chassisBlack);
    g.drawHorizontalLine (1, 1.0f, (float)width - 1.0f);
    g.drawVerticalLine (1, 1.0f, (float)height - 1.0f);

    // Arrow down icon (vintage square-pixel arrow)
    float arrowX = (float)width - 16.0f;
    float arrowY = (float)height * 0.5f - 2.0f;

    juce::Path arrow;
    arrow.startNewSubPath (arrowX, arrowY);
    arrow.lineTo (arrowX + 8.0f, arrowY);
    arrow.lineTo (arrowX + 4.0f, arrowY + 5.0f);
    arrow.closeSubPath();

    g.setColour (box.isEnabled() ? crtAmber : ibmBeigeDark);
    g.fillPath (arrow);

    // Draw selected text directly in vivid CRT amber
    juce::String text = box.getText();
    if (text.isNotEmpty())
    {
        g.setColour (box.isEnabled() ? crtAmber : ibmBeigeDark);
        g.setFont (juce::Font ("Courier New", 12.0f, juce::Font::bold));
        auto textArea = bounds.reduced (8.0f, 0.0f).withTrimmedRight (20.0f);
        g.drawFittedText (text, textArea.toNearestInt(), juce::Justification::centredLeft, 1);
    }
}

void RetroLookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int width, int height)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float)width, (float)height);

    g.setColour (chassisCharcoal);
    g.fillRect (bounds);

    g.setColour (ibmBeigeShadow);
    g.drawRect (bounds, 1.0f);

    // Highlight top/left
    g.setColour (ibmBeigeLight.withAlpha (0.3f));
    g.drawHorizontalLine (0, 0.0f, (float)width);
    g.drawVerticalLine (0, 0.0f, (float)height);
}

void RetroLookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                         bool isSeparator, bool isActive, bool isHighlighted,
                                         bool isTicked, bool /*hasSubMenu*/,
                                         const juce::String& text,
                                         const juce::String& /*shortcutKeyText*/,
                                         const juce::Drawable* /*icon*/,
                                         const juce::Colour* textColour)
{
    if (isSeparator)
    {
        g.setColour (ibmBeigeDark.withAlpha (0.4f));
        g.drawHorizontalLine (area.getY() + area.getHeight() / 2, (float)area.getX() + 4.0f, (float)area.getRight() - 4.0f);
        return;
    }

    if (isHighlighted && isActive)
    {
        g.setColour (juce::Colour (0xff2e3238));
        g.fillRect (area);

        g.setColour (crtAmber.withAlpha (0.4f));
        g.drawRect (area, 1);
    }

    juce::Colour tc = isHighlighted ? crtAmber : (textColour != nullptr ? *textColour : ibmBeigeLight);
    if (!isActive)
        tc = tc.withAlpha (0.4f);

    g.setColour (tc);
    g.setFont (juce::Font ("Courier New", 12.0f, juce::Font::bold));

    auto textArea = area.reduced (8, 0);
    if (isTicked)
    {
        g.drawText (">", textArea.removeFromLeft (12), juce::Justification::centredLeft);
    }

    g.drawText (text, textArea, juce::Justification::centredLeft, true);
}

void RetroLookAndFeel::drawToggleButton (juce::Graphics& g,
                                         juce::ToggleButton& button,
                                         bool /*shouldDrawButtonAsHighlighted*/,
                                         bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    float switchW = 38.0f;
    float switchH = 20.0f;
    float switchX = bounds.getX() + 2.0f;
    float switchY = bounds.getCentreY() - switchH * 0.5f;

    // Recessed socket
    auto socketRect = juce::Rectangle<float> (switchX, switchY, switchW, switchH);
    g.setColour (chassisBlack);
    g.fillRoundedRectangle (socketRect, 3.0f);
    g.setColour (ibmBeigeShadow);
    g.drawRoundedRectangle (socketRect, 3.0f, 1.0f);

    bool isOn = button.getToggleState();

    // Rocker switch lever
    float leverW = switchW * 0.52f;
    float leverH = switchH - 4.0f;
    float leverX = isOn ? (switchX + switchW - leverW - 2.0f) : (switchX + 2.0f);
    float leverY = switchY + 2.0f;

    auto leverRect = juce::Rectangle<float> (leverX, leverY, leverW, leverH);

    juce::ColourGradient leverGrad (ibmBeigeLight, leverRect.getX(), leverRect.getY(),
                                    ibmBeigeDark, leverRect.getRight(), leverRect.getBottom(), false);
    g.setGradientFill (leverGrad);
    g.fillRoundedRectangle (leverRect, 2.0f);
    g.setColour (ibmBeigeDark);
    g.drawRoundedRectangle (leverRect, 2.0f, 1.0f);

    // LED status pip inside the switch
    float pipX = isOn ? (leverX + leverW * 0.5f - 3.0f) : (switchX + switchW - 10.0f);
    float pipY = switchY + switchH * 0.5f - 3.0f;

    if (isOn)
    {
        // Glow
        g.setColour (ledRedOn.withAlpha (0.4f));
        g.fillEllipse (pipX - 2.0f, pipY - 2.0f, 10.0f, 10.0f);
        g.setColour (ledRedOn);
        g.fillEllipse (pipX, pipY, 6.0f, 6.0f);
    }
    else
    {
        g.setColour (ledRedOff);
        g.fillEllipse (pipX, pipY, 6.0f, 6.0f);
    }

    // Button text
    g.setColour (chassisBlack);
    g.setFont (juce::Font ("Courier New", 12.0f, juce::Font::bold));
    auto textArea = bounds.withTrimmedLeft (switchW + 8.0f);
    g.drawText (button.getButtonText(), textArea, juce::Justification::centredLeft);
}

juce::Font RetroLookAndFeel::getComboBoxFont (juce::ComboBox& /*box*/)
{
    return juce::Font ("Courier New", 12.0f, juce::Font::bold);
}

juce::Font RetroLookAndFeel::getLabelFont (juce::Label& /*label*/)
{
    return juce::Font ("Courier New", 11.0f, juce::Font::bold);
}

juce::Label* RetroLookAndFeel::createComboBoxTextBox (juce::ComboBox& /*box*/)
{
    auto* l = new juce::Label (juce::String(), juce::String());
    l->setAlpha (0.0f);
    l->setInterceptsMouseClicks (false, false);
    return l;
}

void RetroLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId));

    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        auto textArea = label.getBorderSize().subtractedFrom (label.getLocalBounds());
        
        juce::Colour textCol = label.findColour (juce::Label::textColourId).withAlpha (alpha);
        bool isCombo = false;

        juce::String txt = label.getText();
        if (txt.contains ("RealSound") || txt.contains ("Sigma-Delta") 
            || txt.contains ("Delta Modulation") || txt.contains ("Direct 1-Bit")
            || txt.contains ("Paper Cone") || txt.contains ("Piezo"))
        {
            textCol = crtAmber.withAlpha (alpha);
            isCombo = true;
        }

        g.setFont (getLabelFont (label));

        // Only draw debossed shadow if text is dark on beige chassis
        if (!isCombo && textCol.getBrightness() < 0.25f)
        {
            g.setColour (ibmBeigeLight.withAlpha (0.6f * alpha));
            g.drawFittedText (label.getText(), textArea.translated (1, 1), label.getJustificationType(),
                              std::max (1, (int)textArea.getHeight() / 12), label.getMinimumHorizontalScale());
        }

        g.setColour (textCol);
        g.drawFittedText (label.getText(), textArea, label.getJustificationType(),
                          std::max (1, (int)textArea.getHeight() / 12), label.getMinimumHorizontalScale());
    }
}

} // namespace Speka
