# Mathematics of Vacuum Tape Simulation

This document formalizes the digital signal processing models to be implemented in the **Vacuum Tape Sim** C++ plugin. These models define the non-linear transfer functions of vintage vacuum tube power supplies and magnetic tape systems.

## 1. Vacuum Tube "Voltage Sag" Compression
The virtual tube models a power supply that "struggles" under load. Heavy transients cause the available supply voltage to drop, creating program-dependent compression.

### 1.1 Current Drain and Recovery (Voltage State)
To prevent the voltage from rippling at the audio rate, the absolute input $|x[n]|$ is first smoothed by a one-pole envelope follower to estimate the energy load $e[n]$:
$$e[n] = e[n-1] + \alpha_{env} \cdot (|x[n]| - e[n-1])$$

The system maintains an internal state variable $V[n] \in [0.0, 1.0]$, representing the available voltage. To ensure smooth, mastering-grade compression without low-frequency ripple, the system uses a **True RMS Detector** with a **Dual-Stage Recovery** and a **Soft-Knee** threshold.

**1. True RMS Detection (Ripple Rejection):**
Instead of tracking peak absolute values, the energy is squared and passed through a 2nd-order (two-pole) low-pass filter to aggressively reject low-frequency ripple while preserving fast transient reaction:
$$x_{sq}[n] = x[n]^2$$
$$e_1[n] = e_1[n-1] + \alpha_{env} \cdot (x_{sq}[n] - e_1[n-1])$$
$$e_{rms}[n] = e_{rms}[n-1] + \alpha_{env} \cdot (e_1[n] - e_{rms}[n-1])$$
*(Using $\tau_{env} \approx 5\text{ms}$).*

**2. Soft-Knee Drain:**
To prevent sharp discontinuities when crossing the threshold $T$, the drain rate uses a squared soft-knee polynomial:
$$Drain[n] = \alpha_{drain} \cdot \max(0, \sqrt{e_{rms}[n]} - T)^2$$

**3. Dual-Stage Program-Dependent Recovery:**
A secondary, slow envelope $e_{slow}[n]$ tracks sustained energy to dynamically adjust the recovery time. If the audio is sustained (like a bassline), recovery is slow ($\tau_{slow} \approx 500\text{ms}$). If it is a fast transient (like a snare), recovery is fast ($\tau_{fast} \approx 50\text{ms}$):
$$e_{slow}[n] = e_{slow}[n-1] + \alpha_{slow\_env} \cdot (e_{rms}[n] - e_{slow}[n-1])$$
$$\beta_{dynamic}[n] = \beta_{fast} \cdot (1 - e_{slow}[n]) + \beta_{slow} \cdot e_{slow}[n]$$

**4. Voltage Update:**
$$V[n] = \max\left(0.1, V[n-1] - Drain[n] + \beta_{dynamic}[n] \cdot (1 - V[n-1])\right)$$

All coefficients ($\alpha, \beta$) are calculated using the standard time-constant formula $\alpha = 1 - \exp(-1 / (\tau \cdot F_s))$.

### 1.2 Dynamic Gain
The output signal $y[n]$ is the input scaled by the available voltage:
$$y[n] = x[n] \cdot V[n]$$

---

## 2. Magnetic Hysteresis (Langevin Model)
To physically model magnetic tape particle saturation, we replace the standard $\tanh$ waveshaper with the **Langevin Function**, which describes anhysteretic magnetization.

### 2.1 The Langevin Function
The Langevin function $L(x)$ is defined as:
$$L(x) = \coth(x) - \frac{1}{x}$$
*(For numerical stability near zero, the Taylor series approximation $L(x) \approx x/3$ is used when $|x| < 10^{-4}$).*

### 2.2 Stateful Hysteresis and Asymmetry
To capture both magnetic lag and even-harmonic generation (asymmetry), the magnetization $y[n]$ tracks the continuous rate of change of the input. We normalize the discrete derivative $\Delta x[n] = (x[n] - x[n-1]) \cdot F_s$ to ensure sample-rate independence:
$$y[n] = L\left(G \cdot (x[n] + x_{bias}) - h \cdot L(\beta \cdot \Delta x[n])\right)$$

- $G$: Drive (Scale factor for input gain).
- $x_{bias}$: A subtle DC offset to introduce asymmetry (even harmonics).
- $h$: Coercivity (Amount of magnetic lag/memory).
- $\beta$: Smoothing factor.

### 2.3 Virtual Bias (Macroscopic Approximation)
Instead of processing an ultrasonic sine wave (which requires 16x oversampling), the macroscopic effects of the tape bias calibration are modeled via a user parameter $B \in [-1.0, 1.0]$, where $0.0$ is perfect calibration.

