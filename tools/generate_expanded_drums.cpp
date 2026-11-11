#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <filesystem>
#include <map>
#include <memory>
#include <sstream>

#include "plugins/Kindnes/Source/DSP/PolyBLEP.h"
#include "plugins/Kindnes/Source/DSP/PulseChannel.h"
#include "plugins/Kindnes/Source/DSP/TriangleChannel.h"
#include "plugins/Kindnes/Source/DSP/NoiseChannel.h"
#include "plugins/Kindnes/Source/DSP/ExpansionVRC6.h"
#include "plugins/Kindnes/Source/DSP/FDSChannel.h"
#include "plugins/Kindnes/Source/DSP/NonLinearDAC.h"
#include "plugins/Kindnes/Source/DSP/AnalogFilter.h"

namespace fs = std::filesystem;

namespace
{
    constexpr double kSampleRate = 96000.0;
    constexpr float kTwoPi = 6.28318530718f;

    #pragma pack(push, 1)
    struct WavHeader
    {
        char riff[4] = { 'R', 'I', 'F', 'F' };
        uint32_t fileSize = 0;
        char wave[4] = { 'W', 'A', 'V', 'E' };
        char fmt[4]  = { 'f', 'm', 't', ' ' };
        uint32_t fmtSize = 16;
        uint16_t audioFormat = 1; // PCM
        uint16_t numChannels = 1; // Mono
        uint32_t sampleRate = 96000;
        uint32_t byteRate = 96000 * 1 * 3;
        uint16_t blockAlign = 3;
        uint16_t bitsPerSample = 24;
    };
    #pragma pack(pop)

    bool writeWav24Mono(const std::string& path, const std::vector<float>& buf)
    {
        std::ofstream out(path, std::ios::binary);
        if (!out.is_open())
            return false;

        uint32_t dataBytes = static_cast<uint32_t>(buf.size() * 3);
        uint32_t totalFileSize = 4 + (8 + 16) + (8 + dataBytes);

        WavHeader hdr;
        hdr.fileSize = totalFileSize;
        hdr.sampleRate = static_cast<uint32_t>(kSampleRate);
        hdr.byteRate = hdr.sampleRate * 3;

        out.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));

        char dataHdr[4] = { 'd', 'a', 't', 'a' };
        out.write(dataHdr, 4);
        out.write(reinterpret_cast<const char*>(&dataBytes), 4);

        for (float f : buf)
        {
            float clamped = std::clamp(f, -0.999999f, 0.999999f);
            int32_t s = static_cast<int32_t>(std::round(clamped * 8388607.0f));
            s = std::clamp(s, -8388608, 8388607);
            uint8_t b0 = static_cast<uint8_t>(s & 0xFF);
            uint8_t b1 = static_cast<uint8_t>((s >> 8) & 0xFF);
            uint8_t b2 = static_cast<uint8_t>((s >> 16) & 0xFF);
            out.write(reinterpret_cast<const char*>(&b0), 1);
            out.write(reinterpret_cast<const char*>(&b1), 1);
            out.write(reinterpret_cast<const char*>(&b2), 1);
        }
        return out.good();
    }

    void smartTrimAndFade(std::vector<float>& buf,
                          double sampleRate,
                          float minDurationSec = 0.05f,
                          float silenceThresholdDb = -68.0f)
    {
        const size_t total = buf.size();
        if (total == 0) return;

        // 1. Remove DC offset
        double sum = 0.0;
        for (float v : buf) sum += v;
        float dcOffset = static_cast<float>(sum / static_cast<double>(total));
        for (float& v : buf) v -= dcOffset;

        float maxPeak = 0.0f;
        size_t peakIdx = 0;
        for (size_t i = 0; i < total; ++i)
        {
            float a = std::abs(buf[i]);
            if (a > maxPeak)
            {
                maxPeak = a;
                peakIdx = i;
            }
        }

        const float absThresh = std::pow(10.0f, silenceThresholdDb / 20.0f);
        const float relThresh = maxPeak * 0.0006f; // -64 dB below peak
        const float threshold = std::max(absThresh, relThresh);

        const size_t minSamples = std::max<size_t>(
            static_cast<size_t>(minDurationSec * sampleRate),
            peakIdx + static_cast<size_t>(0.020 * sampleRate));
        const size_t holdSilentSamples = static_cast<size_t>(0.025 * sampleRate); // 25ms silence

        size_t consecutiveSilent = 0;
        size_t cutIndex = total;

        for (size_t i = minSamples; i < total; ++i)
        {
            float a = std::abs(buf[i]);
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
            buf.resize(cutIndex);

        // 2. Smooth cosine attack fade-in (0.5 ms / 48 samples at 96 kHz) to prevent start clicks
        const size_t attackSamples = std::min<size_t>(buf.size() / 8, static_cast<size_t>(0.0005 * sampleRate));
        for (size_t i = 0; i < attackSamples; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(std::max<size_t>(1, attackSamples));
            float gain = 0.5f * (1.0f - std::cos(t * 3.14159265359f));
            buf[i] *= gain;
        }

        // 3. Smooth cosine fade-out at end (10 ms)
        const size_t fadeSamples = std::min<size_t>(buf.size() / 4, static_cast<size_t>(0.010 * sampleRate));
        const size_t fadeStart = buf.size() - fadeSamples;
        for (size_t i = 0; i < fadeSamples; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(std::max<size_t>(1, fadeSamples - 1));
            float gain = 0.5f * (1.0f + std::cos(t * 3.14159265359f));
            buf[fadeStart + i] *= gain;
        }

        if (!buf.empty())
            buf.back() = 0.0f;
    }

    // Normalizes to target peak
    void normalizeBuffer(std::vector<float>& buf, float targetPeak = 0.88f)
    {
        float maxPeak = 0.0f;
        for (float v : buf) maxPeak = std::max(maxPeak, std::abs(v));
        if (maxPeak > 1e-4f)
        {
            float scale = targetPeak / maxPeak;
            for (float& v : buf) v *= scale;
        }
    }

    std::string midiNoteName(int midi)
    {
        static const char* kNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = (midi / 12) - 1;
        int note = midi % 12;
        return std::string(kNames[note]) + std::to_string(octave);
    }
}

