#pragma once
#include <juce_dsp/juce_dsp.h>
#include "FIRDesigner.h"
#include <vector>

namespace FriedTofuDSP
{

/**
 * RingDelayLine
 * A simple circular delay line for arbitrary sample delays.
 */
class RingDelayLine
{
public:
    void prepare(int maxDelay)
    {
        buffer.assign(static_cast<size_t>(maxDelay + 1), 0.0f);
        writeIndex = 0;
    }

    void reset()
    {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

    inline float process(float inSample, int delay)
    {
        const size_t size = buffer.size();
        size_t readIndex = (writeIndex + size - static_cast<size_t>(delay)) % size;
        float out = buffer[readIndex];
        buffer[writeIndex] = inSample;
        writeIndex = (writeIndex + 1) % size;
        return out;
    }

private:
    std::vector<float> buffer;
    size_t writeIndex { 0 };
};

/**
 * LinearPhaseCrossover3Band
 * 
 * Splits stereo or mono audio into 3 bands: Low, Mid, High
 * using a complementary linear-phase FIR tree structure.
 * 
 * Exact delay alignment:
 * - Total system latency = tau1 + tau2 (where tau1 = tau2 = (numTaps - 1)/2)
 * - All summing branches identically align to tau1 + tau2 samples.
 */
class LinearPhaseCrossover3Band
{
public:
    LinearPhaseCrossover3Band() = default;

    void prepare(const juce::dsp::ProcessSpec& spec, int taps = 255)
    {
        sampleRate = spec.sampleRate;
        numChannels = spec.numChannels;
        numTaps = (taps % 2 == 0) ? taps + 1 : taps;
        tau = (numTaps - 1) / 2;
        totalLatency = tau * 2; // Two filter stages in cascade

        lp1Filter.prepare(spec);
        lp2Filter.prepare(spec);

        delayTau2.resize(numChannels);
        delayHighTau1.resize(numChannels);
        delayLowMidTau1.resize(numChannels);

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            delayTau2[ch].prepare(tau + 64);
            delayHighTau1[ch].prepare(tau + 64);
            delayLowMidTau1[ch].prepare(tau + 64);
        }

        // Buffers for split bands
        for (size_t b = 0; b < 3; ++b)
        {
            bandBuffers[b].setSize(static_cast<int>(numChannels), static_cast<int>(spec.maximumBlockSize));
        }
        tempBuffer.setSize(static_cast<int>(numChannels), static_cast<int>(spec.maximumBlockSize));

        updateCoefficients(currentFc1, currentFc2);
    }

    void reset()
    {
        lp1Filter.reset();
        lp2Filter.reset();
        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            delayTau2[ch].reset();
            delayHighTau1[ch].reset();
            delayLowMidTau1[ch].reset();
        }
    }

    void setCrossoverFrequencies(double fc1, double fc2)
    {
        if (std::abs(fc1 - currentFc1) > 0.5 || std::abs(fc2 - currentFc2) > 0.5)
        {
            updateCoefficients(fc1, fc2);
        }
    }

    int getLatencySamples() const noexcept
    {
        return totalLatency;
    }

