import numpy as np
import matplotlib.pyplot as plt
import os

# Set styling for Carbon & Gold aesthetic
plt.style.use('dark_background')
fig_bg = '#121316'
panel_bg = '#1a1c21'
gold_accent = '#e0b958'
radar_cyan = '#40e0d0'
radar_amber = '#ffaf28'
text_muted = '#969ba8'

fs = 48000
freqs = np.logspace(np.log10(200), np.log10(20000), 1000)
w = 2 * np.pi * freqs / fs

# --- 1. Biquad Peaking / Notch Filter Magnitude ---
def biquad_peaking_mag(f0, gain_db, q, freqs, fs):
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
    H = (b0 + b1 * z + b2 * (z**2)) / (1.0 + a1 * z + a2 * (z**2))
    return 20.0 * np.log10(np.maximum(np.abs(H), 1e-4))

def biquad_highshelf_mag(f0, gain_db, freqs, fs):
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
    H = (b0 + b1 * z + b2 * (z**2)) / (1.0 + a1 * z + a2 * (z**2))
    return 20.0 * np.log10(np.maximum(np.abs(H), 1e-4))

def compute_elevation_spectrum(el_deg):
    el_clamped = np.clip(el_deg, -50.0, 90.0)
    norm_el = (el_clamped + 50.0) / 140.0

    # N1 Primary Pinna Notch
    n1_freq = 6000.0 + norm_el * 5200.0
    mag_n1 = biquad_peaking_mag(n1_freq, -18.0, 5.0, freqs, fs)

    # N2 Secondary Notch
    overhead_factor = np.clip(el_clamped / 90.0, 0.0, 1.0)
    n2_freq = 9800.0 + overhead_factor * 3200.0
    mag_n2 = biquad_peaking_mag(n2_freq, -14.0 * overhead_factor, 6.5, freqs, fs)

    # Overhead Presence Shelf
    mag_shelf = biquad_highshelf_mag(8500.0, overhead_factor * 3.5, freqs, fs)

    # Torso reflection
    below_factor = np.clip(-el_clamped / 50.0, 0.0, 1.0)
    torso_freq = 1200.0 - below_factor * 350.0
    mag_torso = biquad_peaking_mag(torso_freq, -8.0 * below_factor, 2.8, freqs, fs)

    return mag_n1 + mag_n2 + mag_shelf + mag_torso

# ==============================================================================
# FIGURE 1: Vertical Panning Spectral Notches (Elevation Sweep)
# ==============================================================================
fig, ax = plt.subplots(figsize=(11, 6), facecolor=fig_bg)
ax.set_facecolor(panel_bg)

elevations = [-40, -20, 0, 20, 45, 70, 90]
colors = plt.cm.plasma(np.linspace(0.15, 0.95, len(elevations)))

for el, color in zip(elevations, colors):
    mag = compute_elevation_spectrum(el)
    label = f"Elevation {el:+3d}°"
    if el == 90: label += " (Overhead Zenith)"
    elif el == 0: label += " (Horizon Eye-Level)"
    elif el == -40: label += " (Below Chin / Torso)"
    ax.plot(freqs, mag, label=label, color=color, linewidth=2.2)

ax.set_xscale('log')
ax.set_xlim(300, 18000)
ax.set_ylim(-26, 6)
ax.axhline(0, color=text_muted, linestyle='--', alpha=0.3)

# Annotate Notch 1 Shift
ax.annotate('Torso Dip (~900Hz)\nappears below horizon', xy=(950, -6.5), xytext=(350, -18),
            arrowprops=dict(facecolor=radar_amber, edgecolor='none', shrink=0.08, width=1.5, headwidth=6),
            color=radar_amber, fontsize=10, fontweight='bold')

ax.annotate('Moving Pinna Notch N1\nSweeps 6.2kHz → 11.2kHz', xy=(8500, -14), xytext=(2200, -23),
            arrowprops=dict(facecolor=radar_cyan, edgecolor='none', shrink=0.08, width=1.5, headwidth=6),
            color=radar_cyan, fontsize=10, fontweight='bold')

ax.annotate('Zenith Presence & N2\n(Overhead Air)', xy=(13000, 3.0), xytext=(9000, 4.5),
            color=gold_accent, fontsize=10, fontweight='bold')

ax.set_title("FlyBy: Frequency Response vs. Elevation (Vertical Binaural Cues)", fontsize=14, color=gold_accent, pad=12, fontweight='bold')
ax.set_xlabel("Frequency (Hz)", fontsize=11, color='white')
ax.set_ylabel("Magnitude (dB)", fontsize=11, color='white')
ax.grid(True, which='both', color='#2d3038', linestyle=':', linewidth=0.8)
ax.legend(loc='lower left', framealpha=0.8, facecolor=panel_bg, edgecolor=gold_accent, fontsize=9)

artifact_dir = "/Users/kijjaz/.gemini/antigravity-ide/brain/845e1834-6222-413e-882b-d45879521804"
os.makedirs(artifact_dir, exist_ok=True)

fig1_name = "spectrum_vertical_elevation.png"
fig.savefig(fig1_name, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig1_name), dpi=200)
plt.close()

