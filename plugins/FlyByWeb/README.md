# 🛩️ FlyByWeb

[![License: MIT](https://img.shields.io/badge/License-MIT-gold.svg)](https://opensource.org/licenses/MIT)
[![Web Audio API](https://img.shields.io/badge/Web%20Audio-AudioWorklet-brightgreen.svg)](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet)
[![Zero Dependencies](https://img.shields.io/badge/dependencies-0-blue.svg)](#)
[![Bundle Size](https://img.shields.io/badge/bundle%20size-18%20KB-success.svg)](#)

> **Zero-Asset, Ultra-Lightweight 3D Binaural Spatialization Engine for Web Audio, WebGL, and WebXR**  
> *Created by Kijjaz • Gemini 3.8 Flash*  
> *Official Open-Source Web Audio port of the FlyBy DSP spatial engine.*

---

## 📖 What is FlyByWeb?

**FlyByWeb** brings high-fidelity, physics-based 3D binaural audio positioning directly to web browsers without downloading massive SOFA files or multi-megabyte impulse response (IR) bundles.

Traditional browser spatialization approaches force a trade-off:
- The browser's native `PannerNode` (`panningModel: 'HRTF'`) uses a rigid, generic dataset that **blurs punchy drum transients into phasey mush**, lacks anatomical elevation tuning, and provides no natural ground early reflections.
- Existing open-source HRTF libraries require downloading 5–30 MB IR files, causing network loading delays and CORS headaches.

**FlyByWeb solves both problems.** It runs a pure parametric, allocation-free `AudioWorkletProcessor` with:
- **0 KB Asset Downloads:** Instant instantiation on any web page.
- **Dynamic Pinna Moving Notches:** True vertical elevation perception (-50° below horizon up to +90° overhead zenith).
- **Transient Preservation Engine:** Onset detector ($< 2\,\text{ms}$) that momentarily relaxes notch depths to preserve the punch of kicks, snares, and sound effects.
- **Specular Floor & Ceiling Acoustic Grounding:** Eliminates the sterile "floating in a vacuum" headphone sensation.
- **Velocity-Adaptive Anti-Pop Smoothing:** Slew rates scale automatically ($\alpha \in [0.0015, 0.015]$) to track faders and fast motion with zero zipper noise or pops.
- **Autonomous Flight Trajectories:** Ready-to-use 3D flight paths (Swoop, Helical Spiral, Zenith Flyover, Organic Flutter) with one line of code.

---

## ⚡ Features at a Glance

| Feature | How It Works | Benefit |
| :--- | :--- | :--- |
| **Rayleigh-Woodworth ITD** | Continuous spherical head delay model with 4-point Cubic Hermite sub-sample interpolation. | Clean, continuous Doppler pitch shift without digital zipper noise. |
| **Head Shadowing (ILD)** | Continuous 1-pole spherical diffraction filter based on ear-normal angle $\theta$. | Accurate acoustic head diffraction around skull. |
| **Pinna Elevation Notches** | Modulates primary notch ($N_1: 6.0\text{--}11.2\,\text{kHz}$), secondary notch ($N_2: 9.8\text{--}13.0\,\text{kHz}$), zenith presence shelf, and torso comb ($1.2\,\text{kHz}$). | Distinct perception of sound moving up, down, and overhead. |
| **TransientPreserver™** | Fast/slow envelope follower detects attack onsets ($>6\,\text{dB}$ jump in $<2\,\text{ms}$) and relaxes pinna notches for $8\,\text{ms}$. | Drums and sharp transients remain punchy instead of becoming phasey/dull. |
| **Boundary Grounding** | Image-source specular early reflections for floor & ceiling with frequency-dependent surface absorption. | Creates natural acoustic anchoring in virtual rooms. |
| **Autonomous Flight Paths** | Built-in trajectory math for Swoop, Helical Spiral, Zenith Flyover, and Hornet Flutter. | Animate sound in 3D without writing complex orbital trigonometry. |
| **Self-Contained Worklet** | Embedded in-memory Blob registration. | **Zero CORS issues, zero 404 file path errors.** Works out of the box in Vite, Webpack, Next.js, or raw static HTML. |

---

## 📦 Installation

```bash
npm install flyby-web
```

Or import directly via CDN or ES Modules:
```html
<script type="module">
  import { FlyByContext } from 'https://cdn.jsdelivr.net/npm/flyby-web/src/index.js';
</script>
```

---

## 🚀 Quick Start (Vanilla Web Audio)

```javascript
import { FlyByContext } from 'flyby-web';

// 1. Create AudioContext and FlyByContext
const audioCtx = new AudioContext();
const flyby = new FlyByContext(audioCtx);

// 2. Initialize worklet (loads in-memory blob automatically)
await flyby.ready();

// 3. Create a 3D Panner Node
const panner = flyby.createPanner({
  grounding: 0.4,          // Room grounding (floor early reflection)
  enableCeiling: false,    // Ceiling reflection (opt-in, false by default)
  transientCrispness: 0.75, // Transient punch protection (0.0 to 1.0)
  pinnaScale: 1.0,         // Anatomical ear size tuning (0.75x to 1.25x)
  elevationStrength: 1.0,  // Vertical notch cue depth (0.0 to 1.5x)
  dopplerAmount: 1.0       // True acoustic Doppler shift (0.0 to 2.0x)
});

// 4. Connect standard Web Audio source
sourceNode.connect(panner).connect(audioCtx.destination);

// 5. Position sound in 3D Space
// Cartesian: Listener is at (0, 0, 0) facing +Y, with +Z Up and +X Right
panner.setPosition(1.2, 2.0, 0.5);

// Or Spherical coordinates:
panner.setSpherical({
  azimuth: 45,    // -180° to +180° (0° = front, +90° = right, -90° = left)
  elevation: 35,  // -50° (below horizon) to +90° (overhead zenith)
  distance: 2.5   // meters
});
```

---

## 🎮 Autonomous 3D Flight Modes

Bring static sounds to life with autonomous 3D flight paths:

```javascript
// Dive-bomb swoop: starts high in front, swoops low past ear level, pulls up behind
panner.setFlightMode('swoop', { speed: 0.3, radius: 1.5 });

// Helical ascending / descending orbit around head
panner.setFlightMode('helical', { speed: 0.2, radius: 2.0, altitudeMin: -0.5, altitudeMax: 2.5 });

// Straight overhead zenith flyover
panner.setFlightMode('zenith', { speed: 0.25, radius: 2.0 });

// Erratic insect flutter with organic micro-turbulence
panner.setFlightMode('flutter', { speed: 0.4, turbulence: 0.25 });

// Return to manual control
panner.setFlightMode('manual');
```

---

## 🌐 Three.js & WebXR Integration

FlyByWeb includes drop-in helper bindings for Three.js. It automatically transforms world coordinates into listener eye-space:

```javascript
import * as THREE from 'three';
import { FlyByContext, FlyByListener, FlyByAudio } from 'flyby-web';

const flyby = new FlyByContext(audioCtx);
await flyby.ready();

// 1. Attach listener to Three.js camera
const listener = new FlyByListener(flyby);

// 2. Attach 3D audio source to any Mesh (e.g. drone, spaceship, enemy)
const droneAudio = new FlyByAudio(flyby, { grounding: 0.5 });
droneAudio.connect(audioCtx.destination);

// Connect sound generator to droneAudio.panner
soundSource.connect(droneAudio.panner);

// 3. Update in your animation loop:
function animate() {
  requestAnimationFrame(animate);

  listener.update(camera);
  droneAudio.update(droneMesh, camera); // auto-syncs 3D position & eye coordinates

  renderer.render(scene, camera);
}
```

---

## 🎛️ Full Parameter Reference

| Method | Parameters | Range | Default | Description |
| :--- | :--- | :--- | :--- | :--- |
| `setPosition(x, y, z)` | `x, y, z` (meters) | Any | `(0, 1.5, 0)` | Sets Cartesian coordinates. |
| `setSpherical({ azimuth, elevation, distance })` | `azimuth, elevation, distance` | Az: `[-180°, 180°]`<br>El: `[-50°, 90°]`<br>Dist: `[0.3, 8.0]`m | `(0°, 0°, 1.5m)` | Sets Spherical coordinates. |
| `setTransientCrispness(amount)` | `amount` | `0.0` to `1.0` | `0.75` | Controls how aggressively transients bypass pinna notches. |
| `setGrounding(amount)` | `amount` | `0.0` to `1.0` | `0.40` | Intensity of floor grounding early reflection. |
| `setCeilingEnabled(enabled)` | `enabled` (boolean) | `true` or `false` | `false` | Toggle ceiling reflection on/off (opt-in, disabled by default). |
| `setPinnaScale(scale)` | `scale` | `0.75` to `1.25` | `1.00` | Scales pinna notch frequencies for anatomical calibration. |
| `setElevationStrength(strength)` | `strength` | `0.0` to `1.5` | `1.00` | Depth multiplier for elevation spectral notch cues. |
| `setAltitudeBounds(minAlt, maxAlt)` | `minAlt, maxAlt` | Min: `[-2.5, 0.0]`m<br>Max: `[0.0, 5.0]`m | `(-0.8m, 2.5m)` | Floor distance (-Z) and ceiling height (+Z) bounds. |
| `setDopplerAmount(amount)` | `amount` | `0.0` to `2.0` | `1.00` | Scales Doppler frequency shift magnitude. |
| `setDryWet(dryWet)` | `dryWet` | `0.0` to `1.0` | `1.00` | Blend between dry input and binaural output. |
| `setFlightMode(mode, options)` | `mode, options` | `'manual' \| 'swoop' \| 'helical' \| 'zenith' \| 'flutter'` | `'manual'` | Configures autonomous 3D trajectory generator. |

---

## 🔬 Mathematical Foundation

The acoustic DSP models implemented in FlyByWeb are mathematically rigorous and documented in detail in our open technical paper:
- **`FlyBy_Acoustic_Mathematics.pdf`** (located in the repository)

Equations include:
1. **Rayleigh-Woodworth Spherical ITD:**
   $$\tau(\theta) = \frac{a}{c} \cdot (\sin\theta + \theta)$$
2. **Dynamic Primary Pinna Notch ($N_1$):**
   $$f_{N_1}(\phi) = (6000 + 5200 \cdot \phi_{\text{norm}}) \cdot s_{\text{pinna}}$$
3. **Specular Boundary Early Reflections:**
   $$\Delta \tau_{\text{floor}} = \frac{\sqrt{x^2 + y^2 + (z_s + z_l)^2} - d_{\text{direct}}}{c} \cdot f_s$$
4. **Transient Onset Detection:**
   $$\frac{\mathcal{E}_{\text{fast}}[n]}{\mathcal{E}_{\text{slow}}[n] + \epsilon} > 2.0 \implies \text{relax notch depth}$$

---

## 💻 Running the Interactive Demo Locally

Clone the repository and run:

```bash
cd FlyByWeb
python3 -m http.server 8089
```

Open your browser to:
👉 **`http://localhost:8089/demo/index.html`**

Put on headphones and click **"START AUDIO ENGINE"** to experience the interactive 3D radar, vertical elevation profile HUD, and sound generators in real time!

---

## 🤝 Contributing

Contributions, bug reports, and pull requests are warmly welcome!
- Found a bug? Open an issue on GitHub.
- Want to add a feature (e.g. Babylon.js adapter, React Three Fiber component)? PRs are welcome!

---

## 📄 License

**MIT License** — Copyright (c) 2026 Kijjaz & Gemini 3.8 Flash.  
Free for use in commercial games, web applications, installations, and musical projects.
