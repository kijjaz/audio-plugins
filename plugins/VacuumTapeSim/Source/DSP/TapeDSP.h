#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>
#include <vector>

namespace vts
{

class TapeDSP
{
public:
    TapeDSP() {}

    // Public state for UI
    double v_state = 1.0;

    float getMagnitudeAtFrequency(double frequency)
    {
        if (gapLossFilter.coefficients == nullptr) return 1.0f;
        float mag = 1.0f;
        mag *= static_cast<float>(gapLossFilter.coefficients->getMagnitudeForFrequency(frequency, fs));
        mag *= static_cast<float>(headBumpFilter.coefficients->getMagnitudeForFrequency(frequency, fs));
        mag *= static_cast<float>(faradayFilter.coefficients->getMagnitudeForFrequency(frequency, fs));
        return mag;
    }

    float getTransferFunction(float x)
    {
        float driven = drive * (x + asymmetry_offset);
        return langevinNormalized(driven);
    }

    float getCurrentSagGR()
    {
        float gr = static_cast<float>(20.0 * std::log10(std::max(0.001, v_state)));
        return gr;
    }

    void prepare(double sampleRate, int samplesPerBlock)
    {
        fs = sampleRate;
        
        // Reset states
        v_state = 1.0;
        e1 = 0.0;
        e_rms = 0.0;
        e_slow = 0.0;
        prev_x = 0.0f;
        
        // Hiss shaping states
        hiss_b0 = 0.0f;
        hiss_b1 = 0.0f;
        hiss_b2 = 0.0f;
        hiss_env = 0.0f;

        // Modulated Delay Buffer for Wow & Flutter
        maxDelaySamples = static_cast<int>(fs * 0.05) + 16;
        delayBuffer.assign(static_cast<size_t>(maxDelaySamples), 0.0f);
        writeIndex = 0;
        
        // Wow/Flutter LFO phases
        flutterPhase1 = 0.0;
        flutterPhase2 = 0.0;
        wowPhase = 0.0;
        driftState = 0.0;
        
        // Pre-calculate constants
        alpha_env = 1.0 - std::exp(-1.0 / (0.005 * fs));       // 5ms RMS
        alpha_drain = 1.0 - std::exp(-1.0 / (0.01 * fs));      // 10ms Drain multiplier
        alpha_slow_env = 1.0 - std::exp(-1.0 / (0.2 * fs));    // 200ms slow integration
        beta_fast = 1.0 - std::exp(-1.0 / (0.05 * fs));        // 50ms fast recovery
        beta_slow = 1.0 - std::exp(-1.0 / (0.5 * fs));         // 500ms slow recovery
        hiss_env_decay = static_cast<float>(1.0 - std::exp(-1.0 / (0.02 * fs))); // 20ms envelope tracking
        
        // Initialize Biquads
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<uint32_t>(samplesPerBlock), 1 };
        gapLossFilter.prepare(spec);
        headBumpFilter.prepare(spec);
        faradayFilter.prepare(spec);
        preEmphasis.prepare(spec);
        deEmphasis.prepare(spec);
        
        updateParameters(1.0f, 0.5f, 15.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f); // Default init
    }

