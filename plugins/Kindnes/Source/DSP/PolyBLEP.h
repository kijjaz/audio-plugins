#pragma once

#include <cmath>
#include <algorithm>

namespace KindnesDSP
{

/**
 * PolyBLEP (Polynomial Bandlimited Step) anti-aliasing utilities.
 * Implements Stilson & Smith / Valimaki residual polynomial corrections
 * to eliminate aliasing at standard audio rates (44.1k, 48k, 96k, 192k).
 */
class PolyBLEP
{
public:
    /**
     * Computes the PolyBLEP residual correction for a step transition.
     * @param t Current normalized phase [0, 1)
     * @param dt Phase increment per sample (freq / sampleRate)
     */
    static inline float stepCorrection(float t, float dt) noexcept
    {
        if (dt <= 0.0f)
            return 0.0f;

        // Near the beginning of the period (0 <= t < dt)
        if (t < dt)
        {
            float p = t / dt;
            return 2.0f * p - p * p - 1.0f;
        }
        // Near the end of the period (1 - dt < t < 1)
        else if (t > 1.0f - dt)
        {
            float p = (t - 1.0f) / dt;
            return p * p + 2.0f * p + 1.0f;
        }

        return 0.0f;
    }

    /**
     * Generates a bandlimited pulse / square wave with arbitrary duty cycle.
     * @param phase Current phase [0, 1)
     * @param dt Phase increment per sample
     * @param duty Pulse duty cycle (e.g. 0.125, 0.25, 0.50, 0.75)
     */
    static inline float pulse(float phase, float dt, float duty) noexcept
    {
        duty = std::clamp(duty, 0.001f, 0.999f);

        // Naive pulse: +1.0 for phase < duty, -1.0 for phase >= duty
        float val = (phase < duty) ? 1.0f : -1.0f;

        // Apply PolyBLEP correction at phase = 0 (rising edge from -1 to +1, delta = +2)
        val += stepCorrection(phase, dt);

        // Apply PolyBLEP correction at phase = duty (falling edge from +1 to -1, delta = -2)
        float fallingPhase = phase - duty;
        if (fallingPhase < 0.0f)
            fallingPhase += 1.0f;
        val -= stepCorrection(fallingPhase, dt);

        return val;
    }

    /**
     * Bandlimited sawtooth generator.
     */
    static inline float saw(float phase, float dt) noexcept
    {
        // Naive saw: ramps from -1 to +1
        float val = 2.0f * phase - 1.0f;
        // Correct the falling edge transition at phase = 0 (delta = -2)
        val -= stepCorrection(phase, dt);
        return val;
    }

    /**
     * Applies a PolyBLEP residual correction for an arbitrary staircase jump
     * of height `delta = newVal - oldVal` at sub-step phase `subPhase` [0, 1)
     * with sub-step phase increment `subDt`.
     */
    static inline float staircaseBlep(float subPhase, float subDt, float deltaBefore, float deltaAfter) noexcept
    {
        if (subDt <= 0.0f || subDt >= 1.0f)
            return 0.0f;

        if (subPhase < subDt)
        {
            float p = subPhase / subDt;
            // Residual after transition (0 <= p < 1): (2p - p^2 - 1) * (deltaBefore / 2)
            return 0.5f * deltaBefore * (2.0f * p - p * p - 1.0f);
        }
        else if (subPhase > 1.0f - subDt)
        {
            float p = (subPhase - 1.0f) / subDt;
            // Residual before next transition (-1 < p < 0): (p^2 + 2p + 1) * (deltaAfter / 2)
            return 0.5f * deltaAfter * (p * p + 2.0f * p + 1.0f);
        }

        return 0.0f;
    }
};

} // namespace KindnesDSP
