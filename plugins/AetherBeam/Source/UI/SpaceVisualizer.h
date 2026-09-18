#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../DSP/AcousticDatabase.h"
#include "../DSP/BinauralSpatializer.h"

class SpaceVisualizer : public juce::Component, private juce::Timer
{
public:
    std::function<void(float srcXN, float srcYN, float lisXN, float lisYN)> onNodesMoved;
    std::function<void(float srcXN, float srcYN, float lisXN, float lisYN)> onDragEnded;

    SpaceVisualizer()
    {
        startTimerHz(30);
    }

    ~SpaceVisualizer() override
    {
        stopTimer();
    }

    void setSpaceData(const AetherAcoustics::SpaceData* space,
                      AetherAcoustics::Vec3 src,
                      AetherAcoustics::Vec3 lis,
                      const std::vector<AetherAcoustics::RaySegment>& rays)
    {
        currentSpace = space;
        currentSource = src;
        currentListener = lis;
        currentRays = rays;
        repaint();
    }

    void setAcousticTelemetry(MicPolarPattern pattern, float width, float occupancy)
    {
        currentMicPattern = pattern;
        currentStereoWidth = width;
        currentOccupancy = occupancy;
        repaint();
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        lastMousePos = e.position;

        auto pSrc = projectToScreen(currentSource);
        auto pLis = projectToScreen(currentListener);

        if (e.position.getDistanceFrom(pSrc) < 18.0f)
        {
            dragMode = DragTarget::Source;
            stopTimer();
        }
        else if (e.position.getDistanceFrom(pLis) < 18.0f)
        {
            dragMode = DragTarget::Listener;
            stopTimer();
        }
        else
        {
            dragMode = DragTarget::Camera;
        }
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        auto delta = e.position - lastMousePos;
        lastMousePos = e.position;

        if (dragMode == DragTarget::Camera)
        {
            azimuthDeg += delta.x * 0.45f;
            elevationDeg = juce::jlimit(5.0f, 85.0f, elevationDeg - delta.y * 0.45f);
            repaint();
        }
        else if ((dragMode == DragTarget::Source || dragMode == DragTarget::Listener) && currentSpace != nullptr)
        {
            float radAz = juce::degreesToRadians(azimuthDeg);
            float cosAz = std::cos(radAz);
            float sinAz = std::sin(radAz);

            float spanX = currentSpace->maxBound.x - currentSpace->minBound.x;
            float spanY = currentSpace->maxBound.y - currentSpace->minBound.y;
            float maxDim = std::max(spanX, spanY);

            float moveSpeed = (maxDim / 280.0f) / zoomScale;
            float dxRoom = (delta.x * cosAz + delta.y * sinAz) * moveSpeed;
            float dyRoom = (-delta.x * sinAz + delta.y * cosAz) * moveSpeed;

            if (dragMode == DragTarget::Source)
            {
                currentSource.x = juce::jlimit(currentSpace->minBound.x + 0.1f, currentSpace->maxBound.x - 0.1f, currentSource.x + dxRoom);
                currentSource.y = juce::jlimit(currentSpace->minBound.y + 0.1f, currentSpace->maxBound.y - 0.1f, currentSource.y + dyRoom);
            }
            else
            {
                currentListener.x = juce::jlimit(currentSpace->minBound.x + 0.1f, currentSpace->maxBound.x - 0.1f, currentListener.x + dxRoom);
                currentListener.y = juce::jlimit(currentSpace->minBound.y + 0.1f, currentSpace->maxBound.y - 0.1f, currentListener.y + dyRoom);
            }

            // Realtime ray trace during drag
            currentRays = AetherAcoustics::computeRealtimeRays(currentSource, currentListener,
                                                              currentSpace->minBound, currentSpace->maxBound, currentSpace->id, 1, 7);

            if (onNodesMoved)
            {
                float sXN = (currentSource.x - currentSpace->minBound.x) / spanX;
                float sYN = (currentSource.y - currentSpace->minBound.y) / spanY;
                float lXN = (currentListener.x - currentSpace->minBound.x) / spanX;
                float lYN = (currentListener.y - currentSpace->minBound.y) / spanY;
                onNodesMoved(sXN, sYN, lXN, lYN);
            }

            repaint();
        }
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        if ((dragMode == DragTarget::Source || dragMode == DragTarget::Listener) && currentSpace != nullptr)
        {
            currentRays = AetherAcoustics::computeRealtimeRays(currentSource, currentListener,
                                                              currentSpace->minBound, currentSpace->maxBound, currentSpace->id, 4, 96);
            float spanX = currentSpace->maxBound.x - currentSpace->minBound.x;
            float spanY = currentSpace->maxBound.y - currentSpace->minBound.y;
            float sXN = (currentSource.x - currentSpace->minBound.x) / spanX;
            float sYN = (currentSource.y - currentSpace->minBound.y) / spanY;
            float lXN = (currentListener.x - currentSpace->minBound.x) / spanX;
            float lYN = (currentListener.y - currentSpace->minBound.y) / spanY;

            if (onDragEnded)
                onDragEnded(sXN, sYN, lXN, lYN);

            repaint();
        }

        dragMode = DragTarget::None;
        startTimerHz(30);
    }

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override
    {
        zoomScale = juce::jlimit(0.4f, 3.0f, zoomScale * (1.0f + wheel.deltaY * 0.5f));
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        // 1. Sleek Background with subtle border
        g.setColour(juce::Colour(0xff060911));
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(juce::Colour(0x2838bdf8));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

        if (currentSpace == nullptr)
        {
            g.setColour(juce::Colour(0xff64748b));
            g.setFont(13.0f);
            g.drawText("No space selected", getLocalBounds(), juce::Justification::centred);
            return;
        }

        // 2. Draw Floor Perspective Reference Grid
        drawFloorGrid(g);

        // 3. Draw Authentic Architectural 3D Wireframe (Vaults, Domes, Columns)
        drawArchitecturalWireframe(g);

        // 4. Draw Reflection Rays (Color-coded by Order with physical attenuation alpha)
        for (auto it = currentRays.rbegin(); it != currentRays.rend(); ++it)
        {
            const auto& ray = *it;
            if (ray.points.size() < 2) continue;

            if (ray.order == 0)
            {
                // Direct ray: Radiant gold line
                g.setColour(juce::Colour(0xfffbbf24));
                auto p0 = projectToScreen(ray.points[0]);
                auto p1 = projectToScreen(ray.points[1]);
                g.drawLine(p0.x, p0.y, p1.x, p1.y, 2.6f);
            }
            else if (ray.order == 1)
            {
                // 1st order: Warm vibrant amber
                g.setColour(juce::Colour(0xbbf97316));
                for (size_t i = 0; i < ray.points.size() - 1; ++i)
                {
                    auto pA = projectToScreen(ray.points[i]);
                    auto pB = projectToScreen(ray.points[i + 1]);
                    g.drawLine(pA.x, pA.y, pB.x, pB.y, 1.5f);
                }
            }
            else if (ray.order == 2)
            {
                // 2nd order: Electric cyan/emerald bounce
                g.setColour(juce::Colour(0x7738bdf8));
                for (size_t i = 0; i < ray.points.size() - 1; ++i)
                {
                    auto pA = projectToScreen(ray.points[i]);
                    auto pB = projectToScreen(ray.points[i + 1]);
                    g.drawLine(pA.x, pA.y, pB.x, pB.y, 1.1f);
                }
            }
            else if (ray.order == 3)
            {
                // 3rd order: Violet atmospheric path
                g.setColour(juce::Colour(0x44a855f7));
                for (size_t i = 0; i < ray.points.size() - 1; ++i)
                {
                    auto pA = projectToScreen(ray.points[i]);
                    auto pB = projectToScreen(ray.points[i + 1]);
                    g.drawLine(pA.x, pA.y, pB.x, pB.y, 0.8f);
                }
            }
            else
            {
                // 4th order: Deep diffuse trail
                g.setColour(juce::Colour(0x2864748b));
                for (size_t i = 0; i < ray.points.size() - 1; ++i)
                {
                    auto pA = projectToScreen(ray.points[i]);
                    auto pB = projectToScreen(ray.points[i + 1]);
                    g.drawLine(pA.x, pA.y, pB.x, pB.y, 0.6f);
                }
            }
        }

        // 5. Draw Sound Source (Interactive Ruby Red Node with Radial Glow)
        auto srcPt = projectToScreen(currentSource);
        g.setColour(juce::Colour(0x33ef4444));
        g.fillEllipse(srcPt.x - 14.0f, srcPt.y - 14.0f, 28.0f, 28.0f);
        g.setColour(juce::Colour(0xffef4444));
        g.fillEllipse(srcPt.x - 6.0f, srcPt.y - 6.0f, 12.0f, 12.0f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(srcPt.x - 2.0f, srcPt.y - 2.0f, 4.0f, 4.0f);

        // 6. Draw Listener / Mic Capsule (Interactive Cyan Node with Polar Indicator)
        auto lisPt = projectToScreen(currentListener);
        g.setColour(juce::Colour(0x3306b6d4));
        g.fillEllipse(lisPt.x - 14.0f, lisPt.y - 14.0f, 28.0f, 28.0f);
        g.setColour(juce::Colour(0xff06b6d4));
        g.fillEllipse(lisPt.x - 6.0f, lisPt.y - 6.0f, 12.0f, 12.0f);
        g.setColour(juce::Colours::white);
        g.fillEllipse(lisPt.x - 2.0f, lisPt.y - 2.0f, 4.0f, 4.0f);

        // Labels for Source and Listener
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.setColour(juce::Colour(0xfff87171));
        g.drawText("SOURCE (Drag)", static_cast<int>(srcPt.x) + 12, static_cast<int>(srcPt.y) - 14, 100, 14, juce::Justification::left);

        g.setColour(juce::Colour(0xff22d3ee));
        g.drawText("MIC REC (Drag)", static_cast<int>(lisPt.x) + 12, static_cast<int>(lisPt.y) - 14, 110, 14, juce::Justification::left);

        // 7. Info Header Bar
        g.setColour(juce::Colour(0xee0b1120));
        g.fillRoundedRectangle(8.0f, 8.0f, bounds.getWidth() - 16.0f, 30.0f, 4.0f);

        g.setFont(juce::Font(12.5f, juce::Font::bold));
        g.setColour(juce::Colour(0xfff59e0b));
        g.drawText(currentSpace->title, 16, 8, 380, 30, juce::Justification::left);

        float directDist = (currentListener - currentSource).norm();
        juce::String micName = "Binaural HRTF";
        if (currentMicPattern == MicPolarPattern::ORTF_Cardioid) micName = "ORTF Cardioid Pair";
        else if (currentMicPattern == MicPolarPattern::Blumlein_Figure8) micName = "Blumlein Fig-8";
        else if (currentMicPattern == MicPolarPattern::Omni) micName = "Omni Pair";

        juce::String acousticStats = micName + " | Occ: " + juce::String(static_cast<int>(currentOccupancy * 100.0f)) + "% | "
                                   + "RT60: " + juce::String(currentSpace->rt60, 2) + "s | Dist: "
                                   + juce::String(directDist, 1) + "m";
        g.setFont(juce::Font(11.0f, juce::Font::plain));
        g.setColour(juce::Colour(0xff94a3b8));
        g.drawText(acousticStats, getWidth() - 440, 8, 424, 30, juce::Justification::right);

        // 8. Interactive Hint (Bottom)
        g.setFont(juce::Font(9.5f, juce::Font::italic));
        g.setColour(juce::Colour(0x8894a3b8));
        g.drawText("Drag SOURCE / MIC to trace acoustic reflections live | Drag space to orbit 3D camera | Scroll wheel zooms",
                   12, getHeight() - 18, getWidth() - 24, 14, juce::Justification::left);
    }

private:
    void drawFloorGrid(juce::Graphics& g)
    {
        if (currentSpace == nullptr) return;

        float minX = currentSpace->minBound.x;
        float maxX = currentSpace->maxBound.x;
        float minY = currentSpace->minBound.y;
        float maxY = currentSpace->maxBound.y;
        float zFloor = currentSpace->minBound.z;

        g.setColour(juce::Colour(0x1538bdf8));
        int numLinesX = 8;
        for (int i = 0; i <= numLinesX; ++i)
        {
            float fx = minX + (maxX - minX) * (static_cast<float>(i) / numLinesX);
            auto p0 = projectToScreen({ fx, minY, zFloor });
            auto p1 = projectToScreen({ fx, maxY, zFloor });
            g.drawLine(p0.x, p0.y, p1.x, p1.y, 0.8f);
        }

        int numLinesY = 10;
        for (int j = 0; j <= numLinesY; ++j)
        {
            float fy = minY + (maxY - minY) * (static_cast<float>(j) / numLinesY);
            auto p0 = projectToScreen({ minX, fy, zFloor });
            auto p1 = projectToScreen({ maxX, fy, zFloor });
            g.drawLine(p0.x, p0.y, p1.x, p1.y, 0.8f);
        }
    }

    void drawArchitecturalWireframe(juce::Graphics& g)
    {
        if (currentSpace == nullptr) return;

        // Render main architectural edges with subtle slate/cyan depth
        g.setColour(juce::Colour(0x5538bdf8));
        for (const auto& edge : currentSpace->wireframe)
        {
            auto pA = projectToScreen(edge.a);
            auto pB = projectToScreen(edge.b);
            g.drawLine(pA.x, pA.y, pB.x, pB.y, 1.25f);
        }
    }

    juce::Point<float> projectToScreen(const AetherAcoustics::Vec3& p) const
    {
        if (currentSpace == nullptr) return { 0.0f, 0.0f };

        auto bounds = getLocalBounds().toFloat();
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY() + 8.0f;

        AetherAcoustics::Vec3 center{
            (currentSpace->minBound.x + currentSpace->maxBound.x) * 0.5f,
            (currentSpace->minBound.y + currentSpace->maxBound.y) * 0.5f,
            (currentSpace->minBound.z + currentSpace->maxBound.z) * 0.5f
        };

        float extentX = std::max(1.0f, currentSpace->maxBound.x - currentSpace->minBound.x);
        float extentY = std::max(1.0f, currentSpace->maxBound.y - currentSpace->minBound.y);
        float extentZ = std::max(1.0f, currentSpace->maxBound.z - currentSpace->minBound.z);
        float maxExtent = std::max({ extentX, extentY, extentZ });

        float baseScale = (std::min(bounds.getWidth(), bounds.getHeight()) * 0.44f) / (maxExtent * 0.58f);
        float scale = baseScale * zoomScale;

        float radAz = juce::degreesToRadians(azimuthDeg);
        float radEl = juce::degreesToRadians(elevationDeg);

        float cosAz = std::cos(radAz), sinAz = std::sin(radAz);
        float cosEl = std::cos(radEl), sinEl = std::sin(radEl);

        float rx = p.x - center.x;
        float ry = p.y - center.y;
        float rz = p.z - center.z;

        float x1 = rx * cosAz - ry * sinAz;
        float y1 = rx * sinAz + ry * cosAz;
        float z1 = rz;

        float x2 = x1;
        float y2 = y1 * cosEl - z1 * sinEl;
        float z2 = y1 * sinEl + z1 * cosEl;

        float perspective = 1.0f / (1.0f + y2 * 0.003f);
        return { cx + x2 * scale * perspective, cy - z2 * scale * perspective };
    }

    void timerCallback() override
    {
        if (dragMode == DragTarget::None)
        {
            azimuthDeg += 0.22f;
            if (azimuthDeg >= 360.0f) azimuthDeg -= 360.0f;
            repaint();
        }
    }

    enum class DragTarget { None, Camera, Source, Listener };
    DragTarget dragMode = DragTarget::None;

    const AetherAcoustics::SpaceData* currentSpace = nullptr;
    AetherAcoustics::Vec3 currentSource;
    AetherAcoustics::Vec3 currentListener;
    std::vector<AetherAcoustics::RaySegment> currentRays;

    MicPolarPattern currentMicPattern = MicPolarPattern::Binaural;
    float currentStereoWidth = 1.0f;
    float currentOccupancy = 0.0f;

    float azimuthDeg = 35.0f;
    float elevationDeg = 24.0f;
    float zoomScale = 1.0f;

    juce::Point<float> lastMousePos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpaceVisualizer)
};
