# Fried Tofu Multiband (VST3 / AU / Standalone)

**Fried Tofu Multiband** is an analog-modeled multiband diode-clipping and saturation audio plugin. Built alongside **Fried Tofu**, it processes audio across three independent frequency bands (Low, Mid, High) separated by a **complementary linear-phase FIR crossover network** that guarantees exact mathematical reconstruction ($\approx -300\,\text{dB}$ null) with zero phase distortion.

---

## Key Features

- **Linear-Phase Complementary FIR Crossover**:
  - Exact mathematical nulling: $H_{\text{Low}}(z) + H_{\text{Mid}}(z) + H_{\text{High}}(z) = z^{-\tau_{\text{total}}}$
  - Avoids phase smearing and comb-filtering artifacts across band transition boundaries when saturating individual frequency ranges.
  - Automatic latency compensation reported to DAW hosts (`setLatencySamples`).
  - Delay-aligned dry path for transparent master Dry/Wet blending.
- **Wave Digital Filter (WDF) Diode Engine Per Band**:
  - Independent diode network per band with damped Newton–Raphson solver in double precision.
  - Independent band controls:
    - **Drive**: $-12\,\text{dB}$ to $+36\,\text{dB}$
    - **Network Impedance ($R$)**: $200\,\Omega$ to $22\,\text{k}\Omega$
    - **Path A (+) Diode**: Si 1N4148, Ge 1N34A, BAT41 Schottky, Red LED
    - **Path B (−) Diode**: Si 1N4148, Ge 1N34A, BAT41 Schottky, Red LED
    - **Mute, Solo, and Bypass** toggles per band
    - **Band Level Trim**: $-24\,\text{dB}$ to $+12\,\text{dB}$
- **Master Stage**:
  - Crossover Low-Mid cutoff ($60\,\text{Hz} - 1200\,\text{Hz}$)
  - Crossover Mid-High cutoff ($1.2\,\text{kHz} - 12\,\text{kHz}$)
  - Master Drive, Master Trim, and Master Dry/Wet Mix
- **Visualizers**:
  - **Phosphor Green Oscilloscope**: Audio-thread lock-free FIFO display of the output waveform.
  - **Dynamic Transfer Curve**: Real-time $V_{\text{out}}$ vs $V_{\text{in}}$ curve for the currently inspected band.

---

## Building Locally

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build Standalone, VST3, and AU
cmake --build build --config Release --target FriedTofuMultiband_Standalone FriedTofuMultiband_VST3 FriedTofuMultiband_AU --parallel
```

Binaries will be placed in `build/FriedTofuMultiband_artefacts/Release/`.
