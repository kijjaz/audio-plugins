#!/usr/bin/env python3
"""
Tone3000 Cabinet IR to Parametric Biquad Cascade Extractor & Converter
======================================================================
This tool takes any Guitar Cabinet Impulse Response (.wav) from Tone3000
(or custom community captures) and extracts:
1. Low-cut / Box Enclosure Resonance (High-pass + Bass resonance peak)
2. Midrange Body Wood Cavity Notch & Presence Peaks
3. High-frequency Voice Coil Rolloff (Steep low-pass filter)
4. Exports an instant header-only C++ Biquad Filter Bank for real-time DSP
   (with ZERO machine learning latency, zero FFT buffer latency, and ~0% CPU).
"""

import sys
import os
import argparse
import numpy as np
import scipy.io.wavfile as wav
import scipy.signal as signal
from scipy.optimize import minimize
import matplotlib.pyplot as plt

def load_ir_wav(wav_path, target_fs=44100):
    """Loads and normalizes an impulse response WAV file."""
    fs, data = wav.read(wav_path)
    
    # Handle multi-channel (convert to mono)
    if data.ndim > 1:
        data = np.mean(data, axis=1)
        
    # Convert integer formats to float32 [-1.0, 1.0]
    if data.dtype == np.int16:
        data = data.astype(np.float32) / 32768.0
    elif data.dtype == np.int32:
        data = data.astype(np.float32) / 2147483648.0
    elif data.dtype == np.uint8:
        data = (data.astype(np.float32) - 128.0) / 128.0
        
    # Trim leading silence up to threshold
    peak = np.max(np.abs(data))
    if peak > 0:
        start_idx = np.argmax(np.abs(data) > 0.01 * peak)
        data = data[start_idx:]
        
    # Truncate to standard cabinet IR length (e.g. 1024 or 2048 samples)
    max_len = 2048
    if len(data) > max_len:
        # Smooth window cutoff
        window = signal.windows.tukey(len(data), alpha=0.1)
        data = data * window
        data = data[:max_len]
        
    return fs, data

def compute_target_spectrum(ir_data, fs, n_fft=2048):
    """Computes smoothed magnitude spectrum of the cabinet IR."""
    fft_vals = np.fft.rfft(ir_data, n=n_fft)
    freqs = np.fft.rfftfreq(n_fft, d=1.0/fs)
    mag_db = 20.0 * np.log10(np.maximum(np.abs(fft_vals), 1e-5))
    
    # Normalize peak to 0 dB
    mag_db -= np.max(mag_db)
    return freqs, mag_db

def biquad_peak_mag(freqs, fs, f0, gain_db, q):
    """Computes magnitude response in dB of a standard peaking/bell biquad."""
    w = 2.0 * np.pi * freqs / fs
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) / (2.0 * q)
    A = 10.0 ** (gain_db / 40.0)
    
    b0 = 1.0 + alpha * A
    b1 = -2.0 * np.cos(w0)
    b2 = 1.0 - alpha * A
    a0 = 1.0 + alpha / A
    a1 = -2.0 * np.cos(w0)
    a2 = 1.0 - alpha / A
    
    # Frequency response: H(e^jw) = (b0 + b1*z^-1 + b2*z^-2) / (a0 + a1*z^-1 + a2*z^-2)
    z1 = np.exp(-1j * w)
    z2 = np.exp(-2j * w)
    num = (b0 + b1 * z1 + b2 * z2)
    den = (a0 + a1 * z1 + a2 * z2)
    h = num / den
    return 20.0 * np.log10(np.maximum(np.abs(h), 1e-6))

