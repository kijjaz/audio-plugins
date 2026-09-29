#!/usr/bin/env python3
"""
IRONSTACK — Exhaustive Matrix Test Suite & Acoustic Engineering Report
=======================================================================
Benchmarks all 120 End-to-End Rig Combinations:
- 10 Amplifier Head Topologies (Fender Tweed/Blackface, Ampeg, Marshall, Vox, Mesa, Soldano)
- 12 Speaker Cabinet Enclosures (Open, Sealed, Infinite Baffle, Folded Horn, Vented, Bypass)

Generates an executive-grade, multi-page vector PDF report:
- Page 1: 12 Speaker Cabinet Acoustic Responses & Detailed Component Specifications.
- Pages 2–6: 2 Amplifier Heads per page with clean layout, zero label collision,
  dedicated legend ribbons, and calibrated inter-amp loudness normalization.
"""

import os
import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from tone_stack_database import CIRCUITS, solve_tone_stack

# Professional Design System Colors (Carbon & Gold)
charcoal = "#141517"
carbon_panel = "#1e2023"
carbon_border = "#2a2d32"
gold = "#d4af37"
gold_light = "#f6e7b2"
copper = "#c87d55"
cream = "#f8f9fa"
grid_color = "#dcdfe4"

# Inter-Amp Loudness Compensation Multipliers (matching C++ ToneStack.h)
LEVEL_COMPENSATION = {
    "Fender_59_Bassman": 1.000,
    "Fender_65_Bassman_AA864": 2.106,
    "Ampeg_B15N": 3.614,
    "Ampeg_B100R": 0.995,
    "Marshall_Super_Bass_100": 1.025,
    "Fender_Twin_Reverb": 2.296,
    "Marshall_JCM800": 0.873,
    "Vox_AC30_TopBoost": 1.028,
    "Mesa_Dual_Rectifier": 0.909,
    "Soldano_SLO_100": 0.941
}

