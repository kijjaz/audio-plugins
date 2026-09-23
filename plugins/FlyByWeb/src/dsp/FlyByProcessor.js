/**
 * AeroSpatialProcessor - Pure AudioWorklet 3D Binaural Spatializer
 * Ported from FlyBy DSP engine.
 * Zero external dependencies. Zero memory allocation in process().
 */

class FractionalDelayLine {
    constructor(maxDelaySamples = 8192) {
        this.bufferSize = Math.max(64, maxDelaySamples + 16);
        this.buffer = new Float32Array(this.bufferSize);
        this.writeIndex = 0;
    }

    reset() {
        this.buffer.fill(0);
        this.writeIndex = 0;
    }

    write(sample) {
        this.buffer[this.writeIndex] = sample;
        this.writeIndex = (this.writeIndex + 1) % this.bufferSize;
    }

    read(delaySamples) {
        if (delaySamples < 0) delaySamples = 0;
        if (delaySamples > this.bufferSize - 4) delaySamples = this.bufferSize - 4;

        let readPos = this.writeIndex - delaySamples;
        while (readPos < 0) readPos += this.bufferSize;
        while (readPos >= this.bufferSize) readPos -= this.bufferSize;

        const i1 = Math.floor(readPos);
        const frac = readPos - i1;

        const i0 = (i1 - 1 + this.bufferSize) % this.bufferSize;
        const i2 = (i1 + 1) % this.bufferSize;
        const i3 = (i1 + 2) % this.bufferSize;

        const y0 = this.buffer[i0];
        const y1 = this.buffer[i1];
        const y2 = this.buffer[i2];
        const y3 = this.buffer[i3];

        // 4-point Cubic Hermite Interpolation
        const c0 = y1;
        const c1 = 0.5 * (y2 - y0);
        const c2 = y0 - 2.5 * y1 + 2.0 * y2 - 0.5 * y3;
        const c3 = 0.5 * (y3 - y0) + 1.5 * (y1 - y2);

        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }
}

class BiquadFilter {
    constructor() {
        this.b0 = 1.0; this.b1 = 0.0; this.b2 = 0.0;
        this.a1 = 0.0; this.a2 = 0.0;
        this.s1 = 0.0; this.s2 = 0.0;
    }

    reset() {
        this.s1 = 0.0;
        this.s2 = 0.0;
    }

    setPeaking(frequencyHz, gainDb, q, sampleRate) {
        frequencyHz = Math.min(Math.max(20.0, frequencyHz), sampleRate * 0.48);
        q = Math.min(Math.max(0.1, q), 20.0);

        const A = Math.pow(10.0, gainDb / 40.0);
        const w0 = 2.0 * Math.PI * (frequencyHz / sampleRate);
        const cosW0 = Math.cos(w0);
        const sinW0 = Math.sin(w0);
        const alpha = sinW0 / (2.0 * q);

        const a0 = 1.0 + alpha / A;
        this.b0 = (1.0 + alpha * A) / a0;
        this.b1 = (-2.0 * cosW0) / a0;
        this.b2 = (1.0 - alpha * A) / a0;
        this.a1 = (-2.0 * cosW0) / a0;
        this.a2 = (1.0 - alpha / A) / a0;
    }

    setHighShelf(frequencyHz, gainDb, sampleRate) {
        frequencyHz = Math.min(Math.max(20.0, frequencyHz), sampleRate * 0.48);
        const A = Math.pow(10.0, gainDb / 40.0);
        const w0 = 2.0 * Math.PI * (frequencyHz / sampleRate);
        const cosW0 = Math.cos(w0);
        const sinW0 = Math.sin(w0);
        const alpha = sinW0 * 0.5 * Math.SQRT2;

        const a0 = (A + 1.0) - (A - 1.0) * cosW0 + 2.0 * Math.sqrt(A) * alpha;
        this.b0 = (A * ((A + 1.0) + (A - 1.0) * cosW0 + 2.0 * Math.sqrt(A) * alpha)) / a0;
        this.b1 = (-2.0 * A * ((A - 1.0) + (A + 1.0) * cosW0)) / a0;
        this.b2 = (A * ((A + 1.0) + (A - 1.0) * cosW0 - 2.0 * Math.sqrt(A) * alpha)) / a0;
        this.a1 = (2.0 * ((A - 1.0) - (A + 1.0) * cosW0)) / a0;
        this.a2 = ((A + 1.0) - (A - 1.0) * cosW0 - 2.0 * Math.sqrt(A) * alpha) / a0;
    }

