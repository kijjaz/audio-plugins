#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <algorithm>

#include "../Source/DSP/SpekaEngine.h"

// Generate a logarithmic sine sweep
std::vector<float> generateSineSweep(double sampleRate, double durationSec, double startFreq, double endFreq, float amplitude) {
    size_t numSamples = static_cast<size_t>(sampleRate * durationSec);
    std::vector<float> sweep(numSamples);
    double f0 = startFreq;
    double f1 = endFreq;
    double T = durationSec;

    for (size_t i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        double phase = 0.0;
        if (std::abs(f1 - f0) < 1e-4) {
            phase = 2.0 * M_PI * f0 * t;
        } else {
            phase = 2.0 * M_PI * f0 * (std::pow(f1 / f0, t / T) - 1.0) / std::log(f1 / f0);
        }
        sweep[i] = static_cast<float>(amplitude * std::sin(phase));
    }
    return sweep;
}

double measureRmsDb(const std::vector<float>& audio) {
    double sumSq = 0.0;
    size_t count = 0;
    for (float s : audio) {
        if (!std::isnan(s) && !std::isinf(s)) {
            sumSq += s * s;
            count++;
        }
    }
    if (count == 0) return -100.0;
    double rms = std::sqrt(sumSq / count);
    return 20.0 * std::log10(std::max(rms, 1e-5));
}

float measurePeak(const std::vector<float>& audio) {
    float peak = 0.0f;
    for (float s : audio) {
        if (!std::isnan(s) && !std::isinf(s)) {
            peak = std::max(peak, std::abs(s));
        }
    }
    return peak;
}

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
    float trimDb;
};

