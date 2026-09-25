#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <algorithm>

namespace vts
{

class TapeDSP
{
public:
    TapeDSP() {}

    // Public state for UI
    double v_state = 1.0;

    void prepare(double sampleRate, int samplesPerBlock)
    {
        fs = sampleRate;
        
        // Reset states
        v_state = 1.0;
        e1 = 0.0;
        e_rms = 0.0;
        e_slow = 0.0;
        prev_x = 0.0f;
        
        // Pre-calculate constants
        alpha_env = 1.0 - std::exp(-1.0 / (0.005 * fs));       // 5ms RMS
        alpha_drain = 1.0 - std::exp(-1.0 / (0.01 * fs));      // 10ms Drain multiplier
        alpha_slow_env = 1.0 - std::exp(-1.0 / (0.2 * fs));    // 200ms slow integration
        beta_fast = 1.0 - std::exp(-1.0 / (0.05 * fs));        // 50ms fast recovery
        beta_slow = 1.0 - std::exp(-1.0 / (0.5 * fs));         // 500ms slow recovery
        
        // Initialize Biquads
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<uint32_t>(samplesPerBlock), 1 };
        gapLossFilter.prepare(spec);
        headBumpFilter.prepare(spec);
        faradayFilter.prepare(spec);
        preEmphasis.prepare(spec);
        deEmphasis.prepare(spec);
        
        updateParameters(1.0f, 0.5f, 15.0f, 0.0f, 0.0f, 0.0f, 0); // Default init
    }

    void updateParameters(float driveParam, float sagThreshold, float ipsParam, float biasParam, float asymmetry, float wowFlutter, int eqMode)
    {
        this->drive = driveParam;
        this->sag_threshold = sagThreshold;
        this->ips = ipsParam;
        this->bias = biasParam;
        this->asymmetry_offset = asymmetry * 0.1f; // Scale to subtle DC
        this->eq_mode = eqMode;
        
        // Virtual Bias modulations
        beta_hysteresis = 1.0f * std::pow(2.0f, bias); // Base beta is 1.0, scales with bias
        
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
            // Simplified NAB: High Shelf boost at 3183Hz
            preEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 3183.0f, 0.707f, juce::Decibels::decibelsToGain(6.0f));
            deEmphasis.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(fs, 3183.0f, 0.707f, juce::Decibels::decibelsToGain(-6.0f));
        }
        else // CCIR
        {
            // CCIR: High Shelf boost at 4547Hz
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
        
        float e_lin = std::sqrt(std::max(0.0, e_rms));
        float drain = 0.0;
        
        // Invert sag_threshold so 100% means a very low threshold (more sag)
        float T = 1.0f - sag_threshold; 
        if (e_lin > T) {
            drain = alpha_drain * std::pow(e_lin - T, 2.0);
        }
        
        float blend = std::min(1.0, e_slow * 2.0);
        float beta_dynamic = beta_fast * (1.0 - blend) + beta_slow * blend;
        
        v_state = v_state - drain + beta_dynamic * (1.0 - v_state);
        v_state = std::clamp(v_state, 0.1, 1.0);
        
        // Apply Sag Gain
        float sig = x * v_state;
        
        // 2. Pre-Emphasis
        sig = preEmphasis.processSample(sig);
        
        // 3. Langevin Hysteresis
        float dx = (sig - prev_x) * static_cast<float>(fs);
        prev_x = sig;
        
        float h = 0.5f; // Fixed coercivity for now
        float driven = drive * (sig + asymmetry_offset) - h * langevin(beta_hysteresis * dx * 0.001f);
        sig = langevin(driven);
        
        // 4. De-Emphasis
        sig = deEmphasis.processSample(sig);
        
        // 5. Tape Playback Losses
        sig = gapLossFilter.processSample(sig);
        sig = headBumpFilter.processSample(sig);
        sig = faradayFilter.processSample(sig);
        
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
    int eq_mode = 0;
    
    // Filters
    juce::dsp::IIR::Filter<float> gapLossFilter;
    juce::dsp::IIR::Filter<float> headBumpFilter;
    juce::dsp::IIR::Filter<float> faradayFilter;
    juce::dsp::IIR::Filter<float> preEmphasis;
    juce::dsp::IIR::Filter<float> deEmphasis;

    // Langevin Math Function
    float langevin(float val)
    {
        if (std::abs(val) < 1e-4f)
            return val / 3.0f;
        
        return 1.0f / std::tanh(val) - 1.0f / val;
    }
};

} // namespace vts
