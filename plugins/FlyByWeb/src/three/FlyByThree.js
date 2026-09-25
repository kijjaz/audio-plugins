/**
 * FlyByWeb Three.js Integration
 * Provides drop-in helper bindings for Three.js applications.
 */

export class FlyByListener {
    /**
     * @param {FlyByContext} spatialContext
     */
    constructor(spatialContext) {
        this.spatialContext = spatialContext;
        this.matrixWorldInverse = null;
    }

    update(camera) {
        if (!camera) return;
        camera.updateMatrixWorld();
        this.matrixWorldInverse = camera.matrixWorldInverse;
    }
}

export class FlyByAudio {
    /**
     * @param {FlyByContext} spatialContext
     * @param {Object} options Panner configuration options
     */
    constructor(spatialContext, options = {}) {
        this.panner = spatialContext.createPanner(options);
        this._worldPos = { x: 0, y: 0, z: 0 };
    }

    /**
     * Call every frame in requestAnimationFrame(render)
     * @param {THREE.Object3D} targetObject Object emitting the sound
     * @param {THREE.Camera} camera Current active camera
     */
    update(targetObject, camera) {
        if (!targetObject || !camera) return;

        // Get relative position in listener (camera) local coordinates:
        // camera.worldToLocal transforms world position into eye-space:
        // X = Right (+X)
        // Y = Up (+Z in audio coordinate frame)
        // -Z = Forward (+Y in audio coordinate frame)
        if (targetObject.getWorldPosition) {
            targetObject.getWorldPosition(this._worldPos);
            camera.worldToLocal(this._worldPos);

            const audioX = this._worldPos.x;
            const audioY = -this._worldPos.z; // Camera forward in Three.js is -Z
            const audioZ = this._worldPos.y;  // Camera up in Three.js is +Y

            this.panner.setPosition(audioX, audioY, audioZ);
        }
    }

    connect(destination) {
        return this.panner.connect(destination);
    }
}