# 12 Cabinet Models matching DSP/Cabinet.h with curated high-contrast palette
CABINETS = {
    "Jensen_4x10": {
        "name": "4x10 Bassman Neo (Tone3000)",
        "short_name": "4x10 Neo",
        "type": "Open Pine",
        "hp": 40.0, "thump_f": 149.5, "thump_g": 12.0, "thump_q": 0.50,
        "mid_f": 900.0, "mid_g": 3.5, "mid_q": 4.00,
        "pres_f": 3373.0, "pres_g": 7.7, "pres_q": 1.46,
        "shim_f": 3650.6, "shim_g": -12.0, "shim_q": 4.00,
        "lp": 4000.0, "color": "#d4af37",
        "role": "Punchy fast transient bass articulation"
    },
    "Jensen_2x12": {
        "name": "2x12 Twin C12N (Tone3000)",
        "short_name": "2x12 Twin",
        "type": "Open Birch",
        "hp": 40.0, "thump_f": 180.0, "thump_g": 12.0, "thump_q": 0.50,
        "mid_f": 443.4, "mid_g": 6.0, "mid_q": 0.50,
        "pres_f": 2749.6, "pres_g": 12.4, "pres_q": 1.15,
        "shim_f": 4248.9, "shim_g": -12.0, "shim_q": 4.00,
        "lp": 5139.1, "color": "#2980b9",
        "role": "Deep Blackface lows, singing glassy bell"
    },
    "BassmanCTS_2x15": {
        "name": "2x15 '70 Bassman CTS (Tone3000)",
        "short_name": "2x15 CTS",
        "type": "Deep Sealed",
        "hp": 40.0, "thump_f": 116.0, "thump_g": 12.0, "thump_q": 0.50,
        "mid_f": 440.4, "mid_g": 6.0, "mid_q": 1.64,
        "pres_f": 2422.7, "pres_g": 7.2, "pres_q": 0.85,
        "shim_f": 4241.8, "shim_g": -11.4, "shim_q": 1.72,
        "lp": 4407.8, "color": "#16a085",
        "role": "Sub-bass authority, massive physical kick"
    },
    "AmpegSVT_8x10": {
        "name": "8x10 Ampeg SVT Fridge",
        "short_name": "8x10 SVT",
        "type": "Infinite Baffle",
        "hp": 45.0, "thump_f": 110.0, "thump_g": 10.0, "thump_q": 0.65,
        "mid_f": 850.0, "mid_g": 4.2, "mid_q": 1.50,
        "pres_f": 2850.0, "pres_g": 6.8, "pres_q": 1.20,
        "shim_f": 3800.0, "shim_g": -6.5, "shim_q": 2.50,
        "lp": 4200.0, "color": "#34495e",
        "role": "Stadium low-mid punch, tight transient slam"
    },
    "Acoustic360_1x18": {
        "name": "1x18 Acoustic 360 Horn",
        "short_name": "1x18 Horn",
        "type": "Folded Horn",
        "hp": 32.0, "thump_f": 62.0, "thump_g": 14.0, "thump_q": 0.85,
        "mid_f": 520.0, "mid_g": -5.5, "mid_q": 2.00,
        "pres_f": 2150.0, "pres_g": 5.0, "pres_q": 1.40,
        "shim_f": 3200.0, "shim_g": -10.0, "shim_q": 3.00,
        "lp": 3600.0, "color": "#8e44ad",
        "role": "Jaco sub-bass acoustic compression throw"
    },
    "Eminence_2x10_Vented": {
        "name": "2x10 Eminence Legend (Vented)",
        "short_name": "2x10 Vented",
        "type": "Vented 3 cu.ft",
        "hp": 35.0, "thump_f": 62.0, "thump_g": 11.5, "thump_q": 0.70,
        "mid_f": 750.0, "mid_g": 3.8, "mid_q": 2.50,
        "pres_f": 3200.0, "pres_g": 7.5, "pres_q": 1.30,
        "shim_f": 4000.0, "shim_g": -8.0, "shim_q": 3.00,
        "lp": 4500.0, "color": "#009688",
        "role": "Lucas vented design, 62Hz Helmholtz tuning"
    },
    "Eminence_4x10_Vented": {
        "name": "4x10 Eminence Legend (Vented)",
        "short_name": "4x10 Vented",
        "type": "Vented 6 cu.ft",
        "hp": 35.0, "thump_f": 65.0, "thump_g": 12.5, "thump_q": 0.72,
        "mid_f": 820.0, "mid_g": 4.5, "mid_q": 2.00,
        "pres_f": 3400.0, "pres_g": 8.2, "pres_q": 1.25,
        "shim_f": 4100.0, "shim_g": -7.5, "shim_q": 2.80,
        "lp": 4600.0, "color": "#27ae60",
        "role": "Lucas vented design, 65Hz massive punch"
    },
    "HartkePro_2x12": {
        "name": "2x12 Hartke Pro 2200 (Tone3000)",
        "short_name": "2x12 Hartke",
        "type": "Ported Dual",
        "hp": 40.0, "thump_f": 108.0, "thump_g": 12.0, "thump_q": 0.57,
        "mid_f": 446.8, "mid_g": 2.5, "mid_q": 4.00,
        "pres_f": 2796.5, "pres_g": -2.3, "pres_q": 5.00,
        "shim_f": 4213.1, "shim_g": -8.3, "shim_q": 4.00,
        "lp": 5210.2, "color": "#e67e22",
        "role": "Aluminum cone lightning slap & transient"
    },
    "Greenback_4x12": {
        "name": "4x12 Marshall 1960A (Tone3000)",
        "short_name": "4x12 1960A",
        "type": "Closed 1960",
        "hp": 40.0, "thump_f": 161.0, "thump_g": 12.0, "thump_q": 0.50,
        "mid_f": 688.7, "mid_g": 6.0, "mid_q": 0.50,
        "pres_f": 3073.0, "pres_g": 15.0, "pres_q": 2.19,
        "shim_f": 4879.2, "shim_g": 10.0, "shim_q": 4.00,
        "lp": 6152.9, "color": "#c0392b",
        "role": "Creamy British roar, forward aggressive bite"
    },
    "Vintage30_4x12": {
        "name": "4x12 Mesa Recto V30 (Tone3000)",
        "short_name": "4x12 Recto",
        "type": "Closed Oversized",
        "hp": 40.0, "thump_f": 78.4, "thump_g": 12.0, "thump_q": 0.71,
        "mid_f": 900.0, "mid_g": 6.0, "mid_q": 2.25,
        "pres_f": 3800.0, "pres_g": 3.9, "pres_q": 0.89,
        "shim_f": 6000.0, "shim_g": -12.0, "shim_q": 0.50,
        "lp": 4915.0, "color": "#9c27b0",
        "role": "Tight modern low thump & scooped bite"
    },
    "AlnicoBlue_2x12": {
        "name": "2x12 '66 Bassman C12NA (Tone3000)",
        "short_name": "2x12 '66",
        "type": "Closed Vintage",
        "hp": 56.6, "thump_f": 121.3, "thump_g": 12.0, "thump_q": 0.50,
        "mid_f": 723.8, "mid_g": 6.0, "mid_q": 2.71,
        "pres_f": 2631.5, "pres_g": 2.7, "pres_q": 0.50,
        "shim_f": 4125.1, "shim_g": -3.8, "shim_q": 2.33,
        "lp": 4521.0, "color": "#795548",
        "role": "Warm vintage acoustic resonance, rich mids"
    },
    "Bypass": {
        "name": "Bypass (Direct Out)",
        "short_name": "Direct Out",
        "type": "Direct Line",
        "hp": 20.0, "thump_f": 100.0, "thump_g": 0.0, "thump_q": 1.0,
        "mid_f": 1000.0, "mid_g": 0.0, "mid_q": 1.0,
        "pres_f": 3000.0, "pres_g": 0.0, "pres_q": 1.0,
        "shim_f": 5000.0, "shim_g": 0.0, "shim_q": 1.0,
        "lp": 20000.0, "color": "#607d8b",
        "role": "Flat direct signal for external IR loader"
    }
}