The Bias parameter dynamically modulates the Hysteresis smoothing ($\beta$) and the High-Frequency Gap Loss cutoff ($f_c$):
- **Smoothing ($\beta$)**: $\beta(B) = \beta_{base} \cdot 2^B$. Over-biasing ($B > 0$) softens the transients, while under-biasing ($B < 0$) causes faster, grittier saturation.
- **High-Frequency Shift**: The gap loss cutoff frequency (defined in Section 4.1) shifts inversely: $f_c(v, B) = f_c(v) \cdot 2^{-B/2}$. Over-biasing darkens the tone, while under-biasing brightens it.

---

## 3. Pre/Post-Emphasis EQ (NAB vs. CCIR)
To combat noise and limitations of magnetic particles, real tape machines apply a pre-emphasis EQ before hitting the tape (the non-linear Langevin curve), and an exact inverse de-emphasis EQ upon playback. Because the signal is saturated *between* these EQs, frequencies boosted by the pre-emphasis will distort earlier.

This is implemented by wrapping the Hysteresis process (Section 2) in Shelf Filters. A time constant $\tau$ maps to a transition frequency $f = \frac{1}{2\pi \tau}$.

### 3.1 NAB (American Standard)
The NAB standard at 15 IPS uses time constants of $3180 \mu\text{s}$ ($\approx 50 \text{ Hz}$) and $50 \mu\text{s}$ ($\approx 3183 \text{ Hz}$).
- **Pre-Emphasis**: Low-shelf boost at 50Hz, High-shelf boost at 3183Hz.
- **De-Emphasis**: Exact inverse Low-shelf and High-shelf cuts.
*(Result: Pushing the input hard saturates the lows and highs first).*

### 3.2 CCIR / IEC (European Standard)
The CCIR standard at 15 IPS uses only a $35 \mu\text{s}$ ($\approx 4547 \text{ Hz}$) time constant, leaving the low-end flat.
- **Pre-Emphasis**: High-shelf boost at 4547Hz.
- **De-Emphasis**: Exact inverse High-shelf cut.
*(Result: Bass stays cleaner, upper-mids saturate first).*

---

## 4. Tape Playback Losses
Frequency-dependent losses are modeled using standard digital biquad filters implementing the Audio EQ Cookbook formulas. The generalized difference equation for a biquad is:
$$y[n] = (b_0 x[n] + b_1 x[n-1] + b_2 x[n-2] - a_1 y[n-1] - a_2 y[n-2]) / a_0$$

### 4.1 High-Frequency "Gap Loss" (Tape Speed)
The cutoff frequency $f_c$ is determined by the physical wavelength of the signal matching the playback head gap. As a result, the cutoff scales linearly with the physical tape speed parameter $v$ (in inches per second, IPS), modified by the Bias parameter $B$:
$$f_c(v, B) = \left(v \cdot 650.0 \text{ Hz}\right) \cdot 2^{-B/2}$$
*(Example: 15 IPS $\approx$ 9.75 kHz cutoff. 30 IPS $\approx$ 19.5 kHz cutoff).*
Coefficients are calculated using $\omega_0 = 2\pi f_c / F_s$ and $\alpha = \sin(\omega_0) / (2Q)$.

### 4.2 Low-Frequency "Head Bump" (Contour Effect)
To model the geometric resonance of the playback head, a Peaking EQ filter is applied. Because this resonance is tied to physical wavelength, the center frequency shifts dynamically with tape speed $v$:
$$F_{bump}(v) = v \cdot 3.0 \text{ Hz}$$
*(Example: 15 IPS $\approx$ 45 Hz bump. 30 IPS $\approx$ 90 Hz bump).*

The filter uses $Q \approx 1.5$ and $\text{Gain} \approx +2.5 \text{ dB}$. The coefficients are calculated using the peaking EQ formulas, mapping $A = 10^{\text{gain}/40}$.

### 4.3 Low-Frequency Roll-Off (Faraday's Law)
Due to Faraday's law of induction, the output voltage of a tape playback head is proportional to the time derivative of the magnetic flux ($d\Phi/dt$). Therefore, a playback head cannot induce a current from static (DC) or extremely low-frequency magnetic fields. This causes a natural 6dB/octave roll-off toward 0Hz.
To physically model this, we apply a **1st-order High-Pass Filter** with a cutoff frequency around $25\text{ Hz}$.

---

## 5. Tape Transport (Wow & Flutter)
Mechanical instability is modeled as a modulated fractional delay line.

### 5.1 Mean-Reverting Wow (Ornstein-Uhlenbeck Process)
The Wow frequency follows an Ornstein-Uhlenbeck stochastic process, returning to a center value $\mu$:
$$f_{wow}[n] = f_{wow}[n-1] + \theta (\mu - f_{wow}[n-1]) + \sigma \epsilon[n]$$

### 5.2 Fractional Delay
The output is read from a circular buffer using a reading pointer $P[n]$ modulated by the LFOs:
$$P[n] = n - \left(D_{base} + D_{wow} \cdot A_{wow}[n] + D_{flutter} \cdot N[n]\right)$$
*(Sub-sample accuracy is maintained via 3rd-order Hermite interpolation).*
