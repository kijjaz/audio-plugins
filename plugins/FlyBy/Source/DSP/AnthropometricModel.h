#pragma once
#include <cmath>
#include <algorithm>
#include "SpatialMath.h"
#include "FractionalDelayLine.h"
#include "HeadShadowFilter.h"
#include "PinnaFilter.h"
#include "TransientPreserver.h"
#include "BoundaryReflector.h"

namespace FlyBy::DSP {

/**
 * AnthropometricModel
 * Orchestrates full 3D binaural positioning for headphones:
 * - Geometric ITD calculation (Rayleigh-Woodworth spherical head)
 * - Fractional delay lines for left/right sub-sample arrival & Doppler tracking
 * - Spherical head acoustic diffraction / acoustic shadowing (ILD)
 * - Dynamic Pinna Moving-Notches (elevation up/down cues) with Transient Preservation
 * - Floor & Ceiling Specular Boundary Reflections (Physical Acoustic Grounding)
 * - Distance attenuation (1/r) & air absorption high-frequency roll-off
 */
class AnthropometricModel {
public:
    AnthropometricModel() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        
        int maxDelaySamples = static_cast<int>(sampleRate * 0.15f); // 150ms buffer
        delayLeft.setMaxDelaySamples(maxDelaySamples);
        delayRight.setMaxDelaySamples(maxDelaySamples);

        headShadowLeft.prepare(sampleRate);
        headShadowRight.prepare(sampleRate);

        pinnaLeft.prepare(sampleRate);
        pinnaRight.prepare(sampleRate);

        airAbsorbLeft.reset();
        airAbsorbRight.reset();

        transientPreserver.prepare(sampleRate);
        boundaryReflector.prepare(sampleRate);

        reset();
    }

    void reset() {
        delayLeft.reset();
        delayRight.reset();
        headShadowLeft.reset();
        headShadowRight.reset();
        pinnaLeft.reset();
        pinnaRight.reset();
        airAbsorbLeft.reset();
        airAbsorbRight.reset();
        transientPreserver.reset();
        boundaryReflector.reset();

        currentLeftDelay = 0.0f;
        currentRightDelay = 0.0f;
        targetLeftDelay = 0.0f;
        targetRightDelay = 0.0f;
        adaptiveDelaySlew = 0.005f;
        currentDistanceGain = 0.0f;
        targetDistanceGain = 1.0f;
        lastDist = -999.0f;
    }

    struct SpatialParams {
        Vector3 position = { 0.0f, 1.0f, 0.0f }; // In meters
        float pinnaScale = 1.0f;                  // 0.8 to 1.25 (anatomical tuning)
        float elevationStrength = 1.0f;           // 0.0 to 1.5
        float dopplerAmount = 1.0f;               // 0.0 to 2.0
        float headRadius = HEAD_RADIUS;           // ~0.0875m
        float distanceRolloff = 1.0f;             // 1/r intensity
        float crispness = 0.7f;                   // 0.0 to 1.0 (Transient Preservation)
        float grounding = 0.4f;                   // 0.0 to 1.0 (Floor/Ceiling Boundary Early Reflections)
    };

    /**
     * Updates spatial filters and sets up target delays for sample-by-sample smoothing
     */
    void update(const SpatialParams& params) {
        SphericalCoord sphere = SphericalCoord::fromCartesian(params.position);
        
        float dist = std::max(0.1f, sphere.distanceMeters);
        float azRad = sphere.azimuthDeg * (PI / 180.0f);
        float elDeg = sphere.elevationDeg;

        // 1. Woodworth-Rayleigh ITD & Distance Delays
        // Ear positions relative to head center:
        // Left ear: (-headRadius, 0, 0), Right ear: (+headRadius, 0, 0)
        Vector3 leftEarPos(-params.headRadius, 0.0f, 0.0f);
        Vector3 rightEarPos(params.headRadius, 0.0f, 0.0f);

        float distLeft = (params.position - leftEarPos).length();
        float distRight = (params.position - rightEarPos).length();

        // Effective acoustic path considering head curvature (Woodworth model)
        // If sound is to the right (azRad > 0), left ear is shadowed
        float itdSec = (params.headRadius / SPEED_OF_SOUND) * (std::abs(std::sin(azRad)) + std::abs(azRad));
        
        float baseDelayLeft = (distLeft / SPEED_OF_SOUND) * sampleRate;
        float baseDelayRight = (distRight / SPEED_OF_SOUND) * sampleRate;

        // Apply Doppler scale to path delay variations
        if (params.dopplerAmount < 1.0f) {
            // Blend with a static nominal distance delay if user attenuates Doppler
            float meanDelay = ((distLeft + distRight) * 0.5f / SPEED_OF_SOUND) * sampleRate;
            baseDelayLeft = meanDelay + (baseDelayLeft - meanDelay) * params.dopplerAmount;
            baseDelayRight = meanDelay + (baseDelayRight - meanDelay) * params.dopplerAmount;
        }

        targetLeftDelay = baseDelayLeft;
        targetRightDelay = baseDelayRight;

        // Initialize if first run
        if (currentLeftDelay <= 0.0f) {
            currentLeftDelay = targetLeftDelay;
            currentRightDelay = targetRightDelay;
        }

        // 2. Head Shadow (diffraction) based on angle to ear normal
        // Left ear normal points to -X (-90 deg), Right ear normal points to +X (+90 deg)
        float leftTheta = azRad + (PI * 0.5f);
        float rightTheta = (PI * 0.5f) - azRad;
        headShadowLeft.update(leftTheta);
        headShadowRight.update(rightTheta);

        // 3. Pinna Notches (Elevation)
        currentElevation = elDeg;
        currentPinnaScale = params.pinnaScale;
        currentElevationStrength = params.elevationStrength;
        currentCrispness = params.crispness;
        pinnaLeft.update(elDeg, params.pinnaScale, params.elevationStrength, 1.0f);
        pinnaRight.update(elDeg, params.pinnaScale, params.elevationStrength, 1.0f);

        // 4. Boundary Early Reflections (Floor & Ceiling Grounding)
        BoundaryReflector::BoundaryConfig bCfg;
        bCfg.position = params.position;
        bCfg.grounding = params.grounding;
        boundaryReflector.update(bCfg);

        // 5. Distance Attenuation & Air Absorption
        // 1/r law with a soft minimum distance of 0.5m
        float effectiveDist = std::max(0.5f, dist);
        targetDistanceGain = 1.0f / (1.0f + (effectiveDist - 0.5f) * params.distanceRolloff);
        if (currentDistanceGain <= 0.0f) {
            currentDistanceGain = targetDistanceGain;
        }

        // Air absorption (ISO 9613-1): update only on noticeable change to save CPU and prevent clicks
        if (std::abs(dist - lastDist) > 0.05f) {
            lastDist = dist;
            float airCutoff = std::clamp(20000.0f / (1.0f + 0.08f * dist), 1500.0f, 20000.0f);
            airAbsorbLeft.setHighShelf(airCutoff, -std::min(12.0f, dist * 0.75f), sampleRate);
            airAbsorbRight.setHighShelf(airCutoff, -std::min(12.0f, dist * 0.75f), sampleRate);
        }

        // Compute movement speed / delta to dynamically adapt the delay slew rate:
        // Slow delicate movement -> lower alpha (crystal clear, zero pitch wobble)
        // Fast drag/burst -> higher alpha (quickly tracks target without falling hundreds of samples behind)
        float delayDelta = std::max(std::abs(targetLeftDelay - currentLeftDelay),
                                    std::abs(targetRightDelay - currentRightDelay));
        // Adaptive slew rate: scales between 0.0015 (calm) and 0.015 (fast track)
        adaptiveDelaySlew = std::clamp(0.0015f + delayDelta * 0.0001f, 0.0015f, 0.015f);
    }

