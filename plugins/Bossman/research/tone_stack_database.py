#!/usr/bin/env python3
"""
Comprehensive Tone Stack Circuit Database & Comparative Mathematical Benchmark
================================================================================
Models the 6 most iconic guitar tone stacks using exact continuous-time
polynomial nodal equations (Yeh & Smith, DAFx-06) and bilinear discretization:

1. Fender '59 Bassman 5F6-A (Tweed reference)
2. Fender Twin Reverb AB763 (Blackface standard)
3. Marshall 1959 Plexi / JCM800 (British Lead roar)
4. Vox AC30 Top Boost (British chime / brilliant treble)
5. Mesa Boogie Dual Rectifier (Modern aggressive high-gain scoop)
6. Soldano SLO-100 Super Lead Overdrive (Harmonic boutique lead)
"""

import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages

# Theme Colors
charcoal = "#1a1c1e"
gold = "#d4af37"
copper = "#c87d55"
cream = "#f4f5f7"

# ==============================================================================
# 1. CIRCUIT COMPONENT DATABASE
# ==============================================================================

CIRCUITS = {
    "Fender_59_Bassman": {
        "name": "'59 Fender Bassman (5F6-A)",
        "desc": "Original Tweed benchmark; balanced warmth, 800Hz scoop",
        "R1": 250e3,  # Treble pot (Linear)
        "R2": 1e6,    # Bass pot (10% Audio Log)
        "R3": 25e3,   # Mid pot (Linear)
        "R4": 56e3,   # Slope resistor
        "C1": 250e-12,# 250 pF
        "C2": 20e-9,  # 20 nF
        "C3": 20e-9,  # 20 nF
        "color": "#d4af37"
    },
    "Fender_Twin_Reverb": {
        "name": "Fender Twin Reverb (AB763)",
        "desc": "Classic Blackface; huge low-end authority, deeper scoop at 450Hz",
        "R1": 250e3,
        "R2": 250e3,
        "R3": 10e3,
        "R4": 100e3,  # 100k slope creates very pronounced mid scoop
        "C1": 250e-12,
        "C2": 100e-9, # 0.1 uF big bass cap
        "C3": 47e-9,  # 0.047 uF mid cap
        "color": "#3498db"
    },
    "Marshall_JCM800": {
        "name": "Marshall JCM800 / 1959 Plexi",
        "desc": "British crunch; forward midrange, higher treble bite, 33k slope",
        "R1": 220e3,
        "R2": 1e6,
        "R3": 22e3,
        "R4": 33e3,   # 33k slope lets significantly more midrange push through
        "C1": 470e-12,# 470 pF bright cap
        "C2": 22e-9,
        "C3": 22e-9,
        "color": "#e74c3c"
    },
    "Vox_AC30_TopBoost": {
        "name": "Vox AC30 Top Boost",
        "desc": "British chime; 47pF small treble cap, 100k slope, sparkling highs",
        "R1": 1e6,
        "R2": 1e6,
        "R3": 10e3,   # Fixed internal tail or pot
        "R4": 100e3,
        "C1": 47e-12, # 47 pF ultra-sparkle cap
        "C2": 22e-9,
        "C3": 10e-9,
        "color": "#2ecc71"
    },
    "Mesa_Dual_Rectifier": {
        "name": "Mesa Boogie Dual Rectifier",
        "desc": "Modern metal; 680pF treble cap, 50k mid pot for extreme scoop-to-punch",
        "R1": 250e3,
        "R2": 1e6,
        "R3": 50e3,   # 50k mid pot gives massive sweep range
        "R4": 47e3,
        "C1": 680e-12,# 680 pF massive upper-mid presence
        "C2": 20e-9,
        "C3": 20e-9,
        "color": "#9b59b6"
    },
    "Soldano_SLO_100": {
        "name": "Soldano SLO-100",
        "desc": "Boutique high-gain; legendary singing lead, smooth transition",
        "R1": 250e3,
        "R2": 1e6,
        "R3": 25e3,
        "R4": 47e3,   # 47k slope provides balanced vocal resonance
        "C1": 470e-12,
        "C2": 20e-9,
        "C3": 20e-9,
        "color": "#e67e22"
    }
}