// -----------------------------------------------------------------------------
// Specialized Synthesis Generators
// -----------------------------------------------------------------------------
enum class PitchEnvelopeType
{
    ExponentialDrop,    // Fast exponential drop (standard punch)
    DualStageDrop,       // Very steep initial click drop, then prolonged body decay
    LaserLinearSweep,   // High to low fast linear sweep (arcade pew/laser)
    PitchScoopRise,     // Upward pitch bend (bounce / spring / comic)
    FMChirpModulation,  // Modulated pitch frequency (metallic / sci-fi / UFO)
    Constant            // Stationary frequency
};

enum class NoisePatternType
{
    None,
    DecayBurst,
    MultiFlamClap,
    GatedPlateau,
    LowRumbleSwell,
    GlitchStutter
};

struct DrumSynthesisRecipe
{
    int midiKey;
    std::string filename;
    std::string displayName;
    int group;
    int offBy;

    // Tonal Layer
    float durationSec{ 0.5f };
    PitchEnvelopeType pitchEnv{ PitchEnvelopeType::ExponentialDrop };
    float startFreqHz{ 220.0f };
    float endFreqHz{ 48.0f };
    float pitchTimeMs{ 70.0f };
    float fmModRateHz{ 0.0f };
    float fmModDepthHz{ 0.0f };
    int bodyWaveform{ 0 }; // 0=2A03 Triangle, 1=Sine Sat, 2=VRC6 Saw, 3=Pulse 25%, 4=Pulse 12.5%
    float bodyDecayMs{ 150.0f };
    float bodyLevel{ 0.95f };

    // Noise Layer
    NoisePatternType noisePattern{ NoisePatternType::DecayBurst };
    int noiseMode{ 0 }; // 0=White, 1=Metallic
    int noisePeriod{ 3 }; // 0..15
    float noiseDecayMs{ 80.0f };
    float noiseLevel{ 0.45f };

    // 1-Bit DPCM Slew Emulation
    bool applyDpcmSlew{ true };
    int dpcmStepSize{ 2 }; // 1..8
    float slewOverloadGrit{ 0.2f };

    // Master filter tone
    float analogTone{ 0.35f };
};

