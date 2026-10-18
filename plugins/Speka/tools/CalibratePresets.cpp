#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cstring>

#include "../Source/DSP/SpekaEngine.h"

namespace Wav {
struct WavHeader {
    uint32_t sampleRate = 44100;
    uint16_t numChannels = 1;
    uint16_t bitsPerSample = 16;
    uint16_t audioFormat = 1;
};

bool readWav(const std::string& filepath, std::vector<float>& monoAudio, uint32_t& sampleRate) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) return false;

    char riff[4];
    file.read(riff, 4);
    if (std::memcmp(riff, "RIFF", 4) != 0) return false;

    uint32_t fileSize = 0;
    file.read(reinterpret_cast<char*>(&fileSize), 4);

    char wave[4];
    file.read(wave, 4);
    if (std::memcmp(wave, "WAVE", 4) != 0) return false;

    WavHeader header;
    bool foundFmt = false, foundData = false;
    uint32_t dataBytes = 0;
    std::streampos dataPos = 0;

    while (file.good() && (!foundFmt || !foundData)) {
        char chunkId[4];
        uint32_t chunkSize = 0;
        file.read(chunkId, 4);
        file.read(reinterpret_cast<char*>(&chunkSize), 4);
        if (!file) break;

        if (std::memcmp(chunkId, "fmt ", 4) == 0) {
            file.read(reinterpret_cast<char*>(&header.audioFormat), 2);
            file.read(reinterpret_cast<char*>(&header.numChannels), 2);
            file.read(reinterpret_cast<char*>(&header.sampleRate), 4);
            uint32_t byteRate = 0; file.read(reinterpret_cast<char*>(&byteRate), 4);
            uint16_t blockAlign = 0; file.read(reinterpret_cast<char*>(&blockAlign), 2);
            file.read(reinterpret_cast<char*>(&header.bitsPerSample), 2);
            if (chunkSize > 16) file.seekg(chunkSize - 16, std::ios::cur);
            foundFmt = true;
        } else if (std::memcmp(chunkId, "data", 4) == 0) {
            dataBytes = chunkSize;
            dataPos = file.tellg();
            foundData = true;
            file.seekg(chunkSize, std::ios::cur);
        } else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (!foundFmt || !foundData) return false;
    file.seekg(dataPos);

    sampleRate = header.sampleRate;
    int bytesPerSample = header.bitsPerSample / 8;
    int numFrames = dataBytes / (header.numChannels * bytesPerSample);

    monoAudio.resize(numFrames, 0.0f);
    std::vector<int16_t> frameBuf(header.numChannels);

    for (int i = 0; i < numFrames; ++i) {
        file.read(reinterpret_cast<char*>(frameBuf.data()), header.numChannels * 2);
        float sum = 0.0f;
        for (int ch = 0; ch < header.numChannels; ++ch) {
            sum += frameBuf[ch] / 32768.0f;
        }
        monoAudio[i] = sum / (float)header.numChannels;
    }
    return true;
}
} // namespace Wav

struct PresetDef {
    std::string name;
    Speka::ModulationMode mode;
    float carrierFreq;
    Speka::SpeakerType speaker;
    float driveDb;
    float caseRes;
    float rattle;
    float jitter;
    float sag;
    float whine;
    bool autoGate;
    float gateThreshDb;
    float trimDb; // will be calibrated
};

double measureRmsDb(const std::vector<float>& audio) {
    double sumSq = 0.0;
    size_t count = 0;
    for (float s : audio) {
        if (std::abs(s) > 1e-4f) { // only active audio
            sumSq += s * s;
            count++;
        }
    }
    if (count == 0) return -100.0;
    double rms = std::sqrt(sumSq / count);
    return 20.0 * std::log10(std::max(rms, 1e-5));
}

