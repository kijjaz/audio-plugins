#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "plugins/Kindnes/Source/DSP/PolyBLEP.h"
#include "plugins/Kindnes/Source/DSP/PulseChannel.h"
#include "plugins/Kindnes/Source/DSP/TriangleChannel.h"
#include "plugins/Kindnes/Source/DSP/NoiseChannel.h"
#include "plugins/Kindnes/Source/DSP/ExpansionVRC6.h"
#include "plugins/Kindnes/Source/DSP/FDSChannel.h"

namespace
{
    constexpr double kSampleRate = 96000.0;
    constexpr int kRenderSamples = 144000; // 1.5 seconds at 96 kHz

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

    struct SmplChunk
    {
        char chunkId[4] = { 's', 'm', 'p', 'l' };
        uint32_t chunkSize = 36 + 24; // 60 bytes for 1 loop
        uint32_t manufacturer = 0;
        uint32_t product = 0;
        uint32_t samplePeriod = 10416; // 1e9 / 96000 = 10416 ns
        uint32_t midiUnityNote = 60;
        uint32_t midiPitchFraction = 0;
        uint32_t smpteFormat = 0;
        uint32_t smpteOffset = 0;
        uint32_t numSampleLoops = 1;
        uint32_t samplerData = 0;

        // Loop point
        uint32_t loopCuePointId = 0;
        uint32_t loopType = 0; // 0 = forward loop
        uint32_t loopStart = 0;
        uint32_t loopEnd = 0;
        uint32_t loopFraction = 0;
        uint32_t loopPlayCount = 0; // infinite
    };
    #pragma pack(pop)

    bool writeWav24(const std::string& path, const std::vector<float>& buf,
                    uint32_t midiKey, uint32_t loopStart, uint32_t loopEnd)
    {
        std::ofstream out(path, std::ios::binary);
        if (!out.is_open())
            return false;

        uint32_t dataBytes = static_cast<uint32_t>(buf.size() * 3);
        SmplChunk smpl;
        smpl.midiUnityNote = midiKey;
        smpl.loopStart = loopStart;
        smpl.loopEnd = loopEnd;

        uint32_t totalFileSize = 4 + (8 + 16) + (8 + sizeof(SmplChunk)) + (8 + dataBytes);

        WavHeader hdr;
        hdr.fileSize = totalFileSize;
        hdr.sampleRate = static_cast<uint32_t>(kSampleRate);
        hdr.byteRate = hdr.sampleRate * 3;

        out.write(reinterpret_cast<const char*>(&hdr), sizeof(hdr));

        // Write smpl chunk
        out.write(reinterpret_cast<const char*>(&smpl), sizeof(smpl));

        // Write data chunk header
        char dataHdr[4] = { 'd', 'a', 't', 'a' };
        out.write(dataHdr, 4);
        out.write(reinterpret_cast<const char*>(&dataBytes), 4);

        // Write 24-bit PCM samples
        for (float f : buf)
        {
            float clamped = std::clamp(f, -0.999999f, 0.999999f);
            int32_t s = static_cast<int32_t>(clamped * 8388607.0f);
            uint8_t b0 = static_cast<uint8_t>(s & 0xFF);
            uint8_t b1 = static_cast<uint8_t>((s >> 8) & 0xFF);
            uint8_t b2 = static_cast<uint8_t>((s >> 16) & 0xFF);
            out.write(reinterpret_cast<const char*>(&b0), 1);
            out.write(reinterpret_cast<const char*>(&b1), 1);
            out.write(reinterpret_cast<const char*>(&b2), 1);
        }

        return true;
    }

    struct LoopInfo
    {
        uint32_t start = 0;
        uint32_t end = 0;
    };

    LoopInfo findZeroCrossingLoop(const std::vector<float>& buf, double freqHz)
    {
        double periodSamples = kSampleRate / freqHz;
        int searchStart = static_cast<int>(kSampleRate * 0.4); // 0.4s
        int targetEnd   = static_cast<int>(kSampleRate * 1.3); // 1.3s

        int loopStart = -1;
        for (int i = searchStart; i < searchStart + 5000 && i < static_cast<int>(buf.size()) - 1; ++i)
        {
            if (buf[i] <= 0.0f && buf[i + 1] > 0.0f)
            {
                loopStart = i;
                break;
            }
        }
        if (loopStart == -1) loopStart = searchStart;

        int numPeriods = static_cast<int>(std::round((targetEnd - loopStart) / periodSamples));
        if (numPeriods < 1) numPeriods = 1;
        int approxEnd = loopStart + static_cast<int>(numPeriods * periodSamples);

        int loopEnd = -1;
        int bestDist = 999999;
        int window = std::max(5, static_cast<int>(periodSamples * 0.5));
        for (int i = approxEnd - window; i <= approxEnd + window && i < static_cast<int>(buf.size()) - 1; ++i)
        {
            if (buf[i] <= 0.0f && buf[i + 1] > 0.0f)
            {
                int dist = std::abs(i - approxEnd);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    loopEnd = i;
                }
            }
        }
        if (loopEnd == -1) loopEnd = std::min(approxEnd, static_cast<int>(buf.size()) - 1);