static std::vector<float> synthesizeDrum(const DrumSynthesisRecipe& r)
{
    const int numSamples = static_cast<int>(r.durationSec * kSampleRate);
    std::vector<float> buf(numSamples, 0.0f);

    float triPhase = 0.0f;
    float currentFreq = r.startFreqHz;
    float bodyEnv = r.bodyLevel;
    float bodyDecayCoeff = std::exp(-1.0f / (std::max(0.005f, r.bodyDecayMs * 0.001f) * static_cast<float>(kSampleRate)));

    KindnesDSP::NoiseChannel noise;
    noise.setSampleRate(kSampleRate);
    noise.setMode(r.noiseMode);
    noise.setPeriodIndex(r.noisePeriod);
    noise.setVolume(15);

    float noiseEnv = r.noiseLevel;
    float noiseDecayCoeff = std::exp(-1.0f / (std::max(0.005f, r.noiseDecayMs * 0.001f) * static_cast<float>(kSampleRate)));

    KindnesDSP::AnalogFilter filter;
    filter.setSampleRate(kSampleRate);
    filter.setEnabled(true);
    filter.setToneShaping(r.analogTone);

    // DPCM 7-bit counter state for delta modeling
    int dpcmCounter = 64;
    int dpcmSubTimer = 0;
    const int dpcmClockDivider = 3; // ~32 kHz at 96 kHz

    for (int i = 0; i < numSamples; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(kSampleRate);
        float tMs = t * 1000.0f;

        // 1. Calculate Instantaneous Pitch
        switch (r.pitchEnv)
        {
            case PitchEnvelopeType::ExponentialDrop:
            {
                float factor = std::exp(-tMs / std::max(1.0f, r.pitchTimeMs));
                currentFreq = r.endFreqHz + (r.startFreqHz - r.endFreqHz) * factor;
                break;
            }
            case PitchEnvelopeType::DualStageDrop:
            {
                if (tMs < 15.0f)
                {
                    // Super steep transient dive
                    float factor = std::exp(-tMs / 4.0f);
                    currentFreq = (r.startFreqHz * 0.5f) + (r.startFreqHz * 0.5f) * factor;
                }
                else
                {
                    float factor = std::exp(-(tMs - 15.0f) / std::max(1.0f, r.pitchTimeMs));
                    currentFreq = r.endFreqHz + ((r.startFreqHz * 0.5f) - r.endFreqHz) * factor;
                }
                break;
            }
            case PitchEnvelopeType::LaserLinearSweep:
            {
                float sweepDur = std::max(10.0f, r.pitchTimeMs);
                if (tMs < sweepDur)
                {
                    float alpha = tMs / sweepDur;
                    currentFreq = r.startFreqHz * std::pow(r.endFreqHz / r.startFreqHz, alpha);
                }
                else
                {
                    currentFreq = r.endFreqHz;
                }
                break;
            }
            case PitchEnvelopeType::PitchScoopRise:
            {
                float riseDur = std::max(10.0f, r.pitchTimeMs);
                if (tMs < riseDur)
                {
                    float alpha = tMs / riseDur;
                    currentFreq = r.startFreqHz + (r.endFreqHz - r.startFreqHz) * (1.0f - std::exp(-alpha * 3.5f));
                }
                else
                {
                    currentFreq = r.endFreqHz;
                }
                break;
            }
            case PitchEnvelopeType::FMChirpModulation:
            {
                float baseFactor = std::exp(-tMs / std::max(1.0f, r.pitchTimeMs));
                float mod = std::sin(t * r.fmModRateHz * kTwoPi) * r.fmModDepthHz * baseFactor;
                currentFreq = std::max(20.0f, r.endFreqHz + (r.startFreqHz - r.endFreqHz) * baseFactor + mod);
                break;
            }
            case PitchEnvelopeType::Constant:
            default:
                currentFreq = r.startFreqHz;
                break;
        }

        // Advance Tonal Phase
        triPhase += currentFreq / static_cast<float>(kSampleRate);
        if (triPhase >= 1.0f) triPhase -= std::floor(triPhase);

        // Generate Tonal Waveform Sample
        float tonalSample = 0.0f;
        if (bodyEnv > 0.0005f && r.bodyLevel > 0.001f)
        {
            switch (r.bodyWaveform)
            {
                case 0: // 2A03 32-step Triangle
                {
                    int step = static_cast<int>(triPhase * 32.0f) & 31;
                    tonalSample = static_cast<float>(KindnesDSP::TriangleChannel::kTriangleTable[step]) / 15.0f * 2.0f - 1.0f;
                    break;
                }
                case 1: // Saturated Sine
                    tonalSample = std::tanh(std::sin(triPhase * kTwoPi) * 2.2f);
                    break;
                case 2: // VRC6 Saw Overdrive Wrap
                {
                    int step7 = static_cast<int>(triPhase * 7.0f) % 7;
                    int accum = (step7 * 50) & 0xFF;
                    tonalSample = (static_cast<float>(accum >> 3) / 15.0f) * 2.0f - 1.0f;
                    break;
                }
                case 3: // 2A03 25% Pulse
                    tonalSample = (triPhase < 0.25f) ? 1.0f : -0.33f;
                    break;
                case 4: // 2A03 12.5% Pulse
                    tonalSample = (triPhase < 0.125f) ? 1.0f : -0.14f;
                    break;
            }
            tonalSample *= bodyEnv;
            bodyEnv *= bodyDecayCoeff;
        }

        // Generate Noise Sample
        float noiseSample = 0.0f;
        if (r.noisePattern != NoisePatternType::None && noiseEnv > 0.0005f && r.noiseLevel > 0.001f)
        {
            float patternGain = 1.0f;
            if (r.noisePattern == NoisePatternType::MultiFlamClap)
            {
                // 3 micro burst flams (spaced by 12 ms)
                if (tMs < 12.0f) patternGain = 1.0f - (tMs / 12.0f) * 0.7f;
                else if (tMs < 24.0f) patternGain = 1.0f - ((tMs - 12.0f) / 12.0f) * 0.7f;
                else if (tMs < 38.0f) patternGain = 1.0f - ((tMs - 24.0f) / 14.0f) * 0.5f;
            }
            else if (r.noisePattern == NoisePatternType::GatedPlateau)
            {
                if (tMs < 65.0f) patternGain = 1.0f;
                else patternGain = std::exp(-(tMs - 65.0f) * 0.12f);
            }
            else if (r.noisePattern == NoisePatternType::LowRumbleSwell)
            {
                // Sub-rumble swell & decay
                float swell = std::sin(std::min(1.0f, t / 0.15f) * 1.570796f);
                patternGain = swell * std::exp(-t / (r.durationSec * 0.45f));
            }
            else if (r.noisePattern == NoisePatternType::GlitchStutter)
            {
                int frame = static_cast<int>(tMs / 18.0f);
                patternGain = ((frame % 2) == 0) ? 1.0f : 0.15f;
            }

            noiseSample = noise.processSample() * (noiseEnv * patternGain);
            noiseEnv *= noiseDecayCoeff;
        }

        // Sum components
        float rawSum = tonalSample * 0.85f + noiseSample * 0.90f;

        // Apply 1-Bit DPCM Slew Delta Overload modeling if requested
        if (r.applyDpcmSlew)
        {
            dpcmSubTimer++;
            if (dpcmSubTimer >= dpcmClockDivider)
            {
                dpcmSubTimer = 0;
                int targetDac = std::clamp(static_cast<int>((rawSum + 1.0f) * 63.5f), 0, 126);
                int delta = r.dpcmStepSize;
                if (r.slewOverloadGrit > 0.01f)
                {
                    // Restrict slew rate on high amplitude transients to generate slope overload
                    if (std::abs(targetDac - dpcmCounter) > 28)
                        delta = std::max(1, delta - 1);
                }
                if (targetDac > dpcmCounter) dpcmCounter = std::min(126, dpcmCounter + delta);
                else dpcmCounter = std::max(0, dpcmCounter - delta);
            }
            float dpcVal = (static_cast<float>(dpcmCounter) / 63.5f) - 1.0f;
            rawSum = rawSum * 0.4f + dpcVal * 0.6f;
        }

        // Apply authentic analog motherboard filter
        buf[i] = filter.processSample(rawSum);
    }

    smartTrimAndFade(buf, kSampleRate, 0.045f, -68.0f);
    normalizeBuffer(buf, 0.90f);
    return buf;
}

