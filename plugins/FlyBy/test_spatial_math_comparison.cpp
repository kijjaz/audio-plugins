#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <string>
#include "Source/DSP/SpatialMath.h"
#include "Source/DSP/PinnaFilter.h"
#include "Source/DSP/HeadShadowFilter.h"
#include "Source/DSP/AnthropometricModel.h"

using namespace FlyBy::DSP;

struct AcousticMetrics {
    float azimuthDeg;
    float elevationDeg;
    float distanceM;
    
    float itdMicrosec;     // Interaural Time Difference (us)
    float ildDb;           // Interaural Level Difference (dB: Right - Left)
    float notch1FreqHz;    // Pinna Primary Notch
    float notch2FreqHz;    // Pinna Overhead Notch
    float torsoDipDb;      // Chest reflection comb depth
    float headShadowL_Db;  // High frequency attenuation at Left Ear
    float headShadowR_Db;  // High frequency attenuation at Right Ear
};

// Compute acoustic metrics from the model physics
AcousticMetrics analyzeAcoustics(float azDeg, float elDeg, float distM, float sampleRate = 48000.0f) {
    AcousticMetrics m;
    m.azimuthDeg = azDeg;
    m.elevationDeg = elDeg;
    m.distanceM = distM;

    SphericalCoord sphere(azDeg, elDeg, distM);
    Vector3 pos = sphere.toCartesian();

    // 1. Woodworth-Rayleigh ITD
    Vector3 leftEar(-HEAD_RADIUS, 0.0f, 0.0f);
    Vector3 rightEar(HEAD_RADIUS, 0.0f, 0.0f);

    float dL = (pos - leftEar).length();
    float dR = (pos - rightEar).length();
    float timeDiffSec = (dR - dL) / SPEED_OF_SOUND;
    m.itdMicrosec = timeDiffSec * 1.0e6f; // in microseconds

    // 2. Pinna Notches & Torso
    float elClamped = std::clamp(elDeg, -50.0f, 90.0f);
    float normEl = (elClamped + 50.0f) / 140.0f;
    m.notch1FreqHz = 6000.0f + normEl * 5200.0f;

    float overheadFactor = std::clamp(elClamped / 90.0f, 0.0f, 1.0f);
    m.notch2FreqHz = 9800.0f + overheadFactor * 3200.0f;

    float belowFactor = std::clamp(-elClamped / 50.0f, 0.0f, 1.0f);
    m.torsoDipDb = -8.0f * belowFactor;

    // 3. Head Shadow ILD (at 4 kHz high frequency)
    float azRad = azDeg * (PI / 180.0f);
    float leftTheta = azRad + (PI * 0.5f);
    float rightTheta = (PI * 0.5f) - azRad;

    HeadShadowFilter shadowL, shadowR;
    shadowL.prepare(sampleRate);
    shadowR.prepare(sampleRate);
    shadowL.update(leftTheta);
    shadowR.update(rightTheta);

    // Test tone at 4 kHz to measure ILD
    float testFreq = 4000.0f;
    float peakL = 0.0f, peakR = 0.0f;
    float phase = 0.0f;
    float delta = 2.0f * PI * (testFreq / sampleRate);
    for (int i = 0; i < 500; ++i) {
        float in = std::sin(phase);
        float outL = shadowL.process(in);
        float outR = shadowR.process(in);
        if (i > 300) {
            if (std::abs(outL) > peakL) peakL = std::abs(outL);
            if (std::abs(outR) > peakR) peakR = std::abs(outR);
        }
        phase += delta;
    }
    m.headShadowL_Db = 20.0f * std::log10(std::max(1e-4f, peakL));
    m.headShadowR_Db = 20.0f * std::log10(std::max(1e-4f, peakR));
    m.ildDb = m.headShadowR_Db - m.headShadowL_Db;

    return m;
}

void printHeader(const std::string& title) {
    std::cout << "\n=========================================================================================\n";
    std::cout << " " << title << "\n";
    std::cout << "=========================================================================================\n";
    std::cout << std::setw(8)  << "Az(deg)"
              << std::setw(8)  << "El(deg)"
              << std::setw(12) << "ITD (us)"
              << std::setw(12) << "ILD (dB)"
              << std::setw(14) << "Notch1 (Hz)"
              << std::setw(14) << "Notch2 (Hz)"
              << std::setw(12) << "Torso (dB)"
              << std::setw(14) << "Shadow (L/R)\n";
    std::cout << "-----------------------------------------------------------------------------------------\n";
}

