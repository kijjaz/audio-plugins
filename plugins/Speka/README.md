# Speka

**Speka** is an audio engine and DSP plugin that physically simulates digitized 1-bit audio playback through vintage internal PC speakers (such as the IBM 5150, PC/XT, AT, and 1990s PC clones).

Rather than acting as a simple bitcrusher or 1-bit square wave clipper, Speka couples four interacting physical and electromechanical layers:

1. **Digital Modulation & Timer Layer**:
   - Intel 8253/8254 Programmable Interval Timer (PIT) running at master clock $1,193,181.\overline{81}\text{ Hz}$.
   - **RealSound PWM**: Variable carrier frequency ($8\text{ kHz}$ to $28\text{ kHz}$) with timer reload quantization into discrete ticks (e.g. 63 ticks at $18.9\text{ kHz} \approx 6.0\text{ bits}$).
   - **Sigma-Delta PDM**: 2nd-order noise shaping structure.
   - **Delta Modulation** & **Direct 1-Bit Beeper** modes.
   - CPU interrupt latency & DRAM refresh DMA cycle jitter simulation.
   - PolyBLEP band-limited transition smoothing to eliminate foldover aliasing.

2. **Electrical Driver Stage**:
   - Unipolar single-ended 7406 open-collector inverter pull-down switching.
   - $33\,\Omega$ ballast resistor + $8\,\Omega$ voice coil nominal resistance.
   - Voice coil inductance ($L_e = 0.2\text{ mH}$) with motional back-EMF ($e_b = Bl \cdot v$).
   - Dynamic power supply rail sag under heavy duty-cycle current demand.

3. **Electromechanical Transducer Physics**:
   - **2.25-Inch Paper Cone**: Coupled 2nd-order non-linear mass-spring-damper dynamics ($f_0 \approx 350\text{ Hz}, Q_{ms} \approx 4.4$).
   - Progressive suspension stiffening $K(x) = k_0 (1 + \beta_2 x^2 + \beta_4 x^4)$ and spider mechanical bottoming limits.
   - Magnetic flux decay $Bl(x) = Bl_0 \exp(-x^2 / (2 \sigma^2))$.
   - Radiated sound pressure proportional to **cone acceleration** ($\ddot{x} = dv/dt$), naturally differentiating the unipolar DC bias into alternating acoustic pressure.
   - Diaphragm breakup modes ($2.4\text{ kHz}, 3.75\text{ kHz}, 5.1\text{ kHz}$).
   - **12mm Ceramic Piezo Buzzer**: High-pass low-end cutoff ($1.2\text{ kHz}$) with a sharp piercing ceramic resonance ($3.25\text{ kHz}, Q = 8.5$).

4. **Acoustic Enclosure & Chassis Layer**:
   - Internal steel case cavity standing wave modes ($185\text{ Hz}, 340\text{ Hz}, 520\text{ Hz}, 790\text{ Hz}$).
   - Front plastic slotted bezel acoustic low-pass screen ($4.5\text{ kHz}$).
   - Excursion-triggered chassis screw & sheet metal rattle/chatter generator.

---

## Documentation & Mathematical Derivations

