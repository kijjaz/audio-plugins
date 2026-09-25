import numpy as np
import matplotlib.pyplot as plt
import os

# Carbon & Gold Design System Styling
plt.style.use('dark_background')
fig_bg = '#121316'
panel_bg = '#1a1c21'
gold_accent = '#e0b958'
radar_cyan = '#40e0d0'
radar_amber = '#ffaf28'
text_muted = '#969ba8'

fs = 48000
freqs = np.logspace(np.log10(200), np.log10(18000), 1200)
w = 2.0 * np.pi * freqs / fs
z = np.exp(-1j * w)

artifact_dir = "/Users/kijjaz/.gemini/antigravity-ide/brain/845e1834-6222-413e-882b-d45879521804"
os.makedirs(artifact_dir, exist_ok=True)

# --- Complex Biquad Transfer Functions H(z) ---
def biquad_peaking_complex(f0, gain_db, q, freqs, fs):
    A = 10.0 ** (gain_db / 40.0)
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) / (2.0 * q)
    cos_w0 = np.cos(w0)

    b0 = 1.0 + alpha * A
    b1 = -2.0 * cos_w0
    b2 = 1.0 - alpha * A
    a0 = 1.0 + alpha / A
    a1 = -2.0 * cos_w0
    a2 = 1.0 - alpha / A

    b0 /= a0; b1 /= a0; b2 /= a0; a1 /= a0; a2 /= a0
    z = np.exp(-1j * 2 * np.pi * freqs / fs)
    return (b0 + b1 * z + b2 * (z**2)) / (1.0 + a1 * z + a2 * (z**2))

def biquad_highshelf_complex(f0, gain_db, freqs, fs):
    A = 10.0 ** (gain_db / 40.0)
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) * 0.5 * np.sqrt(2.0)
    cos_w0 = np.cos(w0)

    a0 = (A + 1.0) - (A - 1.0) * cos_w0 + 2.0 * np.sqrt(A) * alpha
    b0 = (A * ((A + 1.0) + (A - 1.0) * cos_w0 + 2.0 * np.sqrt(A) * alpha)) / a0
    b1 = (-2.0 * A * ((A - 1.0) + (A + 1.0) * cos_w0)) / a0
    b2 = (A * ((A + 1.0) + (A - 1.0) * cos_w0 - 2.0 * np.sqrt(A) * alpha)) / a0
    a1 = (2.0 * ((A - 1.0) - (A + 1.0) * cos_w0)) / a0
    a2 = ((A + 1.0) - (A - 1.0) * cos_w0 - 2.0 * np.sqrt(A) * alpha) / a0

    z = np.exp(-1j * 2 * np.pi * freqs / fs)
    return (b0 + b1 * z + b2 * (z**2)) / (1.0 + a1 * z + a2 * (z**2))

def compute_elevation_complex_response(el_deg):
    el_clamped = np.clip(el_deg, -50.0, 90.0)
    norm_el = (el_clamped + 50.0) / 140.0

    # N1 Primary Pinna Notch
    n1_freq = 6000.0 + norm_el * 5200.0
    H_n1 = biquad_peaking_complex(n1_freq, -18.0, 5.0, freqs, fs)

    # N2 Secondary Notch
    overhead_factor = np.clip(el_clamped / 90.0, 0.0, 1.0)
    n2_freq = 9800.0 + overhead_factor * 3200.0
    H_n2 = biquad_peaking_complex(n2_freq, -14.0 * overhead_factor, 6.5, freqs, fs)

    # Overhead Presence Shelf
    H_shelf = biquad_highshelf_complex(8500.0, overhead_factor * 3.5, freqs, fs)

    # Torso reflection
    below_factor = np.clip(-el_clamped / 50.0, 0.0, 1.0)
    torso_freq = 1200.0 - below_factor * 350.0
    H_torso = biquad_peaking_complex(torso_freq, -8.0 * below_factor, 2.8, freqs, fs)

    return H_n1 * H_n2 * H_shelf * H_torso

