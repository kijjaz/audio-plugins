import { FlyByContext } from '../src/core/FlyByContext.js';

let audioCtx = null;
let flybyCtx = null;
let panner = null;
let synthGain = null;
let isAudioRunning = false;

let soundSourceType = 'synth'; // 'synth', 'drums', 'noise'
let synthTimer = null;
let noiseNode = null;

// UI DOM references
const btnPower = document.getElementById('btnPower');
const audioStatus = document.getElementById('audioStatus');
const badgeAzimuth = document.getElementById('badgeAzimuth');
const badgeElevation = document.getElementById('badgeElevation');

const radarCanvas = document.getElementById('radarCanvas');
const radarCtx = radarCanvas.getContext('2d');
const elevationCanvas = document.getElementById('elevationCanvas');
const elCtx = elevationCanvas.getContext('2d');

const sliderAz = document.getElementById('sliderAz');
const sliderEl = document.getElementById('sliderEl');
const sliderDist = document.getElementById('sliderDist');
const valAz = document.getElementById('valAz');
const valEl = document.getElementById('valEl');
const valDist = document.getElementById('valDist');

const sliderCrisp = document.getElementById('sliderCrisp');
const sliderGround = document.getElementById('sliderGround');
const sliderDoppler = document.getElementById('sliderDoppler');
const valCrisp = document.getElementById('valCrisp');
const valGround = document.getElementById('valGround');
const valDoppler = document.getElementById('valDoppler');

const sliderPinna = document.getElementById('sliderPinna');
const valPinna = document.getElementById('valPinna');
const sliderElDepth = document.getElementById('sliderElDepth');
const valElDepth = document.getElementById('valElDepth');

const sliderCeil = document.getElementById('sliderCeil');
const valCeil = document.getElementById('valCeil');
const sliderFloor = document.getElementById('sliderFloor');
const valFloor = document.getElementById('valFloor');
const checkCeiling = document.getElementById('checkCeiling');

const sliderSpeed = document.getElementById('sliderSpeed');
const valSpeed = document.getElementById('valSpeed');

// Sound type buttons
const soundButtons = {
    synth: document.getElementById('btnSoundSynth'),
    drums: document.getElementById('btnSoundDrums'),
    noise: document.getElementById('btnSoundNoise')
};

// Mode buttons
const modeButtons = {
    manual: document.getElementById('btnModeManual'),
    swoop: document.getElementById('btnModeSwoop'),
    helical: document.getElementById('btnModeHelical'),
    zenith: document.getElementById('btnModeZenith'),
    flutter: document.getElementById('btnModeFlutter')
};

let currentMode = 'manual';

// Initialize Audio Engine
async function initAudio() {
    if (audioCtx) return;

    audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    flybyCtx = new FlyByContext(audioCtx);

    audioStatus.textContent = 'REGISTERING AUDIOWORKLET...';
    await flybyCtx.ready();

    // Create 3D Panner
    panner = flybyCtx.createPanner({
        posX: 0.0,
        posY: 1.5,
        posZ: 0.0,
        grounding: 0.4,
        transientCrispness: 0.75,
        dopplerAmount: 1.0
    });

    panner.connect(audioCtx.destination);

    synthGain = audioCtx.createGain();
    synthGain.gain.value = 0.25;
    synthGain.connect(panner);

    startSoundSource(soundSourceType);

    audioStatus.textContent = 'ENGINE RUNNING (44.1/48kHz BINAURAL)';
    btnPower.textContent = 'AUDIO RUNNING';
    btnPower.classList.add('active');
    isAudioRunning = true;
}

btnPower.addEventListener('click', async () => {
    if (!audioCtx) {
        await initAudio();
    } else if (audioCtx.state === 'suspended') {
        await audioCtx.resume();
        audioStatus.textContent = 'AUDIO ENGINE RESUMED';
        btnPower.classList.add('active');
        isAudioRunning = true;
    } else {
        await audioCtx.suspend();
        audioStatus.textContent = 'AUDIO ENGINE PAUSED';
        btnPower.classList.remove('active');
        isAudioRunning = false;
    }
});

