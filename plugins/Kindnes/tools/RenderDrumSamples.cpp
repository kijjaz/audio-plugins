#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <filesystem>
#include "../Source/DSP/KindnesEngine.h"
#include "../Source/Audio/KindnesPresets.h"

namespace fs = std::filesystem;

/**
 * Writes a stereo 24-bit PCM WAV file at 96,000 Hz.
 */
static bool writeWav24Bit96k(const std::string& filePath,
                             const std::vector<float>& left,
                             const std::vector<float>& right,
                             uint32_t sampleRate = 96000)
{
    if (left.empty() || left.size() != right.size())
        return false;

    const uint16_t numChannels = 2;
    const uint16_t bitsPerSample = 24;
    const uint16_t bytesPerSample = bitsPerSample / 8; // 3 bytes
    const uint32_t numFrames = static_cast<uint32_t>(left.size());
    const uint32_t dataChunkSize = numFrames * numChannels * bytesPerSample;
    const uint32_t fmtChunkSize = 16;
    const uint32_t riffChunkSize = 4 + (8 + fmtChunkSize) + (8 + dataChunkSize);
    const uint32_t byteRate = sampleRate * numChannels * bytesPerSample;
    const uint16_t blockAlign = numChannels * bytesPerSample;

    std::ofstream out(filePath, std::ios::binary);
    if (!out.is_open())
        return false;

    auto writeU32 = [&](uint32_t v) {
        uint8_t b[4] = {
            static_cast<uint8_t>(v & 0xFF),
            static_cast<uint8_t>((v >> 8) & 0xFF),
            static_cast<uint8_t>((v >> 16) & 0xFF),
            static_cast<uint8_t>((v >> 24) & 0xFF)
        };
        out.write(reinterpret_cast<const char*>(b), 4);
    };

    auto writeU16 = [&](uint16_t v) {
        uint8_t b[2] = {
            static_cast<uint8_t>(v & 0xFF),
            static_cast<uint8_t>((v >> 8) & 0xFF)
        };
        out.write(reinterpret_cast<const char*>(b), 2);
    };

    auto writeSample24 = [&](float s) {
        float clamped = std::clamp(s, -1.0f, 0.99999988f);
        int32_t pcm = static_cast<int32_t>(std::round(clamped * 8388607.0f));
        pcm = std::clamp(pcm, -8388608, 8388607);
        uint8_t b[3] = {
            static_cast<uint8_t>(pcm & 0xFF),
            static_cast<uint8_t>((pcm >> 8) & 0xFF),
            static_cast<uint8_t>((pcm >> 16) & 0xFF)
        };
        out.write(reinterpret_cast<const char*>(b), 3);
    };

    // RIFF header
    out.write("RIFF", 4);
    writeU32(riffChunkSize);
    out.write("WAVE", 4);

    // fmt subchunk
    out.write("fmt ", 4);
    writeU32(fmtChunkSize);
    writeU16(1); // PCM = 1
    writeU16(numChannels);
    writeU32(sampleRate);
    writeU32(byteRate);
    writeU16(blockAlign);
    writeU16(bitsPerSample);

    // data subchunk
    out.write("data", 4);
    writeU32(dataChunkSize);

    for (size_t i = 0; i < numFrames; ++i)
    {
        writeSample24(left[i]);
        writeSample24(right[i]);
    }

    return out.good();
}

/**
 * Smart Tail Detector & Auto-Trimmer:
 * Finds where the drum transient and analog filter tail naturally decay below
 * `silenceThresholdDb` (-68 dBFS) for `holdSilentSamples` (20 ms), trims
 * trailing dead air, and applies a smooth 5 ms cosine fade-out so every sample
 * ends at digital zero (0.0) with zero clicks.
 */