        return { static_cast<uint32_t>(loopStart), static_cast<uint32_t>(loopEnd) };
    }

    double midiToFreq(int midi)
    {
        return 440.0 * std::pow(2.0, (midi - 69) / 12.0);
    }

    std::string midiNoteName(int midi)
    {
        static const char* kNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
        int octave = (midi / 12) - 1;
        int note = midi % 12;
        return std::string(kNames[note]) + std::to_string(octave);
    }

    std::string sanitizeFilename(const std::string& str)
    {
        std::string s = str;
        std::replace(s.begin(), s.end(), '#', 's');
        return s;
    }
}

int main()
{
    std::cout << "=== Kindnes 96kHz SFZ Pack Generator ===" << std::endl;

    // Multi-sample intervals: every minor 3rd from C1 (24) to C7 (96) + C8 (108)
    const std::vector<int> sampleNotes = {
        24, 27, 30, 33,
        36, 39, 42, 45,
        48, 51, 54, 57,
        60, 63, 66, 69,
        72, 75, 78, 81,
        84, 87, 90, 93,
        96, 99, 102, 105, 108
    };

    struct WaveformDef
    {
        std::string id;
        std::string displayName;
        std::string presetFilename;
        int swKey; // MIDI keyswitch
        int generatorType; // 0=Pulse12, 1=Pulse25, 2=Pulse50, 3=Pulse75, 4=Tri32, 5=Vrc6SawClean, 6=Vrc6SawOverdrive, 7=Vrc6ThinPulse, 8=FdsFm
    };

    const std::vector<WaveformDef> waveforms = {
        { "pulse12",           "2A03 Pulse 12.5% (Duty 0)",                       "01_2A03_Pulse_12.5.sfz",        12, 0 },
        { "pulse25",           "2A03 Pulse 25.0% (Duty 1)",                       "02_2A03_Pulse_25.0.sfz",        13, 1 },
        { "pulse50",           "2A03 Pulse 50.0% (Duty 2)",                       "03_2A03_Pulse_50.0.sfz",        14, 2 },
        { "pulse75",           "2A03 Pulse 75.0% (Duty 3 / 180 deg)",             "04_2A03_Pulse_75.0.sfz",        15, 3 },
        { "tri32",             "2A03 32-Step Staircase Triangle (DAC Pop)",       "05_2A03_Triangle_32Step.sfz",    16, 4 },
        { "vrc6_saw_clean",    "VRC6 14-Step Clean Sawtooth",                     "06_VRC6_Clean_Saw.sfz",         19, 5 },
        { "vrc6_saw_overdrive","VRC6 8-Bit Overdrive Wrap Saw",                   "07_VRC6_Overdrive_Saw.sfz",     20, 6 },
        { "vrc6_pulse_thin",   "VRC6 1/16 Thin Needle Pulse (6.25%)",             "08_VRC6_Thin_Pulse.sfz",        21, 7 },
        { "fds_fm_lead",       "FDS 2C33 Hardware FM Lead",                       "09_FDS_2C33_FM_Lead.sfz",       22, 8 }
    };

    // Buffer for synthesis
    std::vector<float> audioBuf(kRenderSamples);

    // Structure to record metadata for SFZ generation
    struct RegionMeta
    {
        std::string relPath;
        int lokey;
        int hikey;
        int center;
        uint32_t loopStart;
        uint32_t loopEnd;
    };
    std::vector<std::vector<RegionMeta>> instrumentRegions(waveforms.size());

    for (size_t w = 0; w < waveforms.size(); ++w)
    {
        const auto& wave = waveforms[w];
        std::cout << "Rendering " << wave.displayName << " across 29 octaves/notes..." << std::endl;

        for (size_t n = 0; n < sampleNotes.size(); ++n)
        {
            int midiNote = sampleNotes[n];
            double freq = midiToFreq(midiNote);

            // Compute key range
            int lokey = (n == 0) ? 24 : (sampleNotes[n - 1] + midiNote) / 2 + 1;
            int hikey = (n == sampleNotes.size() - 1) ? 127 : (midiNote + sampleNotes[n + 1]) / 2;

            std::fill(audioBuf.begin(), audioBuf.end(), 0.0f);

            // Render specific waveform DSP
            if (wave.generatorType >= 0 && wave.generatorType <= 3)
            {
                KindnesDSP::PulseChannel pulse(true);
                pulse.setSampleRate(kSampleRate);
                pulse.setFrequency(static_cast<float>(freq));
                pulse.setVolume(15);
                if (wave.generatorType == 0) pulse.setDutyMode(KindnesDSP::PulseChannel::DutyMode::Duty12_5);
                else if (wave.generatorType == 1) pulse.setDutyMode(KindnesDSP::PulseChannel::DutyMode::Duty25_0);
                else if (wave.generatorType == 2) pulse.setDutyMode(KindnesDSP::PulseChannel::DutyMode::Duty50_0);
                else if (wave.generatorType == 3) pulse.setDutyMode(KindnesDSP::PulseChannel::DutyMode::Duty75_0);

                // Warmup
                for (int i = 0; i < 4800; ++i) pulse.processSample();
                for (int i = 0; i < kRenderSamples; ++i)
                {
                    // Map unipolar DAC [0..15] to bipolar [-1.0, 1.0]
                    float raw = pulse.processSample();
                    audioBuf[i] = ((raw / 15.0f) * 2.0f - 1.0f) * 0.85f;
                }
            }
            else if (wave.generatorType == 4) // Tri32
            {
                KindnesDSP::TriangleChannel tri;
                tri.setSampleRate(kSampleRate);
                tri.setFrequency(static_cast<float>(freq));
                tri.setSteppedDacMode(true);
                tri.triggerNote();
                for (int i = 0; i < 4800; ++i) tri.processSample();
                for (int i = 0; i < kRenderSamples; ++i)
                {
                    // Map stepped DAC [0..15] to bipolar [-1.0, 1.0]
                    float raw = tri.processSample();
                    audioBuf[i] = ((raw - 7.5f) / 7.5f) * 0.85f;
                }
            }
            else if (wave.generatorType == 5 || wave.generatorType == 6) // VRC6 Saw
            {
                KindnesDSP::ExpansionVRC6 vrc;
                vrc.setSampleRate(kSampleRate);
                vrc.setEnabled(true);
                int vol = (wave.generatorType == 5) ? 12 : 15; // 12=Clean, 15=Overdrive wrap
                vrc.setSawtooth(static_cast<float>(freq), vol);
                for (int i = 0; i < 4800; ++i) vrc.processSample();
                for (int i = 0; i < kRenderSamples; ++i)
                    audioBuf[i] = vrc.processSample() * 0.85f;
            }
            else if (wave.generatorType == 7) // VRC6 Thin Pulse 1/16 (6.25%)
            {
                KindnesDSP::ExpansionVRC6 vrc;
                vrc.setSampleRate(kSampleRate);
                vrc.setEnabled(true);
                vrc.setPulse1(static_cast<float>(freq), 0, 15); // duty 0 = 1/16 (6.25%)
                for (int i = 0; i < 4800; ++i) vrc.processSample();
                for (int i = 0; i < kRenderSamples; ++i)
                    audioBuf[i] = vrc.processSample() * 0.85f;
            }
            else if (wave.generatorType == 8) // FDS FM Lead
            {
                KindnesDSP::FDSChannel fds;
                fds.setSampleRate(kSampleRate);
                fds.setEnabled(true);
                fds.setPresetWave(0); // Sine wavetable
                fds.setPresetMod(1);  // FM Bell / Octave
                fds.setVolume(15);
                fds.setFrequency(static_cast<float>(freq));
                for (int i = 0; i < 4800; ++i) fds.processSample();
                for (int i = 0; i < kRenderSamples; ++i)
                    audioBuf[i] = fds.processSample() * 0.85f;
            }

            LoopInfo loop = findZeroCrossingLoop(audioBuf, freq);

            std::string noteName = midiNoteName(midiNote);
            std::string filename = wave.id + "_" + sanitizeFilename(noteName) + ".wav";
            std::string filePath = "Kindnes_SFZ_Pack/samples/waveforms/" + filename;
            std::string relPath = "samples/waveforms/" + filename;

            writeWav24(filePath, audioBuf, midiNote, loop.start, loop.end);

            instrumentRegions[w].push_back({ relPath, lokey, hikey, midiNote, loop.start, loop.end });
        }
    }

    // --- Render Mode 0 and Mode 1 Noise ---
    std::cout << "Rendering 2A03 Mode 0 & Mode 1 Noise..." << std::endl;
    {
        // Mode 0 White Noise (2.0 seconds loop)
        KindnesDSP::NoiseChannel noise;
        noise.setSampleRate(kSampleRate);
        noise.setMode(0);
        noise.setPeriodIndex(0); // Maximum rate white noise
        noise.setVolume(15);
        noise.gate(true);

        std::vector<float> noiseBuf(192000); // 2.0s
        for (size_t i = 0; i < noiseBuf.size(); ++i)
        {
            float raw = noise.processSample();
            noiseBuf[i] = ((raw / 15.0f) * 2.0f - 1.0f) * 0.85f;
        }

        writeWav24("Kindnes_SFZ_Pack/samples/waveforms/noise_mode0_white.wav", noiseBuf, 60, 48000, 180000);
    }
    {
        // Mode 1 Metallic Noise across all 16 hardware periods
        KindnesDSP::NoiseChannel noise;
        noise.setSampleRate(kSampleRate);
        noise.setMode(1);
        noise.setVolume(15);
        noise.gate(true);

        for (int p = 0; p < 16; ++p)
        {
            noise.setPeriodIndex(p);
            std::vector<float> noiseBuf(96000); // 1.0s
            for (size_t i = 0; i < noiseBuf.size(); ++i)
            {
                float raw = noise.processSample();
                noiseBuf[i] = ((raw / 15.0f) * 2.0f - 1.0f) * 0.85f;
            }

            std::string filename = "noise_mode1_period_" + std::to_string(p) + ".wav";
            writeWav24("Kindnes_SFZ_Pack/samples/waveforms/" + filename, noiseBuf, 48 + p, 24000, 90000);
        }
    }

    // =========================================================================
    // 1. Write Standalone 1-Click SFZ Presets (No Keyswitches Needed!)
    // =========================================================================
    std::cout << "Writing individual standalone SFZ presets..." << std::endl;
    for (size_t w = 0; w < waveforms.size(); ++w)
    {
        const auto& wave = waveforms[w];
        std::ofstream sfz("Kindnes_SFZ_Pack/" + wave.presetFilename);
        sfz << "// ============================================================================\n";
        sfz << "// " << wave.displayName << " (96 kHz, 24-bit)\n";
        sfz << "// ============================================================================\n";
        sfz << "<global>\n";
        sfz << "ampeg_attack=0.001\n";
        sfz << "ampeg_decay=0.0\n";
        sfz << "ampeg_sustain=100\n";
        sfz << "ampeg_release=0.040\n\n";

        sfz << "<group>\n";
        sfz << "loop_mode=loop_continuous\n";
        if (wave.generatorType == 4)
            sfz << "amp_veltrack=0\n";
        else
            sfz << "amp_veltrack=100\n";

        for (const auto& r : instrumentRegions[w])
        {
            sfz << "<region> sample=" << r.relPath
                << " lokey=" << r.lokey << " hikey=" << r.hikey
                << " pitch_keycenter=" << r.center
                << " loop_start=" << r.loopStart << " loop_end=" << r.loopEnd << "\n";
        }
    }

    // Standalone 10: Mode 1 Metallic Noise
    {
        std::ofstream sfz("Kindnes_SFZ_Pack/10_2A03_Noise_Metallic_Mode1.sfz");
        sfz << "// ============================================================================\n";
        sfz << "// 2A03 Mode 1 Metallic Tonal Noise (16 Hardware Pitches)\n";
        sfz << "// ============================================================================\n";
        sfz << "<global>\n";
        sfz << "ampeg_attack=0.001\n";
        sfz << "ampeg_decay=0.0\n";
        sfz << "ampeg_sustain=100\n";
        sfz << "ampeg_release=0.040\n\n";
        sfz << "<group> loop_mode=loop_continuous amp_veltrack=100\n";
        for (int p = 0; p < 16; ++p)
        {
            int note = 48 + p; // C3 to D#4
            sfz << "<region> sample=samples/waveforms/noise_mode1_period_" << p << ".wav"
                << " key=" << note << " loop_start=24000 loop_end=90000\n";
        }
    }

    // Standalone 11: Mode 0 White Noise
    {
        std::ofstream sfz("Kindnes_SFZ_Pack/11_2A03_Noise_White_Mode0.sfz");
        sfz << "// ============================================================================\n";
        sfz << "// 2A03 Mode 0 15-Bit Galois LFSR White Noise\n";
        sfz << "// ============================================================================\n";
        sfz << "<global>\n";
        sfz << "ampeg_attack=0.001\n";
        sfz << "ampeg_decay=0.0\n";
        sfz << "ampeg_sustain=100\n";
        sfz << "ampeg_release=0.040\n\n";
        sfz << "<group> loop_mode=loop_continuous amp_veltrack=100\n";
        sfz << "<region> sample=samples/waveforms/noise_mode0_white.wav lokey=24 hikey=127 pitch_keycenter=60 loop_start=48000 loop_end=180000\n";
    }

    // =========================================================================
    // 2. Write Master Keyswitched Instrument: 8Bit_Basic_Waveforms.sfz
    // =========================================================================
    std::cout << "Writing Kindnes_SFZ_Pack/8Bit_Basic_Waveforms.sfz..." << std::endl;
    {
        std::ofstream sfz("Kindnes_SFZ_Pack/8Bit_Basic_Waveforms.sfz");
        sfz << "// ============================================================================\n";
        sfz << "// 8-Bit Classic 2A03 + VRC6 Basic Waveforms (96 kHz, 24-bit)\n";
        sfz << "// Master Keyswitch Selector: C-1 to A#-1 (MIDI 12..22)\n";
        sfz << "// ============================================================================\n";
        sfz << "<global>\n";
        sfz << "sw_default=14\n";
        sfz << "sw_lokey=12\n";
        sfz << "sw_hikey=22\n";
        sfz << "ampeg_attack=0.001\n";
        sfz << "ampeg_decay=0.0\n";
        sfz << "ampeg_sustain=100\n";
        sfz << "ampeg_release=0.040\n\n";

        for (size_t w = 0; w < waveforms.size(); ++w)
        {
            const auto& wave = waveforms[w];
            sfz << "// ----------------------------------------------------------------------------\n";
            sfz << "// Keyswitch " << wave.swKey << " (" << midiNoteName(wave.swKey) << "): " << wave.displayName << "\n";
            sfz << "// ----------------------------------------------------------------------------\n";
            sfz << "<group>\n";
            sfz << "sw_last=" << wave.swKey << "\n";
            sfz << "loop_mode=loop_continuous\n";
            if (wave.generatorType == 4)
                sfz << "amp_veltrack=0 // Authentic 2A03 Triangle has no volume steps\n";
            else
                sfz << "amp_veltrack=100\n";

            for (const auto& r : instrumentRegions[w])
            {
                sfz << "<region> sample=" << r.relPath
                    << " lokey=" << r.lokey << " hikey=" << r.hikey
                    << " pitch_keycenter=" << r.center
                    << " loop_start=" << r.loopStart << " loop_end=" << r.loopEnd << "\n";
            }
            sfz << "\n";
        }

        // Mode 1 Metallic Noise (swKey 17, F-1)
        sfz << "// ----------------------------------------------------------------------------\n";
        sfz << "// Keyswitch 17 (F-1): 2A03 Mode 1 (93-Step Metallic Tonal Noise)\n";
        sfz << "// ----------------------------------------------------------------------------\n";
        sfz << "<group> sw_last=17 loop_mode=loop_continuous amp_veltrack=100\n";
        for (int p = 0; p < 16; ++p)
        {
            int note = 48 + p; // C3 to D#4
            sfz << "<region> sample=samples/waveforms/noise_mode1_period_" << p << ".wav"
                << " key=" << note << " loop_start=24000 loop_end=90000\n";
        }
        sfz << "\n";

        // Mode 0 White Noise (swKey 18, F#-1)
        sfz << "// ----------------------------------------------------------------------------\n";
        sfz << "// Keyswitch 18 (F#-1): 2A03 Mode 0 (15-Bit Galois LFSR White Noise)\n";
        sfz << "// ----------------------------------------------------------------------------\n";
        sfz << "<group> sw_last=18 loop_mode=loop_continuous amp_veltrack=100\n";
        sfz << "<region> sample=samples/waveforms/noise_mode0_white.wav lokey=24 hikey=127 pitch_keycenter=60 loop_start=48000 loop_end=180000\n\n";
    }

    std::cout << "SUCCESS! All 96kHz waveform samples, individual instruments, and master keyswitch SFZ generated cleanly." << std::endl;
    return 0;
}
