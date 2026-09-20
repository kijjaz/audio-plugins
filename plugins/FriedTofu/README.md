# Fried Tofu (VST3 / AU / Standalone)

**Fried Tofu** is an analog-modeled diode-clipper audio plugin reproducing the circuit acoustics of an anti-parallel semiconductor diode clipping network. Built with JUCE and modern C++17, it features a one-port Wave Digital Filter (WDF) root topology, damped Newton–Raphson numerical solver, and polyphase anti-aliasing oversampling.

![Fried Tofu UI Concept](FriedTofu_Math_and_DSP.pdf)

---

## Features

- **Wave Digital Filter (WDF) Diode Core**:
  - One-port non-linear network driven through a variable source impedance $R$ ($200\,\Omega$ to $22\,\text{k}\Omega$).
  - Shockley diode equations solved per-sample via a damped Newton–Raphson root-finder in double precision with warm-start memory.
- **4 Diode Flavors**:
  - **Si 1N4148 (Crisp)**: $I_s = 2.52\,\text{nA}, N = 1.752$ — sharp conduction knee, bright upper harmonics.
  - **Ge 1N34A (Silky)**: $I_s = 200\,\text{pA}, N = 2.190$ — soft knee, warm compression, velvety saturation.
  - **Schottky BAT41 (Flash Fried)**: $I_s = 20\,\text{nA}, N = 1.050$ — low barrier ($\approx 0.32\,\text{V}$), fast conduction, aggressive fuzz.
  - **Red LED (Deep Fried)**: $I_s = 9.3\times 10^{-19}\,\text{A}, N = 1.840$ — high forward headroom ($\approx 1.75\,\text{V}$), open dynamic valve punch.
- **Asymmetric Harmonization**:
  - Path A (+) and Path B (−) diode selectors can be mixed independently. Pairing Silicon with Germanium yields rich second-order even harmonics ($H_2$ at $-26.8\,\text{dBc}$) characteristic of single-ended tube triodes.
- **Oversampling**:
  - 1x (Raw), 2x, and 4x (Crisp Clean) polyphase half-band IIR filter engine eliminating aliasing foldback.
- **Real-Time Visualizers**:
  - Phosphor green time-domain oscilloscope streaming from an audio-thread lock-free ring buffer.
  - Interactive static transfer curve ($V_{\text{out}}$ vs $V_{\text{in}}$) dynamically updated with diode selection and series impedance.
- **Tone Stage & Output**:
  - 2nd-order Butterworth low-pass filter ($700\,\text{Hz}$ to $20\,\text{kHz}$), dry/wet mix blend, and output level trim.

---

## Multi-Platform GitHub Actions CI/CD

This repository includes a multi-platform GitHub Actions build pipeline ([`.github/workflows/build_vst3.yml`](.github/workflows/build_vst3.yml)) that automatically compiles and uploads release artifacts for:
- **macOS**: Universal Binary (`arm64` + `x86_64`) VST3, AU (`.component`), and Standalone App with ad-hoc signing.
- **Windows**: `x64` VST3 plugin and Standalone `.exe`.
- **Linux**: `x64` VST3 plugin and Standalone binary with ALSA/JACK/X11 headers.

---

## Local Building

### Prerequisites
- CMake $\ge 3.22$
- C++17 compatible compiler (Clang / GCC / MSVC)
- Git (JUCE will be fetched automatically via CMake `FetchContent` if not present locally)

### Build Commands
```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Compile VST3, AU, and Standalone
cmake --build build --config Release --target FriedTofu_VST3 FriedTofu_Standalone --parallel
```

Built binaries will appear in `build/FriedTofu_artefacts/Release/`.

---

## Documentation & Measurements

- [FriedTofu_Math_and_DSP.tex](FriedTofu_Math_and_DSP.tex): Formal LaTeX article detailing the WDF derivations, Kirchhoff wave port formulas, Shockley equations, and Newton-Raphson quadratic convergence proofs.
- [FriedTofu_Math_and_DSP.pdf](FriedTofu_Math_and_DSP.pdf): Two-page publication document with diode physical characteristics and empirical harmonic distortion benchmarks.
