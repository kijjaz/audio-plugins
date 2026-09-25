#!/usr/bin/env python3
"""
AetherBeam Showcase Generator:
Generates 3D ray-tracing visualization images and native 96 kHz 24-bit WAV impulse responses
for all 12 architectural spaces across all 4 listener & source preset positions (48 spaces/positions in total).
"""

import os
import json
import struct
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import scipy.signal as signal

FS = 96000
SPEED_OF_SOUND = 343.2
AIR_DENSITY = 1.204
BETA_AIR = 1.20
P0_REF = 2.0e-5
HEAD_RADIUS = 0.0875 # 8.75 cm standard adult head radius
MAX_IR_DURATION = 12.0 # Cap max impulse duration in seconds for ultra-large spaces

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, '..'))
DB_PATH = os.path.join(PROJECT_DIR, 'Source', 'DSP', 'AcousticDatabase.json')
OUTPUT_DIR = SCRIPT_DIR

def write_wav24(filename, fs, audio_lr):
    """Writes a 2-channel 24-bit PCM WAV file efficiently."""
    audio_lr = np.clip(audio_lr, -1.0, 1.0)
    samples = (audio_lr * 8388607.0).astype(np.int32)
    
    # Interleave L and R channels
    interleaved = np.empty((len(samples) * 2,), dtype=np.int32)
    interleaved[0::2] = samples[:, 0]
    interleaved[1::2] = samples[:, 1]
    
    # Extract 3 bytes per 32-bit integer (little endian)
    u32 = interleaved.view(np.uint32)
    b0 = (u32 & 0xFF).astype(np.uint8)
    b1 = ((u32 >> 8) & 0xFF).astype(np.uint8)
    b2 = ((u32 >> 16) & 0xFF).astype(np.uint8)
    
    packed = np.column_stack([b0, b1, b2]).reshape(-1)
    raw_bytes = packed.tobytes()
    
    data_size = len(raw_bytes)
    total_size = 36 + data_size
    
    with open(filename, 'wb') as f:
        f.write(b'RIFF')
        f.write(struct.pack('<I', total_size))
        f.write(b'WAVEfmt \x10\x00\x00\x00\x01\x00\x02\x00')
        f.write(struct.pack('<I', fs))
        f.write(struct.pack('<I', fs * 6))
        f.write(struct.pack('<H', 6))
        f.write(struct.pack('<H', 24))
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        f.write(raw_bytes)

