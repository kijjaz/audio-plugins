import AudioWorkletProcessor from './src/AudioWorkletProcessor.js';

export default class Discrete808Wam extends WebAudioModule {
    async createAudioNode(initialState) {
        await this.audioContext.audioWorklet.addModule(new URL('./src/AudioWorkletProcessor.js', import.meta.url));

        const node = new AudioWorkletNode(this.audioContext, 'discrete-808-processor', {
            processorOptions: {
                numberOfInputs: 0,
                numberOfOutputs: 1,
                outputChannelCount: [2],
            },
        });

        return node;
    }

    async createGui() {
        const div = document.createElement('div');
        div.style.padding = "20px";
        div.style.color = "#eee";
        div.style.background = "#111";
        div.style.border = "1px solid #B8860B";
        div.innerHTML = `
            <h1 style="color: gold; margin: 0;">DISCRETE 808</h1>
            <p style="font-size: 12px; color: #888;">COMPONENT-MODELED SYNTHESIS</p>
            <div style="margin-top: 10px; font-family: monospace; font-size: 11px;">
                STATUS: WASM Engine Loaded (Placeholder)
            </div>
        `;
        return div;
    }
}
