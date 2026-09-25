#pragma once
#include <cmath>
#include <algorithm>

namespace FlyBy::DSP {

/**
 * TransientPreserver
 * Solves the spatial audio "muffled filter" problem:
 * Separates rapid attack transients (e.g. snare hits, pick attacks, vocal consonants)
 * from resonant body/sustain. During transient attacks, it momentarily scales back
 * destructive pinna notch depth so high-frequency clarity and punch are preserved,
 * while applying full pinna notch depth to the sustain to provide vertical height cues.
 */
class TransientPreserver {
public:
    TransientPreserver() = default;

    void prepare(float sampleRate_) {
        sampleRate = sampleRate_;
        // Envelope decay (~25ms)
        alphaDecay = std::exp(-1.0f / (sampleRate * 0.025f));
        // Transient trigger hold/decay (~12ms)
        alphaTransientDecay = std::exp(-1.0f / (sampleRate * 0.012f));
        reset();
    }

    void reset() {
        env = 0.0f;
        transientRatio = 0.0f;
    }

    /**
     * Process audio sample and calculate transient ratio T in [0, 1].
     * T = 1.0 during sharp attack onsets.
     * T = 0.0 during steady state sustain and silence.
     */
    inline float process(float in) {
        float absIn = std::abs(in);

        // Detect sudden positive energy surge
        if (absIn > env) {
            float jump = absIn - env;
            // Surge condition: significant jump above current baseline envelope
            if (jump > 0.015f && jump > env * 0.4f) {
                transientRatio = 1.0f;
            }
            env = absIn;
        } else {
            env += (1.0f - alphaDecay) * (absIn - env);
        }

        // Decay transient ratio quickly back to 0 (sustain state)
        float currentT = transientRatio;
        transientRatio *= alphaTransientDecay;

        return currentT;
    }

    /**
     * Returns a scaling factor in [0.0, 1.0] for notch attenuation.
     * crispness: 0.0 (off, full static notch) to 1.0 (maximum punch preservation).
     * When a transient strikes, returns (1.0 - crispness * T), reducing notch depth.
     */
    inline float getNotchDepthMultiplier(float crispness) const {
        return std::clamp(1.0f - (crispness * transientRatio), 0.05f, 1.0f);
    }

    float getTransientRatio() const { return transientRatio; }

private:
    float sampleRate = 44100.0f;
    float alphaDecay = 0.0f;
    float alphaTransientDecay = 0.0f;
    float env = 0.0f;
    float transientRatio = 0.0f;
};

} // namespace FlyBy::DSP
