import { GRAVITY_TABLE, GRAVITY_COLUMNS } from './gravity_data.js';

// --- DSP Helpers ---
const ONE_POLE_LP = (current, target, coeff) => current + (target - current) * coeff;
// 16kHz at 44.1/48k is roughly 0.6-0.8 coeff. 
// At 48k, fc=16k implies w = 2*pi*16000/48000 = 2.09 rad/samp.
// High cutoff. Let's use a simple 0.5 coeff for "smoothing" FM.

class Voice {
    constructor(sampleRate) {
        this.sampleRate = sampleRate;
        this.active = false;
        this.note = 0;
        this.velocity = 0;
        this.phases = [0, 0, 0]; // Sun, Planet, Moon
        this.envState = 'IDLE'; // IDLE, ATTACK, DECAY, SUSTAIN, RELEASE
        this.envLevel = 0.0;

        // ADSR Params (Exponential-ish rates)
        this.attackRate = 0.002;
        this.decayRate = 0.0005;
        this.releaseRate = 0.0005;
        this.sustainLevel = 0.7;

        // FM Filter States (One pole per FM path)
        this.lp1 = 0;
        this.lp2 = 0;
    }

    noteOn(note, velocity) {
        this.note = note;
        this.velocity = velocity;
        this.active = true;
        this.envState = 'ATTACK';
        this.phases = [0, 0, 0];
        this.lp1 = 0;
        this.lp2 = 0;
    }

    noteOff() {
        this.envState = 'RELEASE';
    }

    process(gravityData, params) {
        if (!this.active) return [0, 0];

        // --- ADSR (Exponential Approach) ---
        let target = 0;
        let rate = 0;

        switch (this.envState) {
            case 'IDLE':
                this.active = false;
                return [0, 0];
            case 'ATTACK':
                target = 1.2;
                rate = this.attackRate;
                if (this.envLevel >= 1.0) {
                    this.envLevel = 1.0;
                    this.envState = 'DECAY';
                }
                break;
            case 'DECAY':
                target = this.sustainLevel;
                rate = this.decayRate;
                break;
            case 'SUSTAIN':
                target = this.sustainLevel;
                rate = 0.00001;
                break;
            case 'RELEASE':
                target = 0.0;
                rate = this.releaseRate;
                if (this.envLevel < 0.001) {
                    this.envLevel = 0.0;
                    this.envState = 'IDLE';
                    this.active = false;
                    return [0, 0];
                }
                break;
        }

        this.envLevel += (target - this.envLevel) * rate;

        // Modulate VCA with Gravity LFO (d12 = Sun-Planet Distance)
        // d12 is ~0.0 to 1.0. Lower = more gravity.
        const lfo = 1.0 - (gravityData.d12 * 0.3 * params.gravityAmt);
        const finalGain = this.envLevel * this.velocity * lfo * 0.5; // Gain staging

        // --- Synthesis ---
        const baseFreq = 440 * Math.pow(2, (this.note - 69) / 12);

        // Osc 1 (Sun) - Sine
        const s1 = Math.sin(this.phases[0]);
        this.phases[0] += (2 * Math.PI * baseFreq) / this.sampleRate;
        if (this.phases[0] > 2 * Math.PI) this.phases[0] -= 2 * Math.PI;

        // Osc 2 (Planet) 
        // FM Filter 1 (Analog Filter)
        this.lp1 += (s1 - this.lp1) * 0.5; // 1 pole LP
        const mod1 = this.lp1 * params.fmIdxPlanet;

        const detune2 = (gravityData.v2 - 0.5) * 10 * params.gravityAmt;
        const f2 = baseFreq * 2.0 + detune2;

        const ph2 = this.phases[1] + mod1;
        const s2 = Math.sin(ph2) + 0.3 * Math.sin(ph2 * 2); // Warm sine

        this.phases[1] += (2 * Math.PI * f2) / this.sampleRate;
        if (this.phases[1] > 2 * Math.PI) this.phases[1] -= 2 * Math.PI;

        // Osc 3 (Moon)
        // FM Filter 2
        this.lp2 += (s2 - this.lp2) * 0.5;
        const mod2 = this.lp2 * params.fmIdxMoon;

        const f3 = f2 * 4.0;
        const s3 = Math.sin(this.phases[2] + mod2) * (0.1 + params.flare);

        this.phases[2] += (2 * Math.PI * f3) / this.sampleRate;
        if (this.phases[2] > 2 * Math.PI) this.phases[2] -= 2 * Math.PI;

        // Panned Outputs
        const outL = (s1 * 0.6 * gravityData.sunL) + (s2 * 0.4 * gravityData.plL) + (s3 * 0.3 * gravityData.mnL);
        const outR = (s1 * 0.6 * gravityData.sunR) + (s2 * 0.4 * gravityData.plR) + (s3 * 0.3 * gravityData.mnR);

        return [outL * finalGain, outR * finalGain];
    }
}