// Sound Generator Logic
function startSoundSource(type) {
    if (!audioCtx) return;
    stopSoundSources();

    if (type === 'synth') {
        document.getElementById('sourceStatus').textContent = 'Playing: Synth Arp Sequence (Punchy Leads)';
        const freqs = [220, 277.18, 329.63, 440, 554.37, 659.25]; // A major arp
        let noteIdx = 0;
        synthTimer = setInterval(() => {
            if (!isAudioRunning) return;
            const osc = audioCtx.createOscillator();
            const noteGain = audioCtx.createGain();
            osc.type = 'sawtooth';
            osc.frequency.value = freqs[noteIdx % freqs.length];
            noteIdx++;

            const now = audioCtx.currentTime;
            noteGain.gain.setValueAtTime(0.001, now);
            noteGain.gain.exponentialRampToValueAtTime(0.3, now + 0.015);
            noteGain.gain.exponentialRampToValueAtTime(0.0001, now + 0.22);

            osc.connect(noteGain).connect(synthGain);
            osc.start(now);
            osc.stop(now + 0.24);
        }, 160);
    } else if (type === 'drums') {
        document.getElementById('sourceStatus').textContent = 'Playing: Transient Beats (808 Kick & Snare)';
        let beat = 0;
        synthTimer = setInterval(() => {
            if (!isAudioRunning) return;
            const now = audioCtx.currentTime;
            if (beat % 2 === 0) {
                // Kick Drum (Fast pitch sweep transient)
                const osc = audioCtx.createOscillator();
                const g = audioCtx.createGain();
                osc.frequency.setValueAtTime(140, now);
                osc.frequency.exponentialRampToValueAtTime(45, now + 0.07);
                g.gain.setValueAtTime(0.5, now);
                g.gain.exponentialRampToValueAtTime(0.001, now + 0.25);
                osc.connect(g).connect(synthGain);
                osc.start(now);
                osc.stop(now + 0.26);
            } else {
                // Snare Drum (White noise burst)
                const bufferSize = audioCtx.sampleRate * 0.12;
                const buffer = audioCtx.createBuffer(1, bufferSize, audioCtx.sampleRate);
                const data = buffer.getChannelData(0);
                for (let i = 0; i < bufferSize; i++) data[i] = Math.random() * 2 - 1;
                const noise = audioCtx.createBufferSource();
                noise.buffer = buffer;
                const g = audioCtx.createGain();
                g.gain.setValueAtTime(0.35, now);
                g.gain.exponentialRampToValueAtTime(0.001, now + 0.12);
                noise.connect(g).connect(synthGain);
                noise.start(now);
            }
            beat++;
        }, 320);
    } else if (type === 'noise') {
        document.getElementById('sourceStatus').textContent = 'Playing: Pink Noise Jet Flyby Stream';
        const bufferSize = audioCtx.sampleRate * 2.0;
        const buffer = audioCtx.createBuffer(1, bufferSize, audioCtx.sampleRate);
        const data = buffer.getChannelData(0);
        let b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0;
        for (let i = 0; i < bufferSize; i++) {
            const white = Math.random() * 2 - 1;
            b0 = 0.99886 * b0 + white * 0.0555179;
            b1 = 0.99332 * b1 + white * 0.0750759;
            b2 = 0.96900 * b2 + white * 0.1538520;
            b3 = 0.86650 * b3 + white * 0.3104856;
            b4 = 0.55000 * b4 + white * 0.5329522;
            b5 = -0.7616 * b5 - white * 0.0168980;
            data[i] = (b0 + b1 + b2 + b3 + b4 + b5 + b6 + white * 0.5362) * 0.07;
            b6 = white * 0.115926;
        }
        noiseNode = audioCtx.createBufferSource();
        noiseNode.buffer = buffer;
        noiseNode.loop = true;
        noiseNode.connect(synthGain);
        noiseNode.start();
    }
}

