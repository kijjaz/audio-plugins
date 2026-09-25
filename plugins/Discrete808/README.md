# Discrete808

**Discrete808** is a component-level DSP emulation of the legendary Roland TR-808 drum machine. Unlike sample-based clones, Discrete808 models the specific analog circuits of the original hardware to capture the non-linear behaviors, interactions, and "happy accidents" that define the classic sound.

## Core Philosophy

*   **No Samples**: Every sound is generated in real-time using mathematical models of the original circuits.
*   **Discrete Component Modeling**: We model the behavior of specific electrical components:
    *   **Bridged-T Networks**: Implementation of the specific bandpass filters used for drum bodies using Zero-Delay Feedback (ZDF) Topology Preserving Transforms (TPT).
    *   **Operational Amplifiers (uPC4558)**: Modeling of slew rate limiting (~1V/us) and saturation curves.
    *   **Non-Linear Envelopes**: Accurate replication of capacitor charge/discharge curves (e.g., the Kick's pitch "Sigh").
    *   **Circuit Drift**: Simulation of thermal instability and component tolerance variances over time.

## Architecture

*   **Platform**: JUCE framework (C++17)
*   **Formats**: VST3, AU, Standalone
*   **DSP**: Custom engine (no external DSP libraries used).

## Build Instructions

```bash
cd plugins/Discrete808/build
cmake ..
make -j4
```

## Voices

1.  **Kick Drum**: Bridged-T Resonator with "Sigh" pitch envelope and accent-dependent Q.
2.  **Snare Drum**: Dual Oscillating Bridged-T networks (Body) + Gated Noise (Snappy).
3.  **Hi-Hats**: 6-Oscillator Schmitt Trigger Bank (Square waves) processed through PolyBLEP for anti-aliasing and a Bandpass filter.

## Adjusting "Analog" Character

The plugin includes internal simulations for:
*   **Temperature**: Slowly oscillates over 30 minutes, affecting tuning and FM relationships.

## Component Customization (Maintenance Hatch)

Discrete808 allows you to circuit-bend each voice individually by opening the "Maintenance Hatch" (click any channel strip). You can swap out the text virtual components to drastically alter the character of the sound.

### Default Configurations

| Sound | Default Op-Amp | Default Diode | Default Cap Age | Character |
| :--- | :--- | :--- | :--- | :--- |
| **Kick** | **uPC4558** | **Silicon** | **0% (New)** | Classic, clean, tight thud. |
| **Snare** | **TL072** | **Germanium** | **40%** | Fuzzier, warmer, with slightly loose transients. |
| **Toms** | **uPC4558** | **Silicon** | **20-30%** | Standard punch with slight vintage softening. |
| **Clap** | **Broken** | **LED** | **80% (Dead)** | Trashy, crunchy, gated texture. |
| **Hats** | **TL072** | **LED** | **0% (New)** | Bright, crisp, and metallic. |

### Component Options

**Op-Amps (Slew Rate & Saturation)**
*   **uPC4558**: The original 808 chip. Darker, smooth slew limiting (~1V/µs).
*   **TL072**: Common mod. Brighter, faster slew rate, cleaner transients.
*   **Broken**: Simulates a failing chip. Asymmetrical clipping and unpredictable gain.

**Diodes (Clipping Symmetry)**
*   **Silicon**: Standard symmetrical clipping (0.7V). Tight and punchy.
*   **Germanium**: Softer knee, lower threshold (0.3V). Warmer and fuzzier.
*   **LED**: Higher threshold (1.8V), hard clipping. Loud and aggressive.

**Capacitor Age**
*   **0% (New)**: Tight envelopes, infinite sustain (where applicable).
*   **100% (Dead)**: Leaky caps. Shortened decays, loss of low end, "dried out" sound.