def render_binaural_ir(space, pos, peak_spl_db=122.0):
    """
    Renders physically motivated 96 kHz 24-bit binaural impulse response
    matching AetherBeam's C++ DSP engine:
    - Discrete nonlinear wave-steepening early arrivals with Woodworth ITD
    - Diffuse late reverberation tail matching Sabine/Eyring RT60 and volume
    """
    rt60 = float(space['rt60'])
    vol = float(space['volume'])
    area = float(space['area'])
    rays = pos['rays']
    
    duration = min(rt60 * 1.35, MAX_IR_DURATION)
    n_samples = int(duration * FS)
    
    out_l = np.zeros(n_samples, dtype=np.float32)
    out_r = np.zeros(n_samples, dtype=np.float32)
    
    # Peak sound pressure (122 dB SPL ~ 25.18 Pa, fortississimo orchestra)
    p_peak = P0_REF * (10.0 ** (peak_spl_db / 20.0))
    stiffness = AIR_DENSITY * (SPEED_OF_SOUND ** 3)
    
    # 1. Early deterministic arrivals
    for k, r in enumerate(rays):
        pts = np.array(r['pts'])
        d = float(r['dist'])
        order = int(r['order'])
        gain = float(r['gain'])
        
        if len(pts) >= 2:
            arr_dir = pts[-1] - pts[-2]
            norm_dir = np.linalg.norm(arr_dir)
            arr_dir = (arr_dir / norm_dir) if norm_dir > 1e-6 else np.array([0.0, 1.0, 0.0])
        else:
            arr_dir = np.array([0.0, 1.0, 0.0])
            
        azimuth = np.arctan2(arr_dir[0], arr_dir[1])
        sin_az = np.sin(azimuth)
        itd = (HEAD_RADIUS / SPEED_OF_SOUND) * (sin_az + np.arcsin(np.clip(sin_az, -1.0, 1.0)))
        
        delay_l = d / SPEED_OF_SOUND - 0.5 * itd
        delay_r = d / SPEED_OF_SOUND + 0.5 * itd
        
        idx_l = int(round(delay_l * FS))
        idx_r = int(round(delay_r * FS))
        
        # Nonlinear shock steepening factor
        gamma = np.clip(1.0 * (d * BETA_AIR * p_peak) / stiffness * 5e3, 0.0, 0.6)
        amp = gain * (1.0 + gamma * 0.5)
        phase = 1.0 if (order == 0 or k % 2 == 0) else -1.0
        
        if 0 <= idx_l < n_samples:
            out_l[idx_l] += amp * phase
        if 0 <= idx_r < n_samples:
            out_r[idx_r] += amp * phase
            
    # 2. 3D Architectural Modal Standing Waves & Schroeder Diffuse Tail
    bounds = space['bounds']
    b_min = np.array(bounds['min'], dtype=np.float32)
    b_max = np.array(bounds['max'], dtype=np.float32)
    L = np.maximum(b_max - b_min, 0.1)
    Lx, Ly, Lz = float(L[0]), float(L[1]), float(L[2])
    
    # Calculate Schroeder transition frequency: f_sch = 2000 * sqrt(RT60 / V)
    f_schroeder = 2000.0 * np.sqrt(rt60 / max(vol, 1.0))
    modal_cutoff = max(min(f_schroeder, 350.0), 120.0) # Up to 350 Hz for small chambers, min 120 Hz
    
    # Discover 3D eigenmodes within physical enclosure
    axial, tangential, oblique = [], [], []
    max_nx = int(modal_cutoff * 2 * Lx / SPEED_OF_SOUND) + 1
    max_ny = int(modal_cutoff * 2 * Ly / SPEED_OF_SOUND) + 1
    max_nz = 0 if space['id'] == 'epidaurus_amphitheatre' else int(modal_cutoff * 2 * Lz / SPEED_OF_SOUND) + 1
    
    for nx in range(max_nx + 1):
        for ny in range(max_ny + 1):
            for nz in range(max_nz + 1):
                if nx == 0 and ny == 0 and nz == 0:
                    continue
                f = 0.5 * SPEED_OF_SOUND * np.sqrt((nx / Lx)**2 + (ny / Ly)**2 + (nz / Lz)**2)
                if 20.0 <= f <= modal_cutoff:
                    order = int(nx > 0) + int(ny > 0) + int(nz > 0)
                    if order == 1:
                        axial.append((f, nx, ny, nz, 'axial', 1.0, 1.35))
                    elif order == 2:
                        tangential.append((f, nx, ny, nz, 'tangential', 0.707, 1.15))
                    else:
                        oblique.append((f, nx, ny, nz, 'oblique', 0.5, 1.0))
                        
    axial.sort(key=lambda x: x[0])
    tangential.sort(key=lambda x: x[0])
    oblique.sort(key=lambda x: x[0])
    
    selected_modes = axial + tangential[:max(0, 64 - len(axial))]
    if len(selected_modes) < 64:
        selected_modes += oblique[:64 - len(selected_modes)]
    selected_modes.sort(key=lambda x: x[0])
    
    # Compute normalized 3D position coordinates within boundary box [0, 1]
    src_norm = np.clip((np.array(pos['src'], dtype=np.float32) - b_min) / L, 0.0, 1.0)
    lis_norm = np.clip((np.array(pos['lis'], dtype=np.float32) - b_min) / L, 0.0, 1.0)
    
    t = np.arange(n_samples, dtype=np.float32) / FS
    decay_env = np.exp(-6.907755 * t / rt60) # -60 dB nominal decay
    hf_damp = np.exp(-1.6 * t / rt60)
    
    # 2a. Diffuse field above modal transition frequency
    seed_val = abs(hash(space['id'])) % 100000 + pos['id'] * 137
    rng = np.random.RandomState(seed_val)
    noise_l = rng.normal(0, 1.0, n_samples).astype(np.float32)
    noise_r = rng.normal(0, 1.0, n_samples).astype(np.float32)
    
    # High-pass diffuse noise above modal cutoff to prevent redundant low-end clutter
    b_hp, a_hp = signal.butter(2, modal_cutoff / (FS / 2.0), btype='high')
    diffuse_l = signal.lfilter(b_hp, a_hp, noise_l * decay_env * (0.55 + 0.45 * hf_damp)).astype(np.float32)
    diffuse_r = signal.lfilter(b_hp, a_hp, noise_r * decay_env * (0.55 + 0.45 * hf_damp)).astype(np.float32)
    
    # 2b. Physical 3D Standing Wave Resonators (Nodes & Antinodes)
    modal_l = np.zeros(n_samples, dtype=np.float32)
    modal_r = np.zeros(n_samples, dtype=np.float32)
    
    for f, nx, ny, nz, mtype, weight, q_mult in selected_modes:
        # Spatial eigenmode shape: Psi(x, y, z) = cos(nx*pi*x/Lx) * cos(ny*pi*y/Ly) * cos(nz*pi*z/Lz)
        psi_src = np.cos(nx * np.pi * src_norm[0]) * np.cos(ny * np.pi * src_norm[1]) * np.cos(nz * np.pi * src_norm[2])
        psi_lis = np.cos(nx * np.pi * lis_norm[0]) * np.cos(ny * np.pi * lis_norm[1]) * np.cos(nz * np.pi * lis_norm[2])
        coupling = psi_src * psi_lis * weight
        if abs(coupling) < 1e-4:
            continue
            
        # Axial modes experience fewer boundary collisions -> ring longer (higher Q)
        tau_mode = (rt60 * q_mult) / 6.907755
        decay_m = np.exp(-t / tau_mode)
        
        # Spatial binaural decorrelation based on mode orientation
        ph0_l = ((nx * 73 + ny * 179 + nz * 283) % 1000) / 1000.0 * 2.0 * np.pi
        ph0_r = ph0_l + 0.25 * np.pi * ((nx % 2) * 2 - 1)
        
        modal_l += (coupling * np.cos(2.0 * np.pi * f * t + ph0_l) * decay_m).astype(np.float32)
        modal_r += (coupling * np.cos(2.0 * np.pi * f * t + ph0_r) * decay_m).astype(np.float32)
        
    # Diffuse & modal onset ramp after Mean Free Path propagation time
    mfp = (4.0 * vol / area) if area > 1.0 else 10.0
    t_onset = mfp / SPEED_OF_SOUND
    onset_ramp = np.clip((t - t_onset) / 0.04, 0.0, 1.0)
    
    tail_gain = 0.25 / np.sqrt(vol + 10.0)
    num_m = max(len(selected_modes), 1)
    modal_scale = (0.22 / np.sqrt(num_m)) * (tail_gain * 3.5)
    
    out_l += (diffuse_l * tail_gain + modal_l * modal_scale) * onset_ramp
    out_r += (diffuse_r * tail_gain + modal_r * modal_scale) * onset_ramp
    
    # 100 ms smooth release fade at buffer tail
    fade_len = int(0.1 * FS)
    out_l[-fade_len:] *= np.linspace(1.0, 0.0, fade_len, dtype=np.float32)
    out_r[-fade_len:] *= np.linspace(1.0, 0.0, fade_len, dtype=np.float32)
    
    # Normalize to -1.0 dBFS (0.891)
    peak = max(np.max(np.abs(out_l)), np.max(np.abs(out_r)), 1e-6)
    target_peak = 0.891
    out_l = (out_l / peak) * target_peak
    out_r = (out_r / peak) * target_peak
    
    return np.column_stack([out_l, out_r])