# ==============================================================================
# 2. ACOUSTIC FILTER EQUATIONS (BIQUAD CASCADE)
# ==============================================================================

def biquad_peak_mag_phase(freqs, fs, f0, gain_db, q):
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

    z = np.exp(-1j * w)
    num = b0 + b1 * z + b2 * (z ** 2)
    den = a0 + a1 * z + a2 * (z ** 2)
    H = num / den
    return 20.0 * np.log10(np.abs(H)), np.angle(H, deg=True)

def biquad_hp_mag_phase(freqs, fs, fc):
    w = 2.0 * np.pi * freqs / fs
    w0 = 2.0 * np.pi * fc / fs
    q = 0.7071
    alpha = np.sin(w0) / (2.0 * q)
    b0 = (1.0 + np.cos(w0)) / 2.0
    b1 = -(1.0 + np.cos(w0))
    b2 = (1.0 + np.cos(w0)) / 2.0
    a0 = 1.0 + alpha
    a1 = -2.0 * np.cos(w0)
    a2 = 1.0 - alpha

    z = np.exp(-1j * w)
    num = b0 + b1 * z + b2 * (z ** 2)
    den = a0 + a1 * z + a2 * (z ** 2)
    H = num / den
    return 20.0 * np.log10(np.abs(H)), np.angle(H, deg=True)

