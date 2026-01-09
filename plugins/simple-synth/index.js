import { WamNode, WebAudioModule } from '@webaudiomodules/sdk';

/**
 * Simple Synth WAM Module
 */
export default class SimpleSynthModule extends WebAudioModule {
    async createAudioNode(initialState) {
        await this.audioContext.audioWorklet.addModule(new URL('./src/processor.js', import.meta.url));

        const node = new WamNode(this, {
            processorOptions: {
                moduleName: 'SimpleSynth',
            }
        });

        if (initialState) node.setState(initialState);
        return node;
    }

    async createGui() {
        const div = document.createElement('div');
        div.style.color = '#fff';
        div.style.textAlign = 'center';
        div.innerHTML = `
            <h3>Simple Sine Synth</h3>
            <p>Polyphonic Sine Wave Generator</p>
            <div style="background: #333; padding: 10px; border-radius: 5px;">
                Playing: <span id="voice-count">0</span> voices
            </div>
        `;

        // Mock UI interaction
        setInterval(() => {
            const count = this.audioNode ? 0 : 0; // In a real app we'd poll the processor
            const span = div.querySelector('#voice-count');
            if (span) span.innerText = 'Active';
        }, 500);

        return { domElement: div };
    }
}
