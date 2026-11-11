# Kindnes (v0.0.4): High-Fidelity 8-Bit 2A03 APU, FDS 2C33, VRC6 Synth & Advanced Drum Designer

**Kindnes** is an authentic and extended recreation of the classic **8-Bit 2A03 / 2A07 APU** (Audio Processing Unit) and vintage expansion audio hardware (**FDS 2C33** & **VRC6**), incorporating hardware-accurate synthesis mechanisms modeled after **Dn-FamiTracker** (`nsfplay` & `Mesen` cores). It bridges cycle-accurate vintage chiptune silicon with modern sound design tools—supporting Polyphonic APU chords, continuous PWM, tracker-style 60Hz/50Hz frame-rate macros with fixed-note transients (`@note`), drag-and-drop 1-bit DPCM sample encoding, and resonant ZDF filtering.

---

## 📦 Direct Downloads (GitHub Releases — 1-Click, No Login Required)

* 🍎 **[Download Kindnes-macOS-Universal.zip](https://github.com/kijjaz/audio-plugins/releases/download/kindnes-v0.0.4/Kindnes-macOS-Universal.zip)** *(Universal binary: Apple Silicon arm64 & Intel x86_64 — VST3, AU, Standalone App)*
* 🪟 **[Download Kindnes-Windows-x64.zip](https://github.com/kijjaz/audio-plugins/releases/download/kindnes-v0.0.4/Kindnes-Windows-x64.zip)** *(Windows 64-bit VST3)*
* 🎛️ **[Download Kindnes-SFZ-Pack-96k-24bit.zip](https://github.com/kijjaz/audio-plugins/releases/download/kindnes-v0.0.4/Kindnes-SFZ-Pack-96k-24bit.zip)** *(Complete 96kHz 24-bit SFZ Sample Pack: 11 basic waveforms, 61 chromatic drum/SFX voices, and 36 vintage arcade speech voices)*
* 📦 **[All GitHub Releases](https://github.com/kijjaz/audio-plugins/releases)**

---

## 🌟 What's New in v0.0.4: Multi-Curve Pitch Engine, Transient Clicks & Chromatic Drum Synth

* **Multi-Curve Pitch Modulation Engine**:
  * **5 Pitch Envelope Shapes**: `Exponential Drop` (classic 808/chiptune kick dive), `Dual-Stage Click Dive` (ultra-steep attack transient for maximum punch), `Laser Linear Sweep` (retro arcade laser zappers), `Pitch Scoop Rise` (comic spring / boing upward glide), and `FM Chirp Modulation` (Simmons drum warp & UFO sci-fi effects).
  * **Dedicated Sweep Time & FM Controls**: Configurable Pitch Sweep Time ($5\text{ ms}\dots 300\text{ ms}$), FM Modulation Rate ($5\text{ Hz}\dots 200\text{ Hz}$), and FM Modulation Depth ($0\dots 500\text{ Hz}$).

* **Dedicated Transient Click Layer**:
  * 4 selectable transient attack modes: `Off`, `2A03 Impulse Spike` (single-sample hardware DAC impulse), `Square Pop` (short pulse attack transient), and `Noise Snap` (high-frequency burst).
  * Independent click level slider ($0.0\dots 1.0$) to guarantee drum attacks punch through dense chiptune leads.

* **Expanded Noise Patterns & Cinematic Explosion Decay**:
  * Added `Low-Rumble Swell` (sine-ramped noise envelope) and `Glitch Stutter` (chopped frame-rate bursts).
  * Noise decay extended from $600\text{ ms}$ up to **$4000\text{ ms}$ (4.0 seconds)** for dramatic chiptune boss battle defeat explosions.

* **Direct DAW MIDI Triggering & Tuned Chromatic Synth Mode**:
  * **Audition Only**: Internal testing and auditioning via the UI pad.
  * **MIDI Note 36/38 (Kick/Snare)**: Play custom drum presets alongside standard MIDI drum tracks.
  * **Tuned Chromatic Synth**: Play custom drum designs across the full 88-key keyboard—turning the drum synthesis engine into a playable 8-bit 808 sub-bass, tuned tom synth, or chromatic arcade sound effect!

* **Expanded Algorithmic Templates (17 Instant Presets)**:
  * Includes new factory templates: `Boss Defeat Cataclysm` (epic 2.4s low-rumble noise swell) and `Comic Spring Boing` (upward pitch scoop rise).

---

## 🌟 Previous Features in v0.0.3: Algorithmic Drum Sound Designer

* **Dedicated 5-Tab Deck Architecture**:
  1. `1. APU & VRC6`: Pulse 1 & 2 (with PolyBLEP PWM), Triangle, Galois LFSR Noise, VRC6 wrapping sawtooth & FDS 2C33 wavetable FM.
  2. `2. DRUM ROM (32)`: 32 built-in 8-bit & vintage 1-bit DPCM drum/sfx voices across 4 banks with 8 large interactive pads.
  3. `3. DRUM DESIGNER`: Algorithmic drum synthesis workspace exposing all internal components of classic chiptune drum construction.
  4. `4. TRACKER FX`: 60Hz/50Hz NTSC/PAL frame sequencers, pitch scoops, delayed vibrato, duty morphing, and pseudo-echo.
  5. `5. MASTER & SVF`: Resonant zero-delay-feedback filter, authentic motherboard RC circuits, ADSR envelope, stereo unison, and output trim.

* **"Easy (Macro)" vs "Flexible (Deep)" Drum Sculpting**:
  * **Easy Mode**: 4 master macro dials for fast, musical drum shaping:
    - `Punch`: Exponential pitch drop depth and steepness.
    - `Body Length`: Envelope decay time of the low-end tonal foundation.
    - `Snappy`: LFSR noise burst intensity and crispness.
    - `1-Bit Grit`: Delta modulation slope overload saturation.
  * **Flexible Mode**: Surgical control across all three physical sound design layers:
    1. **Tonal Body & Pitch Sweep**: Start Frequency (Hz), Target Frequency (Hz), Exponential Pitch Decay (ms), Waveform Selector (2A03 4-Bit Triangle, Saturated Sine, VRC6 Overdrive Saw, 25% Pulse), and Body Level.
    2. **LFSR Noise Transient**: LFSR Mode (Mode 0 White 32,767-step vs Mode 1 Metallic 93-step), Tone Shift Period (0-15), Noise Decay (ms), Envelope Pattern (Single Burst, Multi-Flam Clap 3-burst, Gated Boss Snare Plateau), and Noise Level.
    3. **1-Bit DPCM Hardware Crusher**: Delta Step Size (1-8), Sample Rate Index (0-15), Slew Overload Grit, Synth/DPCM Cross-Blend, and Paired ROM Sample Voice.
  * **Live Real-Time Audition Pad**: Prominent "▶ TEST DRUM SOUND" button with instant response to audition tweaks on the fly.
  * **15 Instant Algorithmic Templates**: One-click recall for Punch Kick, Mega Snare, Hi-Hats, Hand Clap, Rack/Floor Toms, Crash Cymbal, VRC6 Overdriven Kick, 93-Step Snare, Chiptune Rimshot, Cowbell, Simmons Tom, Boss Battle Snare, and Laser Zapper FX.

---

## 🌟 Key Highlights & DSP Innovations

### 1. Cycle-Accurate Ricoh 2A03 APU Core
* **Pulse 1 & 2**: True 4-step hardware duty cycles (`12.5%`, `25.0%`, `50.0%`, and `75.0%` with authentic 180° inverted Duty 1 phase) + sub-sample PolyBLEP anti-aliasing. Includes hardware pitch sweeps with the Pulse 1 one's-complement quirk ($t - \Delta - 1$) vs Pulse 2 two's-complement ($t - \Delta$).
* **Triangle Channel**: Authentic 32-step 4-bit staircase lookup sequence (`15..0..15`) with PolyBLEP staircase edge anti-aliasing and real hardware **DC-offset step pops** when halted, producing the iconic punchy low-frequency transient bite.
* **Sub-Cycle Anti-Aliased Galois LFSR Noise (`nsfplay` Method)**: 15-bit Galois Linear Feedback Shift Register with sub-sample time-weighted state averaging (`accum / accum_clocks`) eliminating point-sampling aliasing at ultrasonic shift rates (up to $447.4\text{ kHz}$):
  - **Mode 0 (Long)**: 32,767-step pseudo-random white noise for snares, explosions, and wind.
  - **Mode 1 (Short)**: 93-step periodic metallic looping buzz for robotic drums, cymbals, and tonal rings.
* **1-bit DPCM Delta Modulation**: 7-bit counter DAC with 16 standard NTSC clock dividers ($4.18\text{ kHz}$ to $33.14\text{ kHz}$) and built-in factory ROM drum samples.

### 2. Famicom Disk System (FDS 2C33) Hardware FM & Wavetable Expansion
* **64-Step 6-Bit Wavetable Oscillator**: Arbitrary 6-bit wavetable resolution ($0\dots 63$) with PolyBLEP staircase anti-aliasing reproducing legendary FDS cartridges (*The Legend of Zelda*, *Metroid*, *Castlevania II*).
* **Authentic 2C33 Hardware FM Modulation Unit**: 64-step 3-bit modulation table (`{0, +1, +2, +4, Reset, -4, -2, -1}`), 7-bit signed `_counter` ($-64\dots +63$), and exact 2C33 fixed-point pitch multiplier & wrap-around arithmetic with **Auto-FM** chromatic harmonic tracking.
* **Post-DAC 1-Pole RC Lowpass Filter**: Dedicated sample-rate invariant $\sim 2.4\text{ kHz}$ RC roll-off matching the Famicom Disk System RAM Adapter analog stage.

### 3. VRC6 Audio Expansion (8-Bit Wrapping Sawtooth & 16-Step Pulse)
* **14-Step 8-Bit Wrapping Sawtooth Accumulator**: Emulates the exact VRC6 `$B000` 6-bit accumulator added on even steps (`(k * accumRate) & 0xFF`, outputting top 5 bits `accum >> 3`). Pushing `VRC6 SAW` to `14–15` triggers the iconic **Dn-FamiTracker 8-bit wrap-around double-saw overdrive**!
* **16-Step Thin Pulse Accompaniment**: Adds VRC6 `1/16` ($6.25\%$) nasal pulse bite alongside the sawtooth.

### 3. Note Effects & Chiptune Embellishments
* **Pitch Scoop & Laser Dive**: Attack transient pitch glides ($-12$ to $+12$ semitones) with microsecond exponential recovery.
* **Delayed Vibrato**: Pitch vibrato that blooms smoothly after a user-defined hold delay, mimicking authentic tracker note macros.
* **Duty Cycle Attack Morph**: Transient duty switching (e.g., sharp $12.5\%$ attack bite morphing automatically into $50\%$ square body).
* **Chiptune Pseudo-Echo**: Automatic ghost note delay line triggering Pulse 2 with configurable frame delay and volume ducking.

### 4. 1-Bit DPCM Sampler Expansion & Drum Machine
* **8-Pad Drum Machine**: Instant on-screen clickable pads or MIDI notes $36\dots 43$ ($C1\dots G1$) for:
  `01. Kick`, `02. Snare`, `03. Hi-Hat`, `04. Slap Bass`, `05. Tom`, `06. Hit`, `07. Speech ("Kindnes")`, `08. Laser`.
* **4-Voice Polyphonic DPCM Playback**: Trigger kicks, snares, and hats simultaneously without voice stealing!
* **Velocity-to-Slew Grit Modulation**: Soft velocity hits trigger heavy "slope overload" (fuzzy 1-bit saturation), while hard hits track transients cleanly.
* **Real-Time Through-DPCM Crusher**: Route external DAW audio tracks through the 2A03 1-bit delta encoder in real-time.
* **Dual Visualizer**: Real-time oscilloscope on the left + live 1-bit DPCM stepped delta staircase analyzer on the right.
* **Drag & Drop**: Drop any `.wav`, `.aif`, or `.mp3` file to encode and preview in 1-bit DPCM on the fly.

### 5. Authentic Non-Linear MOS Resistor Mixing
The 8-bit audio hardware path does **not** sum channels linearly. *Kindnes* implements the exact non-linear resistive transfer equations:
$$\text{pulse\_out} = \frac{95.88}{\frac{8128}{\text{pulse}_1 + \text{pulse}_2} + 100}$$
$$\text{tnd\_out} = \frac{159.79}{\frac{1}{\frac{\text{triangle}}{8227} + \frac{\text{noise}}{12241} + \frac{\text{dmc}}{22638}} + 100}$$
Because the Triangle, Noise, and DPCM channels share the same resistor divider network, playing loud DPCM samples dynamically **ducks and compresses** the triangle bass line, faithfully capturing the physical sound of vintage 8-bit console hardware under heavy load.

### 6. Motherboard Analog Filtering & ZDF Filter
* **90 Hz High-Pass Filter**: DC decoupling and sub-rumble cutoff.
* **440 Hz High-Pass Filter**: Passive low-end tilt shaping creating the tight 8-bit mid-range punch.
* **14 kHz 3-pole Low-Pass Filter**: Smooths out ultrasonic clock imaging, providing warm vintage CRT TV acoustics.
* **Resonant Multi-Mode Filter (ZDF SVF)**: Low-Pass, High-Pass, Band-Pass, and Notch with adjustable Resonance Q (0.1 to 8.0).
* **Exponential ADSR Envelope**: Smooth Attack (up to 1s), Decay (up to 2s), Sustain, and Release (up to 3s) tails for warm 8-bit pads, leads, and plucks.

---

## 🎛️ Architecture & Controls

| Section | Control | Description |
| :--- | :--- | :--- |
| **Header** | **Visualizer** | Real-time oscilloscope with phosphor glow and APU channel telemetry. |
| **Presets** | **Browser** | 32 factory presets across 7 categories (`[DRUMS]`, `[LEAD]`, `[BASS]`, `[ARP]`, `[KEYS]`, `[PAD]`, `[DPCM]/[SFX]`), strictly calibrated to $0.0\text{ dB}$ unity trim. |
| **Pulse 1 & 2** | **Duty / Vol** | Select $12.5\%$, $25\%$, $50\%$, $75\%$, or PWM mode; 4-bit volume (0..15). |
| **Triangle** | **Tri Vol** | 4-bit stepped staircase DAC volume with DC-offset pop attack. |
| **Noise** | **LFSR Mode / Vol** | Mode 0 (White) or Mode 1 (Metallic 93-step loop); 16 timer rates. |
| **VRC6** | **Expansion / Saw** | VRC6 extra pulse & 14-step staircase sawtooth generator. |
| **FDS 2C33** | **Wave / FM Speed / Depth** | 64-step wavetable oscillator with 64-step hardware frequency modulator. |
| **Embellishments** | **Scoop / Vib / Echo / Morph**| Tracker-style pitch scoop, delayed vibrato bloom, pseudo-echo, duty morph. |
| **DPCM 1-Bit** | **32 Voices / 4 Banks / Auto Beat** | 8 glowing pads with 4 bank selections (32 built-in hybrid voices), Open Hi-Hat choke, 1-click `AUTO BEAT`, full-keyboard & split MIDI drum modes (`< C2` drums, `C2+` melodic synth). |
| **Filter & Env** | **SVF / ADSR / Tilt** | Multi-mode resonant ZDF filter, analog motherboard tilt, ADSR envelope. |
| **Master** | **Unison / Spread / Trim** | Stereo unison detune, stereo spread, master trim (resting at $0.0\text{ dB}$). |

---

## 🧪 Scientific & Engineering Standards Compliance

* **Loudness Standard**: All 32 factory presets load with `masterTrim = 0.0 dB` (unity gain). DSP channel gains are balanced internally to nominal $-14.4\text{ dBFS}$ / LUFS.
* **Sample-Rate Invariance**: Continuous-time filter coefficients and PolyBLEP step corrections scale dynamically with `rateRatio = 44100.0 / fs`. Verified at $44.1\text{ kHz}$ and $96.0\text{ kHz}$ ($\Delta\text{RMS} \le 0.15\text{ dB}$, zero NaNs/infinities).
