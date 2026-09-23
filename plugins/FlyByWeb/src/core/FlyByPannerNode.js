/**
 * FlyByPannerNode
 * Custom AudioWorkletNode wrapper providing high-level 3D positioning,
 * spherical coordinate setters, and automated flight paths.
 */

export class FlyByPannerNode extends AudioWorkletNode {
    constructor(context, options = {}) {
        super(context, 'flyby-processor', {
            numberOfInputs: 1,
            numberOfOutputs: 1,
            outputChannelCount: [2],
            parameterData: {
                posX: options.posX ?? 0.0,
                posY: options.posY ?? 1.5,
                posZ: options.posZ ?? 0.0,
                pinnaScale: options.pinnaScale ?? 1.0,
                elevationStrength: options.elevationStrength ?? 1.0,
                transientCrispness: options.transientCrispness ?? 0.7,
                grounding: options.grounding ?? 0.4,
                altitudeMax: options.altitudeMax ?? 2.5,
                altitudeMin: options.altitudeMin ?? -0.8,
                enableCeiling: options.enableCeiling ? 1.0 : 0.0,
                dopplerAmount: options.dopplerAmount ?? 1.0,
                dryWet: options.dryWet ?? 1.0
            }
        });

        this._posX = options.posX ?? 0.0;
        this._posY = options.posY ?? 1.5;
        this._posZ = options.posZ ?? 0.0;

        // Autonomous trajectory state
        this._flightMode = 'manual';
        this._flightPhase = 0.0;
        this._flightOptions = {};
        this._animFrameId = null;
        this._lastTime = performance.now();
    }

    /**
     * Set Cartesian 3D position (X, Y, Z in meters).
     * Listener is at (0, 0, 0) facing +Y with +Z up and +X right.
     */
    setPosition(x, y, z) {
        this._posX = x;
        this._posY = y;
        this._posZ = z;

        const currentTime = this.context.currentTime;
        // Schedule quick smooth ramp to avoid parameter jumps
        this.parameters.get('posX').setValueAtTime(x, currentTime);
        this.parameters.get('posY').setValueAtTime(y, currentTime);
        this.parameters.get('posZ').setValueAtTime(z, currentTime);
    }

    /**
     * Set Spherical coordinates:
     * - azimuth: -180 to +180 deg (0 = front, +90 = right, -90 = left, 180 = behind)
     * - elevation: -50 to +90 deg (0 = horizon, +90 = overhead zenith, -40 = chest level)
     * - distance: distance in meters
     */
    setSpherical({ azimuth = 0, elevation = 0, distance = 1.5 }) {
        const azRad = (azimuth * Math.PI) / 180.0;
        const elRad = (elevation * Math.PI) / 180.0;

        const horizDist = distance * Math.cos(elRad);
        const x = horizDist * Math.sin(azRad);
        const y = horizDist * Math.cos(azRad);
        const z = distance * Math.sin(elRad);

        this.setPosition(x, y, z);
    }

    setTransientCrispness(amount) {
        this.parameters.get('transientCrispness').setValueAtTime(
            Math.min(Math.max(0.0, amount), 1.0),
            this.context.currentTime
        );
    }

    setGrounding(amount) {
        this.parameters.get('grounding').setValueAtTime(
            Math.min(Math.max(0.0, amount), 1.0),
            this.context.currentTime
        );
    }

    /**
     * Enable or disable ceiling reflection (disabled by default)
     * @param {boolean} enabled
     */
    setCeilingEnabled(enabled = false) {
        this.parameters.get('enableCeiling').setValueAtTime(
            enabled ? 1.0 : 0.0,
            this.context.currentTime
        );
    }

    setAltitudeBounds(minAlt = -0.8, maxAlt = 2.5) {
        const t = this.context.currentTime;
        this.parameters.get('altitudeMin').setValueAtTime(minAlt, t);
        this.parameters.get('altitudeMax').setValueAtTime(maxAlt, t);
    }

    setElevationStrength(strength) {
        this.parameters.get('elevationStrength').setValueAtTime(
            Math.min(Math.max(0.0, strength), 1.5),
            this.context.currentTime
        );
    }

    setPinnaScale(scale) {
        this.parameters.get('pinnaScale').setValueAtTime(
            Math.min(Math.max(0.75, scale), 1.25),
            this.context.currentTime
        );
    }