def render_showcase_image(space, pos, output_img_path):
    """
    Renders high-resolution 3D perspective wireframe ray tracing image
    with Carbon & Gold aesthetic matching AetherBeam visualizer.
    """
    wf = np.array(space['wireframe'])
    rays = pos['rays']
    src = np.array(pos['src'])
    lis = np.array(pos['lis'])
    
    fig = plt.figure(figsize=(12, 8), facecolor='#090d16')
    ax = fig.add_subplot(111, projection='3d', facecolor='#090d16')
    
    # 1. Architectural Wireframe (Cyan subtle lines)
    for edge in wf:
        ax.plot([edge[0], edge[3]], [edge[1], edge[4]], [edge[2], edge[5]],
                color='#38bdf8', alpha=0.32, linewidth=1.0)
                
    # 2. Color-coded Acoustic Reflection Rays
    order_styles = {
        0: ('#fbbf24', 2.6, 0.95),  # Direct: Radiant Gold
        1: ('#f97316', 1.8, 0.85),  # 1st order: Warm Amber
        2: ('#38bdf8', 1.2, 0.70),  # 2nd order: Electric Cyan
        3: ('#a855f7', 0.9, 0.55),  # 3rd order: Violet
        4: ('#64748b', 0.7, 0.40)   # 4th order: Diffuse Slate
    }
    
    # Plot higher orders first so early reflections and direct sound remain prominently visible on top
    sorted_rays = sorted(rays, key=lambda r: -int(r['order']))
    for r in sorted_rays:
        pts = np.array(r['pts'])
        ord_val = int(r['order'])
        col, lw, alpha = order_styles.get(ord_val, ('#64748b', 0.6, 0.35))
        ax.plot(pts[:, 0], pts[:, 1], pts[:, 2], color=col, linewidth=lw, alpha=alpha)
        
    # 3. Source & Listener Interactive Nodes
    src_label = pos['name'].split(' to ')[0] if ' to ' in pos['name'] else 'Source'
    lis_label = pos['name'].split(' to ')[1] if ' to ' in pos['name'] else 'Listener / Mic'
    
    ax.scatter([src[0]], [src[1]], [src[2]], color='#ef4444', s=150,
               edgecolors='#ffffff', linewidth=1.6, label=f"Source: {src_label} (fff 122 dB SPL)")
    ax.scatter([lis[0]], [lis[1]], [lis[2]], color='#06b6d4', s=150,
               edgecolors='#ffffff', linewidth=1.6, label=f"Listener: {lis_label}")
               
    # 4. Perspective framing & aspect ratio
    all_pts = [src, lis]
    if len(wf) > 0:
        all_pts.extend(wf[:, :3])
        all_pts.extend(wf[:, 3:])
    all_pts = np.array(all_pts)
    
    max_range = np.ptp(all_pts, axis=0).max() / 2.0
    mid = (all_pts.max(axis=0) + all_pts.min(axis=0)) * 0.5
    ax.set_xlim(mid[0] - max_range, mid[0] + max_range)
    ax.set_ylim(mid[1] - max_range, mid[1] + max_range)
    ax.set_zlim(mid[2] - max_range, mid[2] + max_range)
    
    # 5. Styling
    ax.grid(False)
    ax.xaxis.pane.fill = False
    ax.yaxis.pane.fill = False
    ax.zaxis.pane.fill = False
    ax.xaxis.pane.set_edgecolor('#1e293b')
    ax.yaxis.pane.set_edgecolor('#1e293b')
    ax.zaxis.pane.set_edgecolor('#1e293b')
    ax.tick_params(colors='#64748b', labelsize=8)
    
    ax.view_init(elev=24, azim=-48)
    
    title_text = (
        f"{space['title']} — 3D Physical Beam Tracing Showcase\n"
        f"Position: {pos['name']} | RT60: {space['rt60']}s | Vol: {space['volume']:,.0f} m³ | Rays: {len(rays)} paths"
    )
    ax.set_title(title_text, color='#ffffff', fontsize=11, pad=18, fontweight='bold')
    
    leg = ax.legend(facecolor='#0f172a', edgecolor='#1e293b', labelcolor='#ffffff',
                    loc='upper right', framealpha=0.85, fontsize=8.5)
                    
    plt.savefig(output_img_path, dpi=150, bbox_inches='tight', facecolor=fig.get_facecolor())
    plt.close()

