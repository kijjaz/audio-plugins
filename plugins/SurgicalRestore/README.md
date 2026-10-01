# Surgical Restore

**Neural Mastering Audio Restoration & Vintage Vinyl Denoiser (VST3 / AU / Standalone)**

An advanced, surgical audio restoration suite engineered for historical recording preservation, vintage vinyl digitization, and analog tape recovery. Built with modern **JUCE 8** and powered by **100% synthetically trained machine learning models** executed locally on Apple Silicon / x86_64 SIMD.

---

## 🎧 The Core Problem & Innovation

Traditional restoration plugins often destroy the music they aim to save:
* **The "Snare/Transient" Trap**: Classical click detectors mistake natural drum attacks, acoustic guitar pick transients, and brass attacks for clicks.
* **The "Watery MP3" Artifact**: Spectral denoisers modulate adjacent frequency bins independently, creating audible chirping and phase smearing on high metallic percussion (like cymbals or Thai **ฉิ่ง**).
* **The "Reed/Horn" Distortion**: Overblown saxophones, loud trumpet peaks, and quadruple-reed instruments (**ปี่ / Pi Nai**) blast dense upper harmonics that naive denoisers treat as background hiss or groove crackle.

`Surgical Restore` solves these issues using a **three-stage hybrid architecture**:

```
[Stereo Input] ──► [Mid/Side Matrix (M/S)]
                          │
          ┌───────────────┴───────────────┐
          ▼                               ▼
    [MID CHANNEL]                  [SIDE CHANNEL]
   • Lateral Music                • Vertical Groove Scratches
   • LPC De-Click & Inpaint       • High-Sensitivity De-Click
   • De-Crackle Chatter Filter    • De-Crackle Chatter Filter
          │                               │
          └───────────────┬───────────────┘
                          │
                   [L/R Synthesis]
                          │
                          ▼
            [Decision-Directed Spectral De-Hiss]
            • Synthetic Neural Mask + Bark Critical Band Smoothing
            • Harmonic Comb Shield (Protects Vocal, Brass & Reeds)
            • High-Frequency Air Tilt
                          │
                          ▼
                 [Clean Master Output]
```

---

## 🎛️ Architecture & Features

### 1. Stage 1: Mid/Side (M/S) LPC Residual De-Clicking
* **Mid/Side Acoustic Separation**: On vinyl, music is cut laterally (Mid), while physical scratches, dust particles, and vertical stylus bounces show up predominantly out-of-phase in the **Side channel ($L - R$)**. De-clicking in M/S domain exposes deep scratches with massive signal-to-noise ratio.
* **LPC Innovation Residual**: A 16th-to-20th order Levinson-Durbin inverse filter subtracts predictable harmonic music, exposing isolated impulse anomalies without confusing musical pitch with clicks.
* **Cubic Hermite Inpainting**: Reconstructs missing sample spans (1 to 64 samples) with continuous first and second derivatives, preventing phase clicks.

### 2. Stage 2: Dynamic De-Crackle (Needle Mistracking & Chatter Harvester)
* Targets the dense "sandpaper" buzz (>1,000 micro-impacts per second) caused by playback diamond styli losing groove-wall contact during loud vocal or brass crescendos (stylus pinch effect).
* Uses an instantaneous **Innovation Energy Variance Estimator** to suppress mechanical chatter bursts without dulling vocal warmth.

### 3. Stage 3: Neural Spectral De-Hiss with Harmonic Shielding
* **100% Synthetic Training**: Trained on procedural polyphonic chords, envelopes, pink noise, and 50/60 Hz mains hum directly in GPU memory on Apple Silicon (`mps`). Zero real dirty audio required.
* **Psychoacoustic Bark Critical Band Smoothing**: Smooths adjacent high-frequency bins (> 2.5 kHz) across human ear critical bands, **completely destroying the low-bitrate MP3 "watery swish"** on metallic instruments.
* **Harmonic Comb Shield**: Measures Wiener Spectral Tonality; any frequency bin locked to a musical harmonic line ($f_0, 2f_0, \dots$) is granted 100% pass-through immunity.
* **HF Air Tilt**: Applies full $-12\text{ dB}$ to $-24\text{ dB}$ reduction on low-frequency rumble and tape hum, while leaving high-end room air open and silky.

---

## 🎚️ Console Controls (Carbon & Gold Interface)

* **`DE-CLICK`**: Controls the LPC innovation detection sensitivity (from subtle dust ticks to wide physical scratches).
* **`DE-CRACKLE`**: Adjusts the threshold of the dynamic groove chatter harvester.
* **`DE-HISS (dB)`**: Sets the attenuation depth of the neural spectral denoiser (0 to -24 dB).
* **`HARM SHIELD`**: Adjusts harmonic protection strength for delicate reeds, brass horns, and lead vocals.
* **`AUDITION DELTA`**: Inverts and subtracts the restored signal, allowing you to monitor **only the removed noise, clicks, and crackle** to guarantee zero musical bleed.
* **`BYPASS`**: Hard true-bypass for instant A/B evaluation.

---

## 🧪 Benchmarks & Stress Test Results

Validated with an automated test suite (`SurgicalRestore_StressTest`):
* **Throughput**: **88.3x faster than real-time** at 96 kHz / 24-bit stereo.
* **CPU Consumption**: **~1.13%** of a single Apple Silicon CPU core.
* **Sample Rate Support**: Dynamically validated from **44.1 kHz up to 192 kHz**.
* **Buffer Agility**: Zero NaNs or Infs from **16 samples** (ultra-low latency tracking) up to **4,096 samples** (mastering sessions).
* **Pathological Robustness**: Tested against pure digital silence, full-scale Nyquist alternating square waves ($\pm 1.0$), and over-range continuous DC offset (+2.0f).

---

## 📦 Building & Installation

### Requirements
* **macOS** 12.0+ (Universal binary for Apple Silicon & Intel x86_64) or **Windows** 10/11
* CMake 3.22+
* Ninja or Xcode / Visual Studio

### Build Commands
```bash
cd plugins/SurgicalRestore
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

### Installation
The build generates:
* **VST3**: `build/SurgicalRestore_artefacts/Release/VST3/Surgical Restore.vst3`
* **AU**: `build/SurgicalRestore_artefacts/Release/AU/Surgical Restore.component`
* **Standalone**: `build/SurgicalRestore_artefacts/Release/Standalone/Surgical Restore.app`

Copy to your system plugin folders:
```bash
cp -R "build/SurgicalRestore_artefacts/Release/VST3/Surgical Restore.vst3" ~/Library/Audio/Plug-Ins/VST3/
cp -R "build/SurgicalRestore_artefacts/Release/AU/Surgical Restore.component" ~/Library/Audio/Plug-Ins/Components/
cp -R "build/SurgicalRestore_artefacts/Release/Standalone/Surgical Restore.app" /Applications/
```

---

## 📜 License
Developed by Kijjaz. Open source for educational and audio engineering research.
