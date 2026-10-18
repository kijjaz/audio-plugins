#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "../Source/DSP/SpekaEngine.h"

namespace Wav {

struct WavHeader {
    uint32_t sampleRate = 44100;
    uint16_t numChannels = 1;
    uint16_t bitsPerSample = 16;
    uint16_t audioFormat = 1; // 1 = PCM, 3 = IEEE float
};

bool readWav(const std::string& filepath, std::vector<std::vector<float>>& audioChannels, WavHeader& header) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input file: " << filepath << "\n";
        return false;
    }

    char riff[4];
    file.read(riff, 4);
    if (std::memcmp(riff, "RIFF", 4) != 0) {
        std::cerr << "Error: Not a valid RIFF file: " << filepath << "\n";
        return false;
    }

    uint32_t fileSize = 0;
    file.read(reinterpret_cast<char*>(&fileSize), 4);

    char wave[4];
    file.read(wave, 4);
    if (std::memcmp(wave, "WAVE", 4) != 0) {
        std::cerr << "Error: Not a valid WAVE file: " << filepath << "\n";
        return false;
    }

    bool foundFmt = false;
    bool foundData = false;
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

            uint32_t byteRate = 0;
            file.read(reinterpret_cast<char*>(&byteRate), 4);

            uint16_t blockAlign = 0;
            file.read(reinterpret_cast<char*>(&blockAlign), 2);

            file.read(reinterpret_cast<char*>(&header.bitsPerSample), 2);

            // Skip any extra format bytes (e.g. if chunkSize > 16)
            if (chunkSize > 16) {
                file.seekg(chunkSize - 16, std::ios::cur);
            }
            foundFmt = true;
        } else if (std::memcmp(chunkId, "data", 4) == 0) {
            dataBytes = chunkSize;
            dataPos = file.tellg();
            foundData = true;
            file.seekg(chunkSize, std::ios::cur);
        } else {
            // Skip unknown chunk
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (!foundFmt || !foundData) {
        std::cerr << "Error: Incomplete WAV file (missing fmt or data chunk)\n";
        return false;
    }

    file.seekg(dataPos);

    int bytesPerSample = header.bitsPerSample / 8;
    int numFrames = dataBytes / (header.numChannels * bytesPerSample);

    audioChannels.resize(header.numChannels);
    for (int ch = 0; ch < header.numChannels; ++ch) {
        audioChannels[ch].resize(numFrames, 0.0f);
    }

    std::vector<char> rawBuffer(dataBytes);
    file.read(rawBuffer.data(), dataBytes);

    const char* ptr = rawBuffer.data();
    for (int i = 0; i < numFrames; ++i) {
        for (int ch = 0; ch < header.numChannels; ++ch) {
            float sample = 0.0f;
            if (header.bitsPerSample == 16) {
                int16_t val;
                std::memcpy(&val, ptr, 2);
                ptr += 2;
                sample = static_cast<float>(val) / 32768.0f;
            } else if (header.bitsPerSample == 24) {
                int32_t val = 0;
                uint8_t b0 = static_cast<uint8_t>(ptr[0]);
                uint8_t b1 = static_cast<uint8_t>(ptr[1]);
                uint8_t b2 = static_cast<uint8_t>(ptr[2]);
                ptr += 3;
                val = (b2 << 24) | (b1 << 16) | (b0 << 8);
                sample = static_cast<float>(val) / 2147483648.0f;
            } else if (header.bitsPerSample == 32) {
                if (header.audioFormat == 3) { // IEEE Float
                    std::memcpy(&sample, ptr, 4);
                } else {
                    int32_t val;
                    std::memcpy(&val, ptr, 4);
                    sample = static_cast<float>(val) / 2147483648.0f;
                }
                ptr += 4;
            } else {
                std::cerr << "Error: Unsupported bit depth: " << header.bitsPerSample << "\n";
                return false;
            }
            audioChannels[ch][i] = sample;
        }
    }

    return true;
}

