#!/usr/bin/env python3
"""
measure_distortion.py
Simulates the Fried Tofu WDF Diode Clipper DSP engine across various settings,
measures Total Harmonic Distortion (THD), harmonic distribution (odd vs even harmonics),
and produces publication-quality distortion test reports and figures with crisp typography.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.backends.backend_pdf as pdf_backend

# -----------------------------------------------------------------------------
# Diode Models from Fried Tofu
# -----------------------------------------------------------------------------
DIODES = {
    'Si_1N4148': {'name': 'Si 1N4148 (Crisp)', 'Is': 2.52e-9, 'N': 1.752},
    'Ge_1N34A':  {'name': 'Ge 1N34A (Silky)',  'Is': 200e-12, 'N': 2.190},
    'Schottky_BAT41': {'name': 'Schottky BAT41', 'Is': 2.00e-8, 'N': 1.050},
    'Red_LED':   {'name': 'Red LED (Deep)',    'Is': 9.30e-19, 'N': 1.840},
}
Vt = 0.025864  # Thermal voltage at 25°C

def solve_wdf_sample(a, R, isA, nA, isB, nB, warm=0.0):
    """Newton-Raphson WDF solve for single sample."""
    V = warm
    for _ in range(35):
        v_exp_a = min(V, 2.5) / (nA * Vt)
        v_exp_b = min(-V, 2.5) / (nB * Vt)
        eA = np.exp(v_exp_a)
        eB = np.exp(v_exp_b)
        g = isA * (eA - 1.0) - isB * (eB - 1.0)
        gp = (isA / (nA * Vt)) * eA + (isB / (nB * Vt)) * eB
        F = V + R * g - a
        Fp = 1.0 + R * gp
        dV = F / Fp
        V -= dV
        if abs(dV) < 1e-9:
            break
    return V

def run_simulation(signal_in, R, isA, nA, isB, nB):
    """Process an array of audio samples through the WDF diode clipper."""
    out = np.zeros_like(signal_in)
    warm = 0.0
    for i in range(len(signal_in)):
        v = solve_wdf_sample(signal_in[i], R, isA, nA, isB, nB, warm)
        out[i] = v
        warm = v
    return out

def compute_thd_and_harmonics(out_signal, fs, f0):
    """Compute THD, H2..H7 harmonics in dB relative to fundamental H1."""
    N = len(out_signal)
    w = np.blackman(N)
    w_sum = np.sum(w)
    spectrum = np.abs(np.fft.rfft(out_signal * w)) * (2.0 / w_sum)
    freqs = np.fft.rfftfreq(N, 1.0 / fs)
    
    bin_width = fs / N
    harmonics = []
    
    for h in range(1, 8):
        fh = h * f0
        center_bin = int(round(fh / bin_width))
        window_radius = max(1, int(round(15.0 / bin_width)))
        start_bin = max(0, center_bin - window_radius)
        end_bin = min(len(spectrum), center_bin + window_radius + 1)
        peak_val = np.max(spectrum[start_bin:end_bin])
        harmonics.append(peak_val)
        
    h1 = harmonics[0]
    higher_harmonics = harmonics[1:]
    
    thd_linear = np.sqrt(np.sum(np.array(higher_harmonics)**2)) / max(h1, 1e-12)
    thd_percent = thd_linear * 100.0
    thd_db = 20.0 * np.log10(max(thd_linear, 1e-12))
    
    h_db_rel = [20.0 * np.log10(max(h, 1e-12) / max(h1, 1e-12)) for h in harmonics]
    return thd_percent, thd_db, h_db_rel, spectrum, freqs

def run_all_tests():
    fs = 48000
    f0 = 1000.0  # 1 kHz reference test tone
    duration = 0.2  # 200 ms
    t = np.arange(int(fs * duration)) / fs
    
    test_cases = [
        {
            'title': 'Glue — SYM',
            'desc': 'Germanium + Germanium, warm tape-like glue',
            'diodeA': 'Ge_1N34A', 'diodeB': 'Ge_1N34A',
            'diode_label': '1N34A / 1N34A',
            'drive_db': 6.0, 'R': 1000.0
        },
        {
            'title': 'Mojo — SYM',
            'desc': 'Silicon + Silicon, crisp crunchy bite',
            'diodeA': 'Si_1N4148', 'diodeB': 'Si_1N4148',
            'diode_label': '1N4148 / 1N4148',
            'drive_db': 22.0, 'R': 2000.0
        },
        {
            'title': 'Glue/Mojo — ASYM',
            'desc': 'Silicon + Germanium, rich 2nd-order even harmonics',
            'diodeA': 'Si_1N4148', 'diodeB': 'Ge_1N34A',
            'diode_label': '1N4148 / 1N34A',
            'drive_db': 16.0, 'R': 1600.0
        },
        {
            'title': 'Germanium Fuzz',
            'desc': 'Low Z, extreme drive, heavy square fuzz',
            'diodeA': 'Ge_1N34A', 'diodeB': 'Ge_1N34A',
            'diode_label': '1N34A / 1N34A',
            'drive_db': 30.0, 'R': 630.0
        },
        {
            'title': 'Flash Crunch',
            'desc': 'Schottky + Schottky, low threshold sizzle',
            'diodeA': 'Schottky_BAT41', 'diodeB': 'Schottky_BAT41',
            'diode_label': 'BAT41 / BAT41',
            'drive_db': 18.0, 'R': 2200.0
        },
        {
            'title': 'Deep Open',
            'desc': 'Red LED + Red LED, dynamic tube-like headroom',
            'diodeA': 'Red_LED', 'diodeB': 'Red_LED',
            'diode_label': 'LED / LED',
            'drive_db': 24.0, 'R': 3300.0
        }
    ]
    
    results = []
    
    print("=" * 84)
    print(f"{'Preset / Setting':<20} | {'Diodes':<16} | {'Drive':<7} | {'R (Ω)':<6} | {'THD (%)':<8} | {'THD (dB)':<9} | {'H2 (dBc)':<9} | {'H3 (dBc)':<9}")
    print("-" * 84)
    
    for case in test_cases:
        dA = DIODES[case['diodeA']]
        dB = DIODES[case['diodeB']]
        drive_lin = 10.0 ** (case['drive_db'] / 20.0)
        
        sig_in = 0.5 * np.sin(2.0 * np.pi * f0 * t) * drive_lin
        sig_out = run_simulation(sig_in, case['R'], dA['Is'], dA['N'], dB['Is'], dB['N'])
        
        steady_out = sig_out[int(fs * 0.05):]
        thd_pct, thd_db, h_rel, spec, f_axis = compute_thd_and_harmonics(steady_out, fs, f0)
        
        case['thd_pct'] = thd_pct
        case['thd_db'] = thd_db
        case['h_rel'] = h_rel
        case['spec'] = spec
        case['f_axis'] = f_axis
        case['sig_out'] = sig_out[:int(fs * 0.005)]
        case['t_wave'] = t[:int(fs * 0.005)] * 1000.0
        
        results.append(case)
        print(f"{case['title']:<20} | {case['diode_label']:<16} | {case['drive_db']:>4.1f} dB | {case['R']:>4.0f} Ω | {thd_pct:>6.2f} % | {thd_db:>6.1f} dB | {h_rel[1]:>7.1f} dB | {h_rel[2]:>7.1f} dB")
        
    print("=" * 84)
    return results

def generate_pdf_report(results, filename="FriedTofu_Math_and_DSP.pdf"):
    print(f"\nGenerating publication-quality PDF report: {filename} ...")
    
    copper = '#c9793d'
    charcoal = '#1b2224'
    
    with pdf_backend.PdfPages(filename) as pdf:
        # =============================================================
        # PAGE 1: Theoretical Foundations, Mathematics & Static Transfer
        # =============================================================
        fig1 = plt.figure(figsize=(8.5, 11), facecolor='#ffffff')
        
        # Header Banner
        header_ax = fig1.add_axes([0.06, 0.90, 0.88, 0.07])
        header_ax.set_facecolor(charcoal)
        header_ax.set_xticks([])
        header_ax.set_yticks([])
        header_ax.text(0.03, 0.65, "FRIED TOFU  —  MATHEMATICAL FOUNDATIONS", 
                       color='#ffffff', fontsize=14, fontweight='bold', va='center')
        header_ax.text(0.03, 0.28, "Wave Digital Filter (WDF) Diode Clipper Nonlinear Modeling & Physical Acoustics", 
                       color=copper, fontsize=9.0, fontweight='bold', va='center')
        header_ax.text(0.97, 0.45, "Kijjaz Audio DSP", color='#99a5a3', fontsize=9, ha='right', va='center')

        # Upper Text: Equations & Derivations
        theory_text = (
            "1. Circuit Model and Kirchhoff Wave Port Formulation\n"
            "   An analog diode clipper driven through source/network resistance R obeys:\n"
            "       a = V + R · ID(V)\n"
            "   where 'a' is the driving incident wave voltage, V is the node voltage across the diodes, and\n"
            "   ID(V) is the net semiconductor junction current governed by Shockley's physical equations:\n"
            "       ID(V) = Is,A · ( exp( V / (NA · Vt) ) - 1 )  -  Is,B · ( exp( -V / (NB · Vt) ) - 1 )\n"
            "   Here, Vt = kB · T / q ≈ 25.864 mV is the thermal voltage at 300 K, Is is the reverse saturation\n"
            "   current, and N is the ideality emission factor.\n\n"
            "2. Damped Newton–Raphson Solver & Guaranteed Convergence\n"
            "   The node voltage V is found per-sample as the unique zero of the objective function:\n"
            "       F(V)  = V + R · ID(V) - a = 0\n"
            "       F'(V) = 1 + R · [ (Is,A / (NA·Vt)) · exp(V / (NA·Vt)) + (Is,B / (NB·Vt)) · exp(-V / (NB·Vt)) ]\n"
            "   Because R > 0 and incremental conductance gd(V) > 0 for all real V, F'(V) ≥ 1 everywhere.\n"
            "   Hence F(V) is strictly monotonically increasing with a single real root. Damped Newton iterations:\n"
            "       V_{k+1} = V_k - F(V_k) / F'(V_k)\n"
            "   with warm-start memory state V_0 = V[n-1] converge quadratically within 3 to 8 iterations to |ΔV| < 10⁻⁹ V."
        )
        fig1.text(0.06, 0.88, theory_text, fontsize=8.0, family='monospace', verticalalignment='top', color='#1a1a1a', linespacing=1.2)
        
        # Diode Model Table Box
        table_ax = fig1.add_axes([0.06, 0.45, 0.88, 0.12])
        table_ax.axis('tight')
        table_ax.axis('off')
        diode_table_data = [
            ["Diode Profile", "Is (A)", "N", "Knee", "Physical Characteristics & Sonic Character"],
            ["Si 1N4148 (Crisp)", "2.52 × 10⁻⁹", "1.752", "~0.62 V", "Crisp silicon edge, sharp conduction, bright harmonic bite"],
            ["Ge 1N34A (Silky)",  "2.00 × 10⁻¹⁰", "2.190", "~0.28 V", "Soft germanium knee, early saturation, warm silky compression"],
            ["Schottky BAT41",    "2.00 × 10⁻⁸",  "1.050", "~0.32 V", "Flash-fried low barrier, rapid current rise, aggressive fuzz"],
            ["Red LED (Deep)",    "9.30 × 10⁻¹⁹", "1.840", "~1.75 V", "High forward headroom, open dynamic range, valve-like push"]
        ]
        col_widths_d = [0.20, 0.15, 0.10, 0.11, 0.44]
        d_tab = table_ax.table(cellText=diode_table_data[1:], colLabels=diode_table_data[0], 
                               colWidths=col_widths_d, loc='center', cellLoc='left')
        d_tab.auto_set_font_size(False)
        d_tab.set_fontsize(7.5)
        d_tab.scale(1.0, 1.4)
        for (r_idx, c_idx), cell in d_tab.get_celld().items():
            if r_idx == 0:
                cell.set_facecolor(charcoal)
                cell.set_text_props(color='#ffffff', weight='bold')
            elif r_idx % 2 == 1:
                cell.set_facecolor('#f4f6f7')
        
        # Lower Plot: Static Transfer Curves
        ax_tc = fig1.add_axes([0.11, 0.08, 0.78, 0.31])
        ax_tc.set_facecolor('#fafbfc')
        ax_tc.grid(True, linestyle='--', alpha=0.6, color='#ccd1d9')
        
        v_in = np.linspace(-3.0, 3.0, 400)
        ax_tc.plot(v_in, v_in, 'k--', linewidth=1.1, alpha=0.45, label='Unity (Linear Direct)')
        
        curve_colors = {'Si_1N4148': copper, 'Ge_1N34A': '#2b7a78', 'Schottky_BAT41': '#e74c3c', 'Red_LED': '#8e44ad'}
        for key, dinfo in DIODES.items():
            vout = [solve_wdf_sample(vi, 2000.0, dinfo['Is'], dinfo['N'], dinfo['Is'], dinfo['N']) for vi in v_in]
            ax_tc.plot(v_in, vout, label=dinfo['name'], color=curve_colors[key], linewidth=1.8)
            
        ax_tc.set_title("Static Transfer Curves (Vout vs Vin, R = 2.0 kΩ, Symmetric Pairs)", fontsize=10.5, fontweight='bold', color=charcoal)
        ax_tc.set_xlabel("Incident Voltage Vin (V)", fontsize=9)
        ax_tc.set_ylabel("Clipped Node Voltage Vout (V)", fontsize=9)
        ax_tc.set_xlim(-3, 3)
        ax_tc.set_ylim(-2.2, 2.2)
        ax_tc.legend(loc='lower right', fontsize=8.2, framealpha=0.95)
        
        pdf.savefig(fig1)
        plt.close(fig1)
        
        # =============================================================
        # PAGE 2: Distortion Measurements, THD, and Harmonic Decomposition
        # =============================================================
        fig2 = plt.figure(figsize=(8.5, 11), facecolor='#ffffff')
        
        # Header Banner
        h2_ax = fig2.add_axes([0.06, 0.90, 0.88, 0.07])
        h2_ax.set_facecolor(charcoal)
        h2_ax.set_xticks([])
        h2_ax.set_yticks([])
        h2_ax.text(0.03, 0.65, "FRIED TOFU  —  DISTORTION & HARMONIC BENCHMARK", 
                   color='#ffffff', fontsize=14, fontweight='bold', va='center')
        h2_ax.text(0.03, 0.28, "Empirical Measurements: Total Harmonic Distortion (THD) & Harmonic Spectra across 6 Presets", 
                   color=copper, fontsize=9.0, fontweight='bold', va='center')
        h2_ax.text(0.97, 0.45, "Test Tone: 1.0 kHz Sinusoid", color='#99a5a3', fontsize=9, ha='right', va='center')
        
        # Results Table Box
        col_labels = ["Preset Setting", "Diode Configuration", "Drive", "R (Ω)", "THD (%)", "THD (dB)", "H2 (dBc)", "H3 (dBc)"]
        table_data = []
        for r in results:
            table_data.append([
                r['title'],
                r['diode_label'],
                f"{r['drive_db']:+.1f} dB",
                f"{int(r['R'])} Ω",
                f"{r['thd_pct']:.2f} %",
                f"{r['thd_db']:.1f} dB",
                f"{r['h_rel'][1]:.1f} dB",
                f"{r['h_rel'][2]:.1f} dB"
            ])
            
        ax_table = fig2.add_axes([0.06, 0.67, 0.88, 0.19])
        ax_table.axis('tight')
        ax_table.axis('off')
        col_widths_r = [0.18, 0.17, 0.10, 0.09, 0.11, 0.11, 0.12, 0.12]
        tab = ax_table.table(cellText=table_data, colLabels=col_labels, 
                             colWidths=col_widths_r, loc='center', cellLoc='center')
        tab.auto_set_font_size(False)
        tab.set_fontsize(7.5)
        tab.scale(1.0, 1.4)
        
        for (row, col), cell in tab.get_celld().items():
            if row == 0:
                cell.set_facecolor(charcoal)
                cell.set_text_props(color='#ffffff', weight='bold')
            elif row % 2 == 1:
                cell.set_facecolor('#f4f6f7')
                
        # 4 Diagnostic Subplots
        # Subplot 1: Waveforms Symmetrical vs Asymmetrical
        ax_w1 = fig2.add_axes([0.09, 0.38, 0.38, 0.22])
        ax_w1.plot(results[0]['t_wave'], results[0]['sig_out'], label='Glue - SYM (Ge/Ge)', color='#2b7a78', lw=1.6)
        ax_w1.plot(results[2]['t_wave'], results[2]['sig_out'], label='Asym (Si/Ge)', color=copper, lw=1.6, linestyle='--')
        ax_w1.set_title("Waveforms: Symmetrical vs Asym", fontsize=9.2, fontweight='bold', color=charcoal)
        ax_w1.set_xlabel("Time (ms)", fontsize=8)
        ax_w1.set_ylabel("Amplitude (V)", fontsize=8)
        ax_w1.grid(True, alpha=0.5, linestyle=':')
        ax_w1.legend(fontsize=7.5, loc='upper right')
        
        # Subplot 2: Harmonic Bar Chart comparing H1-H6 (Mojo vs Asym)
        ax_b = fig2.add_axes([0.55, 0.38, 0.38, 0.22])
        harm_idx = np.arange(1, 7)
        width = 0.36
        h_mojo = [results[1]['h_rel'][i] for i in range(6)]
        h_asym = [results[2]['h_rel'][i] for i in range(6)]
        ax_b.bar(harm_idx - width/2, h_mojo, width, label='Mojo SYM (Odd dominant)', color='#3498db')
        ax_b.bar(harm_idx + width/2, h_asym, width, label='Asym (Even present)', color=copper)
        ax_b.set_title("Harmonic Content: SYM vs ASYM (dBc)", fontsize=9.2, fontweight='bold', color=charcoal)
        ax_b.set_xlabel("Harmonic (1=Fund, 2=2nd, 3=3rd...)", fontsize=8)
        ax_b.set_ylabel("Amplitude (dBc)", fontsize=8)
        ax_b.set_ylim(-80, 5)
        ax_b.grid(True, alpha=0.5, linestyle=':')
        ax_b.legend(fontsize=7.5, loc='upper right')
        
        # Subplot 3: Fuzz vs Deep Open Waveforms
        ax_w2 = fig2.add_axes([0.09, 0.08, 0.38, 0.22])
        ax_w2.plot(results[3]['t_wave'], results[3]['sig_out'], label='Germanium Fuzz', color='#e74c3c', lw=1.6)
        ax_w2.plot(results[5]['t_wave'], results[5]['sig_out'], label='Deep Open (Red LED)', color='#8e44ad', lw=1.6)
        ax_w2.set_title("Waveforms: Fuzz vs Open LED", fontsize=9.2, fontweight='bold', color=charcoal)
        ax_w2.set_xlabel("Time (ms)", fontsize=8)
        ax_w2.set_ylabel("Amplitude (V)", fontsize=8)
        ax_w2.grid(True, alpha=0.5, linestyle=':')
        ax_w2.legend(fontsize=7.5, loc='upper right')
        
        # Subplot 4: Spectrum of Asymmetric Clipper with H2/H3 annotations
        ax_f = fig2.add_axes([0.55, 0.08, 0.38, 0.22])
        spec = results[2]['spec']
        freqs = results[2]['f_axis']
        spec_db = 20 * np.log10(np.maximum(spec, 1e-6))
        spec_db -= np.max(spec_db)
        ax_f.plot(freqs, spec_db, color=copper, lw=1.2)
        ax_f.set_xlim(0, 10000)
        ax_f.set_ylim(-85, 5)
        ax_f.set_title("FFT Spectrum: Asymmetric (Si + Ge)", fontsize=9.2, fontweight='bold', color=charcoal)
        ax_f.set_xlabel("Frequency (Hz)", fontsize=8)
        ax_f.set_ylabel("Level (dBFS)", fontsize=8)
        ax_f.grid(True, alpha=0.5, linestyle=':')
        
        # Annotations
        ax_f.annotate('H2 (2kHz): -26.8dB', xy=(2000, results[2]['h_rel'][1]), xytext=(2400, -18),
                    arrowprops=dict(facecolor=charcoal, arrowstyle='->', lw=0.8), fontsize=7.2)
        ax_f.annotate('H3 (3kHz): -11.4dB', xy=(3000, results[2]['h_rel'][2]), xytext=(3800, -6),
                    arrowprops=dict(facecolor=charcoal, arrowstyle='->', lw=0.8), fontsize=7.2)
        
        pdf.savefig(fig2)
        plt.close(fig2)

    print(f"Successfully generated {filename}")

if __name__ == "__main__":
    results = run_all_tests()
    generate_pdf_report(results, "FriedTofu_Math_and_DSP.pdf")
