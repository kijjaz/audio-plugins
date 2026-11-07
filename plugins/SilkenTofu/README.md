# Silken Tofu — Teletronix LA-2A & T4B Electro-Optical Leveling Amplifier + Oversampled Tube/Iron Suite

**Silken Tofu** is a component-level virtual analog and semiconductor-physics emulation of the legendary **1965 Teletronix LA-2A Leveling Amplifier** and its **T4B Electro-Optical Attenuator Cell**—expanded with modern sound-design capabilities that go far beyond 1960s hardware limitations while staying 100% rooted in analog circuit and solid-state physics.

Whereas **Fried Tofu** is a crispy WDF diode clipper, **Silken Tofu** is its silky, squishy, glowing optical sibling.

---

## 📦 Direct 1-Click Downloads (GitHub Releases — No Login Required)

* 🍎 **[Download SilkenTofu-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/silkentofu-v1.0.0/SilkenTofu-macOS-Universal.zip)** *(Universal: Apple Silicon `arm64` & Intel `x86_64` — VST3, AU, Standalone App)*
* 🪟 **[Download SilkenTofu-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/silkentofu-v1.0.0/SilkenTofu-Windows-x64.zip)** *(Windows 64-bit VST3)*
* ⚙️ **[View GitHub Actions CI Build Runs](https://github.com/kijjaz/audio-plugins/actions/workflows/silkentofu.yml)**

---

## 🔬 Circuit & Semiconductor Physics Architecture

### 1. The "T4-Bean" Electro-Optical Cell (Destriau EL + Two-Trap Semiconductor ODEs)
* **Destriau Electroluminescent (EL) Panel Law:** Models the high-voltage AC $\text{ZnS:Cu}$ phosphor panel inside the T4B canister using Destriau's equation $L(V) \propto |V|^\gamma \exp(-V_0 / |V|)$, driven by a `6AQ5A` beam power pentode and `NE-2` neon clamp. Includes capacitive displacement current ($I = C\frac{dV}{dt}$) so high-frequency transients naturally illuminate the panel faster.
* **Two-Trap CdS Carrier Recombination (The "Memory Effect"):** Solves coupled continuous-time differential equations for **shallow traps ($N_1$, ~60 ms fast release)** and **deep crystal lattice traps ($N_2$, 1–15 s history-dependent release tail)**, plus brightness-accelerated attack ($\tau_a(L) = \tau_{a0} / (1 + \beta_a L)$).
* **Crystal Chemistry Selector:**
  * `Vintage CdS (LA-2A)`: Syrupy 1965 Clairex photoconductor with deep multi-second memory glue.
  * `CdSe Vactrol (LA-3A)`: Faster, punchy Cadmium Selenide response for drums and guitars.
  * `Fast Silicon (FET-Opto)`: Sub-millisecond transient snap combined with the LA-2A's optical knee.
* **T4B Pre-Warmth (`Bias Glow`):** Simulates a micro-amp pilot current keeping the EL panel dimly glowing at rest so deep traps never empty to pitch-black $2\text{ M}\Omega$, eliminating "first-syllable cold-start overshoot" on vocals.
* **Negative Trap Memory (`Optical Bloom`):** Sweeping `Trap Memory` negative ($-50\%$) inverts deep-trap coupling so sustained notes swell and bloom upward after the initial transient clamp.

### 2. Oversampled Analog Tube (`12AX7A` + `12BH7A`) & Transformer (`UTC HA-100X` / `A-24`) Stage
* **Selectable `1x / 2x / 4x / 8x` Polyphase Oversampling:** Eliminates digital aliasing foldback even when driving the tube and iron stages hard.
* **`UTC HA-100X` Permalloy Input Transformer:** $1/f$ magnetic flux integrator with Jiles-Atherton ferromagnetic hysteresis that thickens bass fundamentals ($50\text{–}150\text{ Hz}$) while keeping highs airy.
* **`V1 (12AX7A)` Triode + `Fat Ck` Mod + `C2` Grid-Conduction Shift:** Models Koren triode plate curves with positive grid-current charging on coupling capacitor `C2` and an optional $22\text{ }\mu\text{F}$ cathode bypass switch (`Fat Ck`) for rich 2nd-harmonic triode saturation.
* **`V2 (12BH7A)` White Cathode Follower + `600Ω` Load + `B+` Supply Rail Sag:** Models the push-pull totem-pole output stage, vintage $600\text{ }\Omega$ console termination, and shared `275V DC B+` power supply droop ($R_{29} = 4.7\text{ k}\Omega$ / `C7A`).

### 3. Sidechain Sculpting & True External Key Optical Ducking
* **External Key Optical Ducking:** Route any external sidechain (e.g., kick drum or lead vocal) directly into the T4B electroluminescent panel so ducking inherits the 60 ms fast bounce + smooth optical glide.
* **`R37` HF Emphasis + Warm-Cut HPF (`20–350 Hz`) + `SC Listen`:** Sculpt and audition the exact signal illuminating the EL panel.

---

## 🎚️ Calibrated Factory Presets (`0.0 dB` Unity Trim Standard)

All 8 factory presets are internally calibrated on musical program material (~`-14.9 dBFS` RMS) within $\pm 0.2\text{ dB}$ while keeping **Master Trim at `0.0 dB` (Unity)**:

| # | Preset Name | Category | Peak GR | Output RMS | Master Trim |
| :--- | :--- | :--- | :--- | :--- | :--- |
| 1 | **1965 Pasadena Vocal Silk** | Vintage LA-2A | `-6.5 dB` | `-14.79 dBFS` | `0.0 dB` |
| 2 | **Warm Motown Bass Iron (600 Ohm)** | Bass & Low End | `-10.3 dB` | `-14.98 dBFS` | `0.0 dB` |
| 3 | **Punchy CdSe Drum Bus (LA-3A Mod)** | Drums & Transients | `-7.1 dB` | `-14.90 dBFS` | `0.0 dB` |
| 4 | **Silicon Snap Limiter (FET-Opto Hybrid)** | Modern Limiting | `-20.7 dB` | `-14.93 dBFS` | `0.0 dB` |
| 5 | **Reverse Optical Bloom (Negative Trap)** | Creative Sound Design | `-11.6 dB` | `-14.98 dBFS` | `0.0 dB` |
| 6 | **Molten White-Cathode Tube Overdrive** | Harmonic Saturation | `-4.2 dB` | `-14.90 dBFS` | `0.0 dB` |
| 7 | **External Optical Key Ducker** | Sidechain & Mixing | `-15.4 dB` | `-14.94 dBFS` | `0.0 dB` |
| 8 | **Master Bus Luminous Glue** | Mastering | `-3.9 dB` | `-14.83 dBFS` | `0.0 dB` |

