# Discrete808: Mathematical & DSP Reference

This document outlines the mathematical models used in the **Discrete808** emulation. We utilize white-box modeling techniques to convert analog circuit topologies into discrete-time digital structures.

## 1. Bridged-T Resonator (Drum Body)
The core of the Kick and Snare voices is the **Bridged-T Network**, a specific bandpass filter topology that rings when excited by a trigger pulse.

### Analog Transfer Function
The continuous-time transfer function $H(s)$ for a generic bandpass filter is:

$$H(s) = \frac{s \cdot (\omega_c / Q)}{s^2 + s \cdot (\omega_c / Q) + \omega_c^2}$$

Where:
*   $\omega_c = 2\pi f_c$ (Cutoff/Resonant Frequency)
*   $Q$ = Quality Factor (Resonance)

### Digital Implementation: Topology Preserving Transform (TPT)
To preserve the topology and allow for stable modulation of cutoff and Q (essential for the 808's behavior), we use the **Trapezoidal Integration** method (Bilinear Transform) within a Zero-Delay Feedback (ZDF) structure.

**Mapping:**
$$s \leftarrow \frac{2}{T} \frac{1 - z^{-1}}{1 + z^{-1}}$$

**Tuning Coefficient ($g$):**
To approximate the frequency response accurately:
$$g = \tan(\pi \frac{f_c}{f_s})$$

**Damping Coefficient ($k$):**
$$k = \frac{1}{Q}$$

**The TPT Discrete Structure:**
1.  Calculate Highpass prediction ($v_{hp}$), Bandpass prediction ($v_{bp}$), and Lowpass prediction ($v_{lp}$).
2.  Solve for the feedback loop without delay (Zero Delay Feedback).

The state update equations used in `ZDFResonator.h`:
$$v_{bp} = \frac{g(x - s_1) - g \cdot s_2 - g^2 \cdot s_1}{1 + g(g + k)}$$
(Simplified in code for optimization)

## 2. Metallic Oscillators (Hi-Hats)
The TR-808 Hi-Hats use a "Schmitt Trigger" bank of 6 square wave oscillators.

### Alias-Free Generation: PolyBLEP
Naive digital square waves produce infinite harmonics, causing aliasing (audible birds/inharmonic tones) when these harmonics exceed the Nyquist limit ($f_s / 2$).

We use **PolyBLEP** (Polynomial Bandlimited Step) to correct the discontinuities.

For a phase wrap at time $t$ (where the naive waveform jumps), we add a polynomial residual:

For $0 < t < \Delta t$:
$$P(t) = 2 \frac{t}{\Delta t} - (\frac{t}{\Delta t})^2 - 1$$

For $1 - \Delta t < t < 1$:
$$P(t) = (\frac{t-1}{\Delta t})^2 + 2\frac{t-1}{\Delta t} + 1$$

Where $\Delta t$ is the phase increment per sample.
This turns the hard edge into a band-limited slope, suppressing aliasing by ~60dB.

## 3. Circuit Non-Linearities

### The "Sigh" (Kick Pitch Envelope)
The 808 Kick's pitch drop is not linear. It follows the discharge curve of a capacitor.
We model this as an exponential decay applied to the frequency control of the Bridged-T resonator.

$$f(t) = f_{base} + \Delta f \cdot e^{-t / \tau}$$

This "chirp" is what gives the 808 kick its distinctive "thump" attack.

### Operational Amplifier Saturation
The output stage uses uPC4558 op-amps running on a specific rail voltage. We model this saturation using a tunable `tanh` function:

$$y = \tanh(k \cdot x)$$

This creates odd harmonics and soft natural compression.

### Slew Rate Limiting
Vintage op-amps cannot change voltage instantaneously. The maximum rate of change (Volts per microsecond) is limited.
Digital signals can theoretically jump from -1.0 to 1.0 in one sample (infinite slew).

We implement a discrete Slew Limiter:
$$\Delta_{max} = \frac{V_{limit} \cdot 10^6}{f_s}$$
$$y[n] = y[n-1] + \text{clamp}(x[n] - y[n-1], -\Delta_{max}, \Delta_{max})$$

This acts as a non-linear lowpass filter that only affects high-frequency, high-amplitude transients, softening the "click" of the digital envelopes.