int main() {
    std::string testFile = "demo_music/xenon2_megablast.wav";
    std::vector<float> inputAudio;
    uint32_t sampleRate = 44100;

    if (!Wav::readWav(testFile, inputAudio, sampleRate)) {
        std::cerr << "Failed to read " << testFile << "\n";
        return 1;
    }

    double refRmsDb = measureRmsDb(inputAudio);
    std::cout << "Reference Audio RMS: " << std::fixed << std::setprecision(2) << refRmsDb << " dBFS\n\n";

    std::vector<PresetDef> presets = {
        { "Default (IBM 5150 RealSound)", Speka::ModulationMode::RealSoundPWM, 18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 0.0f, 0.50f, 0.25f, 0.15f, 0.40f, 0.05f, true, -72.0f, 0.0f },
        { "1989 Xenon 2 / Speedball",      Speka::ModulationMode::RealSoundPWM, 11025.0f, Speka::SpeakerType::DynamicPaper2Inch, 2.0f, 0.65f, 0.40f, 0.20f, 0.50f, 0.08f, true, -72.0f, 0.0f },
        { "Commander Keen PC Beeper",     Speka::ModulationMode::Direct1Bit,   18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 4.0f, 0.35f, 0.15f, 0.05f, 0.20f, 0.02f, true, -72.0f, 0.0f },
        { "Budget Clone Piezo Screamer",  Speka::ModulationMode::SigmaDeltaPDM, 22050.0f, Speka::SpeakerType::PiezoBuzzer12mm,  -3.0f, 0.20f, 0.10f, 0.10f, 0.30f, 0.04f, true, -72.0f, 0.0f },
        { "Sierra On-Line 1-Bit Voice",   Speka::ModulationMode::DeltaModulation, 14000.0f, Speka::SpeakerType::DynamicPaper2Inch, 3.0f, 0.70f, 0.35f, 0.25f, 0.60f, 0.05f, true, -72.0f, 0.0f },
        { "Overdriven XT Motherboard",    Speka::ModulationMode::RealSoundPWM, 16000.0f, Speka::SpeakerType::DynamicPaper2Inch, 10.0f, 0.80f, 0.70f, 0.35f, 0.75f, 0.12f, true, -72.0f, 0.0f },
        { "Ultra-Clean 1-Bit Hi-Fi",      Speka::ModulationMode::SigmaDeltaPDM, 24000.0f, Speka::SpeakerType::DynamicPaper2Inch, -1.0f, 0.20f, 0.00f, 0.00f, 0.05f, 0.01f, true, -72.0f, 0.0f },
        { "Arcade Steel Chassis Bezel",   Speka::ModulationMode::RealSoundPWM, 18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 1.5f, 0.95f, 0.50f, 0.10f, 0.35f, 0.10f, true, -72.0f, 0.0f }
    };

    // Target RMS: match reference audio RMS (~ -16 dBFS)
    double targetRmsDb = refRmsDb;

    std::cout << std::left << std::setw(32) << "Preset Name"
              << std::setw(14) << "Trim (dB)"
              << std::setw(14) << "Output RMS"
              << std::setw(14) << "Target RMS"
              << std::setw(14) << "Delta (dB)\n";
    std::cout << std::string(88, '-') << "\n";

    for (auto& p : presets) {
        Speka::SpekaEngine engine;
        engine.prepare((double)sampleRate);
        engine.setModulationMode(p.mode);
        engine.setCarrierFrequency(p.carrierFreq);
        engine.setSpeakerType(p.speaker);
        engine.setDriveDb(p.driveDb);
        engine.setChassisResonance(p.caseRes);
        engine.setChassisRattle(p.rattle);
        engine.setJitterAmount(p.jitter);
        engine.setPowerSag(p.sag);
        engine.setCarrierBleed(p.whine);
        engine.setAutoGate(p.autoGate);
        engine.setGateThresholdDb(p.gateThreshDb);
        engine.setOutputTrimDb(0.0f); // 0.0 dB unity trim across all presets

        std::vector<float> output(inputAudio.size());
        for (size_t i = 0; i < inputAudio.size(); ++i) {
            output[i] = engine.processSample(inputAudio[i]);
        }

        double finalRmsDb = measureRmsDb(output);
        double delta = finalRmsDb - targetRmsDb;
        p.trimDb = 0.0f;

        std::cout << std::left << std::setw(32) << p.name
                  << std::setw(14) << "0.0 dB"
                  << std::setw(14) << finalRmsDb
                  << std::setw(14) << targetRmsDb
                  << std::setw(14) << (delta >= 0 ? "+" + std::to_string(delta).substr(0, 4) : std::to_string(delta).substr(0, 5)) << " dB\n";
    }

    std::cout << "\nInternal Component Calibrated Presets C++ Array (All Trim = 0.0 dB):\n\n";
    for (size_t i = 0; i < presets.size(); ++i) {
        const auto& p = presets[i];
        std::cout << "        { \"" << p.name << "\", "
                  << (int)p.mode << ", "
                  << std::fixed << std::setprecision(1) << p.carrierFreq << "f, "
                  << (int)p.speaker << ", "
                  << std::setw(5) << p.driveDb << "f, "
                  << std::setw(4) << p.caseRes << "f, "
                  << std::setw(4) << p.rattle << "f, "
                  << std::setw(4) << p.jitter << "f, "
                  << std::setw(4) << p.sag << "f, "
                  << std::setw(4) << p.whine << "f, "
                  << (p.autoGate ? "true" : "false") << ", "
                  << p.gateThreshDb << "f, "
                  << "0.0f },\n";
    }

    return 0;
}
