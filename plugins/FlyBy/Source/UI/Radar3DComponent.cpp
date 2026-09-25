#include "Radar3DComponent.h"
#include <cmath>

namespace FlyBy::UI {

Radar3DComponent::Radar3DComponent(FlyByAudioProcessor& processor_)
    : processor(processor_)
{
    startTimerHz(30); // 30 FPS HUD refresh
}

Radar3DComponent::~Radar3DComponent() {
    stopTimer();
}

void Radar3DComponent::resized() {
    auto bounds = getLocalBounds().reduced(8);
    int halfWidth = (bounds.getWidth() - 10) / 2;
    azimuthBounds = juce::Rectangle<int>(bounds.getX(), bounds.getY(), halfWidth, bounds.getHeight());
    elevationBounds = juce::Rectangle<int>(bounds.getX() + halfWidth + 10, bounds.getY(), halfWidth, bounds.getHeight());
}

void Radar3DComponent::timerCallback() {
    auto state = processor.getLiveFlightState();
    flightHistory.push_back({ state.posX, state.posY, state.posZ, state.azimuthDeg, state.elevationDeg });
    while (flightHistory.size() > 60) {
        flightHistory.pop_front();
    }
    repaint();
}

void Radar3DComponent::paint(juce::Graphics& g) {
    auto state = processor.getLiveFlightState();
    drawAzimuthRadar(g, azimuthBounds.toFloat(), state);
    drawElevationHUD(g, elevationBounds.toFloat(), state);
}

void Radar3DComponent::drawAzimuthRadar(juce::Graphics& g, juce::Rectangle<float> bounds,
                                       const FlyByAudioProcessor::LiveFlightState& state) {
    // Recessed background panel
    g.setColour(FlyByLookAndFeel::carbonPanel.darker(0.3f));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(FlyByLookAndFeel::carbonBorder);
    g.drawRoundedRectangle(bounds, 8.0f, 1.2f);

    auto center = bounds.getCentre();
    float maxRadius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.42f;

    // Header label
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(FlyByLookAndFeel::goldAccent);
    g.drawText("HORIZONTAL RADAR (AZIMUTH)", bounds.getX() + 10, bounds.getY() + 8,
               bounds.getWidth() - 20, 16, juce::Justification::left);

    // Range rings (1m, 2m, 3m, 4m)
    for (int r = 1; r <= 4; ++r) {
        float ringRadius = maxRadius * (r / 4.0f);
        g.setColour(FlyByLookAndFeel::carbonBorder.withAlpha(0.6f));
        g.drawEllipse(center.x - ringRadius, center.y - ringRadius, ringRadius * 2.0f, ringRadius * 2.0f, 1.0f);
        g.setFont(juce::Font(9.0f));
        g.setColour(FlyByLookAndFeel::textMuted.withAlpha(0.5f));
        g.drawText(juce::String(r) + "m", center.x + 3.0f, center.y - ringRadius - 2.0f, 25, 12, juce::Justification::left);
    }

    // Crosshairs
    g.setColour(FlyByLookAndFeel::carbonBorder.withAlpha(0.7f));
    g.drawLine(center.x - maxRadius, center.y, center.x + maxRadius, center.y, 1.0f); // Left-Right
    g.drawLine(center.x, center.y - maxRadius, center.x, center.y + maxRadius, 1.0f); // Front-Back

    // Cardinal directions
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(FlyByLookAndFeel::goldAccent.withAlpha(0.7f));
    g.drawText("FRONT", center.x - 20, center.y - maxRadius - 14, 40, 14, juce::Justification::centred);
    g.drawText("REAR", center.x - 20, center.y + maxRadius + 2, 40, 14, juce::Justification::centred);
    g.drawText("L", center.x - maxRadius - 16, center.y - 7, 14, 14, juce::Justification::centred);
    g.drawText("R", center.x + maxRadius + 2, center.y - 7, 14, 14, juce::Justification::centred);

    // Listener Head Icon at center
    g.setColour(FlyByLookAndFeel::goldBezel);
    g.fillEllipse(center.x - 6.0f, center.y - 6.0f, 12.0f, 12.0f);
    // Ears
    g.fillRoundedRectangle(center.x - 8.5f, center.y - 3.0f, 2.5f, 6.0f, 1.0f);
    g.fillRoundedRectangle(center.x + 6.0f, center.y - 3.0f, 2.5f, 6.0f, 1.0f);
    // Nose triangle pointing Front
    juce::Path nose;
    nose.startNewSubPath(center.x - 3.0f, center.y - 5.0f);
    nose.lineTo(center.x, center.y - 9.0f);
    nose.lineTo(center.x + 3.0f, center.y - 5.0f);
    nose.closeSubPath();
    g.fillPath(nose);

    // Draw Flight History Trail (Top-Down)
    if (flightHistory.size() > 1) {
        juce::Path trail;
        float distScale = maxRadius / 4.0f; // 4m max span
        for (size_t i = 0; i < flightHistory.size(); ++i) {
            float px = center.x + flightHistory[i].x * distScale;
            float py = center.y - flightHistory[i].y * distScale; // front is -Y on screen
            if (i == 0) trail.startNewSubPath(px, py);
            else trail.lineTo(px, py);
        }
        g.setColour(FlyByLookAndFeel::radarCyan.withAlpha(0.35f));
        g.strokePath(trail, juce::PathStrokeType(1.5f));
    }

    // Current Flight Puck (Position)
    float distScale = maxRadius / 4.0f;
    float puckX = center.x + state.posX * distScale;
    float puckY = center.y - state.posY * distScale;

    // Glowing target ring
    g.setColour(FlyByLookAndFeel::radarCyan.withAlpha(0.3f));
    g.fillEllipse(puckX - 10.0f, puckY - 10.0f, 20.0f, 20.0f);
    g.setColour(FlyByLookAndFeel::radarCyan);
    g.drawEllipse(puckX - 7.0f, puckY - 7.0f, 14.0f, 14.0f, 1.8f);
    g.setColour(juce::Colours::white);
    g.fillEllipse(puckX - 2.5f, puckY - 2.5f, 5.0f, 5.0f);

    // Coordinate readout
    g.setFont(juce::Font(10.0f));
    g.setColour(FlyByLookAndFeel::radarCyan);
    juce::String azText = juce::String(state.azimuthDeg, 1) + " deg";
    g.drawText(azText, bounds.getX() + 10, bounds.getBottom() - 22, 100, 14, juce::Justification::left);
}

void Radar3DComponent::drawElevationHUD(juce::Graphics& g, juce::Rectangle<float> bounds,
                                       const FlyByAudioProcessor::LiveFlightState& state) {
    // Recessed background panel
    g.setColour(FlyByLookAndFeel::carbonPanel.darker(0.3f));
    g.fillRoundedRectangle(bounds, 8.0f);
    g.setColour(FlyByLookAndFeel::carbonBorder);
    g.drawRoundedRectangle(bounds, 8.0f, 1.2f);

    auto center = bounds.getCentre();
    float maxRadius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.42f;

    // Header label
    g.setFont(juce::Font(11.0f, juce::Font::bold));
    g.setColour(FlyByLookAndFeel::goldAccent);
    g.drawText("VERTICAL PROFILE (ELEVATION)", bounds.getX() + 10, bounds.getY() + 8,
               bounds.getWidth() - 20, 16, juce::Justification::left);

    // Horizon line (0 deg elevation)
    g.setColour(FlyByLookAndFeel::goldBezel.withAlpha(0.6f));
    g.drawLine(bounds.getX() + 20.0f, center.y, bounds.getRight() - 20.0f, center.y, 1.5f);
    g.setFont(juce::Font(9.0f));
    g.setColour(FlyByLookAndFeel::goldAccent);
    g.drawText("HORIZON 0 deg", bounds.getX() + 12.0f, center.y - 14.0f, 80, 12, juce::Justification::left);

    // Elevation ticks (-40 deg to +90 deg)
    float elSpanPx = maxRadius * 0.9f; // vertical height for 90 deg
    // Zenith (+90 deg overhead)
    float zenithY = center.y - elSpanPx;
    g.setColour(FlyByLookAndFeel::radarAmber.withAlpha(0.7f));
    const float dashPattern[] = { 4.0f, 4.0f };
    g.drawDashedLine(juce::Line<float>(bounds.getX() + 30.0f, zenithY, bounds.getRight() - 30.0f, zenithY),
                     dashPattern, 2, 1.0f);
    g.drawText("+90 deg ZENITH (OVERHEAD)", bounds.getX() + 32.0f, zenithY - 14.0f, 150, 12, juce::Justification::left);

    // +45 deg mid-elevation
    float midUpY = center.y - elSpanPx * 0.5f;
    g.setColour(FlyByLookAndFeel::carbonBorder.withAlpha(0.6f));
    g.drawLine(bounds.getX() + 40.0f, midUpY, bounds.getRight() - 40.0f, midUpY, 1.0f);
    g.drawText("+45 deg", bounds.getRight() - 65.0f, midUpY - 12.0f, 50, 12, juce::Justification::right);

    // -40 deg below eye level
    float belowY = center.y + elSpanPx * (40.0f / 90.0f);
    g.setColour(FlyByLookAndFeel::carbonBorder.withAlpha(0.6f));
    g.drawLine(bounds.getX() + 40.0f, belowY, bounds.getRight() - 40.0f, belowY, 1.0f);
    g.drawText("-40 deg (CHEST/TORSO)", bounds.getX() + 32.0f, belowY + 2.0f, 130, 12, juce::Justification::left);

    // Listener profile icon (ear level at center)
    g.setColour(FlyByLookAndFeel::goldBezel);
    g.fillEllipse(center.x - 7.0f, center.y - 7.0f, 14.0f, 14.0f);
    // Torso silhouette below
    juce::Path torso;
    torso.startNewSubPath(center.x - 14.0f, center.y + 24.0f);
    torso.lineTo(center.x - 6.0f, center.y + 8.0f);
    torso.lineTo(center.x + 6.0f, center.y + 8.0f);
    torso.lineTo(center.x + 14.0f, center.y + 24.0f);
    torso.closeSubPath();
    g.setColour(FlyByLookAndFeel::goldBezel.withAlpha(0.4f));
    g.fillPath(torso);

    // Flight History Trail (Elevation vs Distance/DistanceY)
    if (flightHistory.size() > 1) {
        juce::Path trail;
        float zScale = elSpanPx / (90.0f * (3.14159f / 180.0f) * 2.0f); // map Z meters
        for (size_t i = 0; i < flightHistory.size(); ++i) {
            float px = center.x + flightHistory[i].y * (maxRadius / 4.0f); // horizontal axis = front/back
            float py = center.y - (flightHistory[i].elevation / 90.0f) * elSpanPx;
            if (i == 0) trail.startNewSubPath(px, py);
            else trail.lineTo(px, py);
        }
        g.setColour(FlyByLookAndFeel::radarAmber.withAlpha(0.45f));
        g.strokePath(trail, juce::PathStrokeType(1.5f));
    }

    // Current Elevation Puck
    float elRatio = std::clamp(state.elevationDeg / 90.0f, -0.6f, 1.0f);
    float puckY = center.y - elRatio * elSpanPx;
    float puckX = center.x + state.posY * (maxRadius / 4.0f);

    g.setColour(FlyByLookAndFeel::radarAmber.withAlpha(0.3f));
    g.fillEllipse(puckX - 10.0f, puckY - 10.0f, 20.0f, 20.0f);
    g.setColour(FlyByLookAndFeel::radarAmber);
    g.drawEllipse(puckX - 7.0f, puckY - 7.0f, 14.0f, 14.0f, 1.8f);
    g.setColour(juce::Colours::white);
    g.fillEllipse(puckX - 2.5f, puckY - 2.5f, 5.0f, 5.0f);

    // Altitude & Elevation readout
    g.setFont(juce::Font(10.0f));
    g.setColour(FlyByLookAndFeel::radarAmber);
    juce::String elText = "EL: " + juce::String(state.elevationDeg, 1) + " deg | ALT: " + juce::String(state.posZ, 2) + "m";
    g.drawText(elText, bounds.getX() + 10, bounds.getBottom() - 22, bounds.getWidth() - 20, 14, juce::Justification::left);
}

void Radar3DComponent::mouseDown(const juce::MouseEvent& e) {
    if (azimuthBounds.contains(e.getPosition())) {
        isDraggingAzimuth = true;
        mouseDrag(e);
    } else if (elevationBounds.contains(e.getPosition())) {
        isDraggingElevation = true;
        mouseDrag(e);
    }
}

void Radar3DComponent::mouseDrag(const juce::MouseEvent& e) {
    auto& apvts = processor.getAPVTS();

    if (isDraggingAzimuth) {
        auto center = azimuthBounds.toFloat().getCentre();
        float dx = e.position.x - center.x;
        float dy = -(e.position.y - center.y); // front is +y
        float angleRad = std::atan2(dx, dy);
        float azDeg = angleRad * (180.0f / 3.14159265f);
        
        float maxRadius = std::min(azimuthBounds.getWidth(), azimuthBounds.getHeight()) * 0.42f;
        float dist = (std::sqrt(dx * dx + dy * dy) / maxRadius) * 4.0f;
        dist = std::clamp(dist, 0.3f, 8.0f);

        if (auto* azParam = apvts.getParameter("azimuth"))
            azParam->setValueNotifyingHost(azParam->convertTo0to1(azDeg));
        if (auto* distParam = apvts.getParameter("distance"))
            distParam->setValueNotifyingHost(distParam->convertTo0to1(dist));
    } else if (isDraggingElevation) {
        auto center = elevationBounds.toFloat().getCentre();
        float maxRadius = std::min(elevationBounds.getWidth(), elevationBounds.getHeight()) * 0.42f;
        float elSpanPx = maxRadius * 0.9f;
        float dy = -(e.position.y - center.y); // up is +el
        float elDeg = (dy / elSpanPx) * 90.0f;
        elDeg = std::clamp(elDeg, -50.0f, 90.0f);

        if (auto* elParam = apvts.getParameter("elevation"))
            elParam->setValueNotifyingHost(elParam->convertTo0to1(elDeg));
    }
}

} // namespace FlyBy::UI
