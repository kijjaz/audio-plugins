# Kijjaz Audio Plugins Collection

A collection of professional, open-source audio DSP plugins and experimental synthesis environments built with modern **JUCE 8** and **Web Audio Modules (WAM)**.

---

## 🚀 Available Plugins & Quick Links

### 1. [Fried Tofu](plugins/FriedTofu/README.md)
* **Type**: Wave Digital Filter (WDF) Diode Clipper & Analog Saturation.
* **Key Features**: True one-port WDF non-linear solver with damped Newton-Raphson iteration, 4 Shockley diode models (Si 1N4148, Ge 1N34A, Schottky BAT41, Red LED), asymmetric odd/even harmonic blending, 4x polyphase oversampling, real-time oscilloscope, and dynamic static transfer curve visualizer.
* 📦 **[Download Latest Fried Tofu Builds (macOS, Windows, Linux)](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu.yml)**

---

### 2. [AetherBeam Reverb](plugins/AetherBeam/README.md)
* **Type**: Real-Time 3D Ray-Tracing Acoustic Reverb & Nonlinear Waveguide.
* **Key Features**: Orders 0–4 physical specular ray tracing (up to 96 paths) with 3D wireframe perspective mesh, spatial microphone polar patterns (Binaural, ORTF, Blumlein, Omni), dynamic audience occupancy absorption simulator, finite-amplitude wave steepening (Fubini-Bessel series at $fff \approx 122\text{ dB SPL}$), and 3-Band Material Damping EQ.
* **Spaces**: Musikverein Vienna, Sydney Opera House, Hagia Sophia, Šibenik Cathedral, Sponza Palace, Capitol Studios Chamber 4, Rome Pantheon, and more.
* 📦 **[Download Latest AetherBeam Builds (macOS, Windows, Linux)](https://github.com/kijjaz/audio-plugins/actions/workflows/aetherbeam.yml)**

---

### 3. [Vacuum Tape Sim](plugins/VacuumTapeSim/README.md)
* **Type**: Analog Tube Compression & Tape Hysteresis Emulation.
* **Key Features**: Physical "Voltage Drain" tube supply sag modeling, dynamic magnetic tape hysteresis coercivity, mechanical wow & flutter transport simulation, and Wallace loss filters.
* 📦 **[Download Latest VacuumTapeSim Builds (macOS, Windows)](https://github.com/kijjaz/audio-plugins/actions/workflows/vacuum_tape_sim.yml)**

---

### 3. [Discrete 808](plugins/Discrete808/)
* **Type**: Component-Level Analog Drum Synthesis.
* **Key Features**: Exact component-level modeling of vintage transistor circuits, bridled T-network resonators, germanium diode clipping, and voice parameter randomization.

---

### 4. [ConcreteMachine](plugins/ConcreteMachine/)
* **Type**: Musique Concrète Workstation & Experimental TapeOS.
* **Key Features**: Chaos-driven tape looping, Morphophone multi-tap head mechanism, and algorithmic spatial montage.

---

### 5. [FlyBy (VST3 / AU)](plugins/FlyBy/README.md)
* **Type**: 3D Binaural Spatial Panner & Dynamic Elevation Engine.
* **Key Features**: Spherical Rayleigh-Woodworth ITD, dual pinna notches ($N_1, N_2$) with overhead zenith presence and torso reflection, transient preservation attack detection to eliminate pinna comb-filtering smear, floor grounding reflections, dynamic speed-adaptive Doppler frequency shifts, and autonomous 3D flight trajectories (Swoop, Helical, Zenith, Flutter).
* 📦 **[Download Latest FlyBy Builds (macOS, Windows)](https://github.com/kijjaz/audio-plugins/actions/workflows/flyby.yml)**

---

### 6. [FlyByWeb (Open-Source Web Audio Library)](plugins/FlyByWeb/README.md)
* **Type**: Zero-Dependency Web Audio 3D Spatial Audio Library & AudioWorklet Engine.
* **Key Features**: Drop-in high-performance AudioWorklet DSP with zero build dependencies, Three.js / WebXR camera tracking integration, full feature parity with FlyBy VST3, opt-in ceiling and floor boundary reflections, and interactive browser-based 3D radar demo.

---

## 💻 Supported Formats & Platforms

| Platform | Formats | Architectures | Installation & Security Notes |
| :--- | :--- | :--- | :--- |
| **Windows** | VST3, Standalone (.exe) | 64-bit (`x64`) | Copy to `C:\Program Files\Common Files\VST3\`. Works out-of-the-box. |
| **macOS** | VST3, AU (Component), Standalone | Universal 2 (`arm64` + `x86_64`) | Native Apple Silicon + Intel. See [macOS Gatekeeper Guide](#-macos-installation--gatekeeper-quarantine-bypass) below. |
| **Linux** | VST3, Standalone | 64-bit (`x86_64`) | Copy to `~/.vst3/`. Works out-of-the-box. |

---

## 🍏 macOS Installation & Gatekeeper / Quarantine Bypass

Because these open-source plugins are compiled and signed automatically via GitHub Actions CI without a commercial Apple Developer ID ($99/yr), macOS Gatekeeper will place newly downloaded binaries into quarantine upon download.

### How to load on any Mac (Intel or Apple Silicon):

#### Method 1: Terminal (Fastest for Audio Producers)
After copying `AetherBeam Reverb.vst3` to `~/Library/Audio/Plug-Ins/VST3/` (or AU to `~/Library/Audio/Plug-Ins/Components/`), open **Terminal** and remove the quarantine attribute:
```bash
xattr -cr ~/Library/Audio/Plug-Ins/VST3/"AetherBeam Reverb.vst3"
xattr -cr ~/Library/Audio/Plug-Ins/Components/"AetherBeam Reverb.component"
```
*(Or for all downloaded plugins in your folder: `xattr -cr ~/Library/Audio/Plug-Ins/VST3/*.vst3`)*

#### Method 2: System Settings GUI
1. Open your DAW (Ableton Live, Logic Pro, Reaper, FL Studio, Bitwig).
2. When macOS displays: *"AetherBeam Reverb cannot be opened because Apple cannot check it for malicious software"*, click **Cancel**.
3. Open **System Settings  → Privacy & Security**, scroll down to the **Security** section.
4. You will see: *"AetherBeam Reverb was blocked from use because it is not from an identified developer"*. Click **Open Anyway**.
5. Restart your DAW or rescan plugins. Click **Open** on the final confirmation.

---

## 🛠️ Automated CI/CD Builds

All plugins in this repository are compiled continuously using **GitHub Actions**:
* Every commit triggers automated release builds on official macOS, Windows, and Linux virtual machines.
* To download the latest compiled binaries for any plugin, click on the respective workflow under the **[Actions Tab](https://github.com/kijjaz/audio-plugins/actions)** and download the artifact zip file.

---

## 📜 License

Created and maintained by **Kijjaz**. Distributed under the MIT License.
