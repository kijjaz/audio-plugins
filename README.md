# Kijjaz Audio Plugins Collection

A collection of professional, open-source audio DSP plugins and experimental synthesis environments built with modern **JUCE 8** and **Web Audio Modules (WAM)**.

---

## 🚀 Available Plugins & Quick Links

### 1. [AetherBeam Reverb](plugins/AetherBeam/README.md)
* **Type**: Real-Time 3D Ray-Tracing Acoustic Reverb & Nonlinear Waveguide.
* **Key Features**: Orders 0–4 physical specular ray tracing (up to 96 paths), strict non-box architectural boundary containment, finite-amplitude wave steepening (Fubini-Bessel series at $fff \approx 122\text{ dB SPL}$), and 3-Band Material Damping EQ.
* **Spaces**: Musikverein Vienna, Sydney Opera House, Hagia Sophia, Šibenik Cathedral, Sponza Palace, Capitol Studios Chamber 4, Rome Pantheon, and more.
* 📦 **[Download Latest AetherBeam Builds (macOS, Windows, Linux)](https://github.com/kijjaz/audio-plugins/actions/workflows/aetherbeam.yml)**

---

### 2. [Vacuum Tape Sim](plugins/VacuumTapeSim/README.md)
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

## 💻 Supported Formats & Platforms

| Platform | Formats | Architectures |
| :--- | :--- | :--- |
| **macOS** | VST3, AU (Component), Standalone | Universal 2 (`arm64` Apple Silicon + `x86_64` Intel) |
| **Windows** | VST3, Standalone (.exe) | 64-bit (`x64`) |
| **Linux** | VST3, Standalone | 64-bit (`x86_64`) |

---

## 🛠️ Automated CI/CD Builds

All plugins in this repository are compiled continuously using **GitHub Actions**:
* Every commit triggers automated release builds on official macOS, Windows, and Linux virtual machines.
* To download the latest compiled binaries for any plugin, click on the respective workflow under the **[Actions Tab](https://github.com/kijjaz/audio-plugins/actions)** and download the artifact zip file.

---

## 📜 License

Created and maintained by **Kijjaz**. Distributed under the MIT License.
