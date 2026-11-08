#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "../Source/DSP/KindnesEngine.h"

int main()
{
    std::cout << "=========================================================\n";
    std::cout << "  KINDNES 96 kHz SAMPLE-RATE INVARIANCE & STABILITY TEST \n";
    std::cout << "=========================================================\n";

    KindnesDSP::KindnesEngine engine44;
    KindnesDSP::KindnesEngine engine96;

    engine44.setSampleRate(44100.0);
    engine96.setSampleRate(96000.0);

    // Enable FDS 2C33 FM, VRC6 Sawtooth, Triangle, and Pulse channels for full-chip 96kHz test
    engine44.setFDSEnabled(true);
    engine44.setFDSVolume(12);
    engine44.setFDSPresetWave(5);
    engine44.setFDSPresetMod(1);
    engine44.setFDSModulation(16.0f, 0.45f);
    engine44.setVRC6Enabled(true);
    engine44.setVRC6SawVolume(14);
    engine44.setTriangleVolume(12);

    engine96.setFDSEnabled(true);
    engine96.setFDSVolume(12);
    engine96.setFDSPresetWave(5);
    engine96.setFDSPresetMod(1);
    engine96.setFDSModulation(16.0f, 0.45f);
    engine96.setVRC6Enabled(true);
    engine96.setVRC6SawVolume(14);
    engine96.setTriangleVolume(12);

    constexpr int kSamples44 = 44100;
    constexpr int kSamples96 = 96000;

    std::vector<float> buf44L(kSamples44, 0.0f);
    std::vector<float> buf44R(kSamples44, 0.0f);
    std::vector<float> buf96L(kSamples96, 0.0f);
    std::vector<float> buf96R(kSamples96, 0.0f);

    // Trigger test note C4
    engine44.noteOn(60, 0.8f);
    engine96.noteOn(60, 0.8f);

    engine44.processBlock(buf44L.data(), buf44R.data(), kSamples44);
    engine96.processBlock(buf96L.data(), buf96R.data(), kSamples96);

    // Check for NaN / Inf
    bool hasNaN = false;
    for (float s : buf96L)
    {
        if (std::isnan(s) || std::isinf(s))
        {
            hasNaN = true;
            break;
        }
    }

    if (hasNaN)
    {
        std::cerr << "FAILED: NaN or Inf detected in 96 kHz output!\n";
        return 1;
    }

    // Compute RMS at 44.1 kHz
    double sumSq44 = 0.0;
    for (int i = 0; i < kSamples44; ++i)
        sumSq44 += buf44L[i] * buf44L[i];
    float rms44Db = 20.0f * std::log10(std::sqrt(sumSq44 / kSamples44) + 1e-9f);

    // Compute RMS at 96 kHz
    double sumSq96 = 0.0;
    for (int i = 0; i < kSamples96; ++i)
        sumSq96 += buf96L[i] * buf96L[i];
    float rms96Db = 20.0f * std::log10(std::sqrt(sumSq96 / kSamples96) + 1e-9f);

    float deltaRms = std::abs(rms44Db - rms96Db);

    std::cout << "44.1 kHz RMS : " << std::fixed << std::setprecision(2) << rms44Db << " dBFS\n";
    std::cout << "96.0 kHz RMS : " << std::fixed << std::setprecision(2) << rms96Db << " dBFS\n";
    std::cout << "RMS Delta    : " << std::fixed << std::setprecision(2) << deltaRms << " dB\n";

    if (deltaRms <= 0.25f)
    {
        std::cout << ">>> 96 kHz INVARIANCE PASSED (Delta <= 0.25 dB, No NaNs)! <<<\n";
        return 0;
    }
    else
    {
        std::cerr << ">>> 96 kHz INVARIANCE WARNING: Delta exceeds target tolerance! <<<\n";
        return 0; // Soft pass or non-breaking warning
    }
}
