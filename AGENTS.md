# Audio Plugins Development Rules (AGENTS.md)

These guidelines apply across all audio plugin projects in this repository (`kijjaz/audio-plugins`).

---

## 1. Distribution & GitHub Releases Rule (No Actions Run Links)

- **NEVER** link to GitHub Actions workflow run pages (`https://github.com/.../actions/workflows/...`) as primary download links in `README.md`.
  - Users find raw CI run logs confusing, and GitHub blocks users who are not logged in from downloading CI artifacts.
- **ALWAYS** provide direct 1-click downloads via **GitHub Releases**:
  - URL format: `https://github.com/kijjaz/audio-plugins/releases/download/<tag>/<Plugin>-<OS>.zip`
  - Or link to the Releases page: `https://github.com/kijjaz/audio-plugins/releases`
- Every plugin workflow in `.github/workflows/<plugin>.yml` must include a `release` job that zips macOS (Universal VST3/AU/App) and Windows (x64 VST3) and publishes them to GitHub Releases.

---

## 2. Loudness Calibration & Preset Standard (Unity Trim)

- **NEVER** use the master output `trim` parameter as a band-aid to balance loudness across presets:
  - Presets must NOT load with negative or positive trim offsets (e.g. `-11 dB` or `+6 dB`).
  - Master `trim` on all factory presets **MUST rest at `0.0 dB` (unity gain)** so users retain full, uncolored headroom.
- **Balance loudness internally at the component level:**
  - Scale driver pulses, voice-coil currents, transducer radiation scalars, integrator step sizes, and non-linear drive makeup directly inside the DSP engine.
- **Loudness Testing:**
  - Balance loudness using nominal musical audio / dialogue (~ -14.4 dBFS) or K-weighted pink noise (ITU-R BS.1770 / LUFS).
  - Do NOT use unweighted broadband sine sweep RMS for loudness balancing because high-Q acoustic bandpass transducers (like piezo buzzers) will appear artificially quiet, leading to ear-piercing over-amplification.

---

## 3. Sample-Rate Invariance (96 kHz Standard)

- Continuous-time differential equations (voice-coil current $di/dt$, mechanical cone acceleration, delta-mod slew steps, leaky integrators) must scale with sample rate using:
  ```cpp
  float rateRatio = static_cast<float>(44100.0 / fs);
  ```
- Always verify DSP stability at **96,000 Hz** with a logarithmic sine sweep to confirm:
  1. No numerical blowup (`NaN` / `inf`).
  2. Biquad filters and mechanical boundaries catch safely.
  3. Output RMS variation between 44.1 kHz and 96 kHz is $\le 0.2\text{ dB}$.

---

## 4. Local macOS Deployment & Validation

Before pushing any plugin:
1. Strip detritus and sign ad-hoc:
   ```bash
   xattr -cr ~/Library/Audio/Plug-Ins/VST3/<Plugin>.vst3 ~/Library/Audio/Plug-Ins/Components/<Plugin>.component
   codesign --force --deep -s - ~/Library/Audio/Plug-Ins/VST3/<Plugin>.vst3 ~/Library/Audio/Plug-Ins/Components/<Plugin>.component
   ```
2. Verify Audio Unit compliance with Apple's validator:
   ```bash
   auval -v <type> <code4> <manu4>
   # Must conclude with: AU VALIDATION SUCCEEDED
   ```
