import { WamNode, WamProcessor } from '@webaudiomodules/sdk';

/**
 * Simple Delay Processor (AudioWorklet side)
 */
class SimpleDelayProcessor extends WamProcessor {
    constructor(options) {
        super(options);
        this.delayLine = [new Float32Array(this.sampleRate * 2), new Float32Array(this.sampleRate * 2)];
        this.writeIndex = 0;
        this.delaySamples = Math.floor(this.sampleRate * 0.5); // 500ms default
        this.feedback = 0.4;
    }

    _process(startSample, endSample, inputs, outputs, parameters) {
        const input = inputs[0];
        const output = outputs[0];

        if (!input || !input[0]) return;

        for (let i = startSample; i < endSample; i++) {
            for (let ch = 0; ch < input.length; ch++) {
                const inSample = input[ch][i];
                const readIndex = (this.writeIndex - this.delaySamples + this.delayLine[ch].length) % this.delayLine[ch].length;
                const delayedSample = this.delayLine[ch][readIndex];

                this.delayLine[ch][this.writeIndex] = inSample + delayedSample * this.feedback;
                output[ch][i] = inSample + delayedSample;
            }
            this.writeIndex = (this.writeIndex + 1) % this.delayLine[0].length;
        }
    }
}

try {
    registerProcessor('SimpleDelayProcessor', SimpleDelayProcessor);
} catch (e) {
    // Already registered
}