def main():
    print("==========================================================================")
    print("      AETHERBEAM: COMPLETE 12-SPACE PRESET SHOWCASE ASSET GENERATOR       ")
    print("==========================================================================")
    
    with open(DB_PATH, 'r', encoding='utf-8') as f:
        spaces = json.load(f)
        
    total_spaces = len(spaces)
    print(f"Loaded {total_spaces} spaces from database: {DB_PATH}")
    
    count = 0
    total_expected = sum(len(s.get('positions', [])) for s in spaces)
    
    for s_idx, space in enumerate(spaces):
        s_id = space['id']
        positions = space.get('positions', [])
        print(f"\n[{s_idx+1}/{total_spaces}] Processing Space: '{space['title']}' ({s_id}) | {len(positions)} positions")
        
        for pos in positions:
            p_id = pos['id']
            img_name = f"showcase_3d_{s_id}_pos{p_id}.png"
            wav_name = f"AetherBeam_fff_Symphonic_{s_id}_pos{p_id}_96k24b.wav"
            
            img_path = os.path.join(OUTPUT_DIR, img_name)
            wav_path = os.path.join(OUTPUT_DIR, wav_name)
            
            count += 1
            print(f"  -> ({count}/{total_expected}) Pos {p_id}: '{pos['name']}'...")
            
            # 1. Render 3D Ray-Tracing Image
            render_showcase_image(space, pos, img_path)
            
            # 2. Render 96 kHz 24-bit Impulse Response WAV
            ir_lr = render_binaural_ir(space, pos)
            write_wav24(wav_path, FS, ir_lr)
            
    print("\n==========================================================================")
    print(f"SUCCESS: Generated {count} 3D images and {count} 96kHz 24-bit IR WAV files!")
    print(f"All assets saved to: {OUTPUT_DIR}")
    print("==========================================================================")

if __name__ == '__main__':
    main()
