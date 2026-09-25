# FlyBy — 3D Binaural Elevation & Flight Trajectory Plugin

**FlyBy** is a high-fidelity 3D binaural audio plugin (VST3 / AU / Standalone) designed to make sound feel as though it moves realistically **up, down, overhead, and around** the listener wearing headphones.

---

## 1. How Vertical (Elevation) Movement Works in FlyBy

Unlike horizontal panning (which relies simply on Interaural Time & Level Differences: ITD / ILD), **elevation localization is monaural and spectral**:

1. **Pinna Moving Notches ($N_1, N_2$)**:
   - As an object ascends from below to directly overhead, acoustic cancellations created by the folds of the outer ear (pinna concha) shift their spectral notch from $\sim 6.0\text{ kHz}$ upward to $\sim 11.5\text{ kHz}$.
   - FlyBy models these dynamic notches with high-Q biquad sweeps calibrated against human anthropometric measurements.
2. **Torso & Shoulder Comb Reflection**:
   - When a sound drops below eye level ($-10^\circ \text{ to } -50^\circ$), acoustic reflections bouncing off the chest and clavicles introduce a lower-mid cancellation dip ($\sim 850\text{ Hz} - 1.3\text{ kHz}$).
3. **Rayleigh Spherical Head Scattering**:
   - Continuous diffraction filter modeling head shadow and high-frequency acoustic shielding for the contralateral ear.
4. **Sub-Sample Fractional Delay Lines**:
   - 4-point Cubic Hermite interpolated delay lines provide click-free sub-sample ITD and realistic Doppler shift as the source moves through space.

---

## 2. Flight Trajectory Engine

FlyBy features pre-programmed 3D autonomous flight paths:
- **Manual 3D Placement**: Direct interactive drag on the radar or DAW parameter automation.
- **Swoop Dive-Bomb**: High-speed dive starting high in front, swooping right past the ear at low altitude, and climbing behind.
- **Helical Orbit**: Spirals up and down around the head (insect / drone flight).
- **Zenith Flyover**: Directly travels from front horizon through $+90^\circ$ overhead zenith to the rear.
- **Hornet Flutter**: Organic micro-altitude jitter and darting motion.

---

## 3. Aeronautical Carbon & Gold GUI

- **Dual-View Flight HUD**:
  - **Horizontal Radar**: Top-down ($XY$) view with concentric distance rings ($1\text{m} - 4\text{m}$) and cardinal orientation.
  - **Vertical Profile HUD**: Side elevation ($Z$) view showing horizon ($0^\circ$), zenith ($+90^\circ$ overhead), and torso bounce levels ($-40^\circ$).
  - **Live Trajectory Trails**: Phosphor cyan/amber motion trails tracking recent flight paths.
  - **Interactive Dragging**: Click and drag on either radar screen to position the sound source in real time.
- **Personalized Pinna Tuning**:
  - Scale factor ($\pm 25\%$) allowing individual listeners to calibrate notch frequencies to match their personal ear canal shape.
