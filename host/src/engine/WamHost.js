import { initializeWamHost } from '@webaudiomodules/sdk';

/**
 * WamHost manages the AudioContext and the WamGroup for hosting WAM plugins.
 */
export class WamHost {
    constructor() {
        this.audioContext = null;
        this.wamGroup = null;
        this.plugins = []; // List of loaded plugin instances
        this.groupId = 'wam-studio-host';
    }

    /**
     * Initialize the AudioContext and WamGroup.
     * Must be called after a user gesture.
     */
    async initialize() {
        if (this.audioContext) return;

        this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
        console.log('AudioContext created, state:', this.audioContext.state);

        if (this.audioContext.state === 'suspended') {
            await this.audioContext.resume();
            console.log('AudioContext resumed');
        }

        // Initialize WAM Group
        // SDK 0.0.12 uses functional initialization
        await initializeWamHost(this.audioContext, this.groupId);
        this.wamGroup = true; // Mark as initialized

        console.log('WamHost initialized', this.groupId);
    }

    /**
     * Load a plugin from a remote or local URL.
     * @param {string} url - The URL to the plugin's index.js
     */
    async loadPlugin(url) {
        if (!this.wamGroup) await this.initialize();

        try {
            // WAM v2 plugins are loaded as ES modules
            const { default: createWamInstance } = await import(url);

            // Create the WAM instance
            const instance = await createWamInstance(this.groupId, this.audioContext);

            this.plugins.push(instance);
            console.log('Plugin loaded:', instance.audioNode.moduleName);

            return instance;
        } catch (error) {
            console.error('Failed to load plugin:', url, error);
            throw error;
        }
    }

    /**
     * Connect plugins in a chain: Host -> Plugin 1 -> Plugin 2 -> Master
     * Handles both Audio and MIDI chaining.
     */
    connectPlugins() {
        if (this.plugins.length === 0) return;

        // Disconnect all first for a clean state
        this.plugins.forEach(p => {
            p.audioNode.disconnect();
            // Clear any previous MIDI listeners to avoid duplicates
            if (p._midiListener) {
                p.audioNode.removeEventListener('wam-midi', p._midiListener);
            }
        });

        for (let i = 0; i < this.plugins.length; i++) {
            const current = this.plugins[i].audioNode;

            // Audio Routing
            if (i < this.plugins.length - 1) {
                const next = this.plugins[i + 1].audioNode;
                current.connect(next);
            } else {
                // Connect the last plugin to the destination
                current.connect(this.audioContext.destination);
            }

            // MIDI Chaining (Passthrough)
            if (i < this.plugins.length - 1) {
                const next = this.plugins[i + 1].audioNode;
                this.plugins[i]._midiListener = (event) => {
                    const { data } = event.detail;
                    if (next.scheduleEvents) {
                        next.scheduleEvents({
                            type: 'wam-midi',
                            time: this.audioContext.currentTime,
                            data
                        });
                    }
                };
                // Register event listener if the node supports it
                if (current.addEventListener) {
                    current.addEventListener('wam-midi', this.plugins[i]._midiListener);
                }
            }
        }
    }

    /**
     * Centralized MIDI input broadcaster.
     * @param {number[]} midiData - standard 3-byte MIDI message
     */
    broadcastMidi(midiData) {
        this.plugins.forEach(instance => {
            if (instance.audioNode.scheduleEvents) {
                instance.audioNode.scheduleEvents({
                    type: 'wam-midi',
                    time: this.audioContext.currentTime,
                    data: { bytes: midiData }
                });
            }
        });
    }
}
