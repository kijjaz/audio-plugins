import { WamNode, WebAudioModule } from '@webaudiomodules/sdk';
import Gui from './src/Gui.js';

export default class CableLiftModule extends WebAudioModule {
    async createAudioNode(initialState) {
        await this.audioContext.audioWorklet.addModule(new URL('./src/AudioWorkletProcessor.js', import.meta.url));

        const node = new WamNode(this, {
            processorOptions: {
                moduleName: 'CableLiftProcessor',
            }
        });

        if (initialState) node.setState(initialState);
        return node;
    }

    async createGui() {
        return new Gui(this);
    }
}