    process(inVal) {
        const out = this.b0 * inVal + this.s1;
        this.s1 = this.b1 * inVal - this.a1 * out + this.s2;
        this.s2 = this.b2 * inVal - this.a2 * out;
        return out;
    }
}

class HeadShadowFilter {
    constructor() {
        this.x1 = 0.0;
        this.y1 = 0.0;
        this.b0 = 1.0;
        this.b1 = 0.0;
        this.a1 = 0.0;
    }

    reset() {
        this.x1 = 0.0;
        this.y1 = 0.0;
    }

    update(thetaRad, sampleRate) {
        const cosTheta = Math.cos(thetaRad);
        let alpha = 1.0 + 0.5 * cosTheta;
        if (alpha < 0.15) alpha = 0.15;

        const f0 = 650.0;
        const w0 = 2.0 * Math.PI * f0;
        const t = 1.0 / sampleRate;

        const denom = 2.0 + w0 * t;
        this.b0 = (2.0 * alpha + w0 * t) / denom;
        this.b1 = (w0 * t - 2.0 * alpha) / denom;
        this.a1 = (w0 * t - 2.0) / denom;
    }

    process(inVal) {
        const out = this.b0 * inVal + this.b1 * this.x1 - this.a1 * this.y1;
        this.x1 = inVal;
        this.y1 = out;
        return out;
    }
}

class PinnaFilter {
    constructor(sampleRate) {
        this.sampleRate = sampleRate;
        this.notchN1 = new BiquadFilter();
        this.notchN2 = new BiquadFilter();
        this.zenithPresence = new BiquadFilter();
        this.torsoComb = new BiquadFilter();

        this.lastEl = -999.0;
        this.lastScale = -999.0;
        this.lastStrength = -999.0;
        this.lastNotchMult = -999.0;
    }

    reset() {
        this.notchN1.reset();
        this.notchN2.reset();
        this.zenithPresence.reset();
        this.torsoComb.reset();
        this.lastEl = -999.0;
        this.lastScale = -999.0;
        this.lastStrength = -999.0;
        this.lastNotchMult = -999.0;
    }

    update(elevationDeg, pinnaScale = 1.0, elevationStrength = 1.0, notchDepthMultiplier = 1.0) {
        if (Math.abs(elevationDeg - this.lastEl) < 0.05 &&
            Math.abs(pinnaScale - this.lastScale) < 0.005 &&
            Math.abs(elevationStrength - this.lastStrength) < 0.005 &&
            Math.abs(notchDepthMultiplier - this.lastNotchMult) < 0.01) {
            return;
        }

        this.lastEl = elevationDeg;
        this.lastScale = pinnaScale;
        this.lastStrength = elevationStrength;
        this.lastNotchMult = notchDepthMultiplier;

        const elClamped = Math.min(Math.max(-50.0, elevationDeg), 90.0);
        const normEl = (elClamped + 50.0) / 140.0;

        // 1. Primary Notch (N1)
        const n1Freq = (6000.0 + normEl * 5200.0) * pinnaScale;
        const effectiveStrength = elevationStrength * notchDepthMultiplier;
        const n1GainDb = -18.0 * effectiveStrength;
        this.notchN1.setPeaking(n1Freq, n1GainDb, 5.0, this.sampleRate);

        // 2. Secondary Notch (N2)
        const overheadFactor = Math.min(Math.max(0.0, elClamped / 90.0), 1.0);
        const n2Freq = (9800.0 + overheadFactor * 3200.0) * pinnaScale;
        const n2GainDb = (-14.0 * overheadFactor) * effectiveStrength;
        this.notchN2.setPeaking(n2Freq, n2GainDb, 6.5, this.sampleRate);

        // 3. Overhead Presence
        const shelfGainDb = (overheadFactor * 3.5) * elevationStrength;
        this.zenithPresence.setHighShelf(8500.0 * pinnaScale, shelfGainDb, this.sampleRate);

        // 4. Torso Comb
        const belowFactor = Math.min(Math.max(0.0, -elClamped / 50.0), 1.0);
        const torsoFreq = 1200.0 - belowFactor * 350.0;
        const torsoGainDb = (-8.0 * belowFactor) * elevationStrength;
        this.torsoComb.setPeaking(torsoFreq, torsoGainDb, 2.8, this.sampleRate);
    }

