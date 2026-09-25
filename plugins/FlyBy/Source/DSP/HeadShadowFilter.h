#pragma once
#include <cmath>
#include "SpatialMath.h"

namespace FlyBy::DSP {

/**
 * HeadShadowFilter
 * Implements the continuous acoustic diffraction around a rigid sphere (Rayleigh model).
 * For ipsilateral ear: high frequencies pass with slight gain.
 * For contralateral ear: high frequencies are progressively shadowed (low-pass shelf).
 */
class HeadShadowFilter {
public:
    HeadShadowFilter() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        reset();
    }

    void reset() {
        x1 = y1 = 0.0f;
    }

    /**
     * Update filter coefficients based on relative azimuth (theta in radians from ear line).
     * theta = 0 means right in front of this ear (0 deg).
     * theta = PI/2 means straight ahead/middle.
     * theta = PI means opposite side of head (shadowed).
     */
    void update(float thetaRad) {
        // Frequency threshold around head diffraction
        // f_0 ~ c / (2 * pi * a) ~ 343 / (2 * pi * 0.0875) ~ 624 Hz
        // Rayleigh-Woodworth continuous approximation:
        // H(s) = (1 + alpha * s / omega0) / (1 + s / omega0)
        // where alpha varies from 1.0 (on-axis) to ~0.2 (shadowed)
        float cosTheta = std::cos(thetaRad);
        
        // alpha parameter controls high-frequency shelf level
        // On ear side: alpha ~ 1.2 to 2.0 (pressure doubling)
        // Shadowed side: alpha ~ 0.1 to 0.4 (-10dB to -20dB shelf)
        float alpha = 1.0f + 0.5f * cosTheta;
        if (alpha < 0.15f) alpha = 0.15f;

        float f0 = 650.0f;
        float w0 = 2.0f * PI * f0;
        float t = 1.0f / sampleRate;

        // Bilinear transform of (alpha + s / w0) / (1 + s / w0)
        float denom = 2.0f + w0 * t;
        b0 = (2.0f * alpha + w0 * t) / denom;
        b1 = (w0 * t - 2.0f * alpha) / denom;
        a1 = (w0 * t - 2.0f) / denom;
    }

    inline float process(float in) {
        float out = b0 * in + b1 * x1 - a1 * y1;
        x1 = in;
        y1 = out;
        return out;
    }

private:
    float sampleRate = 44100.0f;
    float b0 = 1.0f, b1 = 0.0f, a1 = 0.0f;
    float x1 = 0.0f, y1 = 0.0f;
};

} // namespace FlyBy::DSP