# ==============================================================================
# FIGURE 1: BODE PLOT (Magnitude & Phase Response across Elevations)
# ==============================================================================
fig, (ax_mag, ax_phase) = plt.subplots(2, 1, figsize=(11, 8.5), facecolor=fig_bg, sharex=True)
ax_mag.set_facecolor(panel_bg)
ax_phase.set_facecolor(panel_bg)

elevations = [-40, -20, 0, 25, 50, 75, 90]
colors = plt.cm.plasma(np.linspace(0.15, 0.95, len(elevations)))

for el, color in zip(elevations, colors):
    H = compute_elevation_complex_response(el)
    mag_db = 20.0 * np.log10(np.maximum(np.abs(H), 1e-4))
    
    # Phase response unwrapped in degrees
    phase_deg = np.rad2deg(np.unwrap(np.angle(H)))

    lbl = f"{el:+3d}°"
    if el == 90: lbl += " (Zenith Overhead)"
    elif el == 0: lbl += " (Horizon Eye-Level)"
    elif el == -40: lbl += " (Below Chin / Torso)"

    ax_mag.plot(freqs, mag_db, label=lbl, color=color, linewidth=2.0)
    ax_phase.plot(freqs, phase_deg, label=lbl, color=color, linewidth=2.0)

ax_mag.set_xscale('log')
ax_mag.set_xlim(300, 17000)
ax_mag.set_ylim(-26, 6)
ax_mag.set_ylabel("Magnitude (dB)", fontsize=11, color='white')
ax_mag.set_title("FlyBy: Vertical Elevation Bode Plot (Magnitude & Phase Response)", fontsize=13, color=gold_accent, fontweight='bold', pad=10)
ax_mag.grid(True, which='both', color='#2d3038', linestyle=':', linewidth=0.8)
ax_mag.legend(loc='lower left', facecolor=panel_bg, edgecolor=gold_accent, fontsize=8.5, ncol=2)

ax_phase.set_xscale('log')
ax_phase.set_xlim(300, 17000)
ax_phase.set_xlabel("Frequency (Hz)", fontsize=11, color='white')
ax_phase.set_ylabel("Phase (degrees)", fontsize=11, color='white')
ax_phase.grid(True, which='both', color='#2d3038', linestyle=':', linewidth=0.8)

# Annotate phase shifts at notch frequencies
ax_phase.annotate('Sharp phase dispersion\nat moving pinna notch', xy=(8500, -35), xytext=(2200, -75),
                  arrowprops=dict(facecolor=radar_cyan, edgecolor='none', shrink=0.08, width=1.5, headwidth=6),
                  color=radar_cyan, fontsize=10, fontweight='bold')

plt.tight_layout()
fig1_file = "spectrum_elevation_bode.png"
fig.savefig(fig1_file, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig1_file), dpi=200)
plt.close()

# ==============================================================================
# FIGURE 2: Group Delay (tau_g = -dphi/domega) Across Elevation
# ==============================================================================
fig, ax = plt.subplots(figsize=(11, 5.5), facecolor=fig_bg)
ax.set_facecolor(panel_bg)

for el, color in zip(elevations, colors):
    H = compute_elevation_complex_response(el)
    unwrapped_phase = np.unwrap(np.angle(H))
    
    # Group delay tau_g = -dphi / domega in microseconds
    d_phi = np.gradient(unwrapped_phase)
    d_w = np.gradient(2.0 * np.pi * freqs)
    group_delay_us = (-d_phi / d_w) * 1e6

    lbl = f"{el:+3d}°"
    if el == 90: lbl += " (Overhead)"
    elif el == 0: lbl += " (Horizon)"
    elif el == -40: lbl += " (Below)"
    ax.plot(freqs, group_delay_us, label=lbl, color=color, linewidth=2.0)

ax.set_xscale('log')
ax.set_xlim(500, 16000)
ax.set_ylim(-150, 150)
ax.set_title("FlyBy: Group Delay Dispersion vs. Elevation (Micro-Time Shifts by Pinna)", fontsize=13, color=gold_accent, fontweight='bold', pad=12)
ax.set_xlabel("Frequency (Hz)", fontsize=11, color='white')
ax.set_ylabel("Group Delay (microseconds)", fontsize=11, color='white')
ax.grid(True, which='both', color='#2d3038', linestyle=':', linewidth=0.8)
ax.axhline(0, color=text_muted, linestyle='--', alpha=0.4)
ax.legend(loc='upper right', facecolor=panel_bg, edgecolor=gold_accent, fontsize=8.5, ncol=2)

