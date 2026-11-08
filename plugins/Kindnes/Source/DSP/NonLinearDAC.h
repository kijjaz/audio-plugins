#pragma once

#include <cmath>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Ricoh 2A03 Non-Linear DAC Mixing Bus.
 * 
 * Implements the authentic MOS resistor-ladder non-linear DAC transfer curves:
 * 
 *                  95.88
 *   pulse_out = ---------------------------
 *                (8128 / (p1 + p2)) + 100
 * 
 *                         159.79
 *   tnd_out   = --------------------------------------------
 *                     1
 *               ---------------------------------  + 100
 *                (tri/8227 + noise/12241 + dmc/22638)
 * 
 * Crucially, because triangle, noise, and DMC share the same resistive divider network,
 * high-level DPCM samples dynamically reduce ("duck") the triangle and noise channels!
 */
class NonLinearDAC
{
public:
    NonLinearDAC() = default;

    /**
     * Computes the mixed output.
     * @param p1 Pulse 1 level [0..15]
     * @param p2 Pulse 2 level [0..15]
     * @param tri Triangle level [0..15]
     * @param noise Noise level [0..15]
     * @param dmc DPCM counter level [0..127]
     * @param authenticDacMode If true, applies non-linear resistive curves; if false, performs linear sum
     * @return Normalized audio sample roughly in [-1.0, 1.0]
     */
    static inline float mix(float p1, float p2, float tri, float noise, float dmc, bool authenticDacMode = true) noexcept
    {
        if (authenticDacMode)
        {
            // --- 1. Pulse Non-Linear Network ---
            float pulseSum = p1 + p2;
            float pulseOut = 0.0f;
            if (pulseSum > 0.0001f)
            {
                pulseOut = 95.88f / ((8128.0f / pulseSum) + 100.0f);
            }

            // --- 2. Triangle / Noise / DPCM (TND) Non-Linear Network ---
            float tndDenom = (tri / 8227.0f) + (noise / 12241.0f) + (dmc / 22638.0f);
            float tndOut = 0.0f;
            if (tndDenom > 0.000001f)
            {
                tndOut = 159.79f / ((1.0f / tndDenom) + 100.0f);
            }

            // In 2A03 hardware, maximum theoretical output is approx 1.0 when all channels are pegged.
            float total = pulseOut + tndOut;
            if (total <= 0.00001f)
                return 0.0f;

            // Center active audio around zero
            return (total - 0.28f) * 1.8f;
        }
        else
        {
            // Linear digital mode (scaled to similar nominal loudness)
            float pNorm = (p1 + p2) / 30.0f;
            float tNorm = tri / 15.0f;
            float nNorm = noise / 15.0f;
            float dNorm = (dmc - 64.0f) / 64.0f;

            float linSum = 0.40f * pNorm + 0.30f * tNorm + 0.20f * nNorm + 0.25f * dNorm;
            return linSum * 0.65f;
        }
    }
};

} // namespace KindnesDSP