    setDopplerAmount(amount) {
        this.parameters.get('dopplerAmount').setValueAtTime(
            Math.min(Math.max(0.0, amount), 2.0),
            this.context.currentTime
        );
    }

    setDryWet(dryWet) {
        this.parameters.get('dryWet').setValueAtTime(
            Math.min(Math.max(0.0, dryWet), 1.0),
            this.context.currentTime
        );
    }

    /**
     * Start autonomous flight trajectories
     * @param {'manual' | 'swoop' | 'helical' | 'zenith' | 'flutter'} mode
     * @param {Object} options speed, altitudeMin, altitudeMax, radius, turbulence
     */
    setFlightMode(mode = 'manual', options = {}) {
        this._flightMode = mode;
        this._flightOptions = {
            speed: options.speed ?? 0.25,          // Hz
            altitudeMin: options.altitudeMin ?? -0.8,
            altitudeMax: options.altitudeMax ?? 2.5,
            radius: options.radius ?? 1.4,
            turbulence: options.turbulence ?? 0.15,
            ...options
        };

        if (mode === 'manual') {
            if (this._animFrameId) {
                cancelAnimationFrame(this._animFrameId);
                this._animFrameId = null;
            }
            return;
        }

        if (!this._animFrameId) {
            this._lastTime = performance.now();
            this._animateFlight();
        }
    }

    _animateFlight = () => {
        if (this._flightMode === 'manual') return;

        const now = performance.now();
        const dt = (now - this._lastTime) / 1000.0;
        this._lastTime = now;

        const opt = this._flightOptions;
        this._flightPhase = (this._flightPhase + opt.speed * dt) % 1.0;
        const p = this._flightPhase;
        const flutter = (Math.sin(p * Math.PI * 14.0) * opt.turbulence) * 0.3;

        let x = 0, y = 1.5, z = 0;

        switch (this._flightMode) {
            case 'swoop': {
                // Front (+4m) to rear (-4m) swoop down to ear level then pull up
                const travelY = 4.0 - p * 8.0;
                const u = (p - 0.5) * 2.0;
                z = opt.altitudeMin + (u * u) * (opt.altitudeMax - opt.altitudeMin);
                x = opt.radius * Math.sin(p * Math.PI) + flutter;
                y = travelY;
                break;
            }
            case 'helical': {
                // Orbit around listener while oscillating up and down
                const angle = p * Math.PI * 2.0;
                const vertOsc = (Math.sin(p * Math.PI) + 1.0) * 0.5;
                z = opt.altitudeMin + vertOsc * (opt.altitudeMax - opt.altitudeMin);
                x = opt.radius * Math.sin(angle) + flutter;
                y = opt.radius * Math.cos(angle) + flutter;
                break;
            }
            case 'zenith': {
                // Front horizon straight overhead to rear
                const angle = p * Math.PI;
                y = opt.radius * Math.cos(angle);
                z = opt.radius * Math.sin(angle) + opt.altitudeMin * (1.0 - Math.sin(angle));
                x = flutter * 0.5;
                break;
            }
            case 'flutter': {
                const t = p * Math.PI * 2.0;
                x = opt.radius * (Math.sin(t) + 0.5 * Math.sin(3.0 * t)) + flutter;
                y = opt.radius * (Math.cos(t) + 0.3 * Math.cos(2.0 * t)) + flutter;
                const zBase = opt.altitudeMin + 0.5 * (opt.altitudeMax - opt.altitudeMin);
                const zAmp = (opt.altitudeMax - opt.altitudeMin) * 0.45;
                z = zBase + zAmp * Math.sin(4.0 * t + flutter);
                break;
            }
        }

        this.setPosition(x, y, z);
        this._animFrameId = requestAnimationFrame(this._animateFlight);
    };

    getPosition() {
        return { x: this._posX, y: this._posY, z: this._posZ };
    }

    getSpherical() {
        const horizDist = Math.sqrt(this._posX * this._posX + this._posY * this._posY);
        const distance = Math.sqrt(horizDist * horizDist + this._posZ * this._posZ);
        const azimuth = Math.atan2(this._posX, this._posY) * (180.0 / Math.PI);
        const elevation = Math.atan2(this._posZ, Math.max(0.001, horizDist)) * (180.0 / Math.PI);
        return { azimuth, elevation, distance };
    }
}
