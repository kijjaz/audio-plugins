/**
 * FlyByContext
 * Manages loading the AudioWorklet and instantiating 3D panner nodes.
 */
import { FlyByPannerNode } from './FlyByPannerNode.js';
import { FLYBY_PROCESSOR_CODE } from '../dsp/FlyByProcessorInline.js';

export class FlyByContext {
    /**
     * @param {AudioContext} audioContext
     */
    constructor(audioContext = new (window.AudioContext || window.webkitAudioContext)()) {
        this.audioContext = audioContext;
        this._isReady = false;
        this._readyPromise = null;
    }

    /**
     * Loads and registers the AudioWorkletProcessor.
     * Uses inlined Blob by default for instant, zero-CORS, zero-404 operation.
     */
    async ready(processorUrl = null) {
        if (this._isReady) return true;
        if (this._readyPromise) return this._readyPromise;

        this._readyPromise = (async () => {
            if (processorUrl) {
                await this.audioContext.audioWorklet.addModule(processorUrl);
            } else {
                // Auto-create Blob URL from inlined code: guaranteed to work regardless of server root or CORS!
                const blob = new Blob([FLYBY_PROCESSOR_CODE], { type: 'application/javascript' });
                const blobUrl = URL.createObjectURL(blob);
                try {
                    await this.audioContext.audioWorklet.addModule(blobUrl);
                } finally {
                    URL.revokeObjectURL(blobUrl);
                }
            }
            this._isReady = true;
            return true;
        })();

        return this._readyPromise;
    }

    /**
     * Factory function to create a FlyByPannerNode
     * @param {Object} options Initial configuration
     */
    createPanner(options = {}) {
        if (!this._isReady) {
            console.warn('[FlyByWeb] createPanner called before ready() resolved. AudioWorklet must be registered first.');
        }
        return new FlyByPannerNode(this.audioContext, options);
    }
}
