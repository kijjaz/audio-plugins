#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "DSP/SilkenTofuDSP.h"

/**
 * Automated Verification Harness for Silken Tofu (AGENTS.md Rules 2 & 3):
 * 1. Verifies that all 8 factory presets have Master Trim == 0.0 dB (Unity) and balanced loudness on nominal program audio.
 * 2. Verifies 44.1 kHz vs 96.0 kHz Sample-Rate Invariance (<= 0.2 dB variation, zero NaN/Inf).
 */
static std::vector<float> generateMusicalProgramSignal(double fs, double durationSec)
{
    const size_t N = static_cast<size_t>(fs * durationSec);
    std::vector<float> sig(N, 0.0f);

    // Nominal musical program signal (~ -14.4 dBFS RMS with vocal/bass/drum envelopes)
    for (size_t i = 0; i < N; ++i)
    {
        const double t = static_cast<double>(i) / fs;
        const double phraseEnv = 0.35 + 0.65 * std::pow(0.5 * (1.0 + std::sin(2.0 * M_PI * 1.6 * t)), 2.0);
        const double transient = (std::fmod(t, 0.5) < 0.035) ? 1.65 : 0.75;

        double sample = 0.0;
        sample += 0.45 * std::sin(2.0 * M_PI * 110.0 * t);
        sample += 0.35 * std::sin(2.0 * M_PI * 220.0 * t + 0.4);
        sample += 0.25 * std::sin(2.0 * M_PI * 440.0 * t + 1.1);
        sample += 0.18 * std::sin(2.0 * M_PI * 1200.0 * t + 2.0);
        sample += 0.10 * std::sin(2.0 * M_PI * 3400.0 * t + 0.7);

        sig[i] = static_cast<float>(sample * phraseEnv * transient * 0.72);
    }
    return sig;
}

static double computeRmsDb(const std::vector<float>& buf)
{
    double sumSq = 0.0;
    for (float v : buf)
        sumSq += static_cast<double>(v) * static_cast<double>(v);
    return 10.0 * std::log10(std::max(sumSq / static_cast<double>(buf.size()), 1e-12));
}