ax.annotate('Pinna cavity resonance & cancellation\ncauses localized ±60 µs group delay spikes',
            xy=(8200, 55), xytext=(1200, 90),
            arrowprops=dict(facecolor=radar_amber, edgecolor='none', shrink=0.08, width=1.5, headwidth=6),
            color=radar_amber, fontsize=10, fontweight='bold')

plt.tight_layout()
fig2_file = "spectrum_elevation_group_delay.png"
fig.savefig(fig2_file, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig2_file), dpi=200)
plt.close()

# ==============================================================================
# FIGURE 3: Interaural Phase Difference (IPD): Horizontal vs. Vertical
# ==============================================================================
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5), facecolor=fig_bg)
ax1.set_facecolor(panel_bg)
ax2.set_facecolor(panel_bg)

# Horizontal IPD (Left vs Right ear phase difference: Delta_phi = omega * ITD)
az_list = [-90, -45, 0, 45, 90]
az_colors = [radar_cyan, '#7fffd4', '#ffffff', '#ffd700', radar_amber]

head_r = 0.0875
c = 343.0

for az, col in zip(az_list, az_colors):
    az_rad = az * np.pi / 180.0
    itd_s = (head_r / c) * (np.sin(np.abs(az_rad)) + np.abs(az_rad)) * np.sign(az_rad)
    # Phase difference Delta_phi = w * itd
    ipd_rad = 2.0 * np.pi * freqs * itd_s
    # Wrap to [-pi, pi]
    ipd_wrapped = np.angle(np.exp(1j * ipd_rad))
    ax1.plot(freqs, np.rad2deg(ipd_wrapped), label=f"Azimuth {az:+3d}°", color=col, linewidth=2.0)

ax1.set_xscale('log')
ax1.set_xlim(200, 4000) # IPD is biologically dominant below 1.5 - 2 kHz
ax1.set_ylim(-190, 190)
ax1.set_title("Horizontal Panning: Interaural Phase Difference", fontsize=11, color=gold_accent, fontweight='bold')
ax1.set_xlabel("Frequency (Hz)", fontsize=10, color='white')
ax1.set_ylabel("IPD (degrees)", fontsize=10, color='white')
ax1.grid(True, color='#2d3038', linestyle=':')
ax1.legend(facecolor=panel_bg, edgecolor=gold_accent, fontsize=8.5)

# Vertical IPD (Median Plane: elevation -40 to +90)
for el, col in zip(elevations[::2], colors[::2]):
    # Since left and right ears are structurally symmetrical in distance, IPD = 0!
    ipd_vert = np.zeros_like(freqs)
    ax2.plot(freqs, ipd_vert, label=f"Elevation {el:+3d}°", color=col, linewidth=2.5)

ax2.set_xscale('log')
ax2.set_xlim(200, 4000)
ax2.set_ylim(-190, 190)
ax2.set_title("Vertical Panning: Interaural Phase Difference", fontsize=11, color=gold_accent, fontweight='bold')
ax2.set_xlabel("Frequency (Hz)", fontsize=10, color='white')
ax2.set_ylabel("IPD (degrees)", fontsize=10, color='white')
ax2.grid(True, color='#2d3038', linestyle=':')
ax2.legend(facecolor=panel_bg, edgecolor=gold_accent, fontsize=8.5)

ax2.annotate('IPD is IDENTICALLY ZERO at all frequencies\nbecause both ears are equidistant on median plane!\nVertical localization uses Monaural Pinna Phase.',
            xy=(1000, 0), xytext=(240, 70),
            arrowprops=dict(facecolor=gold_accent, edgecolor='none', shrink=0.08, width=1.5, headwidth=6),
            color=gold_accent, fontsize=9.5, fontweight='bold')