function stopSoundSources() {
    if (synthTimer) clearInterval(synthTimer);
    if (noiseNode) {
        try { noiseNode.stop(); } catch (e) {}
        noiseNode = null;
    }
}

// Sound Type Selection
Object.keys(soundButtons).forEach((key) => {
    soundButtons[key].addEventListener('click', () => {
        Object.values(soundButtons).forEach(btn => btn.classList.remove('active'));
        soundButtons[key].classList.add('active');
        soundSourceType = key;
        startSoundSource(key);
    });
});

// Flight Mode Selection
Object.keys(modeButtons).forEach((modeKey) => {
    modeButtons[modeKey].addEventListener('click', () => {
        Object.values(modeButtons).forEach(btn => btn.classList.remove('active'));
        modeButtons[modeKey].classList.add('active');
        currentMode = modeKey;
        if (panner) {
            panner.setFlightMode(modeKey, {
                speed: parseFloat(sliderSpeed.value),
                radius: parseFloat(sliderDist.value)
            });
        }
    });
});

// Slider Input Handlers
function updateManualCoordinates() {
    const az = parseFloat(sliderAz.value);
    const el = parseFloat(sliderEl.value);
    const dist = parseFloat(sliderDist.value);

    valAz.textContent = `${az}°`;
    valEl.textContent = `${el}°`;
    valDist.textContent = `${dist.toFixed(1)}m`;

    if (panner && currentMode === 'manual') {
        panner.setSpherical({ azimuth: az, elevation: el, distance: dist });
    }
}

sliderAz.addEventListener('input', updateManualCoordinates);
sliderEl.addEventListener('input', updateManualCoordinates);
sliderDist.addEventListener('input', updateManualCoordinates);

sliderCrisp.addEventListener('input', () => {
    valCrisp.textContent = `${sliderCrisp.value}%`;
    if (panner) panner.setTransientCrispness(sliderCrisp.value / 100.0);
});

sliderGround.addEventListener('input', () => {
    valGround.textContent = `${sliderGround.value}%`;
    if (panner) panner.setGrounding(sliderGround.value / 100.0);
});

sliderDoppler.addEventListener('input', () => {
    valDoppler.textContent = `${sliderDoppler.value}%`;
    if (panner) panner.setDopplerAmount(sliderDoppler.value / 100.0);
});

sliderPinna.addEventListener('input', () => {
    const scale = sliderPinna.value / 100.0;
    valPinna.textContent = `${scale.toFixed(2)}x`;
    if (panner) panner.setPinnaScale(scale);
});

sliderElDepth.addEventListener('input', () => {
    const depth = sliderElDepth.value / 100.0;
    valElDepth.textContent = `${depth.toFixed(2)}x`;
    if (panner) panner.setElevationStrength(depth);
});

function updateAltitudeBounds() {
    const ceil = parseFloat(sliderCeil.value);
    const floor = parseFloat(sliderFloor.value);
    valCeil.textContent = `${ceil.toFixed(1)}m`;
    valFloor.textContent = `${floor.toFixed(1)}m`;
    if (panner) panner.setAltitudeBounds(floor, ceil);
}

sliderCeil.addEventListener('input', updateAltitudeBounds);
sliderFloor.addEventListener('input', updateAltitudeBounds);

if (checkCeiling) {
    checkCeiling.addEventListener('change', (e) => {
        if (panner) panner.setCeilingEnabled(e.target.checked);
    });
}

sliderSpeed.addEventListener('input', () => {
    valSpeed.textContent = sliderSpeed.value;
    if (panner && currentMode !== 'manual') {
        panner.setFlightMode(currentMode, { speed: parseFloat(sliderSpeed.value) });
    }
});

// Interactive Mouse Drag on Radar
let isDraggingRadar = false;
radarCanvas.addEventListener('mousedown', (e) => {
    isDraggingRadar = true;
    handleRadarDrag(e);
});
window.addEventListener('mouseup', () => { isDraggingRadar = false; });
window.addEventListener('mousemove', (e) => {
    if (isDraggingRadar) handleRadarDrag(e);
});

