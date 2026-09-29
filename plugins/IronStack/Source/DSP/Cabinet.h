#pragma once
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <juce_dsp/juce_dsp.h>

/**
 * Acoustically Modeled Multi-Cabinet Filter Bank
 * Simulates physical acoustic enclosure resonance, speaker cone break-up,
 * and voice-coil inductance rolloff across 5 iconic guitar cabinets + Bypass.
 * Zero Machine Learning, Zero Latency, Pure Biquad Cascade.
 */
class Cabinet {
public:
    enum class Model {
        Jensen_4x10 = 0,     // Fender Bassman Neo 4x10
        Jensen_2x12,         // Fender Twin Reverb 2x12 (Jensen C12N)
        BassmanCTS_2x15,     // 1970 Fender Bassman 2x15 (CTS Speakers)
        AmpegSVT_8x10,       // Ampeg SVT 8x10 "The Fridge" (Infinite Baffle Punch)
        Acoustic360_1x18,    // Acoustic 360 / 301 1x18 Folded Horn (Jaco Pastorius Sub-Thump)
        Eminence_2x10_Vented,// Eminence Legend CA1059 2x10 Vented Box (Lucas 62Hz Fb Design)
        Eminence_4x10_Vented,// Eminence Legend CA1059 4x10 Vented Box (Lucas 65Hz Fb Punch)
        HartkePro_2x12,      // Hartke PRO 2200 2x12 Aluminum Bass Cab
        Greenback_4x12,      // Marshall 1960A 4x12 (Celestion Greenback)
        Vintage30_4x12,      // Mesa Rectifier 4x12 (Celestion Vintage 30)
        BassmanJensen_2x12,  // 1966 Fender Bassman 2x12 (Jensen C12NA)
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
                // Fender Bassman Neo 4x10 (Tone3000 Calibrated): HP 40Hz, Thump 149.5Hz (+12dB, Q=0.50), Mid 900Hz, Presence 3373Hz (+7.7dB), LP 4000Hz
                profile = { "4x10 Bassman Neo (Tone3000)", 40.0f, 149.5f, 12.0f, 0.50f, 900.0f, 3.5f, 4.00f, 3373.0f, 7.7f, 1.46f, 3650.6f, -12.0f, 4.00f, 4000.0f, 1.0f };
                break;
            case Model::Jensen_2x12:
                // Fender Twin Reverb 2x12 Jensen C12N (Tone3000 Calibrated): HP 40Hz, Thump 180Hz (+12dB, Q=0.50), Mid 443Hz, Presence 2750Hz (+12.4dB), LP 5139Hz
                profile = { "2x12 Twin C12N (Tone3000)", 40.0f, 180.0f, 12.0f, 0.50f, 443.4f, 6.0f, 0.50f, 2749.6f, 12.4f, 1.15f, 4248.9f, -12.0f, 4.00f, 5139.1f, 1.0f };
                break;
            case Model::BassmanCTS_2x15:
                // 1970 Fender Bassman 2x15 CTS (Tone3000 Calibrated): HP 40Hz, Thump 116Hz (+12dB, Q=0.50), Mid 440.4Hz (+6dB), Presence 2423Hz (+7.2dB), LP 4408Hz
                profile = { "2x15 '70 Bassman CTS (Tone3000)", 40.0f, 116.0f, 12.0f, 0.50f, 440.4f, 6.0f, 1.64f, 2422.7f, 7.2f, 0.85f, 4241.8f, -11.4f, 1.72f, 4407.8f, 1.0f };
                break;
            case Model::AmpegSVT_8x10:
                // Ampeg SVT 8x10 "The Fridge": Infinite baffle sealed 10" drivers; HP 45Hz, tight punch thump 110Hz (+10dB, Q=0.65), focused mid push 850Hz (+4.2dB), speaker cone break-up 2850Hz (+6.8dB), steep rolloff 4200Hz
                profile = { "8x10 Ampeg SVT Fridge", 45.0f, 110.0f, 10.0f, 0.65f, 850.0f, 4.2f, 1.50f, 2850.0f, 6.8f, 1.20f, 3800.0f, -6.5f, 2.50f, 4200.0f, 1.0f };
                break;
            case Model::Acoustic360_1x18:
                // Acoustic 360 / 301 1x18 Folded Horn (Jaco Pastorius signature): Sub-bass chamber thump 62Hz (+14dB, Q=0.85), folded horn cavity notch 520Hz (-5.5dB), throat resonance 2150Hz (+5.0dB), steep horn cutoff 3600Hz
                profile = { "1x18 Acoustic 360 Horn", 32.0f, 62.0f, 14.0f, 0.85f, 520.0f, -5.5f, 2.00f, 2150.0f, 5.0f, 1.40f, 3200.0f, -10.0f, 3.00f, 3600.0f, 0.95f };
                break;
            case Model::Eminence_2x10_Vented:
                // Eminence Legend CA1059 2x10 Vented Box (Anthony Lucas Design): Vb=3cu.ft, Fb=62Hz (QL=6.98, F3=62.4Hz), Mid 750Hz (+3.8dB), Presence 3200Hz (+7.5dB), LP 4500Hz
                profile = { "2x10 Eminence Legend (Vented)", 35.0f, 62.0f, 11.5f, 0.70f, 750.0f, 3.8f, 2.50f, 3200.0f, 7.5f, 1.30f, 4000.0f, -8.0f, 3.00f, 4500.0f, 1.0f };
                break;
            case Model::Eminence_4x10_Vented:
                // Eminence Legend CA1059 4x10 Vented Box (Anthony Lucas Design): Vb=6cu.ft, Fb=65Hz (QL=6.98, F3=63.4Hz), Mid 820Hz (+4.5dB), Presence 3400Hz (+8.2dB), LP 4600Hz
                profile = { "4x10 Eminence Legend (Vented)", 35.0f, 65.0f, 12.5f, 0.72f, 820.0f, 4.5f, 2.00f, 3400.0f, 8.2f, 1.25f, 4100.0f, -7.5f, 2.80f, 4600.0f, 1.0f };
                break;
            case Model::HartkePro_2x12:
                // Hartke PRO 2200 2x12 Bass Cab (Tone3000 Calibrated): HP 40Hz, Thump 108Hz (+12dB, Q=0.57), Mid 446.8Hz, Presence 2797Hz, LP 5210Hz
                profile = { "2x12 Hartke Pro 2200 (Tone3000)", 40.0f, 108.0f, 12.0f, 0.57f, 446.8f, 2.5f, 4.00f, 2796.5f, -2.3f, 5.00f, 4213.1f, -8.3f, 4.00f, 5210.2f, 1.0f };
                break;
            case Model::Greenback_4x12:
                // Marshall 1960A JCM800 4x12 (Tone3000 Calibrated): HP 40Hz, Thump 161Hz (+12dB, Q=0.50), Mid 689Hz, Presence 3073Hz (+15dB), LP 6153Hz
                profile = { "4x12 Marshall 1960A (Tone3000)", 40.0f, 161.0f, 12.0f, 0.50f, 688.7f, 6.0f, 0.50f, 3073.0f, 15.0f, 2.19f, 4879.2f, 10.0f, 4.00f, 6152.9f, 0.95f };
                break;
            case Model::Vintage30_4x12:
                // Mesa Boogie Rectifier 4x12 V30 (Tone3000 Calibrated): HP 40Hz, Thump 78.4Hz (+12dB, Q=0.71), Mid 900Hz, Presence 3800Hz (+3.9dB), LP 4915Hz
                profile = { "4x12 Mesa Recto V30 (Tone3000)", 40.0f, 78.4f, 12.0f, 0.71f, 900.0f, 6.0f, 2.25f, 3800.0f, 3.9f, 0.89f, 6000.0f, -12.0f, 0.50f, 4915.0f, 0.95f };
                break;
            case Model::BassmanJensen_2x12:
                // 1966 Fender Bassman 2x12 Jensen C12NA (Tone3000 Calibrated): HP 56.6Hz, Thump 121.3Hz (+12dB, Q=0.50), Mid 724Hz, Presence 2632Hz (+2.7dB), LP 4521Hz
                profile = { "2x12 '66 Bassman C12NA (Tone3000)", 56.6f, 121.3f, 12.0f, 0.50f, 723.8f, 6.0f, 2.71f, 2631.5f, 2.7f, 0.50f, 4125.1f, -3.8f, 2.33f, 4521.0f, 1.0f };
                break;
            case Model::Bypass:
                profile = { "Bypass (Direct)", 20.0f, 100.0f, 0.0f, 1.0f, 1000.0f, 0.0f, 1.0f, 3000.0f, 0.0f, 1.0f, 5000.0f, 0.0f, 1.0f, 20000.0f, 1.0f };
                break;
        }
        updateFilters();
    }

    void setPresence(float presence0to10) {
        // presence = 5.0 -> nominal cabinet profile
        // presence = 0.0 -> -6 dB darker cone roll-off
        // presence = 10.0 -> +6 dB upper harmonic bite
        presenceTrimDb = (juce::jlimit(0.0f, 10.0f, presence0to10) - 5.0f) * 1.2f;
        updateFilters();
    }

    void setStereoSpread(float spread0to100) {
        // spread0to100: 0% = mono coherent, 100% = full acoustic mic spread
        spreadAmount = juce::jlimit(0.0f, 1.0f, spread0to100 / 100.0f);
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

    float evaluateMagnitudeDb(float freqHz) const noexcept {
        if (currentModel == Model::Bypass)
            return 0.0f;

        // Compute cascaded biquad response at freqHz
        double w = 2.0 * M_PI * freqHz / sampleRate;
        std::complex<double> z1 = std::polar(1.0, -w);
        std::complex<double> z2 = std::polar(1.0, -2.0 * w);

        // Note: compute each stage from profile parameters
        double w0_thump = 2.0 * M_PI * profile.thumpFreq / sampleRate;
        double a_thump = std::sin(w0_thump) / (2.0 * profile.thumpQ);
        double A_thump = std::pow(10.0, profile.thumpGainDb / 40.0);
        std::complex<double> h_thump = ( (1.0 + a_thump*A_thump) - 2.0*std::cos(w0_thump)*z1 + (1.0 - a_thump*A_thump)*z2 ) /
                                      ( (1.0 + a_thump/A_thump) - 2.0*std::cos(w0_thump)*z1 + (1.0 - a_thump/A_thump)*z2 );

        double w0_mid = 2.0 * M_PI * profile.midFreq / sampleRate;
        double a_mid = std::sin(w0_mid) / (2.0 * profile.midQ);
        double A_mid = std::pow(10.0, profile.midGainDb / 40.0);
        std::complex<double> h_mid = ( (1.0 + a_mid*A_mid) - 2.0*std::cos(w0_mid)*z1 + (1.0 - a_mid*A_mid)*z2 ) /
                                    ( (1.0 + a_mid/A_mid) - 2.0*std::cos(w0_mid)*z1 + (1.0 - a_mid/A_mid)*z2 );

        double w0_pres = 2.0 * M_PI * profile.presenceFreq / sampleRate;
        double a_pres = std::sin(w0_pres) / (2.0 * profile.presenceQ);
        double A_pres = std::pow(10.0, (profile.presenceGainDb + presenceTrimDb) / 40.0);
        std::complex<double> h_pres = ( (1.0 + a_pres*A_pres) - 2.0*std::cos(w0_pres)*z1 + (1.0 - a_pres*A_pres)*z2 ) /
                                     ( (1.0 + a_pres/A_pres) - 2.0*std::cos(w0_pres)*z1 + (1.0 - a_pres/A_pres)*z2 );

        // Highpass
        double w0_hp = 2.0 * M_PI * profile.hpFreq / sampleRate;
        double a_hp = std::sin(w0_hp) / (2.0 * 0.7071);
        std::complex<double> h_hp = ( (1.0 + std::cos(w0_hp))*0.5 - (1.0 + std::cos(w0_hp))*z1 + (1.0 + std::cos(w0_hp))*0.5*z2 ) /
                                   ( (1.0 + a_hp) - 2.0*std::cos(w0_hp)*z1 + (1.0 - a_hp)*z2 );

        // Lowpass 4th order
        double w0_lp = 2.0 * M_PI * profile.lpFreq / sampleRate;
        double a_lp = std::sin(w0_lp) / (2.0 * 0.8);
        std::complex<double> h_lp1 = ( (1.0 - std::cos(w0_lp))*0.5 + (1.0 - std::cos(w0_lp))*z1 + (1.0 - std::cos(w0_lp))*0.5*z2 ) /
                                    ( (1.0 + a_lp) - 2.0*std::cos(w0_lp)*z1 + (1.0 - a_lp)*z2 );

        double total_mag = std::abs(h_thump * h_mid * h_pres * h_hp * h_lp1 * h_lp1) * profile.gainTrim;
        return static_cast<float>(20.0 * std::log10(std::max(total_mag, 1e-5)));
    }

private:
    void updateFilters() {
        if (currentModel == Model::Bypass)
            return;

        // Dynamic stereo microphone spread between Left and Right capsules
        float spread = (channel == Channel::Right) ? (1.0f + 0.035f * spreadAmount) : 1.0f;

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
            sampleRate, presence, profile.presenceQ, std::pow(10.0f, (profile.presenceGainDb + presenceTrimDb) / 20.0f)));

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
    float presenceTrimDb = 0.0f;
    float spreadAmount = 1.0f;

    juce::IIRFilter hpFilter;
    juce::IIRFilter thumpFilter;
    juce::IIRFilter midFilter;
    juce::IIRFilter presenceFilter;
    juce::IIRFilter shimmerFilter;
    juce::IIRFilter lpFilter1;
    juce::IIRFilter lpFilter2;
};