plt.suptitle("Interaural Phase Comparison: Horizontal (Binaural IPD) vs Vertical (Monaural Only)",
             fontsize=13, color=gold_accent, fontweight='bold', y=0.98)
plt.tight_layout()
fig3_file = "spectrum_ipd_comparison.png"
fig.savefig(fig3_file, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig3_file), dpi=200)
plt.close()

# ==============================================================================
# FIGURE 4: DUAL WATERFALL (Magnitude Waterfall + Phase Waterfall)
# ==============================================================================
fig, (ax_wmag, ax_wphase) = plt.subplots(1, 2, figsize=(13, 5.5), facecolor=fig_bg)
ax_wmag.set_facecolor(panel_bg)
ax_wphase.set_facecolor(panel_bg)

el_grid = np.linspace(-40, 90, 150)
freq_grid = np.logspace(np.log10(1000), np.log10(16000), 250)

w_mag_matrix = np.zeros((len(el_grid), len(freq_grid)))
w_phase_matrix = np.zeros((len(el_grid), len(freq_grid)))

for i, el in enumerate(el_grid):
    H = compute_elevation_complex_response(el)
    mag_db = 20.0 * np.log10(np.maximum(np.abs(H), 1e-4))
    phase_deg = np.rad2deg(np.angle(H))
    
    # Interpolate onto grid
    w_mag_matrix[i, :] = np.interp(freq_grid, freqs, mag_db)
    w_phase_matrix[i, :] = np.interp(freq_grid, freqs, phase_deg)

# Mag waterfall
mesh1 = ax_wmag.pcolormesh(freq_grid, el_grid, w_mag_matrix, shading='gouraud', cmap='magma', vmin=-22, vmax=4)
ax_wmag.set_xscale('log')
ax_wmag.set_title("Magnitude Waterfall (Elevation Sweep)", fontsize=11, color=gold_accent, fontweight='bold')
ax_wmag.set_xlabel("Frequency (Hz)", fontsize=10, color='white')
ax_wmag.set_ylabel("Elevation Angle (degrees)", fontsize=10, color='white')
cbar1 = plt.colorbar(mesh1, ax=ax_wmag)
cbar1.set_label("Magnitude (dB)", color='white')
plt.setp(plt.getp(cbar1.ax.axes, 'yticklabels'), color='white')

# Trace Notch 1
n1_track = 6000.0 + ((el_grid + 50.0) / 140.0) * 5200.0
ax_wmag.plot(n1_track, el_grid, color=radar_cyan, linewidth=2.0, linestyle='--', label="Notch 1 Trace")
ax_wmag.legend(facecolor=panel_bg, edgecolor=gold_accent, loc='lower right', fontsize=8)

# Phase waterfall
mesh2 = ax_wphase.pcolormesh(freq_grid, el_grid, w_phase_matrix, shading='gouraud', cmap='twilight', vmin=-180, vmax=180)
ax_wphase.set_xscale('log')
ax_wphase.set_title("Phase Waterfall (Elevation Sweep)", fontsize=11, color=gold_accent, fontweight='bold')
ax_wphase.set_xlabel("Frequency (Hz)", fontsize=10, color='white')
ax_wphase.set_ylabel("Elevation Angle (degrees)", fontsize=10, color='white')
cbar2 = plt.colorbar(mesh2, ax=ax_wphase)
cbar2.set_label("Phase (degrees)", color='white')
plt.setp(plt.getp(cbar2.ax.axes, 'yticklabels'), color='white')

ax_wphase.plot(n1_track, el_grid, color=radar_cyan, linewidth=2.0, linestyle='--', label="Notch 1 Phase Ridge")
ax_wphase.legend(facecolor=panel_bg, edgecolor=gold_accent, loc='lower right', fontsize=8)

plt.suptitle("FlyBy: 2D Magnitude vs Phase Spectrograms Showing Pinna Elevation Tracking",
             fontsize=13, color=gold_accent, fontweight='bold', y=0.98)
plt.tight_layout()
fig4_file = "spectrum_dual_waterfall.png"
fig.savefig(fig4_file, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig4_file), dpi=200)
plt.close()

print("Successfully generated all spectrum, phase, group delay, and waterfall charts!")