def biquad_lowpass_mag(freqs, fs, f0, q=0.707):
    """Computes magnitude response in dB of a 2nd order lowpass filter."""
    w = 2.0 * np.pi * freqs / fs
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) / (2.0 * q)
    cos_w0 = np.cos(w0)
    
    b0 = (1.0 - cos_w0) * 0.5
    b1 = 1.0 - cos_w0
    b2 = (1.0 - cos_w0) * 0.5
    a0 = 1.0 + alpha
    a1 = -2.0 * cos_w0
    a2 = 1.0 - alpha
    
    z1 = np.exp(-1j * w)
    z2 = np.exp(-2j * w)
    h = (b0 + b1 * z1 + b2 * z2) / (a0 + a1 * z1 + a2 * z2)
    return 20.0 * np.log10(np.maximum(np.abs(h), 1e-6))

def biquad_highpass_mag(freqs, fs, f0, q=0.707):
    """Computes magnitude response in dB of a 2nd order highpass filter."""
    w = 2.0 * np.pi * freqs / fs
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) / (2.0 * q)
    cos_w0 = np.cos(w0)
    
    b0 = (1.0 + cos_w0) * 0.5
    b1 = -(1.0 + cos_w0)
    b2 = (1.0 + cos_w0) * 0.5
    a0 = 1.0 + alpha
    a1 = -2.0 * cos_w0
    a2 = 1.0 - alpha
    
    z1 = np.exp(-1j * w)
    z2 = np.exp(-2j * w)
    h = (b0 + b1 * z1 + b2 * z2) / (a0 + a1 * z1 + a2 * z2)
    return 20.0 * np.log10(np.maximum(np.abs(h), 1e-6))

