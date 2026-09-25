#pragma once
#include <cmath>
#include <algorithm>
#include "SpatialMath.h"
#include "FractionalDelayLine.h"
#include "PinnaFilter.h"

namespace FlyBy::DSP {

/**
 * BoundaryReflector
 * Solves the sterile "floating in a vacuum" problem of isolated HRTFs.
 * Synthesizes image-source specular reflections off the physical Floor and Ceiling:
 * 1. Floor Reflection: When source is near or below eye level (Z <= 0.3m),
 *    calculates floor reflection path delay and surface absorption (damped high shelf).
 * 2. Ceiling Reflection: When source approaches ceiling height (Z >= 1.5m),
 *    calculates ceiling reflection bounce with upper acoustic absorption.
 * 3. Proximity Grounding: Automatically blends reflection intensity based on boundary proximity.
 */
class BoundaryReflector {
public:
    BoundaryReflector() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        // 10 meters bounce buffer (~30ms) -> 4096 samples is ample
        floorDelayLeft.setMaxDelaySamples(4096);
        floorDelayRight.setMaxDelaySamples(4096);
        ceilingDelayLeft.setMaxDelaySamples(4096);
        ceilingDelayRight.setMaxDelaySamples(4096);

        floorAbsorbL.reset();
        floorAbsorbR.reset();
        ceilingAbsorbL.reset();
        ceilingAbsorbR.reset();

        reset();
    }

    void reset() {
        floorDelayLeft.reset();
        floorDelayRight.reset();
        ceilingDelayLeft.reset();
        ceilingDelayRight.reset();
        floorAbsorbL.reset();
        floorAbsorbR.reset();
        ceilingAbsorbL.reset();
        ceilingAbsorbR.reset();
    }

    struct BoundaryConfig {
        Vector3 position = { 0.0f, 1.5f, 0.0f }; // Source 3D position relative to listener head
        float grounding = 0.4f;                   // 0.0 to 1.0 (reflection amplitude)
        float floorHeight = 1.6f;                 // Listener head height above floor (meters)
        float roomCeilingHeight = 3.2f;           // Room ceiling height above floor (meters)
    };

    void update(const BoundaryConfig& cfg) {
        groundingGain = std::clamp(cfg.grounding, 0.0f, 1.0f);
        if (groundingGain < 0.001f) return;

        // Listener head is at z = cfg.floorHeight above floor.
        // Source absolute height above floor: zSource = cfg.floorHeight + cfg.position.z
        float zSource = std::max(0.05f, cfg.floorHeight + cfg.position.z);
        float zListener = cfg.floorHeight;

        // 1. Floor Image Source:
        // Reflected z is at -zSource relative to floor.
        // Vertical path delta: dz = zSource + zListener
        float horizDistSq = cfg.position.x * cfg.position.x + cfg.position.y * cfg.position.y;
        float directDist = std::sqrt(horizDistSq + cfg.position.z * cfg.position.z);
        float floorDist = std::sqrt(horizDistSq + (zSource + zListener) * (zSource + zListener));

        // Excess delay beyond direct sound in samples
        float floorExcessDist = std::max(0.1f, floorDist - directDist);
        floorDelaySamples = (floorExcessDist / SPEED_OF_SOUND) * sampleRate;

        // Floor reflection gain decays with distance and angle (Lambertian / 1/r law)
        float floorProximity = std::clamp(1.0f - (zSource / 3.0f), 0.0f, 1.0f);
        floorGain = (directDist / floorDist) * 0.45f * floorProximity * groundingGain;

        // Floor acoustic damping (carpet/wood absorbs highs above 4kHz)
        floorAbsorbL.setHighShelf(3800.0f, -6.0f, sampleRate);
        floorAbsorbR.setHighShelf(3800.0f, -6.0f, sampleRate);

        // 2. Ceiling Image Source:
        // Ceiling is at cfg.roomCeilingHeight above floor.
        // Reflected ceiling path delta: dz = (H - zSource) + (H - zListener)
        float H = std::max(cfg.roomCeilingHeight, zSource + 0.2f);
        float dzCeil = (H - zSource) + (H - zListener);
        float ceilingDist = std::sqrt(horizDistSq + dzCeil * dzCeil);

        float ceilExcessDist = std::max(0.1f, ceilingDist - directDist);
        ceilingDelaySamples = (ceilExcessDist / SPEED_OF_SOUND) * sampleRate;

        // Ceiling reflection is strongest when source is high (+Z)
        float ceilingProximity = std::clamp(1.0f - ((H - zSource) / 2.5f), 0.0f, 1.0f);
        ceilingGain = (directDist / ceilingDist) * 0.40f * ceilingProximity * groundingGain;

        // Smooth delay times and gains to target values
        targetFloorDelay = floorDelaySamples;
        targetCeilingDelay = ceilingDelaySamples;
        targetFloorGain = (floorGain > 0.001f) ? floorGain : 0.0f;
        targetCeilingGain = (ceilingGain > 0.001f) ? ceilingGain : 0.0f;

        // Initialize on first setup
        if (currentFloorDelay <= 0.0f) {
            currentFloorDelay = targetFloorDelay;
            currentCeilingDelay = targetCeilingDelay;
            currentFloorGain = targetFloorGain;
            currentCeilingGain = targetCeilingGain;
        }

        // Adjust slew speed based on movement velocity (larger distance jump = slightly faster slewing)
        float maxDelayDelta = std::max(std::abs(targetFloorDelay - currentFloorDelay),
                                       std::abs(targetCeilingDelay - currentCeilingDelay));
        // Adaptive alpha: between 0.002 (ultra-smooth during slow moves) and 0.012 (fast during rapid drags)
        adaptiveSlewRate = std::clamp(0.002f + maxDelayDelta * 0.0001f, 0.002f, 0.012f);
    }