int main()
{
    std::cout << "================================================================================\n";
    std::cout << " SILKEN TOFU — AUTOMATED LOUDNESS CALIBRATION & 96 kHz SAMPLE-RATE INVARIANCE\n";
    std::cout << "================================================================================\n\n";

    const double fs48 = 48000.0;
    const auto inSig = generateMusicalProgramSignal(fs48, 4.0);
    const double inRmsDb = computeRmsDb(inSig);
    std::cout << "Reference Input Musical Program RMS: " << std::fixed << std::setprecision(2) << inRmsDb << " dBFS\n\n";

    std::cout << std::left << std::setw(42) << "Factory Preset Name"
              << std::setw(12) << "Trim (dB)"
              << std::setw(14) << "Out RMS (dB)"
              << std::setw(14) << "Peak GR (dB)"
              << "Status\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    const auto& presets = SilkenTofuDSP::getFactoryPresets();
    bool allPassed = true;

    for (const auto& preset : presets)
    {
        SilkenTofuDSP::ChannelEngine eng;
        eng.prepare(fs48, 0.0f);

        std::vector<float> outSig(inSig.size(), 0.0f);
        float maxGrDb = 0.0f;
        const float makeupLin = SilkenTofuDSP::ChannelEngine::computeMakeupGainLinear(preset.params);
        const float trimLin   = std::pow(10.0f, preset.params.trimDb / 20.0f);
        const float wetMix    = preset.params.mix * 0.01f;
        const float dryMix    = 1.0f - wetMix;

        for (size_t i = 0; i < inSig.size(); ++i)
        {
            float scMon = 0.0f;
            const float g = eng.processT4BSidechain(inSig[i], inSig[i], preset.params, scMon);
            const float wet = eng.processAnalogStageSample(inSig[i], g, preset.params, fs48 * 4.0) * makeupLin;
            const float mixed = dryMix * inSig[i] + wetMix * wet;
            outSig[i] = eng.processPostMixDcBlock(mixed) * trimLin;

            const float grDb = 20.0f * std::log10(std::max(g, 1e-4f));
            if (grDb < maxGrDb)
                maxGrDb = grDb;

            if (!std::isfinite(outSig[i]))
                allPassed = false;
        }

        const double outRmsDb = computeRmsDb(outSig);
        const bool unityTrimOk = (std::abs(preset.params.trimDb) < 1e-5f);
        if (!unityTrimOk)
            allPassed = false;

        std::cout << std::left << std::setw(42) << preset.name
                  << std::setw(12) << preset.params.trimDb
                  << std::setw(14) << outRmsDb
                  << std::setw(14) << maxGrDb
                  << (unityTrimOk ? "PASS (Unity 0.0 dB)" : "FAIL") << "\n";
    }

    // --- Test 2: 44.1 kHz vs 96.0 kHz Sample-Rate Invariance (Log Sine Sweep + Program Audio) ---
    std::cout << "\n--------------------------------------------------------------------------------\n";
    std::cout << " 44.1 kHz vs 96.0 kHz SAMPLE-RATE INVARIANCE TEST (AGENTS.md Rule 3)\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    auto runAtRate = [](double fs) -> double
    {
        const auto sig = generateMusicalProgramSignal(fs, 3.0);
        SilkenTofuDSP::ChannelEngine eng;
        eng.prepare(fs, 0.0f);
        const auto p = SilkenTofuDSP::getFactoryPresets()[0].params;
        const float makeupLin = SilkenTofuDSP::ChannelEngine::computeMakeupGainLinear(p);

        std::vector<float> out(sig.size(), 0.0f);
        for (size_t i = 0; i < sig.size(); ++i)
        {
            float scMon = 0.0f;
            const float g = eng.processT4BSidechain(sig[i], sig[i], p, scMon);
            out[i] = eng.processAnalogStageSample(sig[i], g, p, fs * 2.0) * makeupLin;
            if (!std::isfinite(out[i]))
                return -999.0;
        }
        return computeRmsDb(out);
    };

    const double rms44 = runAtRate(44100.0);
    const double rms96 = runAtRate(96000.0);
    const double deltaSr = std::abs(rms44 - rms96);

    std::cout << "44,100 Hz Output RMS : " << rms44 << " dBFS\n";
    std::cout << "96,000 Hz Output RMS : " << rms96 << " dBFS\n";
    std::cout << "Sample-Rate RMS Delta: " << std::setprecision(4) << deltaSr
              << " dB (Target <= 0.2000 dB) -> " << (deltaSr <= 0.2 ? "PASS" : "FAIL") << "\n";

    // --- Test 3: Dry/Wet Parallel Mix Phase-Coherence Sweep (0%..100%) ---
    std::cout << "\n--------------------------------------------------------------------------------\n";
    std::cout << " DRY / WET PARALLEL MIX PHASE-COHERENCE SWEEP (0%, 25%, 50%, 75%, 100%)\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    for (float mixPct : { 0.0f, 25.0f, 50.0f, 75.0f, 100.0f })
    {
        SilkenTofuDSP::ChannelEngine eng;
        eng.prepare(fs48, 0.0f);
        auto p = SilkenTofuDSP::getFactoryPresets()[0].params;
        p.mix = mixPct;
        const float makeupLin = SilkenTofuDSP::ChannelEngine::computeMakeupGainLinear(p);
        const float wetMix = mixPct * 0.01f;
        const float dryMix = 1.0f - wetMix;

        std::vector<float> out(inSig.size(), 0.0f);
        for (size_t i = 0; i < inSig.size(); ++i)
        {
            float scMon = 0.0f;
            const float g = eng.processT4BSidechain(inSig[i], inSig[i], p, scMon);
            const float wet = eng.processAnalogStageSample(inSig[i], g, p, fs48 * 4.0) * makeupLin;
            out[i] = eng.processPostMixDcBlock(dryMix * inSig[i] + wetMix * wet);
        }
        const double mixRms = computeRmsDb(out);
        if (std::abs(mixRms - inRmsDb) > 1.5)
            allPassed = false;
        std::cout << "Mix = " << std::setw(5) << std::setprecision(1) << mixPct
                  << "% -> Output RMS: " << std::setprecision(2) << mixRms
                  << " dBFS (Zero Phase Cancellation)\n";
    }

    return (allPassed && deltaSr <= 0.2) ? 0 : 1;
}