bool writeWav16(const std::string& filepath, const std::vector<std::vector<float>>& audioChannels, uint32_t sampleRate) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create output file: " << filepath << "\n";
        return false;
    }

    uint16_t numChannels = static_cast<uint16_t>(audioChannels.size());
    if (numChannels == 0) return false;
    uint32_t numFrames = static_cast<uint32_t>(audioChannels[0].size());

    uint16_t bitsPerSample = 16;
    uint16_t blockAlign = numChannels * (bitsPerSample / 8);
    uint32_t byteRate = sampleRate * blockAlign;
    uint32_t dataBytes = numFrames * blockAlign;
    uint32_t totalChunkSize = 36 + dataBytes;

    // RIFF header
    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&totalChunkSize), 4);
    file.write("WAVE", 4);

    // fmt chunk
    file.write("fmt ", 4);
    uint32_t fmtSize = 16;
    uint16_t audioFormat = 1; // PCM
    file.write(reinterpret_cast<const char*>(&fmtSize), 4);
    file.write(reinterpret_cast<const char*>(&audioFormat), 2);
    file.write(reinterpret_cast<const char*>(&numChannels), 2);
    file.write(reinterpret_cast<const char*>(&sampleRate), 4);
    file.write(reinterpret_cast<const char*>(&byteRate), 4);
    file.write(reinterpret_cast<const char*>(&blockAlign), 2);
    file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    // data chunk
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&dataBytes), 4);

    for (uint32_t i = 0; i < numFrames; ++i) {
        for (uint16_t ch = 0; ch < numChannels; ++ch) {
            float s = audioChannels[ch][i];
            s = std::clamp(s, -1.0f, 1.0f);
            int16_t pcm = static_cast<int16_t>(std::round(s * 32767.0f));
            file.write(reinterpret_cast<const char*>(&pcm), 2);
        }
    }

    return true;
}

} // namespace Wav

void printBanner() {
    std::cout << "=================================================================\n";
    std::cout << "  SPEKA CLI - Physical 1-Bit PC Speaker Simulation Engine       \n";
    std::cout << "  Access Software RealSound PWM / Sigma-Delta & Chassis Physics \n";
    std::cout << "  Version 0.1.0 (c) 2026 Kijjaz & Antigravity Audio             \n";
    std::cout << "=================================================================\n";
}

void printHelp() {
    printBanner();
    std::cout << "Usage: speka_cli [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input <file.wav>     Input audio WAV file (required)\n";
    std::cout << "  -o, --output <file.wav>    Output audio WAV file (default: output_speka.wav)\n";
    std::cout << "  -m, --mode <mode>          Modulation mode:\n";
    std::cout << "                             realsound  : RealSound PWM (default)\n";
    std::cout << "                             sigmadelta : 2nd-order Sigma-Delta PDM\n";
    std::cout << "                             deltamod   : 1-bit Delta Modulation\n";
    std::cout << "                             direct     : Direct zero-crossing beeper\n";
    std::cout << "  -c, --carrier <hz>         PWM carrier frequency in Hz (default: 18900)\n";
    std::cout << "                             (e.g., 18900: Mean Streets, 16500: Pinball)\n";
    std::cout << "  -s, --speaker <type>       Speaker transducer model:\n";
    std::cout << "                             paper : 2.25\" paper cone (IBM 5150 default)\n";
    std::cout << "                             piezo : 12mm ceramic disk buzzer\n";
    std::cout << "  -d, --drive <dB>           Input pre-drive in dB (default: 0.0)\n";
    std::cout << "  -r, --rattle <0.0-1.0>     Chassis screw & panel rattle amount (default: 0.35)\n";
    std::cout << "  -k, --case <0.0-1.0>       Steel case cavity resonance (default: 0.6)\n";
    std::cout << "  -j, --jitter <0.0-1.0>     CPU interrupt & DRAM refresh jitter (default: 0.15)\n";
    std::cout << "  -p, --sag <0.0-1.0>        Power supply rail sag / droop (default: 0.5)\n";
    std::cout << "  -u, --whine <0.0-1.0>      Carrier bleed / high whistle amount (default: 0.05, 0=clean)\n";
    std::cout << "  -g, --gate <0|1>           Auto-gate during silence (default: 1 [enabled])\n";
    std::cout << "  --gate-thresh <dB>         Auto-gate silence threshold in dBFS (default: -72.0 dBFS)\n";
    std::cout << "  -w, --mix <0.0-1.0>        Dry/Wet mix (default: 1.0)\n";
    std::cout << "  -t, --trim <dB>            Output trim gain in dB (default: 0.0)\n";
    std::cout << "  --mono                     Force stereo input to mono speaker\n";
    std::cout << "  -h, --help                 Show this help message\n\n";
    std::cout << "Example:\n";
    std::cout << "  speka_cli -i voice.wav -o pc_voice.wav -m realsound -c 18900 -s paper -d 4.0\n\n";
}