int main() {
    std::cout << "=========================================================================\n";
    std::cout << " Speka DSP Diagnostic: 96 kHz Sine Sweep & Sample-Rate Invariance Test\n";
    std::cout << "=========================================================================\n\n";

    std::vector<PresetDef> presets = {
        { "IBM 5150 RealSound (Default)", Speka::ModulationMode::RealSoundPWM, 18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 0.0f, 0.50f, 0.25f, 0.15f, 0.40f, 0.05f, true, -72.0f, 0.0f },
        { "1989 Xenon 2 / Speedball",      Speka::ModulationMode::RealSoundPWM, 11025.0f, Speka::SpeakerType::DynamicPaper2Inch, 2.0f, 0.65f, 0.40f, 0.20f, 0.50f, 0.08f, true, -72.0f, 0.0f },
        { "Commander Keen PC Beeper",     Speka::ModulationMode::Direct1Bit,   18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 4.0f, 0.35f, 0.15f, 0.05f, 0.20f, 0.02f, true, -72.0f, 0.0f },
        { "Budget Clone Piezo Screamer",  Speka::ModulationMode::SigmaDeltaPDM, 22050.0f, Speka::SpeakerType::PiezoBuzzer12mm,  -3.0f, 0.20f, 0.10f, 0.10f, 0.30f, 0.04f, true, -72.0f, 0.0f },
        { "Sierra On-Line 1-Bit Voice",   Speka::ModulationMode::DeltaModulation, 14000.0f, Speka::SpeakerType::DynamicPaper2Inch, 3.0f, 0.70f, 0.35f, 0.25f, 0.60f, 0.05f, true, -72.0f, 0.0f },
        { "Overdriven XT Motherboard",    Speka::ModulationMode::RealSoundPWM, 16000.0f, Speka::SpeakerType::DynamicPaper2Inch, 10.0f, 0.80f, 0.70f, 0.35f, 0.75f, 0.12f, true, -72.0f, 0.0f },
        { "Ultra-Clean 1-Bit Hi-Fi",      Speka::ModulationMode::SigmaDeltaPDM, 24000.0f, Speka::SpeakerType::DynamicPaper2Inch, -1.0f, 0.20f, 0.00f, 0.00f, 0.05f, 0.01f, true, -72.0f, 0.0f },
        { "Arcade Steel Chassis Bezel",   Speka::ModulationMode::RealSoundPWM, 18900.0f, Speka::SpeakerType::DynamicPaper2Inch, 1.5f, 0.95f, 0.50f, 0.10f, 0.35f, 0.10f, true, -72.0f, 0.0f }
    };

    // 1. Generate 96 kHz Logarithmic Sine Sweep (20 Hz - 20 kHz, -14 dBFS amplitude)
    double sampleRate96k = 96000.0;
    float testAmp = 0.20f; // -14 dBFS peak amplitude
    double duration = 3.0; // 3 seconds sweep
    auto sweep96k = generateSineSweep(sampleRate96k, duration, 20.0, 20000.0, testAmp);
    double inputRms96k = measureRmsDb(sweep96k);

    std::cout << "Sine Sweep Input (96 kHz, 20 Hz - 20 kHz): " << inputRms96k << " dBFS, Peak: " << measurePeak(sweep96k) << "\n\n";

    std::cout << "--- TEST 1: Sine Sweep Frequency Response & Stability at 96 kHz ---\n";
    std::cout << std::left << std::setw(32) << "Preset Name"
              << std::setw(16) << "Sweep RMS (96k)"
              << std::setw(14) << "Peak Output"
              << std::setw(16) << "Numerical Status\n";
    std::cout << std::string(78, '-') << "\n";

    for (const auto& p : presets) {
        Speka::SpekaEngine engine;
        engine.prepare(sampleRate96k);
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
        engine.setOutputTrimDb(0.0f);

        std::vector<float> out96k(sweep96k.size());
        bool hasNaN = false;
        for (size_t i = 0; i < sweep96k.size(); ++i) {
            float s = engine.processSample(sweep96k[i]);
            if (std::isnan(s) || std::isinf(s)) hasNaN = true;
            out96k[i] = s;
        }

        double rms = measureRmsDb(out96k);
        float peak = measurePeak(out96k);

        std::cout << std::left << std::setw(32) << p.name
                  << std::setw(16) << (std::to_string(rms).substr(0, 6) + " dBFS")
                  << std::setw(14) << (std::to_string(peak).substr(0, 5))
                  << std::setw(16) << (hasNaN ? "FAILED (NaN)" : "STABLE") << "\n";
    }

    std::cout << "\n--- TEST 2: Sample-Rate Parity Comparison (44.1 kHz vs 96 kHz) ---\n";
    std::cout << std::left << std::setw(32) << "Preset Name"
              << std::setw(16) << "RMS @ 44.1 kHz"
              << std::setw(16) << "RMS @ 96.0 kHz"
              << std::setw(14) << "Delta (dB)\n";
    std::cout << std::string(78, '-') << "\n";

    // Compare with identical 1 kHz tone at -14 dBFS (where both paper and piezo have full response)
    double durationTone = 1.0;
    auto tone44k = generateSineSweep(44100.0, durationTone, 1000.0, 1000.0, testAmp);
    auto tone96k = generateSineSweep(96000.0, durationTone, 1000.0, 1000.0, testAmp);

    for (const auto& p : presets) {
        Speka::SpekaEngine engine44k, engine96k;
        engine44k.prepare(44100.0);
        engine96k.prepare(96000.0);

        auto configureEngine = [&](Speka::SpekaEngine& eng) {
            eng.setModulationMode(p.mode);
            eng.setCarrierFrequency(p.carrierFreq);
            eng.setSpeakerType(p.speaker);
            eng.setDriveDb(p.driveDb);
            eng.setChassisResonance(p.caseRes);
            eng.setChassisRattle(p.rattle);
            eng.setJitterAmount(p.jitter);
            eng.setPowerSag(p.sag);
            eng.setCarrierBleed(p.whine);
            eng.setAutoGate(p.autoGate);
            eng.setGateThresholdDb(p.gateThreshDb);
            eng.setOutputTrimDb(0.0f);
        };

        configureEngine(engine44k);
        configureEngine(engine96k);

        std::vector<float> out44(tone44k.size());
        for (size_t i = 0; i < tone44k.size(); ++i) out44[i] = engine44k.processSample(tone44k[i]);

        std::vector<float> out96(tone96k.size());
        for (size_t i = 0; i < tone96k.size(); ++i) out96[i] = engine96k.processSample(tone96k[i]);

        double rms44 = measureRmsDb(out44);
        double rms96 = measureRmsDb(out96);
        double delta = rms96 - rms44;

        std::cout << std::left << std::setw(32) << p.name
                  << std::setw(16) << (std::to_string(rms44).substr(0, 6) + " dBFS")
                  << std::setw(16) << (std::to_string(rms96).substr(0, 6) + " dBFS")
                  << std::setw(14) << (delta >= 0 ? "+" + std::to_string(delta).substr(0, 4) : std::to_string(delta).substr(0, 5)) + " dB" << "\n";
    }

    std::cout << "\n=========================================================================\n";
    return 0;
}
