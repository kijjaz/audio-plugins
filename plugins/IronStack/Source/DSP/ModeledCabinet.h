#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

/**
 * Acoustically Modeled Cabinet Filter Bank: FenderJensenP10R
 * Fitted from Tone3000 / Physical Impulse Response.
 * Zero Machine Learning overhead, Zero Latency, Pure Biquad Cascade.
 */
class FenderJensenP10RCabinet {
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {
        sampleRate = spec.sampleRate;
        updateFilters();
        reset();
    }

    void reset() {
        hpFilter.reset();
        thumpFilter.reset();
        midFilter.reset();
        presenceFilter.reset();
        shimmerFilter.reset();
        lpFilter1.reset();
        lpFilter2.reset();
    }

    inline float processSample(float input) noexcept {
        float x = input;
        x = hpFilter.processSingleSampleRaw(x);
        x = thumpFilter.processSingleSampleRaw(x);
        x = midFilter.processSingleSampleRaw(x);
        x = presenceFilter.processSingleSampleRaw(x);
        x = shimmerFilter.processSingleSampleRaw(x);
        x = lpFilter1.processSingleSampleRaw(x);
        x = lpFilter2.processSingleSampleRaw(x);
        return x;
    }

private:
    void updateFilters() {
        // High-pass box cutoff (120.0 Hz)
        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, 120.0f));

        // Speaker thump (111.2 Hz, -6.0 dB, Q=1.43)
        thumpFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, 111.2f, 1.43f, std::pow(10.0f, -6.00f / 20.0f)));

        // Body wood resonance / mid contour (259.7 Hz, +6.0 dB, Q=1.20)
        midFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, 259.7f, 1.20f, std::pow(10.0f, 6.00f / 20.0f)));

        // Cone presence peak (3800.0 Hz, +4.0 dB, Q=5.00)
        presenceFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, 3800.0f, 5.00f, std::pow(10.0f, 4.01f / 20.0f)));

        // Upper shimmer (5228.2 Hz, -11.8 dB, Q=0.59)
        shimmerFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, 5228.2f, 0.59f, std::pow(10.0f, -11.78f / 20.0f)));

        // Voice coil low-pass rolloff (4630.7 Hz, 4th-order slope)
        lpFilter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 4630.7f, 0.8f));
        lpFilter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 5788.4f, 0.707f));
    }

    double sampleRate = 44100.0;
    juce::IIRFilter hpFilter;
    juce::IIRFilter thumpFilter;
    juce::IIRFilter midFilter;
    juce::IIRFilter presenceFilter;
    juce::IIRFilter shimmerFilter;
    juce::IIRFilter lpFilter1;
    juce::IIRFilter lpFilter2;
};
