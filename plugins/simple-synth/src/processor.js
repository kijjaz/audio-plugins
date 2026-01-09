import { WamNode, WamProcessor } from '@webaudiomodules/sdk';

/**
 * Simple Synth Processor (AudioWorklet side)
 */
class SimpleSynthProcessor extends WamProcessor {
    constructor(options) {
        super(options);
        this.voices = new Map(); // MIDI note -> frequency
    }

    _process(startSample, endSample, inputs, outputs, parameters) {
        const output = outputs[0][0]; // mono

        for (let i = startSample; i < endSample; i++) {
            let sample = 0;
            this.voices.forEach((voice, note) => {
                // Very basic naive sine osc
                voice.phase += (2 * Math.PI * voice.freq) / this.sampleRate;
                sample += Math.sin(voice.phase) * 0.2;
            });
            output[i] = sample;
        }

        // Auto-cleanup voices? (Not implemented here for simplicity)
    }

    _onMidi(midiData) {
        const [status, data1, data2] = midiData.bytes;
        const type = status & 0xf0;

        if (type === 0x90 && data2 > 0) { // Note On
            const freq = 440 * Math.pow(2, (data1 - 69) / 12);
            this.voices.set(data1, { freq, phase: 0 });
        } else if (type === 0x80 || (type === 0x90 && data2 === 0)) { // Note Off
            this.voices.delete(data1);
        }
    }
}

try {
    registerProcessor('SimpleSynthProcessor', SimpleSynthProcessor);
} catch (e) {
    // Already registered?
}
