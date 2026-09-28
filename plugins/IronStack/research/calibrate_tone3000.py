#!/usr/bin/env python3
"""
Batch Fitter: Calibrate Bossman Biquad Cabinets Directly Against Tone3000 Captures
===================================================================================
Loads the authentic Tone3000 impulse responses and optimizes the 6 acoustic biquad
parameters for each real cabinet model.
"""

import os
import numpy as np
import scipy.io.wavfile as wav
import scipy.signal as signal
from scipy.optimize import minimize
import matplotlib.pyplot as plt

from extract_cabinet_biquads import (
    load_ir_wav, compute_target_spectrum, fit_cabinet_biquads,
    biquad_highpass_mag, biquad_peak_mag, biquad_lowpass_mag
)

TONE3000_TARGETS = [
    {
        "id": "Bassman_410_Neo",
        "name": "Fender Bassman Neo 4x10 (SM57 OnAxis)",
        "wav": "plugins/Bossman/Tone/Fender Bassman Neo/Fender Bassman Neo 410_SM57_Close_OnAxis.wav",
        "color": "#d4af37"
    },
    {
        "id": "Bassman_1966_Jensen_C12NA",
        "name": "1966 Fender Bassman 2x12 (Jensen C12NA - SM57 Cap Edge)",
        "wav": "plugins/Bossman/Tone/1966 Fender Bassman Cabinet Jensen C12NA Speaker/1966 Bassman Cabinet with Jensen C12NA - SM57 - Cap Edge.wav",
        "color": "#3498db"
    },
    {
        "id": "Bassman_1970_CTS_215",
        "name": "1970 Fender Bassman 2x15 (Original CTS - SM57 Cap)",
        "wav": "plugins/Bossman/Tone/1970 Fender Bassman 2x15 Cabinet Original CTS Speakers/1970 Bassman Cabinet CTS - SM57 Upper - Cap.wav",
        "color": "#16a085"
    },
    {
        "id": "Marshall_1960A_JCM800",
        "name": "Marshall 1960A JCM800 Lead 4x12 (Blended)",
        "wav": "plugins/Bossman/Tone/Marshall JCM800 LEAD-1960 CAB/Marshall JCM900 LEAD-1960 BLENDED 3.wav",
        "color": "#e74c3c"
    },
    {
        "id": "Mesa_Recto_V30",
        "name": "Mesa Boogie Rectifier 4x12 (Celestion V30)",
        "wav": "plugins/Bossman/Tone/The HEAVIEST Mesa Recto V30 IR/MESA RECTO V30.wav",
        "color": "#9b59b6"
    },
    {
        "id": "Fender_Twin_Reverb",
        "name": "Fender Twin Reverb 2x12 (Jensen C12N - Balanced)",
        "wav": "plugins/Bossman/Tone/Fender Twin Reverb/TWIN REVERB __ BALANCED.wav",
        "color": "#2ecc71"
    }
]

def run_calibration():
    print("=" * 70)
    print("CALIBRATING BOSSMAN BIQUADS AGAINST REAL TONE3000 IMPULSE RESPONSES")
    print("=" * 70)

    fitted_results = []
    freqs = np.logspace(np.log10(20.0), np.log10(20000.0), 1000)

    for target in TONE3000_TARGETS:
        print(f"\nProcessing: {target['name']}")
        print(f"File: {target['wav']}")
        fs, ir_data = load_ir_wav(target["wav"])
        f_axis, target_db = compute_target_spectrum(ir_data, fs, n_fft=2048)
        
        # Fit biquad stages
        fit = fit_cabinet_biquads(f_axis, target_db, fs)
        target["fit"] = fit
        target["f_axis"] = f_axis
        target["target_db"] = target_db
        fitted_results.append(target)

        print(f" -> Enclosure Highpass:  {fit['hp_freq']:.1f} Hz")
        print(f" -> Speaker Thump:       {fit['thump_freq']:.1f} Hz ({fit['thump_gain']:+.1f} dB, Q={fit['thump_q']:.2f})")
        print(f" -> Midrange Cavity:     {fit['mid_freq']:.1f} Hz ({fit['mid_gain']:+.1f} dB, Q={fit['mid_q']:.2f})")
        print(f" -> Cone Presence Bite:  {fit['presence_freq']:.1f} Hz ({fit['presence_gain']:+.1f} dB, Q={fit['presence_q']:.2f})")
        print(f" -> Shimmer / Mic Air:   {fit['shimmer_freq']:.1f} Hz ({fit['shimmer_gain']:+.1f} dB, Q={fit['shimmer_q']:.2f})")
        print(f" -> Voice Coil Rolloff:  {fit['lp_freq']:.1f} Hz")

    # Generate 6-Panel High-Resolution Comparison Plot
    fig, axes = plt.subplots(len(fitted_results), 1, figsize=(11, 16), facecolor="#1a1c1e")
    fig.suptitle("REAL TONE3000 CABINET IRs vs FITTED PHYSICAL BIQUAD CASCADES", 
                 color="#ffffff", fontsize=12, fontweight="bold", y=0.99)

    for i, res in enumerate(fitted_results):
        ax = axes[i]
        ax.set_facecolor("#22252a")
        f_sub = res["f_axis"][(res["f_axis"] >= 30) & (res["f_axis"] <= 15000)]
        t_sub = res["target_db"][(res["f_axis"] >= 30) & (res["f_axis"] <= 15000)]
        
        # Plot Raw Tone3000 IR
        ax.semilogx(f_sub, t_sub, color="#666666", alpha=0.55, lw=1.0, label="Raw Tone3000 Measured IR")
        # Plot Biquad Fit
        fit_sub = res["fit"]["full_fitted_db"][(res["f_axis"] >= 30) & (res["f_axis"] <= 15000)]
        ax.semilogx(f_sub, fit_sub, color=res["color"], lw=2.2, label="Bossman Modeled Biquad Cascade")

        ax.set_title(res["name"], color="#ffffff", fontsize=9.5, fontweight="bold", loc="left")
        ax.set_xlim(30, 15000)
        ax.set_ylim(-40, 8)
        ax.grid(True, which="both", ls=":", alpha=0.3, color="#888888")
        ax.tick_params(colors="#cccccc", labelsize=8)
        ax.set_ylabel("Gain (dB)", color="#cccccc", fontsize=8)
        if i == 4:
            ax.set_xlabel("Frequency (Hz)", color="#cccccc", fontsize=9)
        if i == 0:
            ax.legend(loc="lower left", facecolor="#1a1c1e", edgecolor="#444444", labelcolor="#ffffff", fontsize=8)

    plt.tight_layout(rect=[0, 0.02, 1, 0.97])
    out_img = "plugins/Bossman/research/tone3000_calibrated_comparison.png"
    plt.savefig(out_img, dpi=180)
    plt.close()
    print(f"\nSaved master calibration verification image to: {out_img}")

    return fitted_results

if __name__ == "__main__":
    run_calibration()