    process(inVal) {
        let x = this.notchN1.process(inVal);
        x = this.notchN2.process(x);
        x = this.zenithPresence.process(x);
        x = this.torsoComb.process(x);
        return x;
    }
}

class TransientPreserver {
    constructor(sampleRate) {
        this.sampleRate = sampleRate;
        this.fastEnvelope = 0.0;
        this.slowEnvelope = 0.0;
        this.transientState = 0.0;

        this.fastAlpha = 1.0 - Math.exp(-1.0 / (0.0015 * sampleRate)); // 1.5ms attack
        this.slowAlpha = 1.0 - Math.exp(-1.0 / (0.025 * sampleRate));   // 25ms baseline
        this.decayAlpha = 1.0 - Math.exp(-1.0 / (0.008 * sampleRate));  // 8ms recovery
    }

    reset() {
        this.fastEnvelope = 0.0;
        this.slowEnvelope = 0.0;
        this.transientState = 0.0;
    }

    process(sample) {
        const absVal = Math.abs(sample);
        this.fastEnvelope += this.fastAlpha * (absVal - this.fastEnvelope);
        this.slowEnvelope += this.slowAlpha * (absVal - this.slowEnvelope);

        const ratio = (this.fastEnvelope + 1e-6) / (this.slowEnvelope + 1e-6);
        if (ratio > 2.0 && absVal > 0.008) {
            const onsetStrength = Math.min(1.0, (ratio - 2.0) * 0.4);
            if (onsetStrength > this.transientState) {
                this.transientState = onsetStrength;
            }
        } else {
            this.transientState -= this.decayAlpha * this.transientState;
            if (this.transientState < 0.0) this.transientState = 0.0;
        }
    }

    getNotchDepthMultiplier(crispness = 0.7) {
        return Math.max(0.08, 1.0 - this.transientState * crispness * 0.92);
    }
}

class BoundaryReflector {
    constructor(sampleRate) {
        this.sampleRate = sampleRate;
        this.floorDelay = new FractionalDelayLine(4096);
        this.ceilingDelay = new FractionalDelayLine(4096);

        this.floorAbsorb = new BiquadFilter();
        this.ceilingAbsorb = new BiquadFilter();
        this.floorAbsorb.setHighShelf(3800.0, -6.0, sampleRate);
        this.ceilingAbsorb.setHighShelf(5500.0, -4.5, sampleRate);

        this.targetFloorDelay = 100.0;
        this.currentFloorDelay = 100.0;
        this.targetCeilDelay = 150.0;
        this.currentCeilDelay = 150.0;

        this.targetFloorGain = 0.0;
        this.currentFloorGain = 0.0;
        this.targetCeilGain = 0.0;
        this.currentCeilGain = 0.0;

        this.adaptiveSlew = 0.004;
    }

    reset() {
        this.floorDelay.reset();
        this.ceilingDelay.reset();
        this.floorAbsorb.reset();
        this.ceilingAbsorb.reset();
        this.currentFloorGain = 0.0;
        this.currentCeilGain = 0.0;
    }

