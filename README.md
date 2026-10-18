# Kijjaz Audio Plugins Collection

A collection of professional, open-source audio DSP plugins and experimental synthesis environments built with modern **JUCE 8** and **Web Audio Modules (WAM)**.

---

## ⚡ Quick Download Hub (Pre-built Binaries)

All plugins are continuously built for **macOS (Universal arm64 + x86_64)**, **Windows (x64)**, and **Linux**. You can download the latest binaries directly from the latest successful CI runs below:

| Plugin | Type / Format | Status & Download Link | Supported OS |
| :--- | :--- | :--- | :--- |
| **Fried Tofu** | WDF Diode Clipper & Analog Saturation (VST3, AU) | [![FriedTofu](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-Windows-x64.zip) • [Linux](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-Linux-x64.zip) | macOS, Windows, Linux |
| **Fried Tofu Multiband** | 3-Band Linear-Phase WDF Diode Clipper (VST3, AU) | [![FriedTofuMultiband](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu_multiband.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu_multiband.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-Windows-x64.zip) • [Linux](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-Linux-x64.zip) | macOS, Windows, Linux |
| **AetherBeam Reverb** | 3D Ray-Tracing Acoustic Reverb (VST3, AU) | [![AetherBeam](https://github.com/kijjaz/audio-plugins/actions/workflows/aetherbeam.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/aetherbeam.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-Windows-x64.zip) • [Linux](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-Linux-x64.zip) | macOS, Windows, Linux |
| **Vacuum Tape Sim** | Tube Saturation & Tape Transport (VST3, AU) | [![VacuumTapeSim](https://github.com/kijjaz/audio-plugins/actions/workflows/vacuum_tape_sim.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/vacuum_tape_sim.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/vacuumtapesim-v1.0.0/VacuumTapeSim-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/vacuumtapesim-v1.0.0/VacuumTapeSim-Windows-x64.zip) | macOS, Windows |
| **IronStack** | 8-Amp Preamp & Cabinet Simulator (VST3, AU) | [![IronStack](https://github.com/kijjaz/audio-plugins/actions/workflows/ironstack.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/ironstack.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/ironstack-v1.0.0/IronStack-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/ironstack-v1.0.0/IronStack-Windows-x64.zip) | macOS, Windows |
| **Sympathetic Strings** | Dual-Polarization Waveguide & Resonator (VST3, AU) | [![SympatheticStrings](https://github.com/kijjaz/audio-plugins/actions/workflows/sympatheticstrings.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/sympatheticstrings.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/sympatheticstrings-v0.0.2/SympatheticStrings-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/sympatheticstrings-v0.0.2/SympatheticStrings-Windows-x64.zip) | macOS, Windows |
| **FlyBy** | 3D Binaural Spatial Panner (VST3, AU) | [![FlyBy](https://github.com/kijjaz/audio-plugins/actions/workflows/flyby.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/flyby.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/flyby-v1.0.0/FlyBy-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/flyby-v1.0.0/FlyBy-Windows-x64.zip) | macOS, Windows |
| **Speka** | 1980s PC Speaker & PIT 8253 1-Bit Transducer Emulator (VST3, AU, Standalone) | [![Speka](https://github.com/kijjaz/audio-plugins/actions/workflows/speka.yml/badge.svg)](https://github.com/kijjaz/audio-plugins/actions/workflows/speka.yml) <br>📦 **[Direct Releases](https://github.com/kijjaz/audio-plugins/releases)**: [macOS](https://github.com/kijjaz/audio-plugins/releases/download/speka-v0.1.0/Speka-macOS-Universal.zip) • [Windows](https://github.com/kijjaz/audio-plugins/releases/download/speka-v0.1.0/Speka-Windows-x64.zip) | macOS, Windows |
| **FlyByWeb** | Web Audio / AudioWorklet 3D Panner (JavaScript Library) | [Web Demo & Source](plugins/FlyByWeb/README.md) | Browser / Web Audio |

> **💡 How to download pre-built plugins:**
> * **Direct 1-Click Downloads (Fastest, No Login Required):** Click the direct **[macOS]**, **[Windows]**, or **[Linux]** links in the table above or under each plugin's details below to download `.zip` release archives directly without needing a GitHub account.
> * **Release Archive & Changelogs:** Visit **[GitHub Releases](https://github.com/kijjaz/audio-plugins/releases)** for all version packages and notes.

---

## 🚀 Available Plugins & Details

### 1. [Fried Tofu](plugins/FriedTofu/README.md)
* **Type**: Wave Digital Filter (WDF) Diode Clipper & Analog Saturation.
* **Key Features**: True one-port WDF non-linear solver with damped Newton-Raphson iteration, 4 Shockley diode models (Si 1N4148, Ge 1N34A, Schottky BAT41, Red LED), asymmetric odd/even harmonic blending, 4x polyphase oversampling, real-time oscilloscope, and dynamic static transfer curve visualizer.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download FriedTofu-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download FriedTofu-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-Windows-x64.zip)** *(Windows 64-bit VST3)*
  - 🐧 **[Download FriedTofu-Linux-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-v1.0.0/FriedTofu-Linux-x64.zip)** *(Linux 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu.yml)**

---

### 2. [Fried Tofu Multiband](plugins/FriedTofuMultiband/README.md)
* **Type**: 3-Band Linear-Phase WDF Diode Saturation & Clipper.
* **Key Features**: Mathematically perfect nulling ($\approx -300\,\text{dB}$) complementary linear-phase FIR crossover network, independent WDF diode solver per band (Low, Mid, High), per-band Drive, Diode selector, and solo/mute/trim controls.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download FriedTofuMultiband-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download FriedTofuMultiband-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-Windows-x64.zip)** *(Windows 64-bit VST3)*
  - 🐧 **[Download FriedTofuMultiband-Linux-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/friedtofu-multiband-v1.0.0/FriedTofuMultiband-Linux-x64.zip)** *(Linux 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/friedtofu_multiband.yml)**

---

### 3. [AetherBeam Reverb](plugins/AetherBeam/README.md)
* **Type**: Real-Time 3D Ray-Tracing Acoustic Reverb & Nonlinear Waveguide.
* **Key Features**: Orders 0–4 physical specular ray tracing (up to 96 paths) with 3D wireframe perspective mesh, spatial microphone polar patterns (Binaural, ORTF, Blumlein, Omni), dynamic audience occupancy absorption simulator, finite-amplitude wave steepening (Fubini-Bessel series at $fff \approx 122\text{ dB SPL}$), and 3-Band Material Damping EQ.
* **Spaces**: Musikverein Vienna, Sydney Opera House, Hagia Sophia, Šibenik Cathedral, Sponza Palace, Capitol Studios Chamber 4, Rome Pantheon, and more.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download AetherBeam-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download AetherBeam-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-Windows-x64.zip)** *(Windows 64-bit VST3)*
  - 🐧 **[Download AetherBeam-Linux-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/aetherbeam-v1.0.0/AetherBeam-Linux-x64.zip)** *(Linux 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/aetherbeam.yml)**

---

### 4. [Vacuum Tape Sim](plugins/VacuumTapeSim/README.md)
* **Type**: Analog Tube Compression & Tape Hysteresis Emulation.
* **Key Features**: Physical "Voltage Drain" tube supply sag modeling, dynamic magnetic tape hysteresis coercivity, mechanical wow & flutter transport simulation, and Wallace loss filters.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download VacuumTapeSim-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/vacuumtapesim-v1.0.0/VacuumTapeSim-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download VacuumTapeSim-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/vacuumtapesim-v1.0.0/VacuumTapeSim-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/vacuum_tape_sim.yml)**

---

### 5. [IronStack](plugins/IronStack/)
* **Type**: Component-Level Analog Amplifier & Multi-Cabinet Suite.
* **Key Features**: Exact continuous nodal matrix solvers & Bilinear Transform (DAFx-06 Yeh & Smith 3rd-order FMV and 4th-order James/Baxandall networks), kinetic 12AX7 vacuum tube preamp with dynamic DC cathode bias sag, 8 iconic amplifier circuits (Bassman 5F6-A, AA864, Ampeg B-15N, Twin Reverb AB763, JCM800, AC30 Top Boost, Mesa Dual Rectifier, Soldano SLO-100), and 8 Tone3000 acoustically-calibrated cabinet impulse responses.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download IronStack-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/ironstack-v1.0.0/IronStack-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download IronStack-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/ironstack-v1.0.0/IronStack-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/ironstack.yml)**

---

### 6. [Sympathetic Strings (v0.0.2)](plugins/SympatheticStrings/)
* **Type**: Dual-Polarization Waveguide & Parabolic Jawari Resonator (VST3 / AU / Standalone).
* **Key Features**: Dynamic energy-normalized boundary wrapping (Equal-Energy Twang Calibration across all velocities), Jiva (cotton thread) boundary scattering, amplitude-dependent elliptical cross-polarization precession, 6-mode physical wooden body resonator, per-string `[M]` Mute / `[S]` Solo buttons, resonant string loop filter, and flexible drone voicing with custom semitone/cent micro-tuning.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download SympatheticStrings-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/sympatheticstrings-v0.0.2/SympatheticStrings-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download SympatheticStrings-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/sympatheticstrings-v0.0.2/SympatheticStrings-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/sympatheticstrings.yml)**

---

### 7. [FlyBy (VST3 / AU)](plugins/FlyBy/README.md)
* **Type**: 3D Binaural Spatial Panner & Dynamic Elevation Engine.
* **Key Features**: Spherical Rayleigh-Woodworth ITD, dual pinna notches ($N_1, N_2$) with overhead zenith presence and torso reflection, transient preservation attack detection to eliminate pinna comb-filtering smear, floor grounding reflections, dynamic speed-adaptive Doppler frequency shifts, and autonomous 3D flight trajectories (Swoop, Helical, Zenith, Flutter).
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download FlyBy-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/flyby-v1.0.0/FlyBy-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU)*
  - 🪟 **[Download FlyBy-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/flyby-v1.0.0/FlyBy-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/flyby.yml)**

---

### 8. [Speka](plugins/Speka/README.md)
* **Type**: Physical Transducer & 1-Bit Digital Audio Modulation Simulator (IBM PC 5150).
* **Key Features**: Authentic RealSound PWM, Sigma-Delta PDM, Delta Modulation, and Direct 1-Bit modes; full physical voice-coil current integration, 2.25″ Alnico paper cone & 12mm ceramic piezo models, cabinet acoustics with panel rattle, vintage beige desktop chassis with amber CRT oscilloscope, and 8 factory calibrated presets at 0.0 dB trim.
* 📦 **[Direct 1-Click Downloads (GitHub Releases — No Login Required)](https://github.com/kijjaz/audio-plugins/releases)**:
  - 🍎 **[Download Speka-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/speka-v0.1.0/Speka-macOS-Universal.zip)** *(Universal: Apple Silicon arm64 & Intel — VST3, AU, Standalone App)*
  - 🪟 **[Download Speka-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/speka-v0.1.0/Speka-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/speka.yml)**

---

### 9. [FlyByWeb (Open-Source Web Audio Library)](plugins/FlyByWeb/README.md)
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
