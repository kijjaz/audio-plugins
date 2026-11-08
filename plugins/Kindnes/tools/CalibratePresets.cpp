#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "../Source/DSP/KindnesEngine.h"
#include "../Source/Audio/KindnesPresets.h"

int main()
{
    std::cout << "=========================================================\n";
    std::cout << "  KINDNES PRESET LOUDNESS & UNITY-TRIM CALIBRATION AUDIT  \n";
    std::cout << "=========================================================\n";

    KindnesDSP::KindnesEngine engine;
    const auto& presets = KindnesAudio::getFactoryPresets();

    constexpr double kSampleRate = 44100.0;
    constexpr int kNumSamples = 44100 * 2; // 2 seconds
    engine.setSampleRate(kSampleRate);

    std::vector<float> bufL(kNumSamples, 0.0f);
    std::vector<float> bufR(kNumSamples, 0.0f);

    std::cout << std::left << std::setw(48) << "Preset Name"
              << std::setw(12) << "Trim (dB)"
              << std::setw(12) << "Peak (dBFS)"
              << std::setw(12) << "RMS (dBFS)"
              << "Status\n";
    std::cout << std::string(88, '-') << "\n";

    bool allPassed = true;

    for (const auto& p : presets)
    {
        engine.reset();
        std::fill(bufL.begin(), bufL.end(), 0.0f);
        std::fill(bufR.begin(), bufR.end(), 0.0f);

        // Apply preset
        engine.setPlayMode(static_cast<KindnesDSP::KindnesEngine::PlayMode>(p.playMode));
        engine.setAuthenticDAC(p.authenticDAC);
        engine.setAnalogFilterEnabled(p.analogFilter);
        engine.setFilterTone(p.filterTone);

        engine.setPulse1Duty(p.pulse1Duty);
        engine.setPulse2Duty(p.pulse2Duty);
        engine.setPulseContinuousPWM(p.continuousPWM);
        engine.setPulse1Volume(p.pulse1Vol);
        engine.setPulse2Volume(p.pulse2Vol);
        engine.setTriangleVolume(p.triVol);
        engine.setNoiseVolume(p.noiseVol);
        engine.setNoiseMode(p.noiseMode);
        engine.setNoisePeriod(p.noisePeriod);

        engine.setArpEnabled(p.arpEnabled);
        engine.setArpRateMode(static_cast<KindnesDSP::ArpeggiatorMacro::RateMode>(p.arpRateMode));
        engine.setArpSequence(p.arpSteps);

        engine.setVRC6Enabled(p.vrc6Enabled);
        engine.setVRC6SawVolume(p.vrc6SawVol);

        engine.setFDSEnabled(p.fdsEnabled);
        engine.setFDSPresetWave(p.fdsWave);
        engine.setFDSVolume(p.fdsVol);
        engine.setFDSModulation(p.fdsModSpeed, p.fdsModDepth);
        engine.setFDSPresetMod(p.fdsModWave);

        engine.setScoopParameters(p.scoopSemi, p.scoopTime);
        engine.setDelayedVibrato(p.delayedVibDelay, 300.0f, p.delayedVibDepth, 6.0f);
        engine.setDutyMorph(p.dutyMorph, 0, p.pulse1Duty, 3);
        engine.setPseudoEcho(p.pseudoEcho, p.pseudoEchoDelay, p.pseudoEchoVol, 1);

        engine.setSvFilterParameters(p.filterMode, p.cutoffHz, p.resonance);
        engine.setAdsrParameters(p.useAdsr, p.attackMs, p.decayMs, p.sustainLevel, p.releaseMs);
        engine.setLfo1(p.lfoRateHz, p.vibratoCents, p.lfoPwmDepth);
        engine.setGlideTime(p.glideMs);
        engine.setDPCMParameters(p.dpcmSample, p.dpcmChromatic, p.dpcmDeltaStep, p.dpcmRateIndex, false, false, p.dpcmVol);
        engine.setDpcmDrumPadMode(p.dpcmDrumPads);
        engine.setDpcmVelToSlew(p.dpcmVelToSlew);
        engine.setThroughDPCMEnabled(p.throughDpcm);

        engine.setUnisonDetune(p.unisonDetune);
        engine.setStereoSpread(p.stereoSpread);

        // Play standard test C4 note
        engine.noteOn(60, 0.8f);

        // Process audio
        engine.processBlock(bufL.data(), bufR.data(), kNumSamples);

        // Measure Peak & RMS
        float maxPeak = 0.0f;
        double sumSq = 0.0;
        for (int i = 0; i < kNumSamples; ++i)
        {
            float val = 0.5f * (std::abs(bufL[i]) + std::abs(bufR[i]));
            if (val > maxPeak)
                maxPeak = val;
            sumSq += val * val;
        }

        float peakDb = (maxPeak > 0.00001f) ? 20.0f * std::log10(maxPeak) : -100.0f;
        float rmsDb = 20.0f * std::log10(std::sqrt(sumSq / kNumSamples) + 1e-9f);

        // Rule Check: trim MUST be 0.0 dB
        bool trimValid = (std::abs(p.masterTrimDb - 0.0f) < 0.001f);
        // Rule Check: Peak must be safe (< 0 dBFS) and RMS within musical range (-36 dBFS to -8 dBFS)
        bool levelValid = (peakDb <= -0.1f && rmsDb > -36.0f && rmsDb < -8.0f);

        bool pass = trimValid && levelValid;
        if (!pass)
            allPassed = false;

        std::cout << std::left << std::setw(48) << p.name
                  << std::setw(12) << std::fixed << std::setprecision(1) << p.masterTrimDb
                  << std::setw(12) << peakDb
                  << std::setw(12) << rmsDb
                  << (pass ? "[PASS]" : "[FAIL]") << "\n";
    }

    std::cout << std::string(88, '-') << "\n";
    if (allPassed)
    {
        std::cout << ">>> ALL PRESETS COMPLIANT: 0.0 dB Trim & Safe Internal Levels! <<<\n";
        return 0;
    }
    else
    {
        std::cerr << ">>> CALIBRATION FAILURE: Some presets violate guidelines! <<<\n";
        return 1;
    }
}