void printRow(const AcousticMetrics& m) {
    std::string shadowStr = std::to_string((int)m.headShadowL_Db) + "/" + std::to_string((int)m.headShadowR_Db) + " dB";
    std::cout << std::fixed << std::setprecision(1)
              << std::setw(8)  << m.azimuthDeg
              << std::setw(8)  << m.elevationDeg
              << std::setw(12) << m.itdMicrosec
              << std::setw(12) << m.ildDb
              << std::setw(14) << (int)m.notch1FreqHz
              << std::setw(14) << (int)m.notch2FreqHz
              << std::setw(12) << m.torsoDipDb
              << std::setw(14) << shadowStr << "\n";
}

int main() {
    // -----------------------------------------------------------------------
    // TEST 1: PURE HORIZONTAL PANNING (Left -90 to Right +90, Elevation = 0)
    // -----------------------------------------------------------------------
    printHeader("TEST 1: PURE HORIZONTAL PANNING (Varying Azimuth, Elevation = 0 deg)");
    for (float az = -90.0f; az <= 90.0f; az += 30.0f) {
        auto m = analyzeAcoustics(az, 0.0f, 1.5f);
        printRow(m);
    }
    std::cout << "\n>> OBSERVATION 1 (Horizontal):\n"
              << "   - ITD sweeps smoothly from -485 us (left) to +485 us (right).\n"
              << "   - ILD sweeps from -8.3 dB to +8.3 dB (acoustic head shadow at 4kHz).\n"
              << "   - Pinna Notch1 remains frozen at 7857 Hz (constant horizon).\n"
              << "   - Torso reflection remains 0.0 dB (no chest bounce at eye level).\n";

    // -----------------------------------------------------------------------
    // TEST 2: PURE VERTICAL PANNING (Azimuth = 0, Elevation = -40 to +90)
    // -----------------------------------------------------------------------
    printHeader("TEST 2: PURE VERTICAL PANNING (Azimuth = 0 deg, Varying Elevation)");
    for (float el = -40.0f; el <= 90.0f; el += 20.0f) {
        auto m = analyzeAcoustics(0.0f, el, 1.5f);
        printRow(m);
    }
    std::cout << "\n>> OBSERVATION 2 (Vertical):\n"
              << "   - ITD is IDENTICAL at 0.0 us across ALL elevations (perfect interaural symmetry)!\n"
              << "   - ILD is IDENTICAL at 0.0 dB (both ears receive identical volume)!\n"
              << "   - PRIMARY CUE IS SPECTRAL:\n"
              << "     * Notch 1 sweeps dramatically from 6371 Hz (below) up to 11200 Hz (overhead).\n"
              << "     * Notch 2 only awakens at higher elevations, sweeping 9.8 kHz -> 13.0 kHz.\n"
              << "     * Torso reflection dips to -6.4 dB when below chin, disappearing above horizon.\n";

    // -----------------------------------------------------------------------
    // TEST 3: COMBINED 3D TRAJECTORY (Diagonal Climbing Swoop: Front-Left -> Up-Right)
    // -----------------------------------------------------------------------
    printHeader("TEST 3: COMBINED 3D TRAJECTORY (Climbing from Low-Left to High-Right)");
    struct Waypoint { float az, el; };
    std::vector<Waypoint> path = {
        { -90.0f, -40.0f }, // Low Left
        { -45.0f, -20.0f }, // Mid-Low Left
        {   0.0f,   0.0f }, // Center Horizon
        { +45.0f, +45.0f }, // High Right
        { +90.0f, +90.0f }  // Zenith Right (Overhead)
    };
    for (const auto& wp : path) {
        auto m = analyzeAcoustics(wp.az, wp.el, 1.5f);
        printRow(m);
    }
    std::cout << "\n>> OBSERVATION 3 (Combined 3D):\n"
              << "   - ITD and ILD shift as azimuth rotates across the median plane.\n"
              << "   - Notice that at Zenith (+90 deg overhead), even if azimuth is +90 deg,\n"
              << "     the sound is overhead near the head axis, so the ear distance difference shrinks!\n"
              << "   - Meanwhile, Notch1 climbs continuously from 6371 Hz to 11200 Hz.\n"
              << "   - The brain uses the combination of time/level arrival AND moving spectral notches\n"
              << "     to construct a flawless 3-dimensional trajectory.\n\n";

    return 0;
}