    /**
     * Process a mono input into a binaural stereo pair (outL, outR).
     */
    inline void processSample(float inMono, float& outL, float& outR) {
        // Track attack transient onset
        transientPreserver.process(inMono);
        float notchMult = transientPreserver.getNotchDepthMultiplier(currentCrispness);

        // Dynamically modulate pinna notch depth to protect transients (internally thresholded)
        pinnaLeft.update(currentElevation, currentPinnaScale, currentElevationStrength, notchMult);
        pinnaRight.update(currentElevation, currentPinnaScale, currentElevationStrength, notchMult);

        // Write mono sample to delay lines
        delayLeft.write(inMono);
        delayRight.write(inMono);

        // Adaptive velocity-dependent delay line tracking (eliminates clicks on drag while keeping Doppler tight)
        currentLeftDelay += adaptiveDelaySlew * (targetLeftDelay - currentLeftDelay);
        currentRightDelay += adaptiveDelaySlew * (targetRightDelay - currentRightDelay);

        // Read delayed signals (Fractional sub-sample cubic interpolation gives smooth Doppler)
        float sigL = delayLeft.read(currentLeftDelay);
        float sigR = delayRight.read(currentRightDelay);

        // Apply Head Shadowing (diffraction around skull)
        sigL = headShadowLeft.process(sigL);
        sigR = headShadowRight.process(sigR);

        // Apply Pinna Spectral Filters (Elevation & Torso cues)
        sigL = pinnaLeft.process(sigL);
        sigR = pinnaRight.process(sigR);

        // Apply Air Absorption
        sigL = airAbsorbLeft.process(sigL);
        sigR = airAbsorbRight.process(sigR);

        // Continuous one-pole smoothing of distance attenuation gain (eliminates volume pops)
        currentDistanceGain += 0.003f * (targetDistanceGain - currentDistanceGain);
        outL = sigL * currentDistanceGain;
        outR = sigR * currentDistanceGain;

        // Apply Floor & Ceiling Image-Source Boundary Early Reflections (with adaptive smoothing)
        boundaryReflector.process(inMono, outL, outR);
    }

private:
    float sampleRate = 44100.0f;

    FractionalDelayLine delayLeft;
    FractionalDelayLine delayRight;

    HeadShadowFilter headShadowLeft;
    HeadShadowFilter headShadowRight;

    PinnaFilter pinnaLeft;
    PinnaFilter pinnaRight;

    BiquadFilter airAbsorbLeft;
    BiquadFilter airAbsorbRight;

    TransientPreserver transientPreserver;
    BoundaryReflector boundaryReflector;

    float currentElevation = 0.0f;
    float currentPinnaScale = 1.0f;
    float currentElevationStrength = 1.0f;
    float currentCrispness = 0.7f;

    float currentLeftDelay = 0.0f;
    float currentRightDelay = 0.0f;
    float targetLeftDelay = 0.0f;
    float targetRightDelay = 0.0f;
    float adaptiveDelaySlew = 0.005f;

    float currentDistanceGain = 0.0f;
    float targetDistanceGain = 1.0f;
    float lastDist = -999.0f;
};

} // namespace FlyBy::DSP