    /**
     * Process boundary reflections and sum into output channels with sample-accurate adaptive smoothing.
     */
    inline void process(float inMono, float& outL, float& outR) {
        if (groundingGain < 0.001f) return;

        // Write mono signal to reflection delay lines
        floorDelayLeft.write(inMono);
        ceilingDelayLeft.write(inMono);

        // Adaptive one-pole smoothing per sample (zero zipper noise, no clicks)
        currentFloorDelay += adaptiveSlewRate * (targetFloorDelay - currentFloorDelay);
        currentCeilingDelay += adaptiveSlewRate * (targetCeilingDelay - currentCeilingDelay);
        currentFloorGain += 0.004f * (targetFloorGain - currentFloorGain);
        currentCeilingGain += 0.004f * (targetCeilingGain - currentCeilingGain);

        // Floor early reflection
        if (currentFloorGain > 0.0005f) {
            float floorSig = floorDelayLeft.read(currentFloorDelay);
            floorSig = floorAbsorbL.process(floorSig);
            outL += floorSig * currentFloorGain;
            outR += floorSig * currentFloorGain;
        }

        // Ceiling early reflection
        if (currentCeilingGain > 0.0005f) {
            float ceilSig = ceilingDelayLeft.read(currentCeilingDelay);
            ceilSig = ceilingAbsorbL.process(ceilSig);
            outL += ceilSig * currentCeilingGain;
            outR += ceilSig * currentCeilingGain;
        }
    }

private:
    float sampleRate = 44100.0f;
    float groundingGain = 0.0f;

    FractionalDelayLine floorDelayLeft;
    FractionalDelayLine floorDelayRight;
    FractionalDelayLine ceilingDelayLeft;
    FractionalDelayLine ceilingDelayRight;

    BiquadFilter floorAbsorbL;
    BiquadFilter floorAbsorbR;
    BiquadFilter ceilingAbsorbL;
    BiquadFilter ceilingAbsorbR;

    float floorDelaySamples = 100.0f;
    float floorGain = 0.0f;
    float targetFloorDelay = 100.0f;
    float currentFloorDelay = 0.0f;
    float targetFloorGain = 0.0f;
    float currentFloorGain = 0.0f;

    float ceilingDelaySamples = 150.0f;
    float ceilingGain = 0.0f;
    float targetCeilingDelay = 150.0f;
    float currentCeilingDelay = 0.0f;
    float targetCeilingGain = 0.0f;
    float currentCeilingGain = 0.0f;

    float adaptiveSlewRate = 0.004f;
};

} // namespace FlyBy::DSP
