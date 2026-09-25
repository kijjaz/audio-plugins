import { WamNode, WebAudioModule } from '@webaudiomodules/sdk';

/**
 * Simple Delay WAM Module
 */
export default class SimpleDelayModule extends WebAudioModule {
    async createAudioNode(initialState) {
        await this.audioContext.audioWorklet.addModule(new URL('./src/processor.js', import.meta.url));

        const node = new WamNode(this, {
            processorOptions: {
                moduleName: 'SimpleDelay',
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
            <h3>Simple Stereo Delay</h3>
            <p>Feedback Delay Effect</p>
            <div style="background: #333; padding: 10px; border-radius: 5px;">
                Mode: Active
            </div>
        `;
        return { domElement: div };
    }
}