# ==============================================================================
# 2. MATHEMATICAL SOLVER (YEH & SMITH CLOSED-FORM ALGEBRA)
# ==============================================================================

def solve_tone_stack(circuit_key, t=0.5, m=0.5, l=0.5, freqs_hz=None):
    """
    Computes exact continuous-time complex response H(s) for any given tone stack.
    """
    c = CIRCUITS[circuit_key]
    R1, R2, R3, R4 = c["R1"], c["R2"], c["R3"], c["R4"]
    C1, C2, C3 = c["C1"], c["C2"], c["C3"]
    
    m = max(float(m), 1e-5)
    l = max(float(l), 1e-5)
    t = float(t)
    
    # Denominator
    a0 = 1.0
    a1 = (C1*R1 + C1*R3 + C2*R3 + C2*R4 + C3*R4) + m*C3*R3 + l*(C1*R2 + C2*R2)
    a2 = (m*(C1*C3*R1*R3 - C2*C3*R3*R4 + C1*C3*R3**2 + C2*C3*R3**2)
          + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
          - (m**2)*(C1*C3*R3**2 + C2*C3*R3**2)
          + l*(C1*C2*R2*R4 + C1*C2*R1*R2 + C1*C3*R2*R4 + C2*C3*R2*R4)
          + (C1*C2*R1*R4 + C1*C3*R1*R4 + C1*C2*R3*R4 + C1*C2*R1*R3 + C1*C3*R3*R4 + C2*C3*R3*R4))
    a3 = (l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
          - (m**2)*(C1*C2*C3*R1*R3**2 + C1*C2*C3*R3**2*R4)
          + m*(C1*C2*C3*R3**2*R4 + C1*C2*C3*R1*R3**2 - C1*C2*C3*R1*R3*R4)
          + l*C1*C2*C3*R1*R2*R4 + C1*C2*C3*R1*R3*R4)

    # Numerator
    b1 = t*C1*R1 + m*C3*R3 + l*(C1*R2 + C2*R2) + (C1*R3 + C2*R3)
    b2 = (t*(C1*C2*R1*R4 + C1*C3*R1*R4)
          - (m**2)*(C1*C3*R3**2 + C2*C3*R3**2)
          + m*(C1*C3*R1*R3 + C1*C3*R3**2 + C2*C3*R3**2)
          + l*(C1*C2*R1*R2 + C1*C2*R2*R4 + C1*C3*R2*R4)
          + l*m*(C1*C3*R2*R3 + C2*C3*R2*R3)
          + (C1*C2*R1*R3 + C1*C2*R3*R4 + C1*C3*R3*R4))
    b3 = (l*m*(C1*C2*C3*R1*R2*R3 + C1*C2*C3*R2*R3*R4)
          - (m**2)*(C1*C2*C3*R1*R3**2 + C1*C2*C3*R3**2*R4)
          + m*(C1*C2*C3*R1*R3**2 + C1*C2*C3*R3**2*R4)
          + t*C1*C2*C3*R1*R3*R4 - t*m*C1*C2*C3*R1*R3*R4
          + t*l*C1*C2*C3*R1*R2*R4)

    s = 2j * np.pi * freqs_hz
    s2 = s * s
    s3 = s2 * s
    
    num = b1*s + b2*s2 + b3*s3
    den = a0 + a1*s + a2*s2 + a3*s3
    
    H_s = num / den
    mag_db = 20.0 * np.log10(np.maximum(np.abs(H_s), 1e-6))
    phase_deg = np.angle(H_s, deg=True)
    return mag_db, phase_deg

# ==============================================================================
# 3. GENERATE COMPARATIVE BENCHMARK REPORT
# ==============================================================================