def fit_cabinet_biquads(freqs, target_db, fs):
    """
    Fits cabinet acoustic behavior to 6 physical stages:
    1. HP: Open-back box low rolloff (50 - 100 Hz)
    2. Peak 1: Speaker fundamental thump / resonant frequency (80 - 150 Hz)
    3. Peak 2: Body cavity wood resonance / lower mids (300 - 600 Hz)
    4. Peak 3: Cone breakup / presence peak (2.0 - 3.5 kHz)
    5. Peak 4: Upper fizz / mic positioning peak (3.5 - 5.0 kHz)
    6. LP: Voice-coil induction steep rolloff (4.5 - 6.5 kHz)
    """
    # Restrict fitting to guitar-relevant band: 40 Hz to 12 kHz
    valid = (freqs >= 40.0) & (freqs <= 12000.0)
    f_sub = freqs[valid]
    t_sub = target_db[valid]
    
    # Initial parameters [hp_f, p1_f, p1_g, p1_q, p2_f, p2_g, p2_q, p3_f, p3_g, p3_q, p4_f, p4_g, p4_q, lp_f]
    init_params = [
        75.0,                  # HP cutoff
        110.0, 3.5, 1.8,       # Thump peak
        450.0, -4.0, 1.2,      # Mid scoop / box reflection
        2800.0, 6.0, 2.0,      # Presence peak
        4200.0, 2.0, 1.8,      # Cone shimmer
        5200.0                 # LP voice-coil rolloff
    ]
    
    bounds = [
        (40.0, 120.0),         # HP
        (70.0, 180.0), (-6.0, 12.0), (0.5, 4.0),
        (250.0, 900.0), (-16.0, 6.0), (0.5, 4.0),
        (1800.0, 3800.0), (-6.0, 15.0), (0.5, 5.0),
        (3500.0, 6000.0), (-12.0, 10.0), (0.5, 4.0),
        (4000.0, 8000.0)       # LP
    ]
    
    def model_response(p):
        hp_f = p[0]
        p1_f, p1_g, p1_q = p[1], p[2], p[3]
        p2_f, p2_g, p2_q = p[4], p[5], p[6]
        p3_f, p3_g, p3_q = p[7], p[8], p[9]
        p4_f, p4_g, p4_q = p[10], p[11], p[12]
        lp_f = p[13]
        
        resp = (
            biquad_highpass_mag(f_sub, fs, hp_f, q=0.707) +
            biquad_peak_mag(f_sub, fs, p1_f, p1_g, p1_q) +
            biquad_peak_mag(f_sub, fs, p2_f, p2_g, p2_q) +
            biquad_peak_mag(f_sub, fs, p3_f, p3_g, p3_q) +
            biquad_peak_mag(f_sub, fs, p4_f, p4_g, p4_q) +
            biquad_lowpass_mag(f_sub, fs, lp_f, q=0.8) +
            biquad_lowpass_mag(f_sub, fs, lp_f * 1.25, q=0.707) # 4th-order slope
        )
        # Shift to match level
        resp -= np.median(resp[f_sub > 500]) - np.median(t_sub[f_sub > 500])
        return resp
        
    def loss(p):
        sim = model_response(p)
        # Perceptual weight: heavier weight around 500Hz - 5kHz
        weights = 1.0 + 2.0 * np.exp(-((np.log10(f_sub) - np.log10(2000.0))**2) / 0.5)
        return np.sum(weights * ((sim - t_sub) ** 2))
        
    res = minimize(loss, init_params, bounds=bounds, method='L-BFGS-B')
    best_p = res.x
    
    # Compute full curve
    hp_f = best_p[0]
    p1_f, p1_g, p1_q = best_p[1], best_p[2], best_p[3]
    p2_f, p2_g, p2_q = best_p[4], best_p[5], best_p[6]
    p3_f, p3_g, p3_q = best_p[7], best_p[8], best_p[9]
    p4_f, p4_g, p4_q = best_p[10], best_p[11], best_p[12]
    lp_f = best_p[13]
    
    full_fitted = (
        biquad_highpass_mag(freqs, fs, hp_f, q=0.707) +
        biquad_peak_mag(freqs, fs, p1_f, p1_g, p1_q) +
        biquad_peak_mag(freqs, fs, p2_f, p2_g, p2_q) +
        biquad_peak_mag(freqs, fs, p3_f, p3_g, p3_q) +
        biquad_peak_mag(freqs, fs, p4_f, p4_g, p4_q) +
        biquad_lowpass_mag(freqs, fs, lp_f, q=0.8) +
        biquad_lowpass_mag(freqs, fs, lp_f * 1.25, q=0.707)
    )
    full_fitted -= np.median(full_fitted[(freqs > 500) & (freqs < 2000)]) - np.median(target_db[(freqs > 500) & (freqs < 2000)])
    
    fit_summary = {
        'hp_freq': float(hp_f),
        'thump_freq': float(p1_f), 'thump_gain': float(p1_g), 'thump_q': float(p1_q),
        'mid_freq': float(p2_f), 'mid_gain': float(p2_g), 'mid_q': float(p2_q),
        'presence_freq': float(p3_f), 'presence_gain': float(p3_g), 'presence_q': float(p3_q),
        'shimmer_freq': float(p4_f), 'shimmer_gain': float(p4_g), 'shimmer_q': float(p4_q),
        'lp_freq': float(lp_f),
        'full_fitted_db': full_fitted
    }
    return fit_summary

