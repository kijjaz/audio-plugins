# Silken Tofu (v0.0.4) — Teletronix LA-2A & T4B Electro-Optical Leveling Amplifier + Mid-Side & Oversampled Tube/Iron Suite

**Silken Tofu** is a component-level virtual analog and semiconductor-physics emulation of the legendary **1965 Teletronix LA-2A Leveling Amplifier** and its **T4B Electro-Optical Attenuator Cell**—expanded with modern mastering and sound-design capabilities that go far beyond 1960s hardware limitations while staying 100% rooted in analog circuit and solid-state physics.

Whereas **Fried Tofu** is a crispy WDF diode clipper, **Silken Tofu** is its silky, squishy, glowing optical sibling.

📄 **[Read the Full Mathematical Whitepaper (`SilkenTofu_Whitepaper.pdf`)](docs/SilkenTofu_Whitepaper.pdf)** *(Complete continuous-time ODE derivations, Destriau EL law, Two-Trap carrier recombination, Koren triode + White Cathode Follower equations, and calibration benchmarks by Kijjaz and Gemini)*

---

## 📦 Direct 1-Click Downloads (GitHub Releases — No Login Required)

* 🍎 **[Download SilkenTofu-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/silkentofu-v0.0.4/SilkenTofu-macOS-Universal.zip)** *(Universal: Apple Silicon `arm64` & Intel `x86_64` — VST3, AU, Standalone App)*
* 🪟 **[Download SilkenTofu-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/silkentofu-v0.0.4/SilkenTofu-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/silkentofu.yml)**

---

## 🌟 New in Version `0.0.4`

1. **Flexible `0–100%` Stereo / M-S Optical Link & Independent Mid-Side (`M/S`) Parameter Banks:**
   * Switch between **`Stereo (L/R)`**, **`M/S: Edit MID`**, and **`M/S: Edit SIDE`** with two complete 17-parameter channel banks (`MID` and `SIDE`).
   * Use **`Solo Mid` / `Solo Side`** to audition the center or stereo difference signal in isolation, and **`Copy M>S`** to clone your Mid settings to the Side bank in one click.
   * Sweep **`Stereo Link` (`M/S Optic Link`)** continuously from `0%` (unlinked dual-canister / independent M-S) through `80%` (authentic 1965 Pin-6 interconnection) to `100%` (locked GR).
2. **Dual-Needle (`Mid/L` + `Side/R`) Illuminated VU Meter (`[ GR | OUT | IN ]`):**
   * Click `[ GR | OUT | IN ]` (or anywhere on the meter glass) to switch between **Gain Reduction (`dB`)**, **Output VU (`0 VU = -18 dBFS`)**, and **Input VU (`0 VU = -18 dBFS`)** with authentic $300\text{ ms}$ ballistic integration.
   * Whenever `M/S` mode is active or `Stereo Link < 99%`, a translucent **companion ghost needle** (cyan for `Side/R`, warm gold for `Mid/L`) renders behind the primary needle.
3. **Real-Time Scrolling Gain-Reduction & Deep-Trap ($N_2$) History Trace in Card 1 CRT Scope:**
   * Plots a live 60 FPS scrolling optical gain reduction curve (shaded phosphor trace) and deep-trap $N_2$ memory charge curve (amber trace) over the theoretical Two-Trap release trajectory. Click the CRT scope to toggle the live history overlay.
4. **`A / B` State Comparison (`[ A | B | A>B ]`) & Zero-Latency-Jump Soft Bypass (`BYP`):**
   * Compare two complete plugin states (including both `Mid` and `Side` banks) in the top header bar, and toggle **`BYP`** with a sample-accurate crossfade inside the polyphase oversampler delay path so DAW Plugin Delay Compensation (PDC) never jumps or clicks.
5. **Sample-Accurate Parameter Smoothing, $C^1$-Continuous Oversampled Gain Smoother & Closed-Loop Tube Coupling:**
   * $12\text{ ms}$ 1-pole exponential smoothing on all continuous parameters eliminates zipper noise during fast automation.
   * $0.18\text{ ms}$ oversampled $C^1$-continuous T4B envelope smoothing removes piecewise-linear slope corners on fast `Silicon` (`0.8 ms`) transients.
   * Closed-loop feedback couples `12AX7A` harmonic bloom and `275V B+` rail sag back into the `6AQ5` sidechain pentode screen supply.

---

## 🔬 Circuit & Semiconductor Physics Architecture

### 1. The "T4-Bean" Electro-Optical Cell (Destriau EL + Two-Trap Semiconductor ODEs)
* **Destriau Electroluminescent (EL) Panel Law:** Models the high-voltage AC $\text{ZnS:Cu}$ phosphor panel inside the T4B canister using Destriau's equation $L(V) \propto |V|^\gamma \exp(-V_0 / |V|)$, driven by a `6AQ5A` beam power pentode and `NE-2` neon clamp. Includes capacitive displacement current ($I = C\frac{dV}{dt}$) so high-frequency transients naturally illuminate the panel faster.
* **Two-Trap CdS Carrier Recombination (The "Memory Effect"):** Solves coupled continuous-time differential equations for **shallow traps ($N_1$, ~40–60 ms fast release)** and **deep crystal lattice traps ($N_2$, 1–15 s history-dependent release tail)**, plus brightness-accelerated attack ($\tau_a(L) = \tau_{a0} / (1 + \beta_a L)$).
* **Crystal Chemistry Selector:**
  * `Vintage CdS (LA-2A)`: Syrupy 1965 Clairex photoconductor with deep multi-second memory glue.
  * `CdSe Vactrol (LA-3A)`: Faster, punchy Cadmium Selenide response for drums and guitars.
  * `Fast Silicon (FET-Opto)`: Sub-millisecond transient snap combined with the LA-2A's optical knee.