    void updateParameters(float driveParam, float sagThreshold, float ipsParam, float biasParam, float asymmetry, float wowFlutter, int eqMode, float hissAmount)
    {
        this->drive = driveParam;
        this->sag_threshold = sagThreshold;
        this->ips = ipsParam;
        this->bias = biasParam;
        this->asymmetry_offset = asymmetry * 0.1f;
        this->wow_flutter = wowFlutter;
        this->eq_mode = eqMode;
        this->hiss_gain = hissAmount; // 0.0 to 1.0
        
        // Virtual Bias modulations
        beta_hysteresis = 1.0f * std::pow(2.0f, bias);
        
        // 1. Gap Loss (Low Pass)
        float fc = (ips * 650.0f) * std::pow(2.0f, -bias / 2.0f);
        fc = juce::jlimit(20.0f, 20000.0f, fc);
        gapLossFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(fs, fc, 0.707f);
        
        // 2. Head Bump (Peaking)
        float fbump = ips * 3.0f;
        headBumpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(fs, fbump, 1.5f, juce::Decibels::decibelsToGain(2.5f));
        
        // 3. Faraday Roll-off (High Pass)
        faradayFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(fs, 25.0f, 0.707f);
        
        // 4. Pre/Post Emphasis EQ
        if (eq_mode == 0) // NAB
        {
            preEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 3183.0f, 0.707f, juce::Decibels::decibelsToGain(6.0f));
            deEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 3183.0f, 0.707f, juce::Decibels::decibelsToGain(-6.0f));
        }
        else // CCIR
        {
            preEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 4547.0f, 0.707f, juce::Decibels::decibelsToGain(6.0f));
            deEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 4547.0f, 0.707f, juce::Decibels::decibelsToGain(-6.0f));
        }
    }

    float processSample(float x)
    {
        // 1. Vacuum Tube Voltage Sag (RMS + Dual-Stage Recovery)
        float x_sq = x * x;
        e1 = e1 + alpha_env * (x_sq - e1);
        e_rms = e_rms + alpha_env * (e1 - e_rms);
        e_slow = e_slow + alpha_slow_env * (e_rms - e_slow);
        
        double e_lin = std::sqrt(std::max(0.0, e_rms));
        double drain = 0.0;
        
        float T = 1.0f - sag_threshold; 
        if (e_lin > T) {
            drain = alpha_drain * std::pow(e_lin - T, 2.0);
        }
        
        double blend = std::min(1.0, e_slow * 2.0);
        double beta_dynamic = beta_fast * (1.0 - blend) + beta_slow * blend;
        
        v_state = v_state - drain + beta_dynamic * (1.0 - v_state);
        v_state = std::clamp(v_state, 0.1, 1.0);
        
        // Apply Sag Gain
        float sig = static_cast<float>(x * v_state);
        
        // 2. Pre-Emphasis
        sig = preEmphasis.processSample(sig);
        
        // 3. Langevin Hysteresis with Unity-Gain Normalization
        float dx = (sig - prev_x) * static_cast<float>(fs);
        prev_x = sig;
        
        float h = 0.5f;
        float driven = drive * (sig + asymmetry_offset) - h * langevin(beta_hysteresis * dx * 0.001f);
        sig = langevinNormalized(driven);
        
        // 4. Physical Tape Hiss & Barkhausen Modulation Noise
        if (hiss_gain > 0.0001f)
        {
            // Pink-filter approximation (Paul Kellet's filter)
            float white = rng.nextFloat() * 2.0f - 1.0f;
            hiss_b0 = 0.99765f * hiss_b0 + white * 0.0990460f;
            hiss_b1 = 0.96300f * hiss_b1 + white * 0.2965164f;
            hiss_b2 = 0.57000f * hiss_b2 + white * 1.0526913f;
            float pink = hiss_b0 + hiss_b1 + hiss_b2 + white * 0.1848f;
            pink *= 0.15f; // Scale pink amplitude

            // Signal-dependent Barkhausen modulation:
            // Noise level breathes with the instantaneous energy of the magnetic signal
            float absSig = std::abs(sig);
            hiss_env += hiss_env_decay * (absSig - hiss_env);
            float modulationFactor = 1.0f + 1.8f * hiss_env;

            // Speed factor: 7.5 ips has ~3dB more hiss than 30 ips
            float speedFactor = (30.0f / std::max(7.5f, ips));
            speedFactor = std::sqrt(speedFactor);

            // Base hiss amplitude: -60dBFS to -36dBFS based on hiss_gain knob
            float baseAmp = juce::Decibels::decibelsToGain(-60.0f + hiss_gain * 24.0f);
            float totalHiss = pink * baseAmp * modulationFactor * speedFactor;

            // Inject into tape magnetic flux before playback head & de-emphasis
            sig += totalHiss;
        }

        // 5. De-Emphasis (shapes the tape hiss and restores high frequency balance)
        sig = deEmphasis.processSample(sig);
        
        // 6. Tape Playback Losses (gap loss rolls off highest hiss, head bump gives vintage body)
        sig = gapLossFilter.processSample(sig);
        sig = headBumpFilter.processSample(sig);
        sig = faradayFilter.processSample(sig);
        
        // 7. Wow & Flutter Modulated Delay Line
        if (wow_flutter > 0.001f)
        {
            sig = processWowFlutter(sig);
        }
        
        return sig;
    }