class GravityProcessor extends AudioWorkletProcessor {
    constructor() {
        super();
        this.table = GRAVITY_TABLE;
        this.readHead = 0.0;

        // Polyphony
        this.voices = [];
        for (let i = 0; i < 16; i++) this.voices.push(new Voice(sampleRate));

        // Space Dust (Granular Delay)
        this.maxDelaySamples = sampleRate * 2.0; // 2 seconds
        this.delayBuffer = new Float32Array(Math.floor(this.maxDelaySamples * 2)); // Stereo
        this.writePtr = 0;
        this.grains = [];
    }

    static get parameterDescriptors() {
        return [
            { name: 'simulationSpeed', defaultValue: 1.0, minValue: 0.0, maxValue: 5.0 },
            { name: 'gravityAmount', defaultValue: 0.5, minValue: 0.0, maxValue: 1.0 },
            { name: 'dustAmount', defaultValue: 0.2, minValue: 0.0, maxValue: 1.0 },
            { name: 'dustDensity', defaultValue: 0.5, minValue: 0.0, maxValue: 1.0 }
        ];
    }

    process(inputs, outputs, parameters) {
        const output = outputs[0];
        const L = output[0];
        const R = output[1] || L;

        const speed = parameters.simulationSpeed[0];
        const gravAmt = parameters.gravityAmount[0];
        const dustAmt = parameters.dustAmount[0];
        const dustDens = parameters.dustDensity[0];

        // 1. Simulation Update
        const stepInc = (100 * speed) / sampleRate;
        this.readHead += stepInc;
        if (this.readHead >= this.table.length) this.readHead -= this.table.length;

        // 2. Physics Data
        const idx = Math.floor(this.readHead);
        const nextIdx = (idx + 1) % this.table.length;
        const frac = this.readHead - idx;
        const row = this.table[idx];
        const rowN = this.table[nextIdx];
        const lerp = (a, b, f) => a + (b - a) * f;

        // Extract Data
        // 0:x1, 1:y1 ... 6:d12, 7:d23, 11:v3
        const sunX = lerp(row[0], rowN[0], frac);
        const plX = lerp(row[2], rowN[2], frac);
        const mnX = lerp(row[4], rowN[4], frac);
        const d12 = lerp(row[6], rowN[6], frac);
        const d23 = lerp(row[7], rowN[7], frac);
        const v3 = lerp(row[11], rowN[11], frac);

        // Params for Voices
        const pd12 = Math.max(0, 1.0 - d12);
        const gravityData = {
            d12: d12,
            v2: lerp(row[10], rowN[10], frac),
            sunL: 0, sunR: 0,
            plL: 0, plR: 0,
            mnL: 0, mnR: 0
        };
        const voiceParams = {
            gravityAmt: gravAmt,
            fmIdxPlanet: Math.pow(pd12, 3) * 2000 * gravAmt,
            fmIdxMoon: Math.pow(Math.max(0, 1.0 - d23), 2) * 1000 * gravAmt,
            flare: Math.pow(v3, 4) * 5.0
        };

        // Panning
        const calcPan = (x) => {
            const p = Math.max(-1, Math.min(x, 1));
            return [0.5 * (1 - p), 0.5 * (1 + p)];
        };
        [gravityData.sunL, gravityData.sunR] = calcPan(sunX * 0.5);
        [gravityData.plL, gravityData.plR] = calcPan(plX);
        [gravityData.mnL, gravityData.mnR] = calcPan(mnX);

        // 3. Process Voices & Granular
        for (let i = 0; i < L.length; i++) {
            let sumL = 0;
            let sumR = 0;

            // Sum active voices
            for (let v of this.voices) {
                if (v.active) {
                    const [vl, vr] = v.process(gravityData, voiceParams);
                    sumL += vl;
                    sumR += vr;
                }
            }

            // --- Space Dust (Granular) ---
            // 1. Write Dry to Buffer
            this.delayBuffer[this.writePtr] = sumL;
            this.delayBuffer[this.writePtr + 1] = sumR;

            // 2. Spawn Grains
            // Probabilistic spawn
            if (Math.random() < (dustDens * 0.005) && this.grains.length < 256) {
                const delayTimeSamples = 500 + Math.random() * (this.maxDelaySamples - 500);
                // Don't spawn too close to head
                let readStart = this.writePtr - (Math.floor(delayTimeSamples) * 2);
                if (readStart < 0) readStart += this.delayBuffer.length;

                this.grains.push({
                    pos: 0,
                    dur: 2000 + Math.random() * 8000,
                    readPtr: readStart,
                    pan: Math.random(),
                    speed: 0.9 + Math.random() * 0.2 // Slight pitch variations
                });
            }

            // 3. Render Grains
            let dustL = 0;
            let dustR = 0;
            for (let g = this.grains.length - 1; g >= 0; g--) {
                const gr = this.grains[g];

                let rIdx = Math.floor(gr.readPtr);
                if (rIdx < 0) rIdx += this.delayBuffer.length;
                rIdx = rIdx % this.delayBuffer.length;
                // Ensure even index
                if (rIdx % 2 !== 0) rIdx--;

                const dl = this.delayBuffer[rIdx];
                const dr = this.delayBuffer[rIdx + 1];

                // Parabolic Window
                const t = gr.pos / gr.dur;
                const win = 1.0 - Math.pow(2 * t - 1, 2);

                dustL += dl * win * (1 - gr.pan);
                dustR += dr * win * gr.pan;

                gr.pos++;
                gr.readPtr += 2 * gr.speed;
                while (gr.readPtr >= this.delayBuffer.length) gr.readPtr -= this.delayBuffer.length;

                if (gr.pos >= gr.dur) {
                    this.grains.splice(g, 1);
                }
            }

            // Advance Buffer Ptr
            this.writePtr += 2;
            if (this.writePtr >= this.delayBuffer.length) this.writePtr = 0;

            // Final Mix
            L[i] = sumL + dustL * dustAmt;
            if (R) R[i] = sumR + dustR * dustAmt;
        }

        return true;
    }

    // Simulate MIDI for independent testing?
    // In actual WAM, this is called by the host. 
    processMidi(data) {
        // [status, note, vel]
        // Note On: 144 (ch1)
        // Note Off: 128 (ch1)
        const status = data[0] & 0xF0;
        const note = data[1];
        const vel = data[2] / 127.0;

        if (status === 144 && vel > 0) {
            // Note On
            // Find free voice
            let voice = this.voices.find(v => !v.active);
            // Or steal released
            if (!voice) voice = this.voices.find(v => v.active && v.envState === 'RELEASE');
            // Or steal oldest (simplification: just 0)
            if (!voice) voice = this.voices[0];

            voice.noteOn(note, vel);
        } else if (status === 128 || (status === 144 && vel === 0)) {
            // Note Off
            this.voices.forEach(v => {
                if (v.active && v.note === note) v.noteOff();
            });
        }
    }
}

registerProcessor('GravityProcessor', GravityProcessor);