* **T4B Pre-Warmth (`Bias Glow`):** Simulates a micro-amp pilot current keeping the EL panel dimly glowing at rest so deep traps never empty to pitch-black $2\text{ M}\Omega$, eliminating "first-syllable cold-start overshoot" on vocals.
* **Negative Trap Memory (`Optical Bloom`):** Sweeping `Trap Memory` negative ($-50\%$) inverts deep-trap coupling so sustained notes swell and bloom upward after the initial transient clamp.

### 2. Oversampled Analog Tube (`12AX7A` + `12BH7A`) & Transformer (`UTC HA-100X` / `A-24`) Stage
* **Selectable `1x / 2x / 4x / 8x` Polyphase Oversampling:** Eliminates digital aliasing foldback even when driving the tube and iron stages hard, with automatic DAW `setLatencySamples` PDC reporting.
* **`UTC HA-100X` Permalloy Input Transformer:** $1/f$ magnetic flux integrator with Jiles-Atherton ferromagnetic hysteresis that thickens bass fundamentals ($50\text{–}150\text{ Hz}$) while keeping highs airy.
* **`V1 (12AX7A)` Triode + `Fat Ck` Mod + `C2` Grid-Conduction Shift:** Models Koren triode plate curves with positive grid-current charging on coupling capacitor `C2` and an optional $22\text{ }\mu\text{F}$ cathode bypass switch (`Fat Ck`) for rich 2nd-harmonic triode saturation.
* **`V2 (12BH7A)` White Cathode Follower + `600Ω` Load + `B+` Supply Rail Sag:** Models the push-pull totem-pole output stage, vintage $600\text{ }\Omega$ console termination, and shared `275V DC B+` power supply droop ($R_{29} = 4.7\text{ k}\Omega$ / `C7A`).

### 3. Sidechain Sculpting & True External Key Optical Ducking
* **External Key Optical Ducking:** Route any external sidechain (e.g., kick drum or lead vocal) directly into the T4B electroluminescent panel so ducking inherits the fast optical bounce + smooth multi-second glide.
* **`R37` HF Emphasis + Warm-Cut HPF (`20–350 Hz`) + `SC Listen`:** Sculpt and audition the exact signal illuminating the EL panel.

---

## 🎚️ Calibrated Factory Presets (`0.0 dB` Unity Trim Standard)

All 9 factory presets are internally calibrated on musical program material (`-14.92 dBFS` RMS) while keeping **Master Trim strictly at `0.0 dB` (Unity)**:

| # | Preset Name | Routing / Mode | Peak GR | Output RMS | Master Trim |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | **1965 Pasadena Vocal Silk** | Stereo (`80%` Link) | `-8.46 dB` | `-14.97 dBFS` | `0.0 dB` |
| 2 | **Warm Motown Bass Iron (600 Ohm)** | Stereo (`80%` Link) | `-15.11 dB` | `-14.85 dBFS` | `0.0 dB` |
| 3 | **Punchy CdSe Drum Bus (LA-3A Mod)** | Stereo (`80%` Link) | `-12.83 dB` | `-14.77 dBFS` | `0.0 dB` |
| 4 | **Silicon Snap Limiter (FET-Opto Hybrid)** | Stereo (`80%` Link) | `-27.96 dB` | `-14.86 dBFS` | `0.0 dB` |
| 5 | **Reverse Optical Bloom (Negative Trap)** | Stereo (`80%` Link) | `-17.56 dB` | `-14.68 dBFS` | `0.0 dB` |
| 6 | **Molten White-Cathode Tube Overdrive** | Stereo (`80%` Link) | `-6.44 dB` | `-14.45 dBFS` | `0.0 dB` |
| 7 | **External Optical Key Ducker** | Stereo (`80%` Link) | `-19.48 dB` | `-14.90 dBFS` | `0.0 dB` |
| 8 | **Master Bus Luminous Glue** | Stereo (`80%` Link) | `-6.22 dB` | `-14.97 dBFS` | `0.0 dB` |
| 9 | **Mid-Side Mastering Sculptor (M/S)** | Mid-Side (`25%` M/S Link) | `-9.96 dB` | `-14.92 dBFS` | `0.0 dB` |

* **44.1 kHz vs 96.0 kHz Sample-Rate Invariance:** `0.0257 dB` RMS delta ($\le 0.20\text{ dB}$ standard).
* **Dry / Wet Phase Coherence (`0%..100%`):** `0.0 deg` phase lockinside the oversampled domain (`Zero Phase Cancellation`).
