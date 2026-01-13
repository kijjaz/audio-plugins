# Vacuum Tape Sim v0.2

![Vacuum Tape Sim Header](Gemini_Generated_Image_1bzjwn1bzjwn1bzj.png)

A high-fidelity digital signal processing plugin designed to emulate the non-linear characteristics of vintage vacuum tube amplification and magnetic tape recording.

## Overview

**Vacuum Tape Sim** combines a unique "Voltage Drain" vacuum tube model with stateful magnetic hysteresis and mechanical transport imperfections. It is designed to add warmth, character, and organic movement to any audio source, from subtle saturation to extreme tape destruction.

## Key Features

### 1. Vacuum Tube Compression (Voltage Drain Model)
Unlike standard digital compressors, this model emulates the physical "sag" of a vacuum tube's power supply.
- **Dynamic Response**: The system maintains an internal "headroom" state ($V[n]$) that decays when the signal exceeds a threshold and recovers over time.
- **Organic Saturation**: As the voltage drains, the signal naturally compresses and saturates, mimicking the behavior of vintage laboratory equipment.

### 2. Magnetic Tape Hysteresis
A stateful saturation model that accounts for the magnetic "memory" of tape particles.
- **Hysteresis Coercivity**: Uses a modified tanh model with lag to simulate the non-linear relationship between magnetic field strength and tape magnetization.
- **Fast Tracking**: Designed for high-speed response to transient information.

### 3. Frequency-Dependent Losses (Wallace Losses)
Emulates the high-frequency attenuation inherent in physical tape recordings.
- **Tape Speed (IPS)**: Selectable speeds (9.5, 15, and 30 ips) dynamically adjust a first-order IIR low-pass filter, shifting the cutoff frequency based on the selected playback rate.

### 4. Mechanical Transport Simulation
- **Wow**: Low-frequency speed fluctuations modeled via a "drunk walk" LFO.
- **Flutter**: High-frequency speed fluctuations modeled using randomized noise.
- **Cubic Interpolation**: High-quality fractional delay lines ensure smooth modulation without artifacts.

## Parameters

### Compressor Group
| Parameter | Range | Description |
| :--- | :--- | :--- |
| **Threshold** | 0.01 - 1.00 | Sensitivity of the voltage drain mechanism. |
| **Drain** | 0.1 - 50.0 | Speed at which the internal voltage drops once the signal exceeds the threshold. |
| **Recovery** | 0.1 - 50.0 | Speed at which the internal voltage returns to its resting state. |
| **Comp Time** | 0.01 - 1.00 | Master time scaling factor for the compression circuit. |

### Tape Group
| Parameter | Range | Description |
| :--- | :--- | :--- |
| **Drive** | 1.0 - 10.0 | Input gain into the tape saturation stage. |
| **Hysteresis** | 0.00 - 0.50 | Amount of magnetic lag (coercivity) in the saturation model. |
| **Tape Speed** | 9.5, 15, 30 | Playback speed in inches per second (ips), affecting high-frequency response. |
| **Wow** | 0.00 - 1.00 | Depth of the slow-rate speed modulation. |
| **Flutter** | 0.00 - 1.00 | Depth of the fast-rate noise-based speed modulation. |

### Final Stage
| Parameter | Range | Description |
| :--- | :--- | :--- |
| **Output Volume** | -24 to +24 dB | Final gain adjustment post-processing. |
| **Oversampling** | 1x, 2x, 4x, 8x | Internal upsampling to reduce aliasing artifacts from non-linear processes. |

## Technical Details

- **Framework**: Built with JUCE 8 and C++20.
- **Performance**: High-density engine with up to **8x oversampling** for zero-aliasing saturation.
- **Aesthetic**: **v0.2 "Carbon & Gold"** high-density UI featuring a laboratory visualizer, reel animations, and 3D textured controls.

## Build Instructions

This project uses CMake. To build the plugin:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---
*Created by Kijjasak Triyanond & Antigravity (2026)*
*Part of the Audio Research Archive*