    update(posX, posY, posZ, grounding = 0.4, floorHeight = 1.6, roomHeight = 3.2, enableCeiling = 0.0) {
        const groundingGain = Math.min(Math.max(0.0, grounding), 1.0);
        if (groundingGain < 0.001) {
            this.targetFloorGain = 0.0;
            this.targetCeilGain = 0.0;
            return;
        }

        const zSource = Math.max(0.05, floorHeight + posZ);
        const zListener = floorHeight;
        const horizSq = posX * posX + posY * posY;
        const directDist = Math.sqrt(horizSq + posZ * posZ);

        // Floor Image
        const floorDist = Math.sqrt(horizSq + (zSource + zListener) * (zSource + zListener));
        const floorExcess = Math.max(0.1, floorDist - directDist);
        this.targetFloorDelay = (floorExcess / 343.0) * this.sampleRate;
        const floorProx = Math.min(Math.max(0.0, 1.0 - (zSource / 3.0)), 1.0);
        this.targetFloorGain = (directDist / floorDist) * 0.45 * floorProx * groundingGain;

        // Ceiling Image
        const H = Math.max(roomHeight, zSource + 0.2);
        const dzCeil = (H - zSource) + (H - zListener);
        const ceilDist = Math.sqrt(horizSq + dzCeil * dzCeil);
        const ceilExcess = Math.max(0.1, ceilDist - directDist);
        this.targetCeilDelay = (ceilExcess / 343.0) * this.sampleRate;

        // Ceiling reflection is optional (disabled by default)
        if (enableCeiling > 0.5) {
            const ceilProx = Math.min(Math.max(0.0, 1.0 - ((H - zSource) / 2.5)), 1.0);
            this.targetCeilGain = (directDist / ceilDist) * 0.40 * ceilProx * groundingGain;
        } else {
            this.targetCeilGain = 0.0;
        }

        const maxDelta = Math.max(Math.abs(this.targetFloorDelay - this.currentFloorDelay),
                                  Math.abs(this.targetCeilDelay - this.currentCeilDelay));
        this.adaptiveSlew = Math.min(Math.max(0.002, 0.002 + maxDelta * 0.0001), 0.012);
    }

    process(inMono, outPair) {
        this.floorDelay.write(inMono);
        this.ceilingDelay.write(inMono);

        this.currentFloorDelay += this.adaptiveSlew * (this.targetFloorDelay - this.currentFloorDelay);
        this.currentCeilDelay += this.adaptiveSlew * (this.targetCeilDelay - this.currentCeilDelay);
        this.currentFloorGain += 0.004 * (this.targetFloorGain - this.currentFloorGain);
        this.currentCeilGain += 0.004 * (this.targetCeilGain - this.currentCeilGain);

        if (this.currentFloorGain > 0.0005) {
            let fSig = this.floorDelay.read(this.currentFloorDelay);
            fSig = this.floorAbsorb.process(fSig);
            outPair[0] += fSig * this.currentFloorGain;
            outPair[1] += fSig * this.currentFloorGain;
        }

        if (this.currentCeilGain > 0.0005) {
            let cSig = this.ceilingDelay.read(this.currentCeilDelay);
            cSig = this.ceilingAbsorb.process(cSig);
            outPair[0] += cSig * this.currentCeilGain;
            outPair[1] += cSig * this.currentCeilGain;
        }
    }
}

/**
 * Main AudioWorklet Processor for FlyByWeb
 */
class FlyByProcessor extends AudioWorkletProcessor {
    static get parameterDescriptors() {
        return [
            { name: 'posX', defaultValue: 0.0 },
            { name: 'posY', defaultValue: 1.5 },
            { name: 'posZ', defaultValue: 0.0 },
            { name: 'pinnaScale', defaultValue: 1.0, minValue: 0.75, maxValue: 1.25 },
            { name: 'elevationStrength', defaultValue: 1.0, minValue: 0.0, maxValue: 1.5 },
            { name: 'transientCrispness', defaultValue: 0.7, minValue: 0.0, maxValue: 1.0 },
            { name: 'grounding', defaultValue: 0.4, minValue: 0.0, maxValue: 1.0 },
            { name: 'altitudeMax', defaultValue: 2.5, minValue: 0.0, maxValue: 5.0 },
            { name: 'altitudeMin', defaultValue: -0.8, minValue: -2.5, maxValue: 0.0 },
            { name: 'enableCeiling', defaultValue: 0.0, minValue: 0.0, maxValue: 1.0 },
            { name: 'dopplerAmount', defaultValue: 1.0, minValue: 0.0, maxValue: 2.0 },
            { name: 'dryWet', defaultValue: 1.0, minValue: 0.0, maxValue: 1.0 }
        ];
    }

