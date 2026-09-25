#pragma once
#include <cmath>
#include <algorithm>
#include "SpatialMath.h"

namespace FlyBy::DSP {

enum class FlightMode {
    Manual = 0,    // Controlled directly by Azimuth, Elevation, Distance dials / DAW automation
    Swoop,         // High-speed aerial dive: starts high ahead, zooms low past ear, pulls up
    HelicalSpiral, // Spirals up & down around the head (hornet / drone ascent)
    ZenithFlyover, // Direct straight flyover from front horizon (+0 el) to overhead zenith (+90 el) to back
    HornetFlutter  // Erratic organic insect buzz darting up and down
};

/**
 * FlightPathGenerator
 * Generates continuous 3D trajectories in real-time.
 */
class FlightPathGenerator {
public:
    FlightPathGenerator() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        reset();
    }

    void reset() {
        phase = 0.0f;
        jitterPhase = 0.0f;
        currentManualPos = Vector3(0.0f, 1.5f, 0.0f);
        initialized = false;
    }

    struct FlightConfig {
        FlightMode mode = FlightMode::Manual;
        float speedHz = 0.25f;       // 0.05 to 5.0 Hz flight cycle rate
        float altitudeMax = 2.5f;    // max height in meters (+Z)
        float altitudeMin = -0.8f;   // min height in meters (-Z, below eye level)
        float proximityRadius = 1.2f;// closest passing distance in meters
        float turbulence = 0.15f;    // flutter/jitter intensity
        
        // Manual fallbacks
        float manualAzimuth = 0.0f;
        float manualElevation = 0.0f;
        float manualDistance = 1.5f;
    };

    /**
     * Advance flight path by numSamples and return current 3D position
     */
    Vector3 advance(int numSamples, const FlightConfig& config) {
        if (config.mode == FlightMode::Manual) {
            SphericalCoord sphere(config.manualAzimuth, config.manualElevation, config.manualDistance);
            Vector3 targetPos = sphere.toCartesian();

            if (!initialized) {
                currentManualPos = targetPos;
                initialized = true;
                return currentManualPos;
            }

            // Adaptive smoothing based on how fast the user is moving dials/faders
            float deltaDist = (targetPos - currentManualPos).length();
            // Fast moves get a slightly snappier response (0.35) while gentle adjustments get high precision (0.12)
            float speedFactor = std::clamp(0.12f + deltaDist * 0.15f, 0.12f, 0.45f);

            currentManualPos.x += speedFactor * (targetPos.x - currentManualPos.x);
            currentManualPos.y += speedFactor * (targetPos.y - currentManualPos.y);
            currentManualPos.z += speedFactor * (targetPos.z - currentManualPos.z);
            return currentManualPos;
        }

        initialized = false; // Reset so switching back to manual is smooth

        // Advance flight phase
        float deltaPhase = (config.speedHz / sampleRate) * static_cast<float>(numSamples);
        phase += deltaPhase;
        if (phase >= 1.0f) phase -= std::floor(phase);

        // Turbulence noise oscillator
        jitterPhase += deltaPhase * 7.3f;
        if (jitterPhase >= 1.0f) jitterPhase -= std::floor(jitterPhase);
        float flutter = std::sin(jitterPhase * TWO_PI) * config.turbulence * 0.3f;

        Vector3 pos;

        switch (config.mode) {
            case FlightMode::Swoop: {
                // Dive-bomb swoop trajectory:
                // Starts at Y = +4m (front), Z = altitudeMax (high)
                // Swoops down to Z = altitudeMin near Y = 0 (listener ear)
                // Pulls up to Z = altitudeMax behind listener at Y = -4m
                float t = phase; // 0 to 1
                float travelY = 4.0f - t * 8.0f; // +4 to -4m
                
                // Parabolic / catenary swoop curve for elevation (Z)
                float u = (t - 0.5f) * 2.0f; // -1 to +1
                float z = config.altitudeMin + (u * u) * (config.altitudeMax - config.altitudeMin);
                
                // Slight lateral curve so it zooms just past the right or left ear
                float x = config.proximityRadius * std::sin(t * PI);

                pos = Vector3(x + flutter, travelY, z + flutter);
                break;
            }

            case FlightMode::HelicalSpiral: {
                // Orbit around the head while oscillating between altitudeMin and altitudeMax
                float angle = phase * TWO_PI;
                float verticalOsc = (std::sin(phase * TWO_PI * 0.5f) + 1.0f) * 0.5f; // 0 to 1
                float z = config.altitudeMin + verticalOsc * (config.altitudeMax - config.altitudeMin);
                
                float x = config.proximityRadius * std::sin(angle);
                float y = config.proximityRadius * std::cos(angle);

                pos = Vector3(x + flutter, y + flutter, z + flutter);
                break;
            }

            case FlightMode::ZenithFlyover: {
                // Flies straight from front horizon (+0 el) directly over listener's head (+90 el) to behind
                float angle = phase * PI; // 0 to PI
                float r = config.proximityRadius;
                
                float y = r * std::cos(angle);
                float z = r * std::sin(angle) + config.altitudeMin * (1.0f - std::sin(angle));
                float x = flutter * 0.5f;

                pos = Vector3(x, y, z);
                break;
            }

            case FlightMode::HornetFlutter: {
                // Organic darting motion with fast micro-altitude changes
                float t = phase * TWO_PI;
                float x = config.proximityRadius * (std::sin(t) + 0.5f * std::sin(3.0f * t));
                float y = config.proximityRadius * (std::cos(t) + 0.3f * std::cos(2.0f * t));
                
                float zBase = config.altitudeMin + 0.5f * (config.altitudeMax - config.altitudeMin);
                float zAmp = (config.altitudeMax - config.altitudeMin) * 0.45f;
                float z = zBase + zAmp * std::sin(4.0f * t + flutter);

                pos = Vector3(x + flutter, y + flutter, z + flutter);
                break;
            }

            case FlightMode::Manual:
            default:
                pos = Vector3(0.0f, config.proximityRadius, 0.0f);
                break;
        }

        return pos;
    }

    float getPhase() const { return phase; }

private:
    float sampleRate = 44100.0f;
    float phase = 0.0f;
    float jitterPhase = 0.0f;
    Vector3 currentManualPos = { 0.0f, 1.5f, 0.0f };
    bool initialized = false;
};

} // namespace FlyBy::DSP