private:
    double fs = 44100.0;
    
    // Sag State
    double e1 = 0.0;
    double e_rms = 0.0;
    double e_slow = 0.0;
    
    double alpha_env, alpha_drain, alpha_slow_env, beta_fast, beta_slow;
    
    // Hysteresis State
    float prev_x = 0.0f;
    float beta_hysteresis = 1.0f;
    
    // Parameters
    float drive = 1.0f;
    float sag_threshold = 0.5f;
    float ips = 15.0f;
    float bias = 0.0f;
    float asymmetry_offset = 0.0f;
    float wow_flutter = 0.0f;
    int eq_mode = 0;
    float hiss_gain = 0.0f;
    
    // Filters
    juce::dsp::IIR::Filter<float> gapLossFilter;
    juce::dsp::IIR::Filter<float> headBumpFilter;
    juce::dsp::IIR::Filter<float> faradayFilter;
    juce::dsp::IIR::Filter<float> preEmphasis;
    juce::dsp::IIR::Filter<float> deEmphasis;

    // Delay buffer for Wow/Flutter
    std::vector<float> delayBuffer;
    int writeIndex = 0;
    int maxDelaySamples = 4096;

    // Wow/Flutter LFO states
    double flutterPhase1 = 0.0;
    double flutterPhase2 = 0.0;
    double wowPhase = 0.0;
    double driftState = 0.0;
    juce::Random rng;

    // Physical Hiss & Barkhausen states
    float hiss_b0 = 0.0f;
    float hiss_b1 = 0.0f;
    float hiss_b2 = 0.0f;
    float hiss_env = 0.0f;
    float hiss_env_decay = 0.001f;

    // Normalized Langevin: output = 3.0 * (coth(x) - 1/x)
    float langevinNormalized(float val)
    {
        return 3.0f * langevin(val);
    }

    // Standard Langevin Function
    float langevin(float val)
    {
        if (std::abs(val) < 1e-4f)
            return val / 3.0f;
        
        return 1.0f / std::tanh(val) - 1.0f / val;
    }

    float processWowFlutter(float inputSample)
    {
        float centerDelay = static_cast<float>(fs * 0.005);
        
        double dPhaseF1 = juce::MathConstants<double>::twoPi * 18.2 / fs;
        double dPhaseF2 = juce::MathConstants<double>::twoPi * 31.7 / fs;
        flutterPhase1 += dPhaseF1;
        flutterPhase2 += dPhaseF2;
        if (flutterPhase1 > juce::MathConstants<double>::twoPi) flutterPhase1 -= juce::MathConstants<double>::twoPi;
        if (flutterPhase2 > juce::MathConstants<double>::twoPi) flutterPhase2 -= juce::MathConstants<double>::twoPi;
        
        double dPhaseW = juce::MathConstants<double>::twoPi * 1.25 / fs;
        wowPhase += dPhaseW;
        if (wowPhase > juce::MathConstants<double>::twoPi) wowPhase -= juce::MathConstants<double>::twoPi;
        
        float noise = (rng.nextFloat() * 2.0f - 1.0f);
        driftState += 0.0005 * noise - 0.002 * driftState;
        
        float flutterMod = static_cast<float>(0.6 * std::sin(flutterPhase1) + 0.4 * std::cos(flutterPhase2));
        float wowMod = static_cast<float>(0.7 * std::sin(wowPhase) + 0.3 * driftState);
        
        float totalMod = (wowMod * 0.7f + flutterMod * 0.3f) * (wow_flutter * wow_flutter);
        float delayInSamples = centerDelay + totalMod * static_cast<float>(fs * 0.0018);
        delayInSamples = juce::jlimit(2.0f, static_cast<float>(maxDelaySamples - 4), delayInSamples);

        delayBuffer[static_cast<size_t>(writeIndex)] = inputSample;

        float readPos = static_cast<float>(writeIndex) - delayInSamples;
        if (readPos < 0.0f) readPos += static_cast<float>(maxDelaySamples);

        int i1 = static_cast<int>(readPos);
        float frac = readPos - static_cast<float>(i1);

        int i0 = (i1 - 1 + maxDelaySamples) % maxDelaySamples;
        int i2 = (i1 + 1) % maxDelaySamples;
        int i3 = (i1 + 2) % maxDelaySamples;

        float y0 = delayBuffer[static_cast<size_t>(i0)];
        float y1 = delayBuffer[static_cast<size_t>(i1)];
        float y2 = delayBuffer[static_cast<size_t>(i2)];
        float y3 = delayBuffer[static_cast<size_t>(i3)];

        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
        float delayedSample = ((c3 * frac + c2) * frac + c1) * frac + c0;

        writeIndex = (writeIndex + 1) % maxDelaySamples;
        return delayedSample;
    }
};

} // namespace vts