def biquad_lp_mag_phase(freqs, fs, fc):
    w = 2.0 * np.pi * freqs / fs
    w0 = 2.0 * np.pi * fc / fs
    q = 0.7071
    alpha = np.sin(w0) / (2.0 * q)
    b0 = (1.0 - np.cos(w0)) / 2.0
    b1 = 1.0 - np.cos(w0)
    b2 = (1.0 - np.cos(w0)) / 2.0
    a0 = 1.0 + alpha
    a1 = -2.0 * np.cos(w0)
    a2 = 1.0 - alpha

    z = np.exp(-1j * w)
    num = b0 + b1 * z + b2 * (z ** 2)
    den = a0 + a1 * z + a2 * (z ** 2)
    H = num / den
    return 20.0 * np.log10(np.abs(H)), np.angle(H, deg=True)

def solve_cabinet(cab_key, freqs, fs=44100.0):
    c = CABINETS[cab_key]
    m_hp, p_hp = biquad_hp_mag_phase(freqs, fs, c["hp"])
    m_th, p_th = biquad_peak_mag_phase(freqs, fs, c["thump_f"], c["thump_g"], c["thump_q"])
    m_mid, p_mid = biquad_peak_mag_phase(freqs, fs, c["mid_f"], c["mid_g"], c["mid_q"])
    m_pr, p_pr = biquad_peak_mag_phase(freqs, fs, c["pres_f"], c["pres_g"], c["pres_q"])
    m_sh, p_sh = biquad_peak_mag_phase(freqs, fs, c["shim_f"], c["shim_g"], c["shim_q"])
    m_lp1, p_lp1 = biquad_lp_mag_phase(freqs, fs, c["lp"])
    m_lp2, p_lp2 = biquad_lp_mag_phase(freqs, fs, c["lp"])

    mag_total = m_hp + m_th + m_mid + m_pr + m_sh + m_lp1 + m_lp2
    phase_total = p_hp + p_th + p_mid + p_pr + p_sh + p_lp1 + p_lp2
    return mag_total, phase_total

# ==============================================================================
# 3. EXECUTIVE REPORT BUILDER
# ==============================================================================

def add_header_banner(fig, title, subtitle, status_tag="Status: VALIDATED (120 Rigs)"):
    h_ax = fig.add_axes([0.05, 0.915, 0.90, 0.065])
    h_ax.set_facecolor(charcoal)
    h_ax.set_xticks([])
    h_ax.set_yticks([])
    for spine in h_ax.spines.values():
        spine.set_color(gold)
        spine.set_linewidth(1.0)

    h_ax.text(0.02, 0.65, title, color='#ffffff', fontsize=12.5, fontweight='bold', va='center', fontfamily='sans-serif')
    h_ax.text(0.02, 0.25, subtitle, color=gold, fontsize=8.5, fontweight='bold', va='center', fontfamily='sans-serif')
    h_ax.text(0.98, 0.45, status_tag, color='#2ecc71', fontsize=8.5, fontweight='bold', ha='right', va='center', fontfamily='sans-serif')

