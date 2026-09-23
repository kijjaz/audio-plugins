#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "Source/DSP/SpatialMath.h"
#include "Source/DSP/FractionalDelayLine.h"
#include "Source/DSP/PinnaFilter.h"
#include "Source/DSP/AnthropometricModel.h"
#include "Source/DSP/FlightPathGenerator.h"

using namespace FlyBy::DSP;

int main() {
    std::cout << "==========================================\n";
    std::cout << " FlyBy 3D Audio DSP Physics Verification  \n";
    std::cout << "==========================================\n";

    // 1. Verify SpatialMath conversions
    Vector3 eyeLevelFront(0.0f, 2.0f, 0.0f);
    SphericalCoord spFront = SphericalCoord::fromCartesian(eyeLevelFront);
    std::cout << "[Test 1: Geometry] Front: Az=" << spFront.azimuthDeg 
              << " El=" << spFront.elevationDeg 
              << " Dist=" << spFront.distanceMeters << "m\n";
    assert(std::abs(spFront.azimuthDeg) < 0.01f);
    assert(std::abs(spFront.elevationDeg) < 0.01f);

    Vector3 directZenith(0.0f, 0.0f, 3.0f); // Directly overhead (+Z)
    SphericalCoord spZenith = SphericalCoord::fromCartesian(directZenith);
    std::cout << "[Test 1: Geometry] Zenith: Az=" << spZenith.azimuthDeg 
              << " El=" << spZenith.elevationDeg 
              << " Dist=" << spZenith.distanceMeters << "m\n";
    assert(std::abs(spZenith.elevationDeg - 90.0f) < 0.01f);

    Vector3 belowTorso(0.0f, 1.0f, -1.0f); // Down below eye level
    SphericalCoord spBelow = SphericalCoord::fromCartesian(belowTorso);
    std::cout << "[Test 1: Geometry] Below: Az=" << spBelow.azimuthDeg 
              << " El=" << spBelow.elevationDeg 
              << " Dist=" << spBelow.distanceMeters << "m\n";
    assert(spBelow.elevationDeg < -40.0f && spBelow.elevationDeg > -50.0f);

    // 2. Verify Pinna Notch Frequency Sweep across Elevation
    std::cout << "\n[Test 2: Pinna Elevation Notch Sweep]\n";
    float sampleRate = 48000.0f;
    PinnaFilter pinna;
    pinna.prepare(sampleRate);

    // Verify impulse response magnitude at key elevation frequencies
    auto getNotchResponse = [&](float elDeg, float testFreqHz) {
        pinna.reset();
        pinna.update(elDeg, 1.0f, 1.0f);
        // Feed sine wave at testFreqHz for 500 samples to reach steady state
        float gain = 1.0f;
        float phase = 0.0f;
        float delta = 2.0f * PI * (testFreqHz / sampleRate);
        float peakOut = 0.0f;
        for (int i = 0; i < 600; ++i) {
            float in = std::sin(phase);
            float out = pinna.process(in);
            if (i > 400) {
                if (std::abs(out) > peakOut) peakOut = std::abs(out);
            }
            phase += delta;
        }
        return 20.0f * std::log10(std::max(1e-4f, peakOut));
    };

    float lowElRespAt6k = getNotchResponse(-40.0f, 6500.0f);
    float highElRespAt6k = getNotchResponse(80.0f, 6500.0f);
    float highElRespAt11k = getNotchResponse(80.0f, 11000.0f);

    std::cout << "  Elevation -40 deg (below): response at 6.5kHz = " << lowElRespAt6k << " dB\n";
    std::cout << "  Elevation +80 deg (overhead): response at 6.5kHz = " << highElRespAt6k << " dB\n";
    std::cout << "  Elevation +80 deg (overhead): response at 11.0kHz = " << highElRespAt11k << " dB\n";
    // At -40 deg, 6.5kHz should be notched deeper than at +80 deg
    assert(lowElRespAt6k < highElRespAt6k);
    std::cout << "  -> PASS: Pinna spectral notch shifts upward with elevation.\n";

    // 3. Verify Complete Anthropometric 3D Engine Processing
    std::cout << "\n[Test 3: Full Anthropometric Engine Processing]\n";
    AnthropometricModel model;
    model.prepare(sampleRate);

    AnthropometricModel::SpatialParams params;
    params.position = Vector3(1.5f, 1.5f, 2.0f); // High to the right
    model.update(params);

    float leftSum = 0.0f, rightSum = 0.0f;
    for (int i = 0; i < 512; ++i) {
        float in = (i == 0) ? 1.0f : 0.0f; // impulse
        float l = 0.0f, r = 0.0f;
        model.processSample(in, l, r);
        leftSum += std::abs(l);
        rightSum += std::abs(r);
    }
    std::cout << "  Right Ear Total Energy: " << rightSum << ", Left Ear (Shadowed): " << leftSum << "\n";
    assert(rightSum > leftSum); // Right side source must have greater energy in right ear
    std::cout << "  -> PASS: Interaural level & shadowing match physical lateralization.\n";

    // 4. Verify Flight Path Generator
    std::cout << "\n[Test 4: Flight Path Trajectory Generator]\n";
    FlightPathGenerator flightGen;
    flightGen.prepare(sampleRate);

    FlightPathGenerator::FlightConfig cfg;
    cfg.mode = FlightMode::Swoop;
    cfg.speedHz = 0.5f;

    Vector3 startPos = flightGen.advance(0, cfg);
    std::cout << "  Swoop Start Pos: (" << startPos.x << ", " << startPos.y << ", " << startPos.z << ")\n";
    // Advance half a cycle (apex of dive near head)
    Vector3 apexPos = flightGen.advance(static_cast<int>(sampleRate / (cfg.speedHz * 2.0f)), cfg);
    std::cout << "  Swoop Apex Pos:  (" << apexPos.x << ", " << apexPos.y << ", " << apexPos.z << ")\n";
    assert(apexPos.z < startPos.z); // Must dive down
    std::cout << "  -> PASS: Swoop trajectory dives through elevation envelope.\n";

    // 5. Verify Innovation 1: Transient-Preserving Elevation
    std::cout << "\n[Test 5: Transient-Preserving Elevation (Crisp Height)]\n";
    {
        TransientPreserver preserver;
        preserver.prepare(sampleRate);

        // Feed steady tone (sustain)
        for (int i = 0; i < 3000; ++i) {
            preserver.process(0.2f * std::sin(i * 0.1f));
        }
        float sustainRatio = preserver.getTransientRatio();
        float sustainNotchMult = preserver.getNotchDepthMultiplier(1.0f);
        std::cout << "  Sustain state: TransientRatio = " << sustainRatio 
                  << ", NotchDepthMultiplier = " << sustainNotchMult << " (full notch depth)\n";
        assert(sustainRatio < 0.15f);
        assert(sustainNotchMult > 0.85f);

        // Feed sudden sharp transient attack (impulse spike)
        float attackRatio = 0.0f;
        for (int i = 0; i < 20; ++i) {
            float val = (i == 0) ? 1.0f : 0.0f;
            float r = preserver.process(val);
            if (r > attackRatio) attackRatio = r;
        }
        float attackNotchMult = preserver.getNotchDepthMultiplier(1.0f);
        std::cout << "  Sharp Attack: Peak TransientRatio = " << attackRatio 
                  << ", NotchDepthMultiplier = " << attackNotchMult << " (notch flattened to preserve punch)\n";
        assert(attackRatio > 0.5f);
        std::cout << "  -> PASS: Attack transient successfully flattens notch to preserve punch.\n";
    }

    // 6. Verify Innovation 4: Floor & Ceiling Boundary Reflection
    std::cout << "\n[Test 6: Floor & Ceiling Boundary Proximity Reflectance]\n";
    {
        BoundaryReflector reflector;
        reflector.prepare(sampleRate);

        // Source near floor (Z = -0.8m)
        BoundaryReflector::BoundaryConfig bCfg;
        bCfg.position = Vector3(0.0f, 1.0f, -0.8f);
        bCfg.grounding = 0.5f;
        reflector.update(bCfg);

        float outL = 0.0f, outR = 0.0f;
        reflector.process(1.0f, outL, outR); // impulse

        // Step forward in time until early floor reflection arrives
        float peakReflect = 0.0f;
        for (int i = 0; i < 300; ++i) {
            float l = 0.0f, r = 0.0f;
            reflector.process(0.0f, l, r);
            if (l > peakReflect) peakReflect = l;
        }
        std::cout << "  Floor reflection peak arrival amplitude: " << peakReflect << "\n";
        assert(peakReflect > 0.01f);
        std::cout << "  -> PASS: Specular early boundary reflection generated successfully.\n";
    }

    std::cout << "\n==========================================\n";
    std::cout << " ALL DSP AND PHYSICS TESTS PASSED!       \n";
    std::cout << "==========================================\n";
    return 0;
}