def export_cpp_header(fit_data, output_path, preset_name="Fender4x10_Jensen"):
    """Generates a clean, header-only C++ class that drops into any audio plugin."""
    cpp = f"""#pragma once
#include <juce_dsp/juce_dsp.h>
#include <cmath>

/**
 * Acoustically Modeled Cabinet Filter Bank: {preset_name}
 * Fitted from Tone3000 / Physical Impulse Response.
 * Zero Machine Learning overhead, Zero Latency, Pure Biquad Cascade.
 */
class {preset_name}Cabinet {{
public:
    void prepare(const juce::dsp::ProcessSpec& spec) {{
        sampleRate = spec.sampleRate;
        updateFilters();
        reset();
    }}

    void reset() {{
        hpFilter.reset();
        thumpFilter.reset();
        midFilter.reset();
        presenceFilter.reset();
        shimmerFilter.reset();
        lpFilter1.reset();
        lpFilter2.reset();
    }}

    inline float processSample(float input) noexcept {{
        float x = input;
        x = hpFilter.processSingleSampleRaw(x);
        x = thumpFilter.processSingleSampleRaw(x);
        x = midFilter.processSingleSampleRaw(x);
        x = presenceFilter.processSingleSampleRaw(x);
        x = shimmerFilter.processSingleSampleRaw(x);
        x = lpFilter1.processSingleSampleRaw(x);
        x = lpFilter2.processSingleSampleRaw(x);
        return x;
    }}

private:
    void updateFilters() {{
        // High-pass box cutoff ({fit_data['hp_freq']:.1f} Hz)
        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, {fit_data['hp_freq']:.1f}f));

        // Speaker thump ({fit_data['thump_freq']:.1f} Hz, {fit_data['thump_gain']:+.1f} dB, Q={fit_data['thump_q']:.2f})
        thumpFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, {fit_data['thump_freq']:.1f}f, {fit_data['thump_q']:.2f}f, std::pow(10.0f, {fit_data['thump_gain']:.2f}f / 20.0f)));

        // Body wood resonance / mid contour ({fit_data['mid_freq']:.1f} Hz, {fit_data['mid_gain']:+.1f} dB, Q={fit_data['mid_q']:.2f})
        midFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, {fit_data['mid_freq']:.1f}f, {fit_data['mid_q']:.2f}f, std::pow(10.0f, {fit_data['mid_gain']:.2f}f / 20.0f)));

        // Cone presence peak ({fit_data['presence_freq']:.1f} Hz, {fit_data['presence_gain']:+.1f} dB, Q={fit_data['presence_q']:.2f})
        presenceFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, {fit_data['presence_freq']:.1f}f, {fit_data['presence_q']:.2f}f, std::pow(10.0f, {fit_data['presence_gain']:.2f}f / 20.0f)));

        // Upper shimmer ({fit_data['shimmer_freq']:.1f} Hz, {fit_data['shimmer_gain']:+.1f} dB, Q={fit_data['shimmer_q']:.2f})
        shimmerFilter.setCoefficients(juce::IIRCoefficients::makePeakFilter(
            sampleRate, {fit_data['shimmer_freq']:.1f}f, {fit_data['shimmer_q']:.2f}f, std::pow(10.0f, {fit_data['shimmer_gain']:.2f}f / 20.0f)));

        // Voice coil low-pass rolloff ({fit_data['lp_freq']:.1f} Hz, 4th-order slope)
        lpFilter1.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, {fit_data['lp_freq']:.1f}f, 0.8f));
        lpFilter2.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, {fit_data['lp_freq'] * 1.25:.1f}f, 0.707f));
    }}

    double sampleRate = 44100.0;
    juce::IIRFilter hpFilter;
    juce::IIRFilter thumpFilter;
    juce::IIRFilter midFilter;
    juce::IIRFilter presenceFilter;
    juce::IIRFilter shimmerFilter;
    juce::IIRFilter lpFilter1;
    juce::IIRFilter lpFilter2;
}};
"""
    with open(output_path, "w") as f:
        f.write(cpp)
    print(f"Exported C++ Biquad Filter Bank to: {output_path}")