    /**
     * Splits inputBlock into three separate buffers: Low, Mid, High
     */
    void process(const juce::dsp::AudioBlock<const float>& inputBlock,
                 juce::AudioBuffer<float>& outLow,
                 juce::AudioBuffer<float>& outMid,
                 juce::AudioBuffer<float>& outHigh)
    {
        const size_t numSamples = inputBlock.getNumSamples();
        const size_t channels = std::min(numChannels, inputBlock.getNumChannels());

        outLow.setSize(static_cast<int>(channels), static_cast<int>(numSamples), false, false, true);
        outMid.setSize(static_cast<int>(channels), static_cast<int>(numSamples), false, false, true);
        outHigh.setSize(static_cast<int>(channels), static_cast<int>(numSamples), false, false, true);
        tempBuffer.setSize(static_cast<int>(channels), static_cast<int>(numSamples), false, false, true);

        // Stage 1: Split at fc2 into LowMid (LP2) and High (z^-tau2 - LP2)
        // Copy input to tempBuffer and process with LP2
        for (size_t ch = 0; ch < channels; ++ch)
        {
            auto* tempPtr = tempBuffer.getWritePointer(static_cast<int>(ch));
            auto* inPtr = inputBlock.getChannelPointer(ch);
            std::copy(inPtr, inPtr + numSamples, tempPtr);
        }

        juce::dsp::AudioBlock<float> lp2Block(tempBuffer);
        juce::dsp::ProcessContextReplacing<float> lp2Context(lp2Block);
        lp2Filter.process(lp2Context); // Now tempBuffer contains LP2 (LowMid)

        // Calculate High band = z^(-tau2)*In - LP2, then delay High by tau1
        for (size_t ch = 0; ch < channels; ++ch)
        {
            auto* inPtr = inputBlock.getChannelPointer(ch);
            auto* lp2Ptr = tempBuffer.getReadPointer(static_cast<int>(ch));
            auto* highPtr = outHigh.getWritePointer(static_cast<int>(ch));
            auto& dTau2 = delayTau2[ch];
            auto& dHighTau1 = delayHighTau1[ch];

            for (size_t i = 0; i < numSamples; ++i)
            {
                float delayedIn = dTau2.process(inPtr[i], tau);
                float highSample = delayedIn - lp2Ptr[i];
                highPtr[i] = dHighTau1.process(highSample, tau);
            }
        }

        // Stage 2: Split tempBuffer (LowMid) at fc1 into Low (LP1) and Mid (z^-tau1*LowMid - Low)
        // Copy tempBuffer to outLow, then run LP1 on outLow
        for (size_t ch = 0; ch < channels; ++ch)
        {
            auto* lowPtr = outLow.getWritePointer(static_cast<int>(ch));
            auto* lp2Ptr = tempBuffer.getReadPointer(static_cast<int>(ch));
            std::copy(lp2Ptr, lp2Ptr + numSamples, lowPtr);
        }

        juce::dsp::AudioBlock<float> lp1Block(outLow);
        juce::dsp::ProcessContextReplacing<float> lp1Context(lp1Block);
        lp1Filter.process(lp1Context); // Now outLow contains LP1(LP2) = Low band!

        // Mid band = z^(-tau1)*LowMid - Low
        for (size_t ch = 0; ch < channels; ++ch)
        {
            auto* lp2Ptr = tempBuffer.getReadPointer(static_cast<int>(ch));
            auto* lowPtr = outLow.getReadPointer(static_cast<int>(ch));
            auto* midPtr = outMid.getWritePointer(static_cast<int>(ch));
            auto& dLowMidTau1 = delayLowMidTau1[ch];

            for (size_t i = 0; i < numSamples; ++i)
            {
                float delayedLowMid = dLowMidTau1.process(lp2Ptr[i], tau);
                midPtr[i] = delayedLowMid - lowPtr[i];
            }
        }
    }

private:
    void updateCoefficients(double fc1, double fc2)
    {
        currentFc1 = std::clamp(fc1, 40.0, 4000.0);
        currentFc2 = std::clamp(fc2, currentFc1 + 50.0, sampleRate * 0.48);

        auto coeffs1 = FIRDesigner::designLowPass(numTaps, currentFc1, sampleRate);
        auto coeffs2 = FIRDesigner::designLowPass(numTaps, currentFc2, sampleRate);

        *lp1Filter.state = juce::dsp::FIR::Coefficients<float>(coeffs1.data(), coeffs1.size());
        *lp2Filter.state = juce::dsp::FIR::Coefficients<float>(coeffs2.data(), coeffs2.size());
    }

    double sampleRate { 48000.0 };
    size_t numChannels { 2 };
    int numTaps { 255 };
    int tau { 127 };
    int totalLatency { 254 };

    double currentFc1 { 250.0 };
    double currentFc2 { 2500.0 };

    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> lp1Filter;
    juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter<float>, juce::dsp::FIR::Coefficients<float>> lp2Filter;

    std::vector<RingDelayLine> delayTau2;
    std::vector<RingDelayLine> delayHighTau1;
    std::vector<RingDelayLine> delayLowMidTau1;

    std::array<juce::AudioBuffer<float>, 3> bandBuffers;
    juce::AudioBuffer<float> tempBuffer;
};

} // namespace FriedTofuDSP