static void smartTrimAndFade(std::vector<float>& bufL,
                             std::vector<float>& bufR,
                             double sampleRate,
                             float minDurationSec = 0.045f,
                             float silenceThresholdDb = -68.0f)
{
    const size_t total = bufL.size();
    if (total == 0)
        return;

    // 1. Find global peak so threshold adapts if needed
    float maxPeak = 0.0f;
    size_t peakIdx = 0;
    for (size_t i = 0; i < total; ++i)
    {
        float a = std::max(std::abs(bufL[i]), std::abs(bufR[i]));
        if (a > maxPeak)
        {
            maxPeak = a;
            peakIdx = i;
        }
    }

    const float absThresh = std::pow(10.0f, silenceThresholdDb / 20.0f); // ~0.0004f (-68 dBFS)
    const float relThresh = maxPeak * 0.0008f;                          // -62 dB below peak
    const float threshold = std::max(absThresh, relThresh);

    const size_t minSamples = std::max<size_t>(
        static_cast<size_t>(minDurationSec * sampleRate),
        peakIdx + static_cast<size_t>(0.015 * sampleRate));
    const size_t holdSilentSamples = static_cast<size_t>(0.020 * sampleRate); // 20 ms continuous silence

    size_t consecutiveSilent = 0;
    size_t cutIndex = total;

    for (size_t i = minSamples; i < total; ++i)
    {
        float a = std::max(std::abs(bufL[i]), std::abs(bufR[i]));
        if (a < threshold)
        {
            consecutiveSilent++;
            if (consecutiveSilent >= holdSilentSamples)
            {
                cutIndex = i + 1;
                break;
            }
        }
        else
        {
            consecutiveSilent = 0;
        }
    }

    if (cutIndex < total)
    {
        bufL.resize(cutIndex);
        bufR.resize(cutIndex);
    }

    // Apply smooth 5 ms cosine fade-out over the tail end to guarantee zero-crossing termination
    const size_t fadeSamples = std::min<size_t>(bufL.size() / 4, static_cast<size_t>(0.005 * sampleRate));
    const size_t fadeStart = bufL.size() - fadeSamples;
    for (size_t i = 0; i < fadeSamples; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(std::max<size_t>(1, fadeSamples - 1));
        float gain = 0.5f * (1.0f + std::cos(t * 3.14159265359f));
        bufL[fadeStart + i] *= gain;
        bufR[fadeStart + i] *= gain;
    }
}

