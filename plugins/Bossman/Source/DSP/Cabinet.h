#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

/**
 * Acoustically Modeled Multi-Cabinet Filter Bank
 * Simulates physical acoustic enclosure resonance, speaker cone break-up,
 * and voice-coil inductance rolloff across 5 iconic guitar cabinets + Bypass.
 * Zero Machine Learning, Zero Latency, Pure Biquad Cascade.
 */
class Cabinet {
public:
    enum class Model {
        Jensen_4x10 = 0,     // Fender Bassman 4x10 Open-back Pine (Alnico P10R)
        Jensen_2x12,         // Fender Twin Reverb 2x12 Open-back (Ceramic C12N)
        Greenback_4x12,      // Marshall 1960A 4x12 Closed-back (Celestion G12M Greenback)
        Vintage30_4x12,      // Mesa/Boutique 4x12 Closed-back (Celestion Vintage 30)
        AlnicoBlue_2x12,     // Vox AC30 2x12 Semi-open (Celestion Alnico Blue)
        Bypass               // Direct Out (No Cabinet Filter)
    };

    enum class Channel { Left, Right };

    struct AcousticProfile {
        const char* name;
        float hpFreq;          // Box enclosure low-cut (Hz)
        float thumpFreq;       // Speaker cone primary thump resonance (Hz)
        float thumpGainDb;     // Cone resonance boost (dB)
        float thumpQ;          // Q factor of thump
        float midFreq;         // Enclosure cavity reflection / mid contour (Hz)
        float midGainDb;       // Mid dip/peak (dB)
        float midQ;            // Q factor of mid contour
        float presenceFreq;    // Speaker cone break-up / presence peak (Hz)
        float presenceGainDb;  // Presence peak boost (dB)
        float presenceQ;       // Q factor of presence peak
        float shimmerFreq;     // Mic distance & outer cone shimmer (Hz)
        float shimmerGainDb;   // Shimmer peak/dip (dB)
        float shimmerQ;        // Q factor of shimmer
        float lpFreq;          // Voice-coil inductance steep rolloff (Hz)
        float gainTrim;        // Internal output level alignment
    };

