import { WebAudioModule } from '@webaudiomodules/sdk';

export default class GravityPlugin extends WebAudioModule {
    async createAudioNode(initialState) {
        await this.audioContext.audioWorklet.addModule(new URL('./src/AudioWorkletProcessor.js', import.meta.url));

        const worklet = new AudioWorkletNode(this.audioContext, 'GravityProcessor', {
            numberOfInputs: 0,
            numberOfOutputs: 1,
            outputChannelCount: [2]
        });

        // --- Stereo Scope Setup ---
        const splitter = this.audioContext.createChannelSplitter(2);
        this.analyserL = this.audioContext.createAnalyser();
        this.analyserR = this.audioContext.createAnalyser();

        // Scope settings
        this.analyserL.fftSize = 2048; // Buffer size for waveform
        this.analyserR.fftSize = 2048;

        worklet.connect(splitter);
        splitter.connect(this.analyserL, 0);
        splitter.connect(this.analyserR, 1);

        // Output node via a generic Gain to hold the connection chain (WAM SDK expects a single node usually)
        // Or we can return the worklet and attach analysers as "Monitoring" sidechain. 
        // WAM host connects the returned node to destination.
        // We must ensure the worklet is connected to destination too.
        // Let's assume the returned node is connected by host. 
        // Our splitters are side-chained.

        // We attach analysers to the plugin instance (this) so GUI can access them.
        this.scopeNode = worklet;

        return worklet;
    }

    async createGui() {
        await import('./src/Gui.js');
        const gui = document.createElement('gravity-gui');
        gui.plugin = this;
        // Pass Analysers
        gui.analyserL = this.analyserL;
        gui.analyserR = this.analyserR;
        return gui;
    }
}