def plot_comparison(freqs, target_db, fit_data, img_path="cabinet_fit_comparison.png"):
    """Renders a technical comparison between raw Tone3000 IR and the Biquad Filter Bank."""
    plt.figure(figsize=(10, 5), facecolor="#1a1c1e")
    ax = plt.gca()
    ax.set_facecolor("#22252a")
    
    plt.semilogx(freqs, target_db, color="#7f8c8d", alpha=0.6, lw=1.2, label="Raw Tone3000 Cabinet IR")
    plt.semilogx(freqs, fit_data['full_fitted_db'], color="#d4af37", lw=2.2, label="Modeled Biquad Cascade (0% ML CPU)")
    
    plt.title("Tone3000 Cabinet Simulation: Raw IR vs Physical Biquad Cascade", color="#ffffff", fontsize=11, fontweight="bold")
    plt.xlabel("Frequency (Hz)", color="#cccccc", fontsize=9)
    plt.ylabel("Magnitude (dB)", color="#cccccc", fontsize=9)
    plt.xlim(30, 15000)
    plt.ylim(-45, 10)
    plt.grid(True, which="both", ls=":", alpha=0.3, color="#888888")
    ax.tick_params(colors="#cccccc")
    
    # Annotate stages
    plt.annotate(f"Thump ({fit_data['thump_freq']:.0f}Hz)", xy=(fit_data['thump_freq'], fit_data['thump_gain']),
                 xytext=(fit_data['thump_freq']*1.2, fit_data['thump_gain']+4),
                 arrowprops=dict(facecolor="#d4af37", arrowstyle="->", lw=0.9), color="#d4af37", fontsize=7.5)
    plt.annotate(f"Presence Peak ({fit_data['presence_freq']:.0f}Hz)", xy=(fit_data['presence_freq'], fit_data['presence_gain']),
                 xytext=(fit_data['presence_freq']*0.5, fit_data['presence_gain']+6),
                 arrowprops=dict(facecolor="#d4af37", arrowstyle="->", lw=0.9), color="#d4af37", fontsize=7.5)
    plt.annotate(f"Voice Coil Rolloff ({fit_data['lp_freq']:.0f}Hz)", xy=(fit_data['lp_freq'], -10),
                 xytext=(fit_data['lp_freq']*1.2, 0),
                 arrowprops=dict(facecolor="#d4af37", arrowstyle="->", lw=0.9), color="#d4af37", fontsize=7.5)
                 
    plt.legend(facecolor="#1a1c1e", edgecolor="#444444", labelcolor="#ffffff", fontsize=8.5)
    plt.tight_layout()
    plt.savefig(img_path, dpi=180)
    plt.close()
    print(f"Saved plot comparison to: {img_path}")

def main():
    parser = argparse.ArgumentParser(description="Extract Biquad Filter Bank from Tone3000 Cabinet IR")
    parser.add_argument("--ir", type=str, default="plugins/Discrete808/build/_deps/juce-src/examples/Assets/guitar_amp.wav",
                        help="Path to cabinet IR WAV file")
    parser.add_argument("--out-cpp", type=str, default="plugins/Bossman/Source/DSP/ModeledCabinet.h",
                        help="Output path for C++ header")
    parser.add_argument("--out-img", type=str, default="plugins/Bossman/research/cabinet_fit_comparison.png",
                        help="Output path for comparison plot")
    args = parser.parse_args()

    print(f"Loading Cabinet IR: {args.ir}")
    fs, ir_data = load_ir_wav(args.ir)
    freqs, target_db = compute_target_spectrum(ir_data, fs)

    print("Fitting physical biquad stages to acoustic profile...")
    fit_data = fit_cabinet_biquads(freqs, target_db, fs)
    print(f"-> Box Highpass: {fit_data['hp_freq']:.1f} Hz")
    print(f"-> Speaker Thump: {fit_data['thump_freq']:.1f} Hz ({fit_data['thump_gain']:+.1f} dB, Q={fit_data['thump_q']:.2f})")
    print(f"-> Mid Cavity: {fit_data['mid_freq']:.1f} Hz ({fit_data['mid_gain']:+.1f} dB, Q={fit_data['mid_q']:.2f})")
    print(f"-> Cone Presence: {fit_data['presence_freq']:.1f} Hz ({fit_data['presence_gain']:+.1f} dB, Q={fit_data['presence_q']:.2f})")
    print(f"-> Voice Coil Rolloff: {fit_data['lp_freq']:.1f} Hz")

    export_cpp_header(fit_data, args.out_cpp, "FenderJensenP10R")
    plot_comparison(freqs, target_db, fit_data, args.out_img)

if __name__ == "__main__":
    main()
