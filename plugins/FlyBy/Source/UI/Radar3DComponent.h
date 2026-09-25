#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <deque>
#include "FlyByLookAndFeel.h"
#include "../PluginProcessor.h"

namespace FlyBy::UI {

/**
 * Radar3DComponent
 * Dual-View Aeronautical Flight Display:
 * Left Panel: Top-down Azimuth & Distance Radar (Horizontal XY Plane)
 * Right Panel: Side Elevation & Altitude HUD (Vertical Z Plane)
 * Features live kinetic trajectory trails, degree ticks, and direct interactive mouse drag.
 */
class Radar3DComponent : public juce::Component, public juce::Timer {
public:
    Radar3DComponent(FlyByAudioProcessor& processor_);
    ~Radar3DComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    FlyByAudioProcessor& processor;

    struct TrailPoint {
        float x, y, z;
        float azimuth, elevation;
    };
    std::deque<TrailPoint> flightHistory;

    juce::Rectangle<int> azimuthBounds;
    juce::Rectangle<int> elevationBounds;

    bool isDraggingAzimuth = false;
    bool isDraggingElevation = false;

    void drawAzimuthRadar(juce::Graphics& g, juce::Rectangle<float> bounds, const FlyByAudioProcessor::LiveFlightState& state);
    void drawElevationHUD(juce::Graphics& g, juce::Rectangle<float> bounds, const FlyByAudioProcessor::LiveFlightState& state);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Radar3DComponent)
};

} // namespace FlyBy::UI