    void prepare(const juce::dsp::ProcessSpec &spec, Channel ch) {
        sampleRate = spec.sampleRate;
        channel = ch;
        setModel(currentModel);
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

    void setModel(Model m) {
        currentModel = m;
        switch (currentModel) {
            case Model::Jensen_4x10:
                // Fender 4x10 Jensen P10R: Fast bass rolloff at 115Hz, scooped 420Hz, crisp 3.8kHz presence
                profile = { "4x10 Jensen P10R", 110.0f, 115.0f, 2.8f, 1.8f, 420.0f, -4.5f, 1.4f, 3800.0f, 4.5f, 3.5f, 5200.0f, -6.0f, 1.2f, 4800.0f, 1.0f };
                break;
            case Model::Jensen_2x12:
                // Fender 2x12 Jensen C12N: Deep low punch at 85Hz, wider scoop, airy glass at 4.2kHz
                profile = { "2x12 Jensen C12N", 80.0f, 92.0f, 3.5f, 1.6f, 380.0f, -5.5f, 1.2f, 4200.0f, 5.0f, 3.0f, 5800.0f, -4.0f, 1.5f, 5200.0f, 1.05f };
                break;
            case Model::Greenback_4x12:
                // Marshall 4x12 Greenback: Dense woody mids at 550Hz, thick 110Hz thump, creamy rolled-off 3.2kHz peak
                profile = { "4x12 Greenback", 75.0f, 110.0f, 4.2f, 2.0f, 550.0f, 2.0f, 1.1f, 3200.0f, 3.8f, 2.5f, 4500.0f, -8.0f, 1.0f, 4400.0f, 0.95f };
                break;
            case Model::Vintage30_4x12:
                // Mesa 4x12 Vintage 30: Massive 100Hz tight thump, aggressive 3.5kHz cone bite (+6dB spike), steep 4.8kHz rolloff
                profile = { "4x12 Vintage 30", 70.0f, 105.0f, 5.0f, 2.2f, 480.0f, -2.5f, 1.3f, 3500.0f, 6.5f, 3.8f, 4800.0f, -10.0f, 1.1f, 4700.0f, 0.90f };
                break;
            case Model::AlnicoBlue_2x12:
                // Vox 2x12 Alnico Blue: Singing vocal upper midrange at 2.8kHz, sparkling bell chime at 4.5kHz
                profile = { "2x12 Alnico Blue", 85.0f, 100.0f, 2.5f, 1.5f, 650.0f, 1.5f, 1.3f, 2800.0f, 4.0f, 2.8f, 4500.0f, 3.5f, 2.2f, 5500.0f, 1.0f };
                break;
            case Model::Bypass:
                profile = { "Bypass (Direct)", 20.0f, 100.0f, 0.0f, 1.0f, 1000.0f, 0.0f, 1.0f, 3000.0f, 0.0f, 1.0f, 5000.0f, 0.0f, 1.0f, 20000.0f, 1.0f };
                break;
        }
        updateFilters();
    }

    inline float processSample(float input) noexcept {
        if (currentModel == Model::Bypass)
            return input;

        float x = input;
        x = hpFilter.processSingleSampleRaw(x);
        x = thumpFilter.processSingleSampleRaw(x);
        x = midFilter.processSingleSampleRaw(x);
        x = presenceFilter.processSingleSampleRaw(x);
        x = shimmerFilter.processSingleSampleRaw(x);
        x = lpFilter1.processSingleSampleRaw(x);
        x = lpFilter2.processSingleSampleRaw(x);
        return x * profile.gainTrim;
    }

    const AcousticProfile& getProfile() const noexcept { return profile; }

private:
    void updateFilters() {
        if (currentModel == Model::Bypass)
            return;

        // Subtle stereo microphone spread between Left and Right capsules
        float spread = (channel == Channel::Right) ? 1.035f : 1.0f;

        float hp = juce::jlimit(20.0f, static_cast<float>(sampleRate * 0.45), profile.hpFreq * spread);
        float thump = juce::jlimit(40.0f, static_cast<float>(sampleRate * 0.45), profile.thumpFreq * spread);
        float mid = juce::jlimit(100.0f, static_cast<float>(sampleRate * 0.45), profile.midFreq * spread);
        float presence = juce::jlimit(500.0f, static_cast<float>(sampleRate * 0.45), profile.presenceFreq * spread);
        float shimmer = juce::jlimit(1000.0f, static_cast<float>(sampleRate * 0.45), profile.shimmerFreq * spread);
        float lp = juce::jlimit(1000.0f, static_cast<float>(sampleRate * 0.45), profile.lpFreq * spread);

        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, hp));

        thumpFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, thump, profile.thumpQ, std::pow(10.0f, profile.thumpGainDb / 20.0f)));

        midFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, mid, profile.midQ, std::pow(10.0f, profile.midGainDb / 20.0f)));

        presenceFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, presence, profile.presenceQ, std::pow(10.0f, profile.presenceGainDb / 20.0f)));

        shimmerFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, shimmer, profile.shimmerQ, std::pow(10.0f, profile.shimmerGainDb / 20.0f)));

        // 4th order steep low-pass slope for voice coil inductance
        lpFilter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, lp, 0.8f));
        lpFilter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, juce::jmin(static_cast<float>(sampleRate * 0.48), lp * 1.25f), 0.707f));
    }

    double sampleRate = 44100.0;
    Channel channel = Channel::Left;
    Model currentModel = Model::Jensen_4x10;
    AcousticProfile profile;

    juce::IIRFilter hpFilter;
    juce::IIRFilter thumpFilter;
    juce::IIRFilter midFilter;
    juce::IIRFilter presenceFilter;
    juce::IIRFilter shimmerFilter;
    juce::IIRFilter lpFilter1;
    juce::IIRFilter lpFilter2;
};