The full mathematical formulation, circuit diagrams, and acoustic equations are documented in:
- LaTeX source: [`SpekaMath.tex`](file:///Users/kijjaz/Desktop/Antigravity/2026/20260926%20Audio%20Plugins/plugins/Speka/SpekaMath.tex)
- Compiled paper: [`SpekaMath.pdf`](file:///Users/kijjaz/Desktop/Antigravity/2026/20260926%20Audio%20Plugins/plugins/Speka/SpekaMath.pdf)

---

## Building and Running the CLI

### Direct Build with Clang / GCC
```bash
clang++ -std=c++17 -O3 -I Source tools/SpekaCLI.cpp -o speka_cli
```

### CMake Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

---

## CLI Usage

```bash
./speka_cli [options]
```

### Options
| Flag | Description | Default |
| :--- | :--- | :--- |
| `-i, --input <file.wav>` | Input audio WAV file (required) | — |
| `-o, --output <file.wav>` | Output audio WAV file | `output_speka.wav` |
| `-m, --mode <mode>` | `realsound`, `sigmadelta`, `deltamod`, `direct` | `realsound` |
| `-c, --carrier <hz>` | PWM carrier frequency in Hz (e.g. 18900, 16500, 11025) | `18900` |
| `-s, --speaker <type>` | `paper` (2.25" IBM 5150 cone) or `piezo` (12mm buzzer) | `paper` |
| `-d, --drive <dB>` | Input pre-drive gain in dB | `0.0` |
| `-r, --rattle <0.0-1.0>` | Chassis screw & panel rattle amount | `0.35` |
| `-k, --case <0.0-1.0>` | Steel case cavity resonance amount | `0.60` |
| `-j, --jitter <0.0-1.0>` | CPU interrupt & DRAM refresh latency jitter | `0.15` |
| `-p, --sag <0.0-1.0>` | Power supply rail droop | `0.50` |
| `-u, --whine <0.0-1.0>` | Carrier bleed / high whistle amount (0=clean/warm, 1=raw) | `0.05` |
| `-g, --gate <0\|1>` | Auto-gate during silence / pauses (0=off, 1=on) | `1` (on) |
| `--gate-thresh <dB>` | Auto-gate silence threshold in dBFS | `-72.0` |
| `-w, --mix <0.0-1.0>` | Dry/Wet blend | `1.0` |
| `-t, --trim <dB>` | Output trim gain in dB | `0.0` |
| `--mono` | Force stereo input to mono PC speaker | `false` |

### Example Commands
```bash
# Classic 1989 Access Software RealSound (Mean Streets style)
./speka_cli -i speech.wav -o pc_speech.wav -m realsound -c 18900 -s paper -d 4.0

# 1992 Pinball Fantasies style (16.5 kHz carrier)
./speka_cli -i music.wav -o pinball_speaker.wav -m realsound -c 16500 -s paper

# 12mm Ceramic Piezo Buzzer in a clone laptop / desktop
./speka_cli -i sound.wav -o piezo_sound.wav -m realsound -c 18900 -s piezo -d 6.0

# High-resolution 2nd-order Sigma-Delta PDM
./speka_cli -i drum.wav -o sigmadelta_drum.wav -m sigmadelta -s paper
```

---

## Plugin (VST3 / AU / Standalone) — Version 0.1.0

Speka is built with a 1980s IBM PC desktop aesthetic, featuring:
- **IBM 5150 Beige Molded Plastic Chassis**: Debossed stamped typography, model identification badge (`IBM PC 5150 v0.1.0`), and jeweled red activity/squelch LED.
- **Transducer Bay**: Recessed speaker compartment behind horizontal ventilation slats with real-time animated excursion of the **2.25″ Alnico Paper Cone** or **12mm Ceramic Piezo Buzzer**.
- **Amber CRT Oscilloscope**: IBM 5151 monochrome amber phosphor screen with real-time waveform beam, reticle grid, and BIOS/DOS 8253 diagnostic telemetry.
- **Ribbed Vintage Knobs & Squelch Rocker**: Molded dials with recessed amber LCD digital value readouts.

### Internal Component Calibration (All Presets at 0.0 dB Unity Trim)

Instead of using the user-facing **Output Trim** knob as an artificial patch, all internal DSP components have been physically calibrated so that switching modulation modes, transducers, and drive stages natively delivers equal perceived loudness (~ -14.4 dBFS):
- **Direct 1-Bit Modulator**: Rail-to-rail square wave driver pulse scaled by 0.28x (-11.1 dB) so high-crest-factor comparator switching matches the acoustic energy envelope of modulated PWM.
- **Piezoceramic Radiation**: In `processPiezo()`, ceramic resonance saturation scalar calibrated from 2.2x to 3.45x to match the acoustic power of the 2.25″ paper cone.
- **1-Bit Delta Modulation**: Slope integrator with demodulated voice-coil drive (0.09x step, 0.95 leaky memory) delivering full Sierra-style speech grit at nominal audio power.
- **Sigma-Delta PDM**: 2nd-order CIFB noise-shaping calibrated with +2.6 dB makeup to match PWM duty-cycle energy.
- **Pre-Drive Saturation Makeup**: Non-linear compensation ($1 / 	ext{drive}^{0.80}$) prevents high overdrive settings from blasting the output while preserving intense harmonic distortion.

| Preset Name | Modulation Mode | Carrier (Hz) | Transducer | Drive | Sag | Rattle | Preset Trim | Output RMS | Character / Application |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **1. IBM 5150 RealSound (Default)** | RealSound PWM | 18,900 | 2.25″ Paper | 0.0 dB | 0.40 | 0.25 | **0.0 dB** | -14.62 dBFS | Authentic 1989 Access Software 6-bit PWM |
| **2. 1989 Xenon 2 / Speedball** | RealSound PWM | 11,025 | 2.25″ Paper | +2.0 dB | 0.50 | 0.40 | **0.0 dB** | -13.93 dBFS | Gritty low-carrier Bitmap Brothers soundtrack |
| **3. Commander Keen PC Beeper** | Direct 1-Bit | 18,900 | 2.25″ Paper | +4.0 dB | 0.20 | 0.15 | **0.0 dB** | -14.41 dBFS | High-energy square wave clicks & vintage beeps |
| **4. Budget Clone Piezo Screamer** | Sigma-Delta PDM | 22,050 | 12mm Piezo | -3.0 dB | 0.30 | 0.10 | **0.0 dB** | -14.35 dBFS | Piercing 4 kHz ceramic disc resonance |
| **5. Sierra On-Line 1-Bit Voice** | Delta Modulation | 14,000 | 2.25″ Paper | +3.0 dB | 0.60 | 0.35 | **0.0 dB** | -14.78 dBFS | Slew-rate limited digitized speech (Space Quest III) |
| **6. Overdriven XT Motherboard** | RealSound PWM | 16,000 | 2.25″ Paper | +10.0 dB | 0.75 | 0.70 | **0.0 dB** | -14.08 dBFS | Heavy coil saturation, supply droop & chassis rattle |
| **7. Ultra-Clean 1-Bit Hi-Fi** | Sigma-Delta PDM | 24,000 | 2.25″ Paper | -1.0 dB | 0.05 | 0.00 | **0.0 dB** | -14.46 dBFS | Clean 2nd-order noise shaped modern 1-bit audio |
| **8. Arcade Steel Chassis Bezel** | RealSound PWM | 18,900 | 2.25″ Paper | +1.5 dB | 0.35 | 0.50 | **0.0 dB** | -13.72 dBFS | Deep steel enclosure standing waves & acoustic bleed |

---

### Plugin Locations (macOS)
- **VST3**: `~/Library/Audio/Plug-Ins/VST3/Speka.vst3`
- **AU**: `~/Library/Audio/Plug-Ins/Components/Speka.component`
- **Standalone**: `plugins/Speka/build/Speka_artefacts/Release/Standalone/Speka.app`