function handleRadarDrag(e) {
    const rect = radarCanvas.getBoundingClientRect();
    const cx = rect.width / 2;
    const cy = rect.height / 2;
    const px = (e.clientX - rect.left) - cx;
    const py = cy - (e.clientY - rect.top); // Invert Y so up is front (+Y)

    const azDeg = Math.atan2(px, py) * (180.0 / Math.PI);
    const maxPx = rect.width * 0.44;
    const distPx = Math.sqrt(px * px + py * py);
    const distMeters = Math.min(8.0, Math.max(0.3, (distPx / maxPx) * 5.0));

    sliderAz.value = Math.round(azDeg);
    sliderDist.value = distMeters.toFixed(1);

    if (currentMode !== 'manual') {
        modeButtons['manual'].click();
    }
    updateManualCoordinates();
}

// Visual Loop (Radar + Elevation Profile)
function renderVisuals() {
    requestAnimationFrame(renderVisuals);

    let sp = { azimuth: 0, elevation: 0, distance: 1.5 };
    if (panner) {
        sp = panner.getSpherical();
    } else {
        sp.azimuth = parseFloat(sliderAz.value);
        sp.elevation = parseFloat(sliderEl.value);
        sp.distance = parseFloat(sliderDist.value);
    }

    badgeAzimuth.textContent = `AZ: ${sp.azimuth.toFixed(1)}°`;
    badgeElevation.textContent = `EL: ${sp.elevation.toFixed(1)}°`;

    // 1. Draw Radar (Horizontal Plane)
    const rw = radarCanvas.width;
    const rh = radarCanvas.height;
    const rcx = rw / 2;
    const rcy = rh / 2;
    const maxR = rw * 0.42;

    radarCtx.fillStyle = '#090b10';
    radarCtx.fillRect(0, 0, rw, rh);

    // Grid circles
    radarCtx.strokeStyle = '#232936';
    radarCtx.lineWidth = 1.0;
    [0.25, 0.5, 0.75, 1.0].forEach(f => {
        radarCtx.beginPath();
        radarCtx.arc(rcx, rcy, maxR * f, 0, Math.PI * 2);
        radarCtx.stroke();
    });

    // Crosshairs
    radarCtx.strokeStyle = 'rgba(197, 160, 89, 0.3)';
    radarCtx.beginPath();
    radarCtx.moveTo(rcx, rcy - maxR); radarCtx.lineTo(rcx, rcy + maxR);
    radarCtx.moveTo(rcx - maxR, rcy); radarCtx.lineTo(rcx + maxR, rcy);
    radarCtx.stroke();

    // Cardinal Labels
    radarCtx.fillStyle = '#8e9bb0';
    radarCtx.font = '10px "JetBrains Mono", monospace';
    radarCtx.textAlign = 'center';
    radarCtx.fillText('FRONT 0°', rcx, rcy - maxR - 8);
    radarCtx.fillText('REAR 180°', rcx, rcy + maxR + 16);
    radarCtx.fillText('-90° LEFT', rcx - maxR - 28, rcy + 4);
    radarCtx.fillText('+90° RIGHT', rcx + maxR + 32, rcy + 4);

    // Listener Head Icon
    radarCtx.fillStyle = '#c5a059';
    radarCtx.beginPath();
    radarCtx.arc(rcx, rcy, 8, 0, Math.PI * 2);
    radarCtx.fill();
    // Nose
    radarCtx.beginPath();
    radarCtx.moveTo(rcx - 4, rcy - 7);
    radarCtx.lineTo(rcx, rcy - 14);
    radarCtx.lineTo(rcx + 4, rcy - 7);
    radarCtx.fill();

    // Source blip
    const rAzRad = (sp.azimuth * Math.PI) / 180.0;
    const normDist = Math.min(1.0, sp.distance / 5.0);
    const sourceX = rcx + Math.sin(rAzRad) * (normDist * maxR);
    const sourceY = rcy - Math.cos(rAzRad) * (normDist * maxR);

    // Glow aura
    const glowGrad = radarCtx.createRadialGradient(sourceX, sourceY, 2, sourceX, sourceY, 18);
    glowGrad.addColorStop(0, 'rgba(255, 170, 0, 0.8)');
    glowGrad.addColorStop(1, 'rgba(255, 170, 0, 0.0)');
    radarCtx.fillStyle = glowGrad;
    radarCtx.beginPath();
    radarCtx.arc(sourceX, sourceY, 18, 0, Math.PI * 2);
    radarCtx.fill();

    // Core point
    radarCtx.fillStyle = '#ffaa00';
    radarCtx.beginPath();
    radarCtx.arc(sourceX, sourceY, 6, 0, Math.PI * 2);
    radarCtx.fill();

    // 2. Draw Elevation Canvas (Vertical Profile)
    const ew = elevationCanvas.width;
    const eh = elevationCanvas.height;
    const ecx = ew / 2;
    const ecy = eh / 2;
    const elSpan = eh * 0.38;

    elCtx.fillStyle = '#090b10';
    elCtx.fillRect(0, 0, ew, eh);

    // Horizon line (0 deg)
    elCtx.strokeStyle = 'rgba(197, 160, 89, 0.6)';
    elCtx.lineWidth = 1.5;
    elCtx.beginPath();
    elCtx.moveTo(30, ecy);
    elCtx.lineTo(ew - 30, ecy);
    elCtx.stroke();

    elCtx.fillStyle = '#c5a059';
    elCtx.font = '10px "JetBrains Mono", monospace';
    elCtx.textAlign = 'left';
    elCtx.fillText('HORIZON 0° (EAR LEVEL)', 35, ecy - 8);

    // Zenith line (+90 deg)
    const zenithY = ecy - elSpan;
    elCtx.setLineDash([4, 4]);
    elCtx.strokeStyle = '#ffaa00';
    elCtx.beginPath();
    elCtx.moveTo(40, zenithY);
    elCtx.lineTo(ew - 40, zenithY);
    elCtx.stroke();
    elCtx.setLineDash([]);
    elCtx.fillText('+90° ZENITH (OVERHEAD NOTCH N1/N2)', 45, zenithY - 8);

    // Chest line (-40 deg)
    const chestY = ecy + elSpan * (40 / 90);
    elCtx.strokeStyle = '#232936';
    elCtx.beginPath();
    elCtx.moveTo(40, chestY);
    elCtx.lineTo(ew - 40, chestY);
    elCtx.stroke();
    elCtx.fillText('-40° TORSO / GROUND REFLECTIONS', 45, chestY + 16);

    // Head profile
    elCtx.fillStyle = '#c5a059';
    elCtx.beginPath();
    elCtx.arc(ecx, ecy, 10, 0, Math.PI * 2);
    elCtx.fill();

    // Elevation Source Blip
    const elFraction = sp.elevation / 90.0;
    const blipY = ecy - (elFraction * elSpan);
    const blipX = ecx + (normDist * (ew * 0.35) * (sp.azimuth >= 0 ? 1 : -1));

    const elGlow = elCtx.createRadialGradient(blipX, blipY, 2, blipX, blipY, 16);
    elGlow.addColorStop(0, 'rgba(56, 239, 125, 0.8)');
    elGlow.addColorStop(1, 'rgba(56, 239, 125, 0.0)');
    elCtx.fillStyle = elGlow;
    elCtx.beginPath();
    elCtx.arc(blipX, blipY, 16, 0, Math.PI * 2);
    elCtx.fill();

    elCtx.fillStyle = '#38ef7d';
    elCtx.beginPath();
    elCtx.arc(blipX, blipY, 5, 0, Math.PI * 2);
    elCtx.fill();
}

renderVisuals();