int main()
{
    constexpr double kSampleRate = 96000.0;
    constexpr int kWarmupSamples = 9600;       // 100 ms DC warmup before trigger
    constexpr int kMaxOneShotSamples = 192000; // 2.0 s max buffer for one-shots

    std::string outDir = "plugins/Kindnes/drum_samples_96k_24bit";
    fs::create_directories(outDir);

    std::cout << "=========================================================================================\n";
    std::cout << "  KINDNES v0.0.2 — 96,000 Hz / 24-BIT DRUM SAMPLE LIBRARY (32 VOICES + 4 GROOVES)\n";
    std::cout << "  Output Directory: " << outDir << "\n";
    std::cout << "=========================================================================================\n";
    std::cout << std::left << std::setw(46) << "File Name"
              << std::setw(14) << "Duration"
              << std::setw(14) << "Frames"
              << std::setw(12) << "Peak (dB)"
              << "RMS (dB)\n";
    std::cout << std::string(96, '-') << "\n";

    // All 32 Drum & Percussion Voices across 4 Banks
    struct VoiceInfo {
        std::string filename;
        int voiceIndex;
        float minDur;
    };

    const std::vector<VoiceInfo> voices = {
        // --- BANK 1: Classic NES Master Kit (0..7) ---
        { "01_Bank1_Sunsoft_Punch_Kick.wav",           0,  0.08f },
        { "02_Bank1_Mega_Crisp_Snare.wav",            1,  0.06f },
        { "03_Bank1_Closed_HiHat.wav",                2,  0.04f },
        { "04_Bank1_Open_HiHat.wav",                  3,  0.15f },
        { "05_Bank1_8Bit_Hand_Clap.wav",              4,  0.08f },
        { "06_Bank1_High_Rack_Tom.wav",               5,  0.08f },
        { "07_Bank1_Low_Floor_Tom.wav",               6,  0.10f },
        { "08_Bank1_Arcade_Crash_Cymbal.wav",         7,  0.25f },

        // --- BANK 2: VRC6 & FDS Synth Drums (8..15) ---
        { "09_Bank2_VRC6_Overdriven_Kick.wav",        8,  0.08f },
        { "10_Bank2_93Step_Metallic_Snare.wav",       9,  0.06f },
        { "11_Bank2_Chiptune_Rimshot.wav",            10, 0.04f },
        { "12_Bank2_8Bit_Arcade_Cowbell.wav",         11, 0.08f },
        { "13_Bank2_FDS_FM_Simmons_Tom.wav",          12, 0.10f },
        { "14_Bank2_Gated_Boss_Snare.wav",            13, 0.08f },
        { "15_Bank2_Chiptune_Shaker.wav",             14, 0.04f },
        { "16_Bank2_FDS_Metallic_Ride_Bell.wav",      15, 0.15f },

        // --- BANK 3: Sunsoft & Konami 1-Bit ROM Kit (16..23) ---
        { "17_Bank3_1Bit_Slap_Bass_Pop.wav",          16, 0.08f },
        { "18_Bank3_1Bit_Slap_Bass_Thumb.wav",        17, 0.10f },
        { "19_Bank3_Orchestra_Hit_Major.wav",         18, 0.12f },
        { "20_Bank3_Orchestra_Hit_Minor.wav",         19, 0.12f },
        { "21_Bank3_Konami_1Bit_Timpani.wav",         20, 0.12f },
        { "22_Bank3_2A03_Deep_Sub_Boom.wav",          21, 0.15f },
        { "23_Bank3_Slope_Overload_Fuzz_Snare.wav",   22, 0.08f },
        { "24_Bank3_1Bit_Vinyl_Scratch.wav",          23, 0.06f },

        // --- BANK 4: Retro Arcade Vocal & SFX Kit (24..31) ---
        { "25_Bank4_Speech_KINDNES.wav",              24, 0.15f },
        { "26_Bank4_Speech_POWER_UP.wav",             25, 0.15f },
        { "27_Bank4_Retro_Laser_Zapper.wav",          26, 0.08f },
        { "28_Bank4_Coin_Chime_B5_E6.wav",            27, 0.12f },
        { "29_Bank4_Jump_Spring_Boing.wav",           28, 0.10f },
        { "30_Bank4_Boss_Explosion_Rumble.wav",       29, 0.25f },
        { "31_Bank4_UFO_Warp_Siren.wav",              30, 0.18f },
        { "32_Bank4_PowerDown_Glitch.wav",            31, 0.12f }
    };

    KindnesDSP::KindnesEngine engine;
    engine.setSampleRate(kSampleRate);

    auto renderAndSave = [&](const std::string& filename,
                             std::vector<float>& bufL,
                             std::vector<float>& bufR,
                             float minDur,
                             float normTarget = 0.78f)
    {
        smartTrimAndFade(bufL, bufR, kSampleRate, minDur, -68.0f);

        float peak = 0.0f;
        for (size_t i = 0; i < bufL.size(); ++i)
            peak = std::max(peak, std::max(std::abs(bufL[i]), std::abs(bufR[i])));

        if (peak > 1e-4f && peak < 0.55f)
        {
            float scale = normTarget / peak;
            for (size_t i = 0; i < bufL.size(); ++i)
            {
                bufL[i] *= scale;
                bufR[i] *= scale;
            }
        }

        float finalPeak = 0.0f;
        double sumSq = 0.0;
        for (size_t i = 0; i < bufL.size(); ++i)
        {
            float m = 0.5f * (std::abs(bufL[i]) + std::abs(bufR[i]));
            finalPeak = std::max(finalPeak, std::max(std::abs(bufL[i]), std::abs(bufR[i])));
            sumSq += m * m;
        }
        float peakDb = 20.0f * std::log10(finalPeak + 1e-9f);
        float rmsDb = 20.0f * std::log10(std::sqrt(sumSq / static_cast<double>(bufL.size())) + 1e-9f);
        double durMs = (static_cast<double>(bufL.size()) / kSampleRate) * 1000.0;

        std::string fullPath = outDir + "/" + filename;
        writeWav24Bit96k(fullPath, bufL, bufR, 96000);

        std::ostringstream durStr;
        durStr << std::fixed << std::setprecision(1) << durMs << " ms";

        std::cout << std::left << std::setw(46) << filename
                  << std::setw(14) << durStr.str()
                  << std::setw(14) << bufL.size()
                  << std::setw(12) << std::fixed << std::setprecision(2) << peakDb
                  << std::fixed << std::setprecision(2) << rmsDb << "\n";
    };

    // Render each one-shot voice using triggerHybridDrumPad(voiceIndex, 0.95f)
    for (const auto& v : voices)
    {
        engine.reset();
        engine.setSampleRate(kSampleRate);
        engine.setAuthenticDAC(true);
        engine.setAnalogFilterEnabled(true);
        engine.setFilterTone(0.35f);
        engine.setPulse1Volume(0);
        engine.setPulse2Volume(0);
        engine.setTriangleVolume(0);
        engine.setNoiseVolume(0);
        engine.setFDSEnabled(false);
        engine.setVRC6Enabled(false);
        engine.setDpcmDrumPadMode(true);
        engine.setDPCMParameters(v.voiceIndex, false, 2, 15, false, false, 15);

        std::vector<float> warmL(kWarmupSamples, 0.0f);
        std::vector<float> warmR(kWarmupSamples, 0.0f);
        engine.processBlock(warmL.data(), warmR.data(), kWarmupSamples);

        std::vector<float> bufL(kMaxOneShotSamples, 0.0f);
        std::vector<float> bufR(kMaxOneShotSamples, 0.0f);

        engine.triggerHybridDrumPad(v.voiceIndex, 0.95f);
        engine.processBlock(bufL.data(), bufR.data(), kMaxOneShotSamples);

        renderAndSave(v.filename, bufL, bufR, v.minDur);
    }

    std::cout << std::string(96, '-') << "\n";
    std::cout << "Rendering Full-Bar (120 BPM) Drum Grooves:\n";

    // 4 Distinct Bank Grooves (1 full bar = 16 sixteenths @ 120 BPM, 2.0s hold + natural tail)
    for (int bank = 0; bank < 4; ++bank)
    {
        engine.reset();
        engine.setSampleRate(kSampleRate);
        engine.setAuthenticDAC(true);
        engine.setAnalogFilterEnabled(true);
        engine.setFilterTone(0.38f);
        engine.setPulse1Volume(0);
        engine.setPulse2Volume(0);
        engine.setTriangleVolume(0);
        engine.setNoiseVolume(0);
        engine.setDpcmDrumPadMode(true);
        engine.setArpEnabled(true);
        engine.setArpRateMode(KindnesDSP::ArpeggiatorMacro::RateMode::HostSync_16th, 120.0);
        engine.setDPCMParameters(bank * 8, false, 2, 15, false, false, 15);

        std::vector<float> warmL(kWarmupSamples, 0.0f);
        std::vector<float> warmR(kWarmupSamples, 0.0f);
        engine.processBlock(warmL.data(), warmR.data(), kWarmupSamples);

        const int beatHoldSamples = static_cast<int>(1.92 * kSampleRate);
        const int tailMaxSamples = static_cast<int>(0.60 * kSampleRate);
        const int totalMax = beatHoldSamples + tailMaxSamples;

        std::vector<float> bufL(totalMax, 0.0f);
        std::vector<float> bufR(totalMax, 0.0f);

        engine.noteOn(60, 0.95f);
        engine.processBlock(bufL.data(), bufR.data(), beatHoldSamples);
        engine.noteOff(60);
        engine.processBlock(bufL.data() + beatHoldSamples, bufR.data() + beatHoldSamples, tailMaxSamples);

        std::string grooveFilename;
        if (bank == 0) grooveFilename = "33_Groove_Bank1_NES_Master_Arcade_Beat_120BPM.wav";
        else if (bank == 1) grooveFilename = "34_Groove_Bank2_VRC6_FDS_Synth_Beat_120BPM.wav";
        else if (bank == 2) grooveFilename = "35_Groove_Bank3_Sunsoft_Konami_Funk_120BPM.wav";
        else grooveFilename = "36_Groove_Bank4_Retro_Arcade_SFX_Madness_120BPM.wav";

        renderAndSave(grooveFilename, bufL, bufR, 1.92f);
    }

    std::cout << std::string(96, '-') << "\n";
    std::cout << ">>> Successfully rendered 32 Drum Voices + 4 Grooves (36 files) at 96,000 Hz / 24-bit PCM! <<<\n";
    return 0;
}
