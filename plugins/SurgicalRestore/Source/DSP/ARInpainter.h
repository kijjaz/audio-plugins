#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace sr_dsp
{

/**
 * High-precision Autoregressive (AR) & Cubic Hermite Inpainter.
 * Reconstructs missing/corrupted samples with continuous first & second derivatives.
 */
class ARInpainter
{
public:
    static void inpaint (float* buffer, int totalSamples, int startBad, int endBad)
    {
        int gapLen = endBad - startBad + 1;
        if (gapLen <= 0 || startBad < 4 || endBad >= totalSamples - 4)
            return;

        // Context points
        int p0 = std::max (0, startBad - 3);
        int p1 = startBad - 1;
        int p2 = endBad + 1;
        int p3 = std::min (totalSamples - 1, endBad + 3);

        float y0 = buffer[p0];
        float y1 = buffer[p1];
        float y2 = buffer[p2];
        float y3 = buffer[p3];

        // Slopes at boundary
        float m1 = 0.5f * (y2 - y0) / (float)(p2 - p0);
        float m2 = 0.5f * (y3 - y1) / (float)(p3 - p1);

        // Cubic Hermite Spline interpolation across gap
        for (int i = startBad; i <= endBad; ++i)
        {
            float t = (float)(i - p1) / (float)(p2 - p1);
            float t2 = t * t;
            float t3 = t2 * t;

            float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
            float h10 = t3 - 2.0f * t2 + t;
            float h01 = -2.0f * t3 + 3.0f * t2;
            float h11 = t3 - t2;

            buffer[i] = h00 * y1 + h10 * (float)(p2 - p1) * m1 + h01 * y2 + h11 * (float)(p2 - p1) * m2;
        }
    }
};

} // namespace sr_dsp