# ==============================================================================
# FIGURE 2: Horizontal vs Vertical ITD & ILD Comparison
# ==============================================================================
fig, (ax_itd, ax_ild) = plt.subplots(1, 2, figsize=(12, 5), facecolor=fig_bg)
ax_itd.set_facecolor(panel_bg)
ax_ild.set_facecolor(panel_bg)

angles = np.linspace(-90, 90, 181)

# Horizontal ITD (Woodworth formula)
head_r = 0.0875
c = 343.0
az_rad = angles * np.pi / 180.0
itd_horiz = (head_r / c) * (np.sin(np.abs(az_rad)) + np.abs(az_rad)) * np.sign(az_rad) * 1e6 # us
itd_vert = np.zeros_like(angles) # Symmetrical median plane

ax_itd.plot(angles, itd_horiz, color=radar_cyan, linewidth=2.8, label="Horizontal (Azimuth: -90° to +90°)")
ax_itd.plot(angles, itd_vert, color=radar_amber, linewidth=2.8, linestyle='--', label="Vertical (Elevation: -90° to +90°)")
ax_itd.set_title("Interaural Time Difference (ITD)", fontsize=12, color=gold_accent, fontweight='bold')
ax_itd.set_xlabel("Angle (degrees)", fontsize=10, color='white')
ax_itd.set_ylabel("ITD (microseconds)", fontsize=10, color='white')
ax_itd.grid(True, color='#2d3038', linestyle=':')
ax_itd.legend(facecolor=panel_bg, edgecolor=gold_accent, fontsize=9)
ax_itd.axhline(0, color=text_muted, alpha=0.3)
ax_itd.axvline(0, color=text_muted, alpha=0.3)

# Horizontal ILD (4kHz Rayleigh Shadowing)
ild_horiz = 9.3 * np.sin(az_rad) # ~9.3 dB peak at 90 deg
ild_vert = np.zeros_like(angles) # 0 dB on median plane

ax_ild.plot(angles, ild_horiz, color=radar_cyan, linewidth=2.8, label="Horizontal (ILD Left vs Right)")
ax_ild.plot(angles, ild_vert, color=radar_amber, linewidth=2.8, linestyle='--', label="Vertical (ILD Left vs Right)")
ax_ild.set_title("Interaural Level Difference at 4 kHz (ILD)", fontsize=12, color=gold_accent, fontweight='bold')
ax_ild.set_xlabel("Angle (degrees)", fontsize=10, color='white')
ax_ild.set_ylabel("ILD (dB: Right - Left)", fontsize=10, color='white')
ax_ild.grid(True, color='#2d3038', linestyle=':')
ax_ild.legend(facecolor=panel_bg, edgecolor=gold_accent, fontsize=9)
ax_ild.axhline(0, color=text_muted, alpha=0.3)
ax_ild.axvline(0, color=text_muted, alpha=0.3)

plt.suptitle("Why Vertical Panning Requires Pinna Cues: ITD & ILD Are Exactly Zero on Median Plane!",
             fontsize=13, color=gold_accent, fontweight='bold', y=0.98)
plt.tight_layout()
fig2_name = "spectrum_itd_ild_comparison.png"
fig.savefig(fig2_name, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig2_name), dpi=200)
plt.close()

# ==============================================================================
# FIGURE 3: 2D Spectrogram / Waterfall of Continuous Elevation Sweep
# ==============================================================================
fig, ax = plt.subplots(figsize=(10, 6), facecolor=fig_bg)
ax.set_facecolor(panel_bg)

el_grid = np.linspace(-40, 90, 200)
freq_grid = np.logspace(np.log10(1000), np.log10(16000), 300)

spec_matrix = np.zeros((len(el_grid), len(freq_grid)))
for i, el in enumerate(el_grid):
    spec_matrix[i, :] = compute_elevation_spectrum(el)[np.searchsorted(freqs, freq_grid)]

mesh = ax.pcolormesh(freq_grid, el_grid, spec_matrix, shading='gouraud', cmap='magma', vmin=-22, vmax=4)
ax.set_xscale('log')
ax.set_xlim(1000, 16000)
ax.set_xlabel("Frequency (Hz)", fontsize=11, color='white')
ax.set_ylabel("Elevation Angle (degrees)", fontsize=11, color='white')
ax.set_title("Waterfall Spectrogram: Pinna Spectral Notch Trajectory as Source Moves Up", fontsize=13, color=gold_accent, fontweight='bold', pad=12)

cbar = plt.colorbar(mesh, ax=ax)
cbar.set_label("Magnitude (dB)", color='white')
cbar.ax.yaxis.set_tick_params(color='white')
plt.setp(plt.getp(cbar.ax.axes, 'yticklabels'), color='white')

# Trace Notch 1 center
n1_track = 6000.0 + ((el_grid + 50.0) / 140.0) * 5200.0
ax.plot(n1_track, el_grid, color=radar_cyan, linewidth=2.5, linestyle='--', label="Notch 1 Centerline Path")
ax.legend(facecolor=panel_bg, edgecolor=gold_accent, loc='lower right')

plt.tight_layout()
fig3_name = "spectrum_elevation_waterfall.png"
fig.savefig(fig3_name, dpi=200)
fig.savefig(os.path.join(artifact_dir, fig3_name), dpi=200)
plt.close()

print(f"Generated and copied to artifact directory:\n - {fig1_name}\n - {fig2_name}\n - {fig3_name}")