def generate_comparative_report(pdf_path="ToneStack_Comparative_Analysis.pdf"):
    print("Generating Comparative Circuit Report for 6 Classic Amp Tone Stacks...")
    freqs = np.logspace(np.log10(10.0), np.log10(20000.0), 1000)
    
    with PdfPages(pdf_path) as pdf:
        # PAGE 1: Frequency & Notch Comparison across All 6 Amps
        fig1 = plt.figure(figsize=(11, 8.5), facecolor='#ffffff')
        
        # Header banner
        h_ax = fig1.add_axes([0.05, 0.90, 0.90, 0.07])
        h_ax.set_facecolor(charcoal)
        h_ax.set_xticks([])
        h_ax.set_yticks([])
        h_ax.text(0.02, 0.65, "CLASSIC AMP TONE STACK COMPARATIVE BENCHMARK", 
                  color='#ffffff', fontsize=13, fontweight='bold', va='center')
        h_ax.text(0.02, 0.28, "Mathematical Comparison of 6 Iconic Circuit Topologies (Fender, Marshall, Vox, Mesa, Soldano)", 
                  color=gold, fontsize=9.0, fontweight='bold', va='center')
        h_ax.text(0.98, 0.45, "Yeh & Smith DAFx-06 Nodal Synthesis", color='#99a5a3', fontsize=9, ha='right', va='center')

        # Subplot 1: Default Settings (T=0.5, M=0.5, B=0.5) - Tone Signature Comparison
        ax1 = fig1.add_axes([0.08, 0.52, 0.40, 0.33])
        ax1.set_facecolor(cream)
        for key, c in CIRCUITS.items():
            mag_db, _ = solve_tone_stack(key, t=0.5, m=0.5, l=0.5, freqs_hz=freqs)
            ax1.semilogx(freqs, mag_db, label=c["name"], color=c["color"], lw=1.8)
        ax1.set_title("Factory Default Tone Signatures (T=5, M=5, B=5)", fontsize=9.5, fontweight='bold', color=charcoal)
        ax1.set_xlabel("Frequency (Hz)", fontsize=8)
        ax1.set_ylabel("Magnitude (dB)", fontsize=8)
        ax1.set_xlim(20, 20000)
        ax1.set_ylim(-30, 2)
        ax1.grid(True, which="both", ls=":", alpha=0.6)
        ax1.legend(fontsize=7.0, loc="lower right")

        # Subplot 2: Phase Comparison
        ax2 = fig1.add_axes([0.55, 0.52, 0.40, 0.33])
        ax2.set_facecolor(cream)
        for key, c in CIRCUITS.items():
            _, phase_deg = solve_tone_stack(key, t=0.5, m=0.5, l=0.5, freqs_hz=freqs)
            ax2.semilogx(freqs, phase_deg, label=c["name"], color=c["color"], lw=1.8)
        ax2.set_title("Phase Trajectories across Tone Stacks", fontsize=9.5, fontweight='bold', color=charcoal)
        ax2.set_xlabel("Frequency (Hz)", fontsize=8)
        ax2.set_ylabel("Phase (Degrees)", fontsize=8)
        ax2.set_xlim(20, 20000)
        ax2.set_ylim(-90, 90)
        ax2.grid(True, which="both", ls=":", alpha=0.6)
        ax2.legend(fontsize=7.0, loc="lower left")

        # Subplot 3: Extreme Mid Scoop (Mid = 0.0, Treble = 1.0, Bass = 1.0)
        ax3 = fig1.add_axes([0.08, 0.10, 0.40, 0.33])
        ax3.set_facecolor(cream)
        for key, c in CIRCUITS.items():
            mag_db, _ = solve_tone_stack(key, t=1.0, m=0.0, l=1.0, freqs_hz=freqs)
            ax3.semilogx(freqs, mag_db, label=c["name"], color=c["color"], lw=1.8)
        ax3.set_title("Heavy Mid-Scoop Response (T=10, M=0, B=10)", fontsize=9.5, fontweight='bold', color=charcoal)
        ax3.set_xlabel("Frequency (Hz)", fontsize=8)
        ax3.set_ylabel("Magnitude (dB)", fontsize=8)
        ax3.set_xlim(20, 20000)
        ax3.set_ylim(-35, 5)
        ax3.grid(True, which="both", ls=":", alpha=0.6)
        ax3.legend(fontsize=7.0, loc="lower left")

        # Subplot 4: Mid Push / Solo Boost (Mid = 1.0, Treble = 0.5, Bass = 0.3)
        ax4 = fig1.add_axes([0.55, 0.10, 0.40, 0.33])
        ax4.set_facecolor(cream)
        for key, c in CIRCUITS.items():
            mag_db, _ = solve_tone_stack(key, t=0.5, m=1.0, l=0.3, freqs_hz=freqs)
            ax4.semilogx(freqs, mag_db, label=c["name"], color=c["color"], lw=1.8)
        ax4.set_title("Mid Push / Solo Lead Setting (T=5, M=10, B=3)", fontsize=9.5, fontweight='bold', color=charcoal)
        ax4.set_xlabel("Frequency (Hz)", fontsize=8)
        ax4.set_ylabel("Magnitude (dB)", fontsize=8)
        ax4.set_xlim(20, 20000)
        ax4.set_ylim(-20, 2)
        ax4.grid(True, which="both", ls=":", alpha=0.6)
        ax4.legend(fontsize=7.0, loc="lower right")

        pdf.savefig(fig1)
        plt.close(fig1)

        # PAGE 2: Circuit Component Matrix & Acoustic Feature Analysis
        fig2 = plt.figure(figsize=(11, 8.5), facecolor='#ffffff')
        
        # Header banner
        h2_ax = fig2.add_axes([0.05, 0.90, 0.90, 0.07])
        h2_ax.set_facecolor(charcoal)
        h2_ax.set_xticks([])
        h2_ax.set_yticks([])
        h2_ax.text(0.02, 0.65, "CIRCUIT COMPONENT SPECIFICATIONS & ACOUSTIC MAPPING", 
                   color='#ffffff', fontsize=13, fontweight='bold', va='center')
        h2_ax.text(0.02, 0.28, "Mathematical Component Values and Characteristic Notch Frequencies", 
                   color=gold, fontsize=9.0, fontweight='bold', va='center')
        h2_ax.text(0.98, 0.45, "Ready for C++ Modular Implementation", color='#2ecc71', fontsize=9, fontweight='bold', ha='right', va='center')

        # Table: Component Values
        table_cols = ["Amplifier Circuit", "R1 (Treb)", "R2 (Bass)", "R3 (Mid)", "R4 (Slope)", "C1 (Treb)", "C2 (Bass)", "C3 (Mid)", "Notch Freq", "Acoustic Personality"]
        table_rows = []
        for key, c in CIRCUITS.items():
            # Find notch frequency at default
            mag, _ = solve_tone_stack(key, 0.5, 0.5, 0.5, freqs)
            notch_idx = np.argmin(mag[(freqs > 200) & (freqs < 2000)])
            notch_f = freqs[(freqs > 200) & (freqs < 2000)][notch_idx]
            
            table_rows.append([
                c["name"],
                f"{int(c['R1']/1000)}k",
                f"{c['R2']/1e6:.1f}M" if c['R2']>=1e6 else f"{int(c['R2']/1000)}k",
                f"{int(c['R3']/1000)}k",
                f"{int(c['R4']/1000)}k",
                f"{int(c['C1']*1e12)} pF",
                f"{int(c['C2']*1e9)} nF",
                f"{int(c['C3']*1e9)} nF",
                f"{notch_f:.0f} Hz",
                c["desc"]
            ])

        ax_tab = fig2.add_axes([0.05, 0.25, 0.90, 0.60])
        ax_tab.axis('tight')
        ax_tab.axis('off')
        tab = ax_tab.table(cellText=table_rows, colLabels=table_cols, loc='center', cellLoc='center')
        tab.auto_set_font_size(False)
        tab.set_fontsize(7.8)
        tab.scale(1.0, 2.2)
        for (row, col), cell in tab.get_celld().items():
            if row == 0:
                cell.set_facecolor(charcoal)
                cell.set_text_props(color='#ffffff', weight='bold')
            elif row % 2 == 1:
                cell.set_facecolor('#f4f6f7')

        pdf.savefig(fig2)
        plt.close(fig2)

    print(f"Comparative report successfully created at: {pdf_path}")

if __name__ == "__main__":
    generate_comparative_report("plugins/Bossman/research/ToneStack_Comparative_Analysis.pdf")
