console.log('Main.js is executing');
import { WamHost } from './engine/WamHost.js';
import { MasterRecorder } from './engine/MasterRecorder.js';

const host = new WamHost();
let recorder = null;
let currentPluginInstance = null;
let activeTab = 'url';

// UI Elements
const btnInit = document.getElementById('btn-init');
const transportControls = document.getElementById('transport-controls');
const btnRecord = document.getElementById('btn-record');
const btnStop = document.getElementById('btn-stop');
const btnExport = document.getElementById('btn-export');
const btnAddPlugin = document.getElementById('btn-add-plugin');
const pluginModal = document.getElementById('plugin-modal');
const btnLoadConfirm = document.getElementById('btn-load-confirm');
const btnModalClose = document.getElementById('btn-modal-close');
const pluginUrlInput = document.getElementById('plugin-url');
const pluginCodeInput = document.getElementById('plugin-code');
const faustCodeInput = document.getElementById('faust-code');
const slotsContainer = document.getElementById('slots-container');
const wamGuiContainer = document.getElementById('wam-gui');
const activePluginName = document.getElementById('active-plugin-name');
const midiStatus = document.getElementById('midi-status');

// Tabs
document.querySelectorAll('.tab-btn').forEach(btn => {
    btn.addEventListener('click', () => {
        document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
        document.querySelectorAll('.tab-content').forEach(c => c.classList.add('hidden'));

        btn.classList.add('active');
        activeTab = btn.dataset.tab;
        document.getElementById(`tab-${activeTab}`).classList.remove('hidden');
    });
});

// Init Engine
btnInit.addEventListener('click', async () => {
    try {
        btnInit.innerText = 'Initializing...';
        btnInit.disabled = true;

        await host.initialize();
        recorder = new MasterRecorder(host.audioContext);

        btnInit.classList.add('hidden');
        transportControls.classList.remove('hidden');

        setupMidi();
        console.log('Engine started successfully');
    } catch (e) {
        btnInit.innerText = 'Initialization Failed';
        btnInit.disabled = false;
        console.error('Engine Init Error:', e);
        alert('Failed to initialize engine. Check console for details.');
    }
});

// Plugin Management
btnAddPlugin.addEventListener('click', () => {
    pluginModal.classList.remove('hidden');
});

btnModalClose.addEventListener('click', () => {
    pluginModal.classList.add('hidden');
});

btnLoadConfirm.addEventListener('click', async () => {
    let url = '';
    let name = 'Plugin';

    btnLoadConfirm.disabled = true;
    btnLoadConfirm.innerText = 'Loading...';

    try {
        let instance = null;

        if (activeTab === 'url') {
            url = pluginUrlInput.value;
            instance = await host.loadPlugin(url);
        } else if (activeTab === 'code') {
            // Live JS Compile
            const code = pluginCodeInput.value;
            const blob = new Blob([code], { type: 'text/javascript' });
            url = URL.createObjectURL(blob);
            instance = await host.loadPlugin(url);
        } else if (activeTab === 'faust') {
            // Faust Compile
            const dspCode = faustCodeInput.value;
            // For now, we'll alert that Faust needs the SDK, 
            // but we'll show the intent.
            alert("Faust dynamic compilation requires @grame/faustwam SDK. Integrating SDK pattern...");
            // Mocking a successful compile if we had the SDK:
            // instance = await FaustLoader.compile(dspCode, "FaustNoise", host.audioContext, host.groupId);
            throw new Error("Faust SDK not integrated in this build.");
        }

        if (instance) {
            addPluginToUI(instance);
            host.connectPlugins();
            recorder.connect(host.audioContext.destination);
            pluginModal.classList.add('hidden');
        }
    } catch (e) {
        console.error(e);
        alert('Failed to load plugin: ' + e.message);
    } finally {
        btnLoadConfirm.disabled = false;
        btnLoadConfirm.innerText = 'Load';
    }
});

function addPluginToUI(instance) {
    const slot = document.createElement('div');
    slot.className = 'plugin-slot';
    const name = instance.audioNode?.moduleName || 'Unnamed Plugin';

    slot.innerHTML = `
        <div class="plugin-info">
            <div class="plugin-name">${name}</div>
            <div class="plugin-type">WAM v2 ${instance.audioNode?.scheduleEvents ? '[MIDI]' : ''}</div>
        </div>
        <div class="slot-actions">
            <button class="btn-small btn-view">View</button>
            <button class="btn-small btn-remove">X</button>
        </div>
    `;

    const empty = slotsContainer.querySelector('.empty-notif');
    if (empty) empty.remove();

    slotsContainer.appendChild(slot);

    slot.querySelector('.btn-view').addEventListener('click', () => {
        showPluginGUI(instance);
        document.querySelectorAll('.plugin-slot').forEach(s => s.classList.remove('active'));
        slot.classList.add('active');
    });

    slot.querySelector('.btn-remove').addEventListener('click', () => {
        const index = host.plugins.indexOf(instance);
        if (index > -1) {
            // Cleanup MIDI listeners if any
            if (instance._midiListener && instance.audioNode.removeEventListener) {
                instance.audioNode.removeEventListener('wam-midi', instance._midiListener);
            }
            host.plugins.splice(index, 1);
            slot.remove();
            host.connectPlugins();
            if (currentPluginInstance === instance) {
                wamGuiContainer.innerHTML = '';
                activePluginName.innerText = 'None';
            }
            if (host.plugins.length === 0) {
                slotsContainer.innerHTML = '<div class="empty-notif">No plugins loaded. Click + to add.</div>';
            }
        }
    });

    showPluginGUI(instance);
    document.querySelectorAll('.plugin-slot').forEach(s => s.classList.remove('active'));
    slot.classList.add('active');
}

async function showPluginGUI(instance) {
    currentPluginInstance = instance;
    activePluginName.innerText = instance.audioNode?.moduleName || "Plugin";

    wamGuiContainer.innerHTML = '';
    try {
        if (instance.createGui) {
            const { domElement } = await instance.createGui();
            wamGuiContainer.appendChild(domElement);
        } else {
            throw new Error("No GUI");
        }
    } catch (e) {
        wamGuiContainer.innerHTML = `<div style="color: grey; font-style: italic;">No GUI available for this plugin.</div>`;
    }
}

// Transport
let recordedBlob = null;

btnRecord.addEventListener('click', () => {
    recorder.start();
    btnRecord.disabled = true;
    btnStop.disabled = false;
    btnExport.disabled = true;
    btnRecord.innerText = '● Recording...';
});

btnStop.addEventListener('click', async () => {
    recordedBlob = await recorder.stop();
    btnRecord.disabled = false;
    btnStop.disabled = true;
    btnExport.disabled = false;
    btnRecord.innerText = '● Record';
});

btnExport.addEventListener('click', () => {
    if (recordedBlob) {
        recorder.download(recordedBlob);
    }
});

// MIDI
function setupMidi() {
    if (navigator.requestMIDIAccess) {
        navigator.requestMIDIAccess().then(onMidiSuccess, onMidiFailure);
    } else {
        midiStatus.innerText = 'MIDI: Not supported';
    }
}

function onMidiSuccess(midiAccess) {
    midiStatus.innerText = 'MIDI: Ready';
    for (const input of midiAccess.inputs.values()) {
        input.onmidimessage = (message) => {
            host.broadcastMidi(message.data);
        };
    }
}

function onMidiFailure() {
    midiStatus.innerText = 'MIDI: Access failed';
}
