#pragma once
#include <vector>
#include <cmath>
#include "AcousticDefinitions.h"

class NonlinearDelayLine
{
public:
    void prepare(double sampleRate, float maxDelaySec)
    {
        fs = static_cast<float>(sampleRate);
        bufferSize = static_cast<int>(std::ceil(maxDelaySec * fs)) + 16;
        buffer.assign(bufferSize, 0.0f);
        writePos = 0;
    }

    void reset()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writePos = 0;
        filterState = 0.0f;
        ap1_in = 0.0f;
        ap1_out = 0.0f;
        ap2_in = 0.0f;
        ap2_out = 0.0f;
    }

    inline void write(float inputSample)
    {
        buffer[writePos] = inputSample;
        writePos = (writePos + 1) % bufferSize;
    }

    // Read with dynamic wave-steepening delay modulation, surface scattering, and ISO 9613-1 air damping
    inline float readNonlinear(float nominalDelaySec, float pressureModDepth, float gammaFubini, float alphaDamp, float surfaceScattering = 0.0f)
    {
        // 1. Instantaneous delayed sample estimation
        float nominalDelaySamples = nominalDelaySec * fs;
        float readPosNominal = static_cast<float>(writePos) - nominalDelaySamples;
        while (readPosNominal < 0.0f) readPosNominal += static_cast<float>(bufferSize);

        int idx0 = static_cast<int>(readPosNominal);
        float frac = readPosNominal - static_cast<float>(idx0);
        int idx1 = (idx0 + 1) % bufferSize;
        float delayedEst = (1.0f - frac) * buffer[idx0] + frac * buffer[idx1];

        // 2. Wave steepening delay perturbation: positive crests arrive earlier
        float dynamicDelayMod = -pressureModDepth * delayedEst;
        float readPosActual = readPosNominal + dynamicDelayMod;
        while (readPosActual < 0.0f) readPosActual += static_cast<float>(bufferSize);
        while (readPosActual >= static_cast<float>(bufferSize)) readPosActual -= static_cast<float>(bufferSize);

        // 4-point Hermite/Lagrange interpolation
        int i1 = static_cast<int>(readPosActual);
        float f = readPosActual - static_cast<float>(i1);
        int i0 = (i1 - 1 + bufferSize) % bufferSize;
        int i2 = (i1 + 1) % bufferSize;
        int i3 = (i1 + 2) % bufferSize;

        float ym1 = buffer[i0];
        float y0  = buffer[i1];
        float y1  = buffer[i2];
        float y2  = buffer[i3];

        float c0 = y0;
        float c1 = 0.5f * (y1 - ym1);
        float c2 = ym1 - 2.5f * y0 + 2.0f * y1 - 0.5f * y2;
        float c3 = 0.5f * (y2 - ym1) + 1.5f * (y0 - y1);
        float interpolated = ((c3 * f + c2) * f + c1) * f + c0;

        // 3. Fubini dynamic waveshaping: y = x - gamma * x^3 (harmonic overtone steepening)
        float shaped = interpolated - gammaFubini * (interpolated * interpolated * interpolated);

        // 4. Surface Scattering & Roughness Allpass Dispersion
        float scattered = shaped;
        if (surfaceScattering > 0.01f)
        {
            float gScat = std::clamp(surfaceScattering * 0.45f, 0.0f, 0.45f);
            // 2-stage allpass phase-dispersion diffusion for micro-surface roughness
            float ap1 = -gScat * scattered + ap1_in;
            ap1_out = ap1;
            ap1_in = scattered + gScat * ap1;

            float ap2 = -gScat * ap1_out + ap2_in;
            ap2_out = ap2;
            ap2_in = ap1_out + gScat * ap2;

            scattered = (1.0f - surfaceScattering) * shaped + surfaceScattering * ap2_out;
        }

        // 5. ISO 9613-1 physical atmospheric molecular relaxation & wall absorption
        filterState = (1.0f - alphaDamp) * scattered + alphaDamp * filterState;
        return filterState;
    }

private:
    float fs = 96000.0f;
    std::vector<float> buffer;
    int bufferSize = 0;
    int writePos = 0;
    float filterState = 0.0f;
    float ap1_in = 0.0f, ap1_out = 0.0f;
    float ap2_in = 0.0f, ap2_out = 0.0f;
};