int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile = "output_speka.wav";
    std::string modeStr = "realsound";
    std::string speakerStr = "paper";
    float carrierFreq = 18900.0f;
    float driveDb = 0.0f;
    float rattleAmount = 0.35f;
    float caseAmount = 0.60f;
    float jitterAmount = 0.15f;
    float sagAmount = 0.50f;
    float carrierBleed = 0.05f;
    bool autoGate = true;
    float gateThreshDb = -72.0f;
    float mix = 1.0f;
    float trimDb = 0.0f;
    bool forceMono = false;

    if (argc < 2) {
        printHelp();
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputFile = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputFile = argv[++i];
        } else if ((arg == "-m" || arg == "--mode") && i + 1 < argc) {
            modeStr = argv[++i];
        } else if ((arg == "-c" || arg == "--carrier") && i + 1 < argc) {
            carrierFreq = std::stof(argv[++i]);
        } else if ((arg == "-s" || arg == "--speaker") && i + 1 < argc) {
            speakerStr = argv[++i];
        } else if ((arg == "-d" || arg == "--drive") && i + 1 < argc) {
            driveDb = std::stof(argv[++i]);
        } else if ((arg == "-r" || arg == "--rattle") && i + 1 < argc) {
            rattleAmount = std::stof(argv[++i]);
        } else if ((arg == "-k" || arg == "--case") && i + 1 < argc) {
            caseAmount = std::stof(argv[++i]);
        } else if ((arg == "-j" || arg == "--jitter") && i + 1 < argc) {
            jitterAmount = std::stof(argv[++i]);
        } else if ((arg == "-p" || arg == "--sag") && i + 1 < argc) {
            sagAmount = std::stof(argv[++i]);
        } else if ((arg == "-u" || arg == "--whine") && i + 1 < argc) {
            carrierBleed = std::stof(argv[++i]);
        } else if ((arg == "-g" || arg == "--gate") && i + 1 < argc) {
            autoGate = (std::stoi(argv[++i]) != 0);
        } else if (arg == "--gate-thresh" && i + 1 < argc) {
            gateThreshDb = std::stof(argv[++i]);
        } else if ((arg == "-w" || arg == "--mix") && i + 1 < argc) {
            mix = std::stof(argv[++i]);
        } else if ((arg == "-t" || arg == "--trim") && i + 1 < argc) {
            trimDb = std::stof(argv[++i]);
        } else if (arg == "--mono") {
            forceMono = true;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            return 1;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: Input file must be specified with -i <file.wav>\n";
        return 1;
    }

    printBanner();

    std::vector<std::vector<float>> channels;
    Wav::WavHeader header;
    if (!Wav::readWav(inputFile, channels, header)) {
        return 1;
    }

    std::cout << "Loaded input: " << inputFile << "\n";
    std::cout << "  Format: " << header.sampleRate << " Hz, "
              << header.numChannels << " channel(s), "
              << header.bitsPerSample << "-bit, "
              << channels[0].size() << " frames ("
              << std::fixed << std::setprecision(2)
              << (static_cast<double>(channels[0].size()) / header.sampleRate) << "s)\n\n";

    // Handle mono fold-down if requested or if already mono
    if (forceMono && channels.size() > 1) {
        std::cout << "Downmixing to authentic PC speaker mono...\n";
        std::vector<float> mono(channels[0].size(), 0.0f);
        for (size_t i = 0; i < mono.size(); ++i) {
            float sum = 0.0f;
            for (size_t ch = 0; ch < channels.size(); ++ch) {
                sum += channels[ch][i];
            }
            mono[i] = sum / static_cast<float>(channels.size());
        }
        channels.clear();
        channels.push_back(mono);
    }

    // Configure Speka engines for each channel
    std::vector<Speka::SpekaEngine> engines(channels.size());

    Speka::ModulationMode modMode = Speka::ModulationMode::RealSoundPWM;
    if (modeStr == "sigmadelta") modMode = Speka::ModulationMode::SigmaDeltaPDM;
    else if (modeStr == "deltamod") modMode = Speka::ModulationMode::DeltaModulation;
    else if (modeStr == "direct") modMode = Speka::ModulationMode::Direct1Bit;

    Speka::SpeakerType spkType = (speakerStr == "piezo") ?
        Speka::SpeakerType::PiezoBuzzer12mm : Speka::SpeakerType::DynamicPaper2Inch;

    std::cout << "Configuring Speka DSP Pipeline:\n";
    std::cout << "  Modulation Mode   : " << modeStr << "\n";
    if (modMode == Speka::ModulationMode::RealSoundPWM) {
        float ticks = std::floor(1193182.0f / carrierFreq);
        float bits = std::log2(ticks);
        std::cout << "  Carrier Frequency : " << carrierFreq << " Hz ("
                  << static_cast<int>(ticks) << " ticks/period, ~"
                  << std::setprecision(1) << bits << " bits effective resolution)\n";
    }
    std::cout << "  Speaker Type      : " << ((spkType == Speka::SpeakerType::PiezoBuzzer12mm) ? "12mm Ceramic Piezo Buzzer" : "2.25\" Paper Cone (IBM 5150)") << "\n";
    std::cout << "  Input Drive       : " << driveDb << " dB\n";
    std::cout << "  Chassis Resonance : " << caseAmount << "\n";
    std::cout << "  Chassis Rattle    : " << rattleAmount << "\n";
    std::cout << "  Jitter / Drift    : " << jitterAmount << "\n";
    std::cout << "  Carrier Whine     : " << carrierBleed << " (0=silent, 1=raw)\n";
    std::cout << "  Auto-Gate Squelch : " << (autoGate ? "Enabled" : "Disabled") << "\n";
    if (autoGate) {
        std::cout << "  Gate Threshold    : " << gateThreshDb << " dBFS\n";
    }
    std::cout << "  Wet/Dry Mix       : " << mix << "\n";
    std::cout << "  Output Trim       : " << trimDb << " dB\n\n";

    for (auto& eng : engines) {
        eng.prepare(header.sampleRate);
        eng.setModulationMode(modMode);
        eng.setCarrierFrequency(carrierFreq);
        eng.setSpeakerType(spkType);
        eng.setDriveDb(driveDb);
        eng.setChassisResonance(caseAmount);
        eng.setChassisRattle(rattleAmount);
        eng.setJitterAmount(jitterAmount);
        eng.setPowerSag(sagAmount);
        eng.setCarrierBleed(carrierBleed);
        eng.setAutoGate(autoGate);
        eng.setGateThresholdDb(gateThreshDb);
        eng.setWetDryMix(mix);
        eng.setOutputTrimDb(trimDb);
    }

    std::cout << "Processing audio frames...";
    std::cout.flush();

    auto startTime = std::chrono::high_resolution_clock::now();

    size_t numFrames = channels[0].size();
    for (size_t ch = 0; ch < channels.size(); ++ch) {
        for (size_t i = 0; i < numFrames; ++i) {
            channels[ch][i] = engines[ch].processSample(channels[ch][i]);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    std::cout << " Done! (" << std::fixed << std::setprecision(1) << elapsedMs << " ms)\n";

    if (!Wav::writeWav16(outputFile, channels, header.sampleRate)) {
        return 1;
    }

    std::cout << "Successfully written rendered audio to: " << outputFile << "\n";
    return 0;
}
