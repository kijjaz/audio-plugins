#pragma once
#include <cmath>
#include <algorithm>
#include "AcousticDefinitions.h"

class BinauralSpatializer
{
public:
    void prepare(double sampleRate)
    {
        fs = static_cast<float>(sampleRate);
        reset();
    }

    void reset()
    {
        filterStateL = 0.0f;
        filterStateR = 0.0f;
    }

    // Computes Woodworth Interaural Time Difference (ITD in seconds)
    static inline void computeWoodworthITD(float azimuthRad, float& itdSecL, float& itdSecR)
    {
        float theta = std::clamp(azimuthRad, -1.5707963f, 1.5707963f);
        float absTheta = std::abs(theta);
        float totalItdSec = (AetherAcoustics::HEAD_RADIUS / AetherAcoustics::SPEED_OF_SOUND) * (absTheta + std::sin(absTheta));

        if (theta >= 0.0f) // Sound from Right
        {
            itdSecL = +totalItdSec * 0.5f; // Left ear delayed
            itdSecR = -totalItdSec * 0.5f; // Right ear advanced
        }
        else // Sound from Left
        {
            itdSecL = -totalItdSec * 0.5f;
            itdSecR = +totalItdSec * 0.5f;
        }
    }

    // Head-shadow ILD filter (contralateral high-shelf attenuation)
    inline void processHeadShadow(float inSample, float azimuthRad, float& outL, float& outR)
    {
        float theta = std::clamp(azimuthRad, -1.5707963f, 1.5707963f);
        float absTheta = std::abs(theta);
        float alphaShadow = std::sin(absTheta) * 0.45f;
        float w0 = 2.0f * 3.14159265f * 1800.0f / fs;
        float decay = std::exp(-w0);

        if (theta >= 0.0f) // Right ear clear, Left ear shadowed
        {
            outR = inSample;
            float targetL = inSample * (1.0f - alphaShadow);
            filterStateL = targetL + decay * (filterStateL - inSample);
            outL = filterStateL;
        }
        else // Left ear clear, Right ear shadowed
        {
            outL = inSample;
            float targetR = inSample * (1.0f - alphaShadow);
            filterStateR = targetR + decay * (filterStateR - inSample);
            outR = filterStateR;
        }
    }

private:
    float fs = 96000.0f;
    float filterStateL = 0.0f;
    float filterStateR = 0.0f;
};
