#pragma once
#include <juce_dsp/juce_dsp.h>
#include "WDFDiodeClipper.h"
#include "LinearPhaseCrossover.h"
#include <array>

namespace FriedTofuDSP
{

struct BandSettings
{
    float driveDb       { 0.0f };
    float networkZ      { 2200.0f };
    int   diodeA        { 0 };
    int   diodeB        { 0 };
    float gainDb        { 0.0f };
    bool  mute          { false };
    bool  solo          { false };
    bool  bypass        { false };
};

class MultibandEngine
{
public:
    static constexpr int NumBands = 3;

    MultibandEngine() = default;

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        crossover.prepare(spec, 255);

        for (size_t b = 0; b < NumBands; ++b)
        {
            bandBuffers[b].setSize(static_cast<int>(spec.numChannels), static_cast<int>(spec.maximumBlockSize));
            for (size_t ch = 0; ch < spec.numChannels; ++ch)
            {
                diodeClippers[b][ch].reset();
            }
        }
    }

    void reset()
    {
        crossover.reset();
        for (int b = 0; b < NumBands; ++b)
            for (auto& clipper : diodeClippers[b])
                clipper.reset();
    }

    void setCrossoverFrequencies(double fc1, double fc2)
    {
        crossover.setCrossoverFrequencies(fc1, fc2);
    }

    void setBandSettings(int bandIndex, const BandSettings& settings)
    {
        if (bandIndex >= 0 && bandIndex < NumBands)
        {
            bands[bandIndex] = settings;
            for (auto& clipper : diodeClippers[bandIndex])
            {
                clipper.setNetworkZ(settings.networkZ);
                clipper.setDiodeA(settings.diodeA);
                clipper.setDiodeB(settings.diodeB);
            }
        }
    }

    int getLatencySamples() const noexcept
    {
        return crossover.getLatencySamples();
    }

    /**
     * Process an input audio block in place.
     * Splits into Low/Mid/High bands, saturates each band with its own WDF clipper,
     * applies band gain/mute/solo, and sums back together.
     */
    void process(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        juce::dsp::AudioBlock<const float> inBlock(buffer);
        crossover.process(inBlock, bandBuffers[0], bandBuffers[1], bandBuffers[2]);

        // Check if any band is soloed
        bool anySolo = false;
        for (int b = 0; b < NumBands; ++b)
        {
            if (bands[b].solo)
            {
                anySolo = true;
                break;
            }
        }

        // Process each band
        for (int b = 0; b < NumBands; ++b)
        {
            const auto& settings = bands[b];
            bool shouldProcessAndAudible = true;

            if (settings.mute)
                shouldProcessAndAudible = false;
            else if (anySolo && !settings.solo)
                shouldProcessAndAudible = false;

            if (!shouldProcessAndAudible)
            {
                bandBuffers[b].clear();
                continue;
            }

            if (!settings.bypass)
            {
                const float driveLin = juce::Decibels::decibelsToGain(settings.driveDb);
                const float bandGainLin = juce::Decibels::decibelsToGain(settings.gainDb);

                for (int ch = 0; ch < numChannels; ++ch)
                {
                    auto* data = bandBuffers[b].getWritePointer(ch);
                    auto& clipper = diodeClippers[b][ch % 2];

                    for (int i = 0; i < numSamples; ++i)
                    {
                        float inSample = data[i] * driveLin;
                        float saturated = clipper.processSample(inSample);
                        data[i] = saturated * bandGainLin;
                    }
                }
            }
            else
            {
                const float bandGainLin = juce::Decibels::decibelsToGain(settings.gainDb);
                bandBuffers[b].applyGain(bandGainLin);
            }
        }

        // Sum bands back to output buffer
        buffer.clear();
        for (int b = 0; b < NumBands; ++b)
        {
            for (int ch = 0; ch < numChannels; ++ch)
            {
                buffer.addFrom(ch, 0, bandBuffers[b], ch, 0, numSamples);
            }
        }
    }

private:
    double sampleRate { 48000.0 };
    LinearPhaseCrossover3Band crossover;
    std::array<BandSettings, NumBands> bands;
    std::array<std::array<WDFDiodeClipper, 2>, NumBands> diodeClippers;
    std::array<juce::AudioBuffer<float>, NumBands> bandBuffers;
};

} // namespace FriedTofuDSP