    constructor() {
        super();
        this.sampleRate = sampleRate;
        const maxBufferSamples = Math.floor(sampleRate * 0.15); // 150ms buffer

        this.delayL = new FractionalDelayLine(maxBufferSamples);
        this.delayR = new FractionalDelayLine(maxBufferSamples);

        this.headShadowL = new HeadShadowFilter();
        this.headShadowR = new HeadShadowFilter();

        this.pinnaL = new PinnaFilter(this.sampleRate);
        this.pinnaR = new PinnaFilter(this.sampleRate);

        this.airAbsorbL = new BiquadFilter();
        this.airAbsorbR = new BiquadFilter();

        this.transientPreserver = new TransientPreserver(this.sampleRate);
        this.boundaryReflector = new BoundaryReflector(this.sampleRate);

        this.currentLeftDelay = 0.0;
        this.currentRightDelay = 0.0;
        this.targetLeftDelay = 0.0;
        this.targetRightDelay = 0.0;
        this.adaptiveDelaySlew = 0.005;

        this.currentDistGain = 1.0;
        this.targetDistGain = 1.0;
        this.lastDist = -999.0;

        this.outPair = new Float32Array(2);
    }

    process(inputs, outputs, parameters) {
        const input = inputs[0];
        const output = outputs[0];
        if (!input || input.length === 0 || !output || output.length === 0) return true;

        const numSamples = output[0].length;
        const inL = input[0];
        const inR = input[1] || inL;
        const outL = output[0];
        const outR = output[1] || output[0];

        // Read AudioParams (block-rate or parameter arrays)
        const posX = parameters.posX.length > 1 ? parameters.posX[0] : parameters.posX[0];
        const posY = parameters.posY.length > 1 ? parameters.posY[0] : parameters.posY[0];
        const posZ = parameters.posZ.length > 1 ? parameters.posZ[0] : parameters.posZ[0];
        const pinnaScale = parameters.pinnaScale[0];
        const elevationStrength = parameters.elevationStrength[0];
        const crispness = parameters.transientCrispness[0];
        const grounding = parameters.grounding[0];
        const altitudeMax = parameters.altitudeMax[0];
        const altitudeMin = parameters.altitudeMin[0];
        const enableCeiling = parameters.enableCeiling ? parameters.enableCeiling[0] : 0.0;
        const dopplerAmount = parameters.dopplerAmount[0];
        const dryWet = parameters.dryWet[0];
        const dryGain = 1.0 - dryWet;
        const wetGain = dryWet;

        // Spherical trigonometry
        const horizDist = Math.sqrt(posX * posX + posY * posY);
        const dist = Math.max(0.1, Math.sqrt(horizDist * horizDist + posZ * posZ));
        const azRad = Math.atan2(posX, posY);
        const elDeg = Math.atan2(posZ, Math.max(0.001, horizDist)) * (180.0 / Math.PI);

        // Woodworth Rayleigh Ear Distances
        const headRadius = 0.0875;
        const distL = Math.sqrt((posX + headRadius) * (posX + headRadius) + posY * posY + posZ * posZ);
        const distR = Math.sqrt((posX - headRadius) * (posX - headRadius) + posY * posY + posZ * posZ);

        let baseDelayL = (distL / 343.0) * this.sampleRate;
        let baseDelayR = (distR / 343.0) * this.sampleRate;

        if (dopplerAmount < 1.0) {
            const meanDelay = ((distL + distR) * 0.5 / 343.0) * this.sampleRate;
            baseDelayL = meanDelay + (baseDelayL - meanDelay) * dopplerAmount;
            baseDelayR = meanDelay + (baseDelayR - meanDelay) * dopplerAmount;
        }

        this.targetLeftDelay = baseDelayL;
        this.targetRightDelay = baseDelayR;
        if (this.currentLeftDelay <= 0.0) {
            this.currentLeftDelay = baseDelayL;
            this.currentRightDelay = baseDelayR;
        }

        // Head shadow angles
        const leftTheta = azRad + (Math.PI * 0.5);
        const rightTheta = (Math.PI * 0.5) - azRad;
        this.headShadowL.update(leftTheta, this.sampleRate);
        this.headShadowR.update(rightTheta, this.sampleRate);

        // Distance Attenuation (1/r law with soft minimum)
        const effectiveDist = Math.max(0.5, dist);
        this.targetDistGain = 1.0 / (1.0 + (effectiveDist - 0.5));

        // Air Absorption high shelf
        if (Math.abs(dist - this.lastDist) > 0.05) {
            this.lastDist = dist;
            const airCutoff = Math.min(Math.max(1500.0, 20000.0 / (1.0 + 0.08 * dist)), 20000.0);
            const airDb = -Math.min(12.0, dist * 0.75);
            this.airAbsorbL.setHighShelf(airCutoff, airDb, this.sampleRate);
            this.airAbsorbR.setHighShelf(airCutoff, airDb, this.sampleRate);
        }

        // Boundary Reflections: map altitudeMin to floor distance, altitudeMax to ceiling height
        const floorHeight = Math.max(0.5, -altitudeMin);
        const roomHeight = floorHeight + Math.max(1.0, altitudeMax);
        this.boundaryReflector.update(posX, posY, posZ, grounding, floorHeight, roomHeight, enableCeiling);

        // Adaptive Delay Slew Rate
        const delayDelta = Math.max(Math.abs(this.targetLeftDelay - this.currentLeftDelay),
                                    Math.abs(this.targetRightDelay - this.currentRightDelay));
        this.adaptiveDelaySlew = Math.min(Math.max(0.0015, 0.0015 + delayDelta * 0.0001), 0.015);

        // Sample processing loop
        for (let i = 0; i < numSamples; ++i) {
            const inMono = 0.5 * (inL[i] + inR[i]);

            // 1. Transient detection & notch relaxation
            this.transientPreserver.process(inMono);
            const notchMult = this.transientPreserver.getNotchDepthMultiplier(crispness);

            this.pinnaL.update(elDeg, pinnaScale, elevationStrength, notchMult);
            this.pinnaR.update(elDeg, pinnaScale, elevationStrength, notchMult);

            // 2. Fractional Delay Lines with Hermite Interpolation
            this.delayL.write(inMono);
            this.delayR.write(inMono);

            this.currentLeftDelay += this.adaptiveDelaySlew * (this.targetLeftDelay - this.currentLeftDelay);
            this.currentRightDelay += this.adaptiveDelaySlew * (this.targetRightDelay - this.currentRightDelay);

            let sigL = this.delayL.read(this.currentLeftDelay);
            let sigR = this.delayR.read(this.currentRightDelay);

            // 3. Head Shadow diffraction
            sigL = this.headShadowL.process(sigL);
            sigR = this.headShadowR.process(sigR);

            // 4. Pinna notches & Torso Comb
            sigL = this.pinnaL.process(sigL);
            sigR = this.pinnaR.process(sigR);

            // 5. Air Absorption
            sigL = this.airAbsorbL.process(sigL);
            sigR = this.airAbsorbR.process(sigR);

            // 6. Distance Attenuation
            this.currentDistGain += 0.003 * (this.targetDistGain - this.currentDistGain);
            sigL *= this.currentDistGain;
            sigR *= this.currentDistGain;

            // 7. Specular Ground / Ceiling Reflections
            this.outPair[0] = sigL;
            this.outPair[1] = sigR;
            this.boundaryReflector.process(inMono, this.outPair);

            // Output Mix
            outL[i] = inL[i] * dryGain + this.outPair[0] * wetGain;
            outR[i] = inR[i] * dryGain + this.outPair[1] * wetGain;
        }

        return true;
    }
}

registerProcessor('flyby-processor', FlyByProcessor);