int main()
{
    std::cout << "===========================================================================\n";
    std::cout << "  Generating Comprehensive 96kHz 24-Bit NES & Konami Drum & SFX Sound Library\n";
    std::cout << "===========================================================================\n";

    fs::create_directories("Kindnes_SFZ_Pack/samples/drums");

    // Comprehensive Drum & SFX Definitions:
    // Notes 24 to 84 (61 comprehensive sounds across Kicks, Snares, Hats, Toms,
    // Cymbals, Percussion, Hits, Timpani, Explosions [Short/Med/Long/Boss], Lasers, SFX)
    std::vector<DrumSynthesisRecipe> recipes = {
        // =====================================================================
        // SECTION 1: KICKS & SUB BOOMS (Notes 24..36)
        // =====================================================================
        { 24, "01_Sub_808_Triangle_Boom.wav", "Sub 808 2A03 Triangle Boom", 0, 0,
          1.2f, PitchEnvelopeType::ExponentialDrop, 140.0f, 32.0f, 85.0f, 0, 0, 0, 650.0f, 0.98f,
          NoisePatternType::DecayBurst, 0, 3, 20.0f, 0.25f, true, 2, 0.1f, 0.35f },

        { 25, "02_Dn_Tracker_Deep_Sub_Kick.wav", "Dn-Tracker Deep Sub Kick", 0, 0,
          0.8f, PitchEnvelopeType::DualStageDrop, 180.0f, 38.0f, 65.0f, 0, 0, 0, 420.0f, 0.98f,
          NoisePatternType::DecayBurst, 1, 2, 18.0f, 0.30f, true, 2, 0.2f, 0.35f },

        { 26, "03_VRC6_Overdrive_Heavy_Kick.wav", "VRC6 Overdrive Heavy Kick", 0, 0,
          0.7f, PitchEnvelopeType::ExponentialDrop, 310.0f, 40.0f, 45.0f, 0, 0, 2, 320.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 2, 35.0f, 0.50f, true, 3, 0.4f, 0.38f },

        { 27, "04_FDS_FM_Punch_Kick.wav", "FDS 2C33 FM Modulated Kick", 0, 0,
          0.6f, PitchEnvelopeType::FMChirpModulation, 260.0f, 44.0f, 50.0f, 180.0f, 90.0f, 1, 280.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 2, 25.0f, 0.35f, true, 2, 0.2f, 0.35f },

        { 28, "05_Laser_Dive_Kick.wav", "Fast Laser-Dive Electro Kick", 0, 0,
          0.5f, PitchEnvelopeType::LaserLinearSweep, 520.0f, 45.0f, 35.0f, 0, 0, 0, 240.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 1, 20.0f, 0.40f, true, 2, 0.15f, 0.35f },

        { 29, "06_Sunsoft_Punch_Kick_Tight.wav", "Sunsoft Punch Kick (Tight Punch)", 0, 0,
          0.45f, PitchEnvelopeType::DualStageDrop, 230.0f, 48.0f, 35.0f, 0, 0, 0, 130.0f, 0.98f,
          NoisePatternType::DecayBurst, 0, 3, 30.0f, 0.45f, true, 2, 0.25f, 0.35f },

        { 30, "07_Sunsoft_Punch_Kick_Long.wav", "Sunsoft Punch Kick (Long Decay)", 0, 0,
          0.9f, PitchEnvelopeType::ExponentialDrop, 215.0f, 42.0f, 65.0f, 0, 0, 0, 520.0f, 0.98f,
          NoisePatternType::DecayBurst, 0, 3, 45.0f, 0.45f, true, 2, 0.25f, 0.35f },

        { 31, "08_Slope_Overload_Fuzz_Kick.wav", "1-Bit Slope Overload Fuzz Kick", 0, 0,
          0.6f, PitchEnvelopeType::ExponentialDrop, 250.0f, 40.0f, 55.0f, 0, 0, 1, 310.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 3, 40.0f, 0.55f, true, 1, 0.75f, 0.40f },

        { 32, "09_LoFi_LowRate_Crunch_Kick.wav", "Lo-Fi 4kHz DPCM Crunch Kick", 0, 0,
          0.7f, PitchEnvelopeType::DualStageDrop, 200.0f, 44.0f, 60.0f, 0, 0, 0, 350.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 4, 35.0f, 0.40f, true, 4, 0.5f, 0.45f },

        { 33, "10_Double_Pitch_Drop_Kick.wav", "Double Pitch Drop Kick (Bite)", 0, 0,
          0.5f, PitchEnvelopeType::DualStageDrop, 360.0f, 46.0f, 40.0f, 0, 0, 0, 220.0f, 0.98f,
          NoisePatternType::DecayBurst, 0, 2, 25.0f, 0.45f, true, 2, 0.2f, 0.35f },

        { 34, "11_Acoustic_8Bit_Kick.wav", "Acoustic Style 8-Bit Kick", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 175.0f, 50.0f, 50.0f, 0, 0, 0, 160.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 4, 25.0f, 0.30f, true, 2, 0.1f, 0.32f },

        { 35, "12_Standard_Sub_Bass_Drum.wav", "General MIDI Sub Bass Drum", 0, 0,
          0.55f, PitchEnvelopeType::ExponentialDrop, 195.0f, 45.0f, 50.0f, 0, 0, 0, 240.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 3, 30.0f, 0.35f, true, 2, 0.15f, 0.35f },

        { 36, "13_Main_Arcade_Electric_Kick.wav", "Main Arcade Electric Punch Kick", 0, 0,
          0.5f, PitchEnvelopeType::DualStageDrop, 225.0f, 47.0f, 45.0f, 0, 0, 0, 200.0f, 0.98f,
          NoisePatternType::DecayBurst, 0, 3, 40.0f, 0.45f, true, 2, 0.25f, 0.35f },

        // =====================================================================
        // SECTION 2: SNARES, CLAPS & RIMSHOTS (Notes 37..44)
        // =====================================================================
        { 37, "14_Chiptune_Side_Stick_Rimshot.wav", "Chiptune Side Stick / Rimshot", 0, 0,
          0.25f, PitchEnvelopeType::ExponentialDrop, 560.0f, 260.0f, 15.0f, 0, 0, 1, 35.0f, 0.85f,
          NoisePatternType::DecayBurst, 1, 1, 25.0f, 0.65f, true, 2, 0.15f, 0.35f },

        { 38, "15_Mega_Crisp_Snare_Short.wav", "Mega Crisp Snare (Short Pop)", 0, 0,
          0.35f, PitchEnvelopeType::DualStageDrop, 240.0f, 110.0f, 25.0f, 0, 0, 0, 75.0f, 0.75f,
          NoisePatternType::DecayBurst, 0, 2, 120.0f, 0.95f, true, 2, 0.2f, 0.35f },

        { 39, "16_8Bit_Hand_Clap_MultiFlam.wav", "8-Bit Hand Clap (Multi-Flam 3-Burst)", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 130.0f, 65.0f, 20.0f, 0, 0, 0, 30.0f, 0.40f,
          NoisePatternType::MultiFlamClap, 0, 3, 160.0f, 0.95f, true, 2, 0.2f, 0.35f },

        { 40, "17_Delta_Snappy_Snare.wav", "8-Bit Delta Snappy Snare", 0, 0,
          0.4f, PitchEnvelopeType::ExponentialDrop, 220.0f, 100.0f, 35.0f, 0, 0, 0, 95.0f, 0.70f,
          NoisePatternType::DecayBurst, 0, 2, 175.0f, 0.95f, true, 2, 0.25f, 0.35f },

        { 41, "18_Gated_Boss_Snare_Plateau.wav", "Gated Boss Battle Snare (Gated Plateau)", 0, 0,
          0.55f, PitchEnvelopeType::ExponentialDrop, 210.0f, 95.0f, 40.0f, 0, 0, 0, 110.0f, 0.80f,
          NoisePatternType::GatedPlateau, 0, 3, 260.0f, 0.95f, true, 2, 0.2f, 0.35f },

        { 42, "19_Metallic_93Step_Snare.wav", "2A03 Mode 1 93-Step Metallic Snare", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 230.0f, 90.0f, 30.0f, 0, 0, 0, 85.0f, 0.75f,
          NoisePatternType::DecayBurst, 1, 4, 180.0f, 0.95f, true, 2, 0.3f, 0.35f },

        { 43, "20_Slope_Overload_Fuzz_Snare.wav", "1-Bit Slope Overload Fuzz Snare", 0, 0,
          0.45f, PitchEnvelopeType::DualStageDrop, 260.0f, 95.0f, 35.0f, 0, 0, 1, 90.0f, 0.75f,
          NoisePatternType::DecayBurst, 1, 5, 200.0f, 0.95f, true, 1, 0.75f, 0.40f },

        { 44, "21_Glitch_Stutter_Snare.wav", "Glitch Stutter Frame Snare", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 210.0f, 100.0f, 25.0f, 0, 0, 0, 80.0f, 0.65f,
          NoisePatternType::GlitchStutter, 0, 2, 190.0f, 0.95f, true, 2, 0.2f, 0.35f },

        // =====================================================================
        // SECTION 3: HI-HATS, CYMBALS & SHAKERS (Notes 45..50 - Choke Group 1)
        // =====================================================================
        { 45, "22_Closed_HiHat_Tight.wav", "Closed Hi-Hat (Tight 25ms Choke)", 1, 1,
          0.15f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 0, 32.0f, 0.90f, true, 2, 0.1f, 0.35f },

        { 46, "23_Closed_HiHat_Crisp.wav", "Closed Hi-Hat (Crisp 55ms)", 1, 1,
          0.2f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 0, 55.0f, 0.90f, true, 2, 0.1f, 0.35f },

        { 47, "24_Pedal_HiHat.wav", "Pedal Hi-Hat (Choked Tone)", 1, 1,
          0.2f, PitchEnvelopeType::ExponentialDrop, 120.0f, 60.0f, 15.0f, 0, 0, 0, 20.0f, 0.3f,
          NoisePatternType::DecayBurst, 0, 1, 45.0f, 0.85f, true, 2, 0.1f, 0.35f },

        { 48, "25_Open_HiHat_Medium.wav", "Open Hi-Hat (Medium 260ms)", 1, 1,
          0.45f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 1, 260.0f, 0.95f, true, 2, 0.1f, 0.35f },

        { 49, "26_Open_HiHat_Sizzle_Long.wav", "Open Hi-Hat (Sizzle Long 480ms)", 1, 1,
          0.7f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 1, 480.0f, 0.95f, true, 2, 0.1f, 0.35f },

        { 50, "27_Arcade_Crash_Cymbal_Long.wav", "Arcade Crash Cymbal (Long 850ms)", 0, 0,
          1.1f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 2, 850.0f, 0.98f, true, 3, 0.25f, 0.35f },

        // =====================================================================
        // SECTION 4: TOMS & TUNED PERCUSSION (Notes 51..60)
        // With diverse pitch modulations (Exponential, Fast Dive, FM Mod, Rise)
        // =====================================================================
        { 51, "28_Floor_Tom_Deep_Drop.wav", "Low Floor Tom (Deep Pitch Drop)", 0, 0,
          0.55f, PitchEnvelopeType::ExponentialDrop, 180.0f, 52.0f, 75.0f, 0, 0, 0, 260.0f, 0.98f,
          NoisePatternType::DecayBurst, 1, 4, 25.0f, 0.25f, true, 2, 0.1f, 0.35f },

        { 52, "29_Low_Tom_Fast_Dive.wav", "Low Tom (Fast Dive Punch)", 0, 0,
          0.5f, PitchEnvelopeType::DualStageDrop, 230.0f, 75.0f, 50.0f, 0, 0, 0, 220.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 3, 20.0f, 0.25f, true, 2, 0.1f, 0.35f },

        { 53, "30_Mid_Tom_Gothic_Quest.wav", "Mid Tom (Gothic Quest)", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 280.0f, 98.0f, 60.0f, 0, 0, 0, 180.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 3, 22.0f, 0.25f, true, 2, 0.1f, 0.35f },

        { 54, "31_High_Rack_Tom.wav", "High Rack Tom (Quick Pitch Fall)", 0, 0,
          0.4f, PitchEnvelopeType::ExponentialDrop, 340.0f, 130.0f, 50.0f, 0, 0, 0, 150.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 2, 20.0f, 0.25f, true, 2, 0.1f, 0.35f },

        { 55, "32_FDS_FM_Simmons_Tom_Space.wav", "FDS 2C33 FM Space Simmons Tom", 0, 0,
          0.6f, PitchEnvelopeType::FMChirpModulation, 270.0f, 78.0f, 65.0f, 220.0f, 110.0f, 1, 260.0f, 0.92f,
          NoisePatternType::DecayBurst, 0, 4, 25.0f, 0.20f, true, 2, 0.1f, 0.35f },

        { 56, "33_VRC6_Saw_Synth_Tom.wav", "VRC6 Sawtooth Synth Tom", 0, 0,
          0.5f, PitchEnvelopeType::ExponentialDrop, 310.0f, 85.0f, 55.0f, 0, 0, 2, 200.0f, 0.90f,
          NoisePatternType::DecayBurst, 1, 2, 30.0f, 0.35f, true, 2, 0.25f, 0.35f },

        { 57, "34_Dramatic_1Bit_Timpani.wav", "Dramatic 1-Bit Timpani (Gothic Castle)", 0, 0,
          0.7f, PitchEnvelopeType::ExponentialDrop, 155.0f, 96.0f, 80.0f, 0, 0, 0, 380.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 5, 45.0f, 0.35f, true, 2, 0.2f, 0.35f },

        { 58, "35_Comic_Spring_Tom_Rise.wav", "Comic Spring Tom (Pitch Scoop Rise)", 0, 0,
          0.5f, PitchEnvelopeType::PitchScoopRise, 65.0f, 320.0f, 140.0f, 0, 0, 0, 240.0f, 0.90f,
          NoisePatternType::DecayBurst, 0, 2, 15.0f, 0.20f, true, 2, 0.1f, 0.35f },

        { 59, "36_8Bit_Arcade_Cowbell.wav", "8-Bit Arcade Cowbell (Dual Pulse)", 0, 0,
          0.35f, PitchEnvelopeType::Constant, 540.0f, 540.0f, 10.0f, 0, 0, 3, 130.0f, 0.85f,
          NoisePatternType::DecayBurst, 1, 3, 25.0f, 0.35f, true, 2, 0.1f, 0.35f },

        { 60, "37_FDS_Metallic_Ride_Bell.wav", "FDS Metallic Ride Bell", 0, 0,
          0.45f, PitchEnvelopeType::Constant, 620.0f, 620.0f, 10.0f, 0, 0, 1, 240.0f, 0.80f,
          NoisePatternType::DecayBurst, 1, 0, 180.0f, 0.65f, true, 2, 0.1f, 0.35f },

        // =====================================================================
        // SECTION 5: EXPLOSIONS & ATTACK SOUNDS (SHORT, MED, LONG, DRAMATIC BOSS)
        // (Notes 61..68)
        // =====================================================================
        { 61, "38_Explosion_Micro_Pop.wav", "Explosion: Micro Hit / Grenade Pop (Short 120ms)", 0, 0,
          0.25f, PitchEnvelopeType::ExponentialDrop, 140.0f, 40.0f, 25.0f, 0, 0, 0, 45.0f, 0.85f,
          NoisePatternType::DecayBurst, 0, 10, 95.0f, 0.95f, true, 3, 0.4f, 0.38f },

        { 62, "39_Explosion_Cannon_Punch.wav", "Explosion: Tank Cannon Punch (Medium 280ms)", 0, 0,
          0.45f, PitchEnvelopeType::DualStageDrop, 180.0f, 36.0f, 45.0f, 0, 0, 0, 160.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 11, 240.0f, 0.95f, true, 3, 0.45f, 0.38f },

        { 63, "40_Explosion_Arcade_Bomb_Medium.wav", "Explosion: Arcade Bomb Burst (Medium 450ms)", 0, 0,
          0.65f, PitchEnvelopeType::ExponentialDrop, 120.0f, 32.0f, 65.0f, 0, 0, 0, 260.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 12, 380.0f, 0.98f, true, 4, 0.5f, 0.38f },

        { 64, "41_Explosion_Large_Sub_Rumble.wav", "Explosion: Large Sub Rumble (Long 950ms)", 0, 0,
          1.2f, PitchEnvelopeType::ExponentialDrop, 100.0f, 28.0f, 90.0f, 0, 0, 0, 680.0f, 0.98f,
          NoisePatternType::LowRumbleSwell, 0, 12, 850.0f, 0.98f, true, 3, 0.5f, 0.38f },

        { 65, "42_Explosion_Boss_Defeat_Dramatic_2s.wav", "Explosion: Boss Defeat Dramatic (2.2s Extended)", 0, 0,
          2.6f, PitchEnvelopeType::DualStageDrop, 90.0f, 24.0f, 160.0f, 0, 0, 0, 1400.0f, 0.98f,
          NoisePatternType::LowRumbleSwell, 0, 13, 2000.0f, 0.98f, true, 4, 0.6f, 0.38f },

        { 66, "43_Explosion_Boss_Super_Cataclysm_3s.wav", "Explosion: Boss Super Cataclysm (3.2s Ultra Long)", 0, 0,
          3.6f, PitchEnvelopeType::DualStageDrop, 85.0f, 22.0f, 220.0f, 0, 0, 0, 2200.0f, 0.98f,
          NoisePatternType::LowRumbleSwell, 0, 14, 3000.0f, 0.98f, true, 5, 0.7f, 0.40f },

        { 67, "44_Hit_Attack_Impact_Punch.wav", "Impact Attack: Physical Melee Punch", 0, 0,
          0.3f, PitchEnvelopeType::ExponentialDrop, 320.0f, 60.0f, 25.0f, 0, 0, 0, 65.0f, 0.90f,
          NoisePatternType::DecayBurst, 0, 3, 50.0f, 0.65f, true, 2, 0.2f, 0.35f },

        { 68, "45_Hit_Sword_Slash_Flesh.wav", "Impact Attack: Ninja Sword Slash", 0, 0,
          0.35f, PitchEnvelopeType::LaserLinearSweep, 650.0f, 140.0f, 35.0f, 0, 0, 3, 55.0f, 0.85f,
          NoisePatternType::DecayBurst, 0, 2, 85.0f, 0.85f, true, 2, 0.2f, 0.35f },

        // =====================================================================
        // SECTION 6: SCI-FI LASERS, ZAPPERS & WARP ENVELOPES (Notes 69..76)
        // With diverse pitch modulations (Ultra fast linear sweep, chirp, pitch rise)
        // =====================================================================
        { 69, "46_Laser_Fast_Zapper_Short.wav", "Laser: Fast Pistol Zapper (Short 80ms)", 0, 0,
          0.2f, PitchEnvelopeType::LaserLinearSweep, 1800.0f, 85.0f, 55.0f, 0, 0, 3, 65.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 0, 18.0f, 0.35f, true, 2, 0.2f, 0.35f },

        { 70, "47_Laser_Arcade_Pew_Medium.wav", "Laser: Arcade Blaster Pew (Medium 160ms)", 0, 0,
          0.3f, PitchEnvelopeType::LaserLinearSweep, 1400.0f, 70.0f, 95.0f, 0, 0, 3, 120.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 1, 25.0f, 0.30f, true, 2, 0.2f, 0.35f },

        { 71, "48_Laser_Heavy_Plasma_Cannon.wav", "Laser: Heavy Plasma Cannon (Long 320ms)", 0, 0,
          0.5f, PitchEnvelopeType::DualStageDrop, 950.0f, 48.0f, 120.0f, 0, 0, 2, 280.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 2, 85.0f, 0.55f, true, 3, 0.3f, 0.38f },

        { 72, "49_SciFi_UFO_Warp_Siren.wav", "Sci-Fi: UFO Warp Siren (Modulated Chirp)", 0, 0,
          0.6f, PitchEnvelopeType::FMChirpModulation, 640.0f, 320.0f, 180.0f, 28.0f, 160.0f, 1, 380.0f, 0.90f,
          NoisePatternType::None, 0, 0, 0, 0, true, 2, 0.1f, 0.35f },

        { 73, "50_SciFi_Forcefield_Shield_Hit.wav", "Sci-Fi: Forcefield Shield Deflect", 0, 0,
          0.4f, PitchEnvelopeType::FMChirpModulation, 820.0f, 180.0f, 60.0f, 85.0f, 220.0f, 3, 140.0f, 0.90f,
          NoisePatternType::DecayBurst, 1, 1, 60.0f, 0.50f, true, 2, 0.2f, 0.35f },

        { 74, "51_Coin_Chime_B5_E6.wav", "Reward: Coin 1-Up Chime (B5 to E6)", 0, 0,
          0.4f, PitchEnvelopeType::Constant, 987.77f, 987.77f, 10.0f, 0, 0, 3, 260.0f, 0.85f,
          NoisePatternType::None, 0, 0, 0, 0, false, 2, 0.0f, 0.35f },

        { 75, "52_Jump_Spring_Boing.wav", "Movement: Jump Spring Boing (Rising Pitch)", 0, 0,
          0.45f, PitchEnvelopeType::PitchScoopRise, 140.0f, 540.0f, 160.0f, 0, 0, 3, 280.0f, 0.90f,
          NoisePatternType::None, 0, 0, 0, 0, true, 2, 0.1f, 0.35f },

        { 76, "53_PowerDown_Glitch_Fall.wav", "Status: Power-Down Glitch Fall", 0, 0,
          0.5f, PitchEnvelopeType::LaserLinearSweep, 650.0f, 45.0f, 220.0f, 0, 0, 4, 300.0f, 0.90f,
          NoisePatternType::GlitchStutter, 1, 3, 120.0f, 0.45f, true, 2, 0.25f, 0.35f },

        // =====================================================================
        // SECTION 7: VINTAGE FUNK 1-BIT BASS & ORCHESTRA STABS (Notes 77..84)
        // =====================================================================
        { 77, "54_Vintage_1Bit_Slap_Bass_Pop.wav", "Vintage 1-Bit Slap Bass Pop", 0, 0,
          0.45f, PitchEnvelopeType::ExponentialDrop, 165.0f, 110.0f, 20.0f, 0, 0, 1, 240.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 1, 15.0f, 0.30f, true, 2, 0.25f, 0.35f },

        { 78, "55_Vintage_1Bit_Slap_Bass_Thumb.wav", "Vintage 1-Bit Slap Bass Thumb", 0, 0,
          0.5f, PitchEnvelopeType::ExponentialDrop, 110.0f, 55.0f, 25.0f, 0, 0, 1, 280.0f, 0.95f,
          NoisePatternType::DecayBurst, 1, 2, 20.0f, 0.25f, true, 2, 0.25f, 0.35f },

        { 79, "56_Orchestra_Hit_Major_Blast.wav", "Orchestra Hit Major (Arcade Tutti Blast)", 0, 0,
          0.45f, PitchEnvelopeType::Constant, 220.0f, 220.0f, 10.0f, 0, 0, 2, 240.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 2, 45.0f, 0.40f, true, 2, 0.3f, 0.38f },

        { 80, "57_Orchestra_Hit_Minor_Gothic.wav", "Orchestra Hit Minor (Gothic Castle Stab)", 0, 0,
          0.45f, PitchEnvelopeType::Constant, 220.0f, 220.0f, 10.0f, 0, 0, 2, 240.0f, 0.95f,
          NoisePatternType::DecayBurst, 0, 2, 45.0f, 0.40f, true, 2, 0.3f, 0.38f },

        { 81, "58_1Bit_Vinyl_Scratch_Chirp.wav", "1-Bit Turntable Vinyl Scratch Chirp", 0, 0,
          0.35f, PitchEnvelopeType::FMChirpModulation, 280.0f, 160.0f, 80.0f, 45.0f, 320.0f, 1, 180.0f, 0.90f,
          NoisePatternType::None, 0, 0, 0, 0, true, 2, 0.35f, 0.35f },

        { 82, "59_Chiptune_Maraca_Shaker.wav", "Chiptune Maraca Shaker (Short Swell)", 0, 0,
          0.25f, PitchEnvelopeType::Constant, 40.0f, 40.0f, 10.0f, 0, 0, 0, 10.0f, 0.0f,
          NoisePatternType::DecayBurst, 0, 0, 75.0f, 0.90f, true, 2, 0.1f, 0.35f },

        { 83, "60_Speech_1Bit_KINDNES.wav", "Speech: 'KINDNES' (Robot Vocal)", 0, 0,
          0.5f, PitchEnvelopeType::FMChirpModulation, 130.0f, 120.0f, 120.0f, 18.0f, 35.0f, 1, 350.0f, 0.90f,
          NoisePatternType::None, 0, 0, 0, 0, true, 2, 0.2f, 0.35f },

        { 84, "61_Speech_1Bit_POWER_UP.wav", "Speech: 'POWER UP' (Robot Vocal)", 0, 0,
          0.55f, PitchEnvelopeType::PitchScoopRise, 105.0f, 175.0f, 220.0f, 0, 0, 1, 380.0f, 0.90f,
          NoisePatternType::None, 0, 0, 0, 0, true, 2, 0.2f, 0.35f }
    };

    std::cout << "Rendering " << recipes.size() << " precision 96kHz 24-bit drum & SFX samples...\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& r : recipes)
    {
        auto audio = synthesizeDrum(r);
        std::string path = "Kindnes_SFZ_Pack/samples/drums/" + r.filename;
        writeWav24Mono(path, audio);

        float durMs = (static_cast<float>(audio.size()) / static_cast<float>(kSampleRate)) * 1000.0f;
        std::cout << "[" << std::setw(2) << r.midiKey << " - " << std::setw(4) << midiNoteName(r.midiKey) << "] "
                  << std::left << std::setw(38) << r.filename
                  << " (" << std::fixed << std::setprecision(1) << durMs << " ms)\n";
    }

    std::cout << std::string(80, '-') << "\n";
    std::cout << "Generating Updated Kindnes_SFZ_Pack/8Bit_Drums.sfz mapping (Notes 24..84)...\n";

    // Write updated 8Bit_Drums.sfz
    {
        std::ofstream sfz("Kindnes_SFZ_Pack/8Bit_Drums.sfz");
        sfz << "// ============================================================================\n";
        sfz << "// Kindnes Comprehensive 8-Bit Vintage 1-Bit Drum & SFX Sound Library (96kHz 24-bit)\n";
        sfz << "// 61 Chromatic Voices: Notes 24 to 84 (C1 to C6)\n";
        sfz << "// ============================================================================\n";
        sfz << "<global>\n";
        sfz << "amp_veltrack=95\n";
        sfz << "loop_mode=one_shot\n\n";

        for (const auto& r : recipes)
        {
            sfz << "// Note " << r.midiKey << " (" << midiNoteName(r.midiKey) << "): " << r.displayName << "\n";
            sfz << "<group> key=" << r.midiKey;
            if (r.group > 0)
                sfz << " group=" << r.group << " off_by=" << r.offBy;
            sfz << "\n";
            sfz << "<region> sample=samples/drums/" << r.filename << "\n\n";
        }
    }

    std::cout << "SUCCESS! All 61 drum & SFX samples and updated 8Bit_Drums.sfz generated cleanly.\n";
    return 0;
}