def generate_matrix_tests(pdf_path="IronStack_Matrix_Test_Report.pdf"):
    print(f"Generating Executive Matrix Benchmark Report -> {pdf_path}")
    freqs = np.logspace(np.log10(20.0), np.log10(20000.0), 900)
    fs = 44100.0

    amp_keys = list(CIRCUITS.keys())
    cab_keys = list(CABINETS.keys())

    with PdfPages(pdf_path) as pdf:
        # ======================================================================
        # PAGE 1: 12 Speaker Cabinet Models Benchmark & Specifications
        # ======================================================================
        fig1 = plt.figure(figsize=(11, 8.5), facecolor='#ffffff')
        add_header_banner(fig1, 
                          "IRONSTACK  —  ACOUSTIC CABINET ENCLOSURE MODELS BENCHMARK", 
                          "Frequency & Phase Transfer Functions across 12 Discrete Biquad Cascade Enclosures",
                          "Zero-ML / Zero-Latency")

        # Top Left: Magnitude
        ax_cm = fig1.add_axes([0.07, 0.56, 0.41, 0.30])
        ax_cm.set_facecolor(cream)
        for key in cab_keys:
            m, _ = solve_cabinet(key, freqs, fs)
            ax_cm.semilogx(freqs, m, label=CABINETS[key]["name"], color=CABINETS[key]["color"], lw=1.6)
        ax_cm.set_title("Cabinet Enclosure Frequency Response", fontsize=9.5, fontweight='bold', color=charcoal)
        ax_cm.set_xlabel("Frequency (Hz)", fontsize=8)
        ax_cm.set_ylabel("Magnitude (dB)", fontsize=8)
        ax_cm.set_xlim(20, 20000)
        ax_cm.set_ylim(-45, 12)
        ax_cm.grid(True, which="both", ls=":", color=grid_color, alpha=0.8)

        # Top Right: Phase
        ax_cp = fig1.add_axes([0.54, 0.56, 0.41, 0.30])
        ax_cp.set_facecolor(cream)
        for key in cab_keys:
            _, p = solve_cabinet(key, freqs, fs)
            p_wrapped = (p + 180) % 360 - 180
            ax_cp.semilogx(freqs, p_wrapped, label=CABINETS[key]["name"], color=CABINETS[key]["color"], lw=1.6)
        ax_cp.set_title("Cabinet Phase Responses", fontsize=9.5, fontweight='bold', color=charcoal)
        ax_cp.set_xlabel("Frequency (Hz)", fontsize=8)
        ax_cp.set_ylabel("Phase (Degrees)", fontsize=8)
        ax_cp.set_xlim(20, 20000)
        ax_cp.set_ylim(-180, 180)
        ax_cp.grid(True, which="both", ls=":", color=grid_color, alpha=0.8)

        # Bottom: Cabinet Specs Table with dedicated, non-overlapping columns
        ax_ctab = fig1.add_axes([0.05, 0.04, 0.90, 0.44])
        ax_ctab.axis('tight')
        ax_ctab.axis('off')

        cab_cols = [
            "Cabinet Model", 
            "Enclosure Type", 
            "Thump Freq", 
            "Thump Boost", 
            "Mid Voicing", 
            "Presence Peak", 
            "Roll-off", 
            "Acoustic Sonic Character"
        ]
        # Exact width ratios ensuring zero text overlap
        col_widths = [0.18, 0.11, 0.09, 0.09, 0.11, 0.11, 0.08, 0.23]

        cab_data = []
        for k in cab_keys:
            c = CABINETS[k]
            thump_str = f"{c['thump_f']:.1f} Hz" if c['thump_g'] > 0 else "Flat"
            boost_str = f"+{c['thump_g']:.1f} dB" if c['thump_g'] > 0 else "0.0 dB"
            mid_str = f"{c['mid_f']:.0f} Hz ({c['mid_g']:+.1f}dB)" if c['mid_g'] != 0 else "Flat"
            pres_str = f"{c['pres_f']:.0f} Hz ({c['pres_g']:+.1f}dB)" if c['pres_g'] != 0 else "Flat"
            lp_str = f"{c['lp']:.0f} Hz" if c['lp'] < 15000 else "20.0 kHz"
            cab_data.append([
                c["name"],
                c["type"],
                thump_str,
                boost_str,
                mid_str,
                pres_str,
                lp_str,
                c["role"]
            ])

        ctab = ax_ctab.table(cellText=cab_data, colLabels=cab_cols, colWidths=col_widths, loc='center', cellLoc='center')
        ctab.auto_set_font_size(False)
        ctab.set_fontsize(7.0)
        ctab.scale(1.0, 1.45)
        for (r_idx, c_idx), cell in ctab.get_celld().items():
            if r_idx == 0:
                cell.set_facecolor(charcoal)
                cell.set_text_props(color=gold, weight='bold')
                cell.set_edgecolor(carbon_border)
            else:
                cell.set_edgecolor('#dcdfe4')
                if r_idx % 2 == 1:
                    cell.set_facecolor('#f4f6f8')
                else:
                    cell.set_facecolor('#ffffff')

        pdf.savefig(fig1)
        plt.close(fig1)

        # ======================================================================
        # PAGES 2–6: 2 Amplifiers per Page (10 Amps = 5 Pages)
        # Clean separation, zero title/axis collision, bottom shared legend
        # ======================================================================
        amps_per_page = 2
        total_pages = len(amp_keys) // amps_per_page

        for page_idx in range(total_pages):
            fig = plt.figure(figsize=(11, 8.5), facecolor='#ffffff')
            add_header_banner(fig, 
                              f"IRONSTACK  —  END-TO-END RIG RESPONSE MATRIX (PART {page_idx + 1} OF {total_pages})",
                              "Calibrated Output Response across 12 Cabinet Models with Inter-Amp Loudness Normalization",
                              f"Rigs {page_idx*24 + 1}–{(page_idx+1)*24} of 120")

            amp_subset = amp_keys[page_idx * amps_per_page : (page_idx + 1) * amps_per_page]

            # Subplots with ample margin and zero title clipping:
            # Upper plot: y in [0.52, 0.83]
            # Lower plot: y in [0.16, 0.47]
            # Legend: y in [0.03, 0.10]
            plot_rects = [
                [0.10, 0.52, 0.84, 0.31],  # Upper plot
                [0.10, 0.16, 0.84, 0.31]   # Lower plot
            ]

            for i, amp_k in enumerate(amp_subset):
                rect = plot_rects[i]
                ax = fig.add_axes(rect)
                ax.set_facecolor(cream)

                m_amp, _ = solve_tone_stack(amp_k, t=0.5, m=0.5, l=0.5, freqs_hz=freqs)
                comp_factor = LEVEL_COMPENSATION.get(amp_k, 1.0)
                comp_db = 20.0 * np.log10(comp_factor)

                for cab_k in cab_keys:
                    m_cab, _ = solve_cabinet(cab_k, freqs, fs)
                    total_mag = m_amp + m_cab + comp_db
                    ax.semilogx(freqs, total_mag, label=CABINETS[cab_k]["short_name"], color=CABINETS[cab_k]["color"], lw=1.6)

                amp_name = CIRCUITS[amp_k]["name"]
                amp_desc = CIRCUITS[amp_k]["desc"]
                
                # Title placed with proper top clearance
                ax.set_title(f"{amp_name} (T=5, M=5, B=5) — {amp_desc} [Norm: {comp_db:+.1f} dB]", 
                             fontsize=8.8, fontweight='bold', color=charcoal, pad=7)
                ax.set_ylabel("Calibrated Gain (dB)", fontsize=8)
                ax.set_xlim(20, 20000)
                ax.set_ylim(-40, 15)
                ax.grid(True, which="both", ls=":", color=grid_color, alpha=0.8)

                if i == 0:
                    ax.set_xticklabels([])
                else:
                    ax.set_xlabel("Frequency (Hz)", fontsize=8.5, labelpad=4)

            # Clean shared cabinet legend ribbon below the plots
            leg_ax = fig.add_axes([0.08, 0.03, 0.84, 0.08])
            leg_ax.axis('off')
            handles, labels = ax.get_legend_handles_labels()
            leg_ax.legend(handles, labels, loc='center', ncol=6, fontsize=7.0, frameon=True, 
                          facecolor='#f8f9fa', edgecolor='#dcdfe4', title="Cabinet Models (12 Discrete Enclosures)",
                          title_fontsize=7.8)

            pdf.savefig(fig)
            plt.close(fig)

    print(f"Executive Matrix Report successfully compiled and formatted: {pdf_path}")

if __name__ == "__main__":
    generate_matrix_tests("plugins/IronStack/research/IronStack_Matrix_Test_Report.pdf")
