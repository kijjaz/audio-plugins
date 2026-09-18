#pragma once
#include <cmath>
#include <algorithm>
#include "AcousticDefinitions.h"

enum class MicPolarPattern
{
    Binaural = 0,       // Woodworth spherical head diffraction + ILD shadow
    ORTF_Cardioid,      // 17cm spacing, 110-deg subtended cardioid pair
    Blumlein_Figure8,   // Coincident 90-deg figure-8 velocity ribbons
    Omni                // Coincident pressure capsules
};

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

    // Evaluates spatial microphone capsule gains and delays for various stereo configurations
    static inline void computeMicrophoneResponse(MicPolarPattern pattern, float azimuthRad,
                                                float& itdSecL, float& itdSecR,
                                                float& gainL, float& gainR)
    {
        switch (pattern)
        {
            case MicPolarPattern::ORTF_Cardioid:
            {
                // ORTF: 17 cm spacing, capsules angled at +/- 55 degrees (+/- 0.9599 rad)
                // Cardioid polar formula: g(theta) = 0.5 + 0.5 * cos(theta - theta_capsule)
                constexpr float ortfAngleL = -0.959931f;
                constexpr float ortfAngleR = +0.959931f;
                constexpr float ortfDistance = 0.17f; // 17 cm

                // Time of flight delay due to 17cm baseline
                float itd = (ortfDistance * 0.5f / AetherAcoustics::SPEED_OF_SOUND) * std::sin(azimuthRad);
                itdSecL = +itd;
                itdSecR = -itd;

                // Cardioid capsule response
                float angL = azimuthRad - ortfAngleL;
                float angR = azimuthRad - ortfAngleR;
                gainL = std::max(0.0f, 0.5f + 0.5f * std::cos(angL));
                gainR = std::max(0.0f, 0.5f + 0.5f * std::cos(angR));
                break;
            }

            case MicPolarPattern::Blumlein_Figure8:
            {
                // Blumlein: Coincident velocity ribbon capsules angled at +/- 45 degrees (+/- 0.7854 rad)
                // Figure-8 polar formula: g(theta) = cos(theta - theta_capsule)
                constexpr float blumAngleL = -0.785398f;
                constexpr float blumAngleR = +0.785398f;

                itdSecL = 0.0f; // Coincident capsules
                itdSecR = 0.0f;

                gainL = std::cos(azimuthRad - blumAngleL);
                gainR = std::cos(azimuthRad - blumAngleR);
                break;
            }

            case MicPolarPattern::Omni:
            {
                // Spaced Omni (AB stereo with 20 cm baseline)
                constexpr float abDistance = 0.20f;
                float itd = (abDistance * 0.5f / AetherAcoustics::SPEED_OF_SOUND) * std::sin(azimuthRad);
                itdSecL = +itd;
                itdSecR = -itd;
                gainL = 1.0f;
                gainR = 1.0f;
                break;
            }

            case MicPolarPattern::Binaural:
            default:
            {
                computeWoodworthITD(azimuthRad, itdSecL, itdSecR);
                gainL = 1.0f;
                gainR = 1.0f;
                break;
            }
        }
    }

    // Mid/Side (M/S) stereo width processor: width = 0.0 (mono), 1.0 (natural), 2.0 (super-wide)
    static inline void applyStereoWidth(float& inL, float& inR, float width)
    {
        float mid = 0.5f * (inL + inR);
        float side = 0.5f * (inL - inR);
        side *= width;
        inL = mid + side;
        inR = mid - side;
    }

    // Head-shadow ILD filter (contralateral high-shelf attenuation for Binaural mode)
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
