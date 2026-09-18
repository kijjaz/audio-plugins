#pragma once
#include <vector>
#include <array>
#include <atomic>
#include <algorithm>
#include "NonlinearDelayLine.h"
#include "AcousticDefinitions.h"
#include "BinauralSpatializer.h"

class NonlinearWaveguideArray
{
public:
    static constexpr int MAX_BEAM_PATHS = 128;

    void prepare(double sampleRate, float maxDistanceMeters = 150.0f)
    {
        fs = static_cast<float>(sampleRate);
        float maxDelaySec = (maxDistanceMeters / AetherAcoustics::SPEED_OF_SOUND) + 0.15f;

        for (auto& delayLine : delayLines)
        {
            delayLine.prepare(sampleRate, maxDelaySec);
        }

        spatializer.prepare(sampleRate);
        reset();
    }

    void reset()
    {
        for (auto& delayLine : delayLines)
        {
            delayLine.reset();
        }
        spatializer.reset();
    }

    // Thread-safe update from UI or Audio thread (no allocations, double-buffered lock-free)
    void updateBeams(const AetherAcoustics::BeamDescriptor* descriptors, int count)
    {
        int writeIdx = 1 - activeBufferIndex.load(std::memory_order_relaxed);
        int n = std::min(count, MAX_BEAM_PATHS);

        for (int i = 0; i < n; ++i)
        {
            beamBuffers[writeIdx][i] = descriptors[i];
        }
        beamCounts[writeIdx] = n;

        // Atomic commit
        activeBufferIndex.store(writeIdx, std::memory_order_release);
    }

    inline void processSample(float inSample, float peakSPL, float nonlinearityScale, 
                              MicPolarPattern micPattern, float stereoWidth,
                              float& outL, float& outR, float& lateInjection)
    {
        outL = 0.0f;
        outR = 0.0f;
        lateInjection = 0.0f;

        int readIdx = activeBufferIndex.load(std::memory_order_acquire);
        int count = beamCounts[readIdx];
        const auto& beams = beamBuffers[readIdx];

        float pPeak = AetherAcoustics::P0_REF * std::pow(10.0f, peakSPL / 20.0f);
        float stiffness = AetherAcoustics::ACOUSTIC_STIFFNESS;

        for (int k = 0; k < count; ++k)
        {
            const auto& desc = beams[k];

            // 1. Write audio sample to path delay line
            delayLines[k].write(inSample);

            // 2. Physical wave-steepening parameters
            float d = desc.distanceMeters;
            float gamma = std::clamp(nonlinearityScale * (d * AetherAcoustics::BETA_AIR * pPeak) / stiffness * 5e3f, 0.0f, 0.6f);
            float steepeningDepth = nonlinearityScale * (d * AetherAcoustics::BETA_AIR * pPeak) / stiffness * fs * 0.1f;
            float alphaDamp = std::clamp(0.10f + 0.012f * d, 0.05f, 0.85f);

            // 3. Microphone Directionality & ITD/ILD
            float azimuthRad = std::atan2(desc.dirX, desc.dirY);
            float itdSecL = 0.0f, itdSecR = 0.0f;
            float micGainL = 1.0f, micGainR = 1.0f;

            BinauralSpatializer::computeMicrophoneResponse(micPattern, azimuthRad, itdSecL, itdSecR, micGainL, micGainR);

            float delayL = desc.delaySec + itdSecL;
            float delayR = desc.delaySec + itdSecR;

            // 4. Pressure-modulated read
            float pathL = delayLines[k].readNonlinear(delayL, steepeningDepth, gamma, alphaDamp);
            float pathR = delayLines[k].readNonlinear(delayR, steepeningDepth, gamma, alphaDamp);

            float gain = desc.gain * desc.absorptionFactor;
            float phaseSign = (desc.order == 0) ? 1.0f : ((k % 2 == 1) ? -1.0f : 1.0f);

            // 5. Accumulate early beams with polar pattern capsule weighting
            outL += pathL * gain * phaseSign * micGainL;
            outR += pathR * gain * phaseSign * micGainR;

            // 6. Seed late diffuse FDN with reflections (orders >= 1), scaled properly to energize the diffuse field
            if (desc.order >= 1)
            {
                lateInjection += (pathL + pathR) * 0.5f * (gain * 2.5f);
            }
        }

        // 7. Apply continuous Mid/Side stereo width
        BinauralSpatializer::applyStereoWidth(outL, outR, stereoWidth);
    }

private:
    float fs = 96000.0f;
    std::array<NonlinearDelayLine, MAX_BEAM_PATHS> delayLines;
    std::array<std::array<AetherAcoustics::BeamDescriptor, MAX_BEAM_PATHS>, 2> beamBuffers{};
    std::array<int, 2> beamCounts{ 0, 0 };
    std::atomic<int> activeBufferIndex{ 0 };
    BinauralSpatializer spatializer;
};
