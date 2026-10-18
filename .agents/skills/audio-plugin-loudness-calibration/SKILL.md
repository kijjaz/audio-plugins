---
name: audio-plugin-loudness-calibration
description: >-
  Procedures for calibrating loudness, factory presets, internal DSP component balancing,
  and sample-rate invariance (44.1 kHz vs 96 kHz) for audio plugins.
  Use when tuning preset volume, designing gain structures, or testing DSP stability across sample rates.
---

# Audio Plugin Loudness Calibration & Sample-Rate Invariance

This skill defines the engineering rules for level calibration in the `kijjaz/audio-plugins` codebase.

## 🚨 Golden Rule: Component-Level Calibration

1. **NEVER use the master output `trim` parameter to patch loudness discrepancies across presets:**
   - Presets must NOT load with artificial offsets like `-11.1 dB` or `+6.8 dB` trim.
   - Every preset's master `trim` parameter must rest at unity **`0.0 dB`** so the user has full, uncolored headroom.
2. **Calibrate the internal DSP components directly:**
   - **Modulators / Oscillators**: Scale the driver pulse height or modulation depth to match nominal acoustic energy (e.g. scaling high-crest-factor 1-bit square waves down by ~0.28x to match PWM envelope).
   - **Transducer Radiation**: Calibrate radiation gain at the transducer/speaker model (e.g. tuning piezo saturation scalar from 2.2x to 3.45x to match paper cone output).
   - **Slope / Integrators**: Tune step sizes and leaky integration constants so slope-overload does not excessively attenuate speech or transients.
   - **Drive Saturation Makeup**: Incorporate non-linear drive compensation ($1 / \text{drive}^k$) so increasing the drive knob adds harmonic grit without creating a drastic volume explosion.

---

## 🎧 Loudness Calibration vs. Sine Sweep Testing

### 1. Loudness Balance: Use Program Audio or K-Weighted Noise
- **Why**: Transducers like piezo buzzers have severe high-Q bandpass characteristics (zero bass below 1 kHz, +15 dB resonance at 3.25 kHz). Unweighted broadband sine sweep RMS artificially penalizes band-limited devices, causing over-compensation that produces piercing shrieks.
- **Method**: Calibrate loudness using nominal music/speech (e.g. ~ -14.4 dBFS RMS, crest factor 10–14 dB) or K-weighted pink noise (ITU-R BS.1770 / LUFS).
- **Target**: All presets with `trim = 0.0 dB` should land within **$\pm 0.5\text{ dB}$** of the target reference.

### 2. Sample-Rate Invariance: Validate at 44.1 kHz AND 96.0 kHz
- Continuous-time differential equations (voice-coil current $di/dt$, mechanical cone acceleration, delta-mod slew steps) must scale with sample rate:
  ```cpp
  float rateRatio = static_cast<float>(44100.0 / fs);
  deltaStep_ = baseStep * rateRatio;
  deltaLeak_ = std::pow(baseLeak, rateRatio);
  ```
- **96 kHz Sine Sweep Test**:
  - Run a logarithmic sine sweep ($20\text{ Hz} \to 20,000\text{ Hz}$) at $96,000\text{ Hz}$ to verify:
    1. Zero numerical instability (`NaN` or `inf`).
    2. Mechanical excursion end-stops catch without blowup.
    3. Output RMS delta between 44.1 kHz and 96 kHz is $\le 0.2\text{ dB}$.

---

## 🧪 Automated Verification Script

Always write or maintain an automated calibration harness in `tools/`:
- `tools/CalibratePresets.cpp`: Iterates through all presets with audio material, prints an output table verifying that all presets sit within target loudness at `0.0 dB` trim.
- `tools/TestSineSweep96k.cpp`: Tests stability and sample-rate parity at 96 kHz.
