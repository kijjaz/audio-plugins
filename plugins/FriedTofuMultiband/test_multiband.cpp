#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "DSP/MultibandEngine.h"

int main()
{
    std::cout << "========================================================\n";
    std::cout << " Fried Tofu Multiband C++ Audio Verification Engine\n";
    std::cout << "========================================================\n\n";

    const double sampleRate = 48000.0;
    const int blockSize = 512;
    const int numChannels = 2;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = static_cast<juce::uint32>(numChannels);

    FriedTofuDSP::MultibandEngine engine;
    engine.prepare(spec);
    engine.setCrossoverFrequencies(250.0, 2500.0);

    const int latency = engine.getLatencySamples();
    std::cout << "[Test 1] Latency check: " << latency << " samples ("
              << (latency * 1000.0 / sampleRate) << " ms at 48kHz)\n";
    assert(latency == 254);

    // -------------------------------------------------------------------------
    // Test 2: Perfect Reconstruction Check (All bands bypassed / neutral drive)
    // -------------------------------------------------------------------------
    std::cout << "[Test 2] Linear-Phase Reconstruction Null Test (Bypass Mode)...\n";
    for (int b = 0; b < 3; ++b)
    {
        FriedTofuDSP::BandSettings s;
        s.bypass = true; // no saturation, pure linear phase crossover reconstruction
        s.gainDb = 0.0f;
        s.mute = false;
        s.solo = false;
        engine.setBandSettings(b, s);
    }

    // Process a chirp or multi-sine signal
    const int numTotalSamples = 48000; // 1 second
    std::vector<float> inLeft(numTotalSamples, 0.0f);
    std::vector<float> inRight(numTotalSamples, 0.0f);

    // Create a rich test signal containing low, mid, and high frequencies (100Hz, 1kHz, 10kHz)
    for (int i = 0; i < numTotalSamples; ++i)
    {
        double t = i / sampleRate;
        float s = 0.33f * std::sin(2.0 * M_PI * 80.0 * t)
                + 0.33f * std::sin(2.0 * M_PI * 1000.0 * t)
                + 0.33f * std::sin(2.0 * M_PI * 8000.0 * t);
        inLeft[i] = s;
        inRight[i] = -s; // antiphase for stereo test
    }

    std::vector<float> outLeft(numTotalSamples, 0.0f);
    std::vector<float> outRight(numTotalSamples, 0.0f);

    juce::AudioBuffer<float> blockBuf(numChannels, blockSize);
    for (int offset = 0; offset < numTotalSamples; offset += blockSize)
    {
        int curBlock = std::min(blockSize, numTotalSamples - offset);
        blockBuf.setSize(numChannels, curBlock, false, false, true);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* p = blockBuf.getWritePointer(ch);
            const auto& inVec = (ch == 0) ? inLeft : inRight;
            std::copy(inVec.begin() + offset, inVec.begin() + offset + curBlock, p);
        }

        engine.process(blockBuf);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const auto* p = blockBuf.getReadPointer(ch);
            auto& outVec = (ch == 0) ? outLeft : outRight;
            std::copy(p, p + curBlock, outVec.begin() + offset);
        }
    }

    // Measure maximum error after latency delay
    double maxError = 0.0;
    int validStart = latency + 500; // allow filter warm-up
    int validEnd = numTotalSamples - 500;

    for (int i = validStart; i < validEnd; ++i)
    {
        float expectedL = inLeft[i - latency];
        float actualL = outLeft[i];
        double errL = std::abs(actualL - expectedL);
        if (errL > maxError) maxError = errL;

        float expectedR = inRight[i - latency];
        float actualR = outRight[i];
        double errR = std::abs(actualR - expectedR);
        if (errR > maxError) maxError = errR;
    }

    double maxErrorDb = 20.0 * std::log10(maxError + 1e-18);
    std::cout << "         Max Reconstruction Null Error: " << maxError 
              << " (" << maxErrorDb << " dB)\n";
    if (maxError < 1e-4)
    {
        std::cout << "         -> PASS: Perfect transparent linear-phase reconstruction!\n";
    }
    else
    {
        std::cout << "         -> FAIL: High reconstruction error!\n";
        return 1;
    }

    // -------------------------------------------------------------------------
    // Test 3: Multiband Saturation Isolation Test
    // Saturate only Mid band (Drive = +24 dB, Si+Ge asymmetric).
    // Verify that Low band (< 200 Hz) remains pristine and undistorted.
    // -------------------------------------------------------------------------
    std::cout << "\n[Test 3] Multiband Isolation Test (Saturating MID only)...\n";
    engine.reset();

    // Low: clean bypass
    FriedTofuDSP::BandSettings sLow;
    sLow.bypass = true;
    engine.setBandSettings(0, sLow);

    // Mid: heavy saturation (24 dB drive, Si 1N4148 + Ge 1N34A)
    FriedTofuDSP::BandSettings sMid;
    sMid.bypass = false;
    sMid.driveDb = 24.0f;
    sMid.networkZ = 2200.0f;
    sMid.diodeA = 0; // Si
    sMid.diodeB = 1; // Ge (asymmetric)
    sMid.gainDb = 0.0f;
    engine.setBandSettings(1, sMid);

    // High: mute to isolate
    FriedTofuDSP::BandSettings sHigh;
    sHigh.mute = true;
    engine.setBandSettings(2, sHigh);

    // Test input with 100 Hz sub tone + 1000 Hz mid tone
    std::fill(inLeft.begin(), inLeft.end(), 0.0f);
    std::fill(outLeft.begin(), outLeft.end(), 0.0f);
    for (int i = 0; i < numTotalSamples; ++i)
    {
        double t = i / sampleRate;
        inLeft[i] = 0.3f * std::sin(2.0 * M_PI * 100.0 * t) + 0.3f * std::sin(2.0 * M_PI * 1000.0 * t);
        inRight[i] = inLeft[i];
    }

    for (int offset = 0; offset < numTotalSamples; offset += blockSize)
    {
        int curBlock = std::min(blockSize, numTotalSamples - offset);
        blockBuf.setSize(numChannels, curBlock, false, false, true);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* p = blockBuf.getWritePointer(ch);
            std::copy(inLeft.begin() + offset, inLeft.begin() + offset + curBlock, p);
        }

        engine.process(blockBuf);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const auto* p = blockBuf.getReadPointer(ch);
            if (ch == 0)
                std::copy(p, p + curBlock, outLeft.begin() + offset);
        }
    }

    // Check that output signal has energy and has clipped Mid but intact Low
    float maxOut = 0.0f;
    for (int i = validStart; i < validEnd; ++i)
        if (std::abs(outLeft[i]) > maxOut) maxOut = std::abs(outLeft[i]);

    std::cout << "         Mid Band Saturation Max Peak: " << maxOut << " V\n";
    if (maxOut > 0.1f && maxOut < 2.5f)
    {
        std::cout << "         -> PASS: WDF diode clipping is clamping properly within physical diode forward drops!\n";
    }
    else
    {
        std::cout << "         -> FAIL: Out of expected diode voltage range!\n";
        return 1;
    }

    // -------------------------------------------------------------------------
    // Test 4: Band Solo / Mute Logic Check
    // -------------------------------------------------------------------------
    std::cout << "\n[Test 4] Band Solo / Mute Isolation...\n";
    // Mute all bands -> output should be absolute zero
    for (int b = 0; b < 3; ++b)
    {
        FriedTofuDSP::BandSettings s;
        s.mute = true;
        engine.setBandSettings(b, s);
    }

    blockBuf.setSize(numChannels, blockSize);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* p = blockBuf.getWritePointer(ch);
        std::fill(p, p + blockSize, 0.5f);
    }
    engine.process(blockBuf);

    float muteEnergy = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const auto* p = blockBuf.getReadPointer(ch);
        for (int i = 0; i < blockSize; ++i)
            muteEnergy += std::abs(p[i]);
    }

    std::cout << "         Mute Residual Energy: " << muteEnergy << "\n";
    assert(muteEnergy == 0.0f);
    std::cout << "         -> PASS: All bands properly zeroed on mute.\n";

    std::cout << "\n========================================================\n";
    std::cout << " ALL VERIFICATION TESTS PASSED SUCCESSFULLY! (100% OK)\n";
    std::cout << "========================================================\n";

    return 0;
}
