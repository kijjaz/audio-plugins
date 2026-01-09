/**
 * MasterRecorder captures audio from a node and exports it as a WAV file.
 */
export class MasterRecorder {
    constructor(audioContext) {
        this.audioContext = audioContext;
        this.mediaStreamDestination = audioContext.createMediaStreamDestination();
        this.mediaRecorder = null;
        this.chunks = [];
        this.isRecording = false;
    }

    /**
     * Connect the source node (e.g. Master out) to the recorder.
     * @param {AudioNode} sourceNode 
     */
    connect(sourceNode) {
        sourceNode.connect(this.mediaStreamDestination);
    }

    start() {
        if (this.isRecording) return;
        this.chunks = [];
        this.mediaRecorder = new MediaRecorder(this.mediaStreamDestination.stream);

        this.mediaRecorder.ondataavailable = (e) => {
            if (e.data.size > 0) this.chunks.push(e.data);
        };

        this.mediaRecorder.start();
        this.isRecording = true;
        console.log('Recording started');
    }

    stop() {
        return new Promise((resolve) => {
            if (!this.isRecording) {
                resolve(null);
                return;
            }

            this.mediaRecorder.onstop = () => {
                const blob = new Blob(this.chunks, { type: 'audio/wav' });
                this.isRecording = false;
                console.log('Recording stopped');
                resolve(blob);
            };

            this.mediaRecorder.stop();
        });
    }

    /**
     * Helper to download the blob as a WAV file.
     * @param {Blob} blob 
     */
    download(blob, filename = 'performance.wav') {
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.style.display = 'none';
        a.href = url;
        a.download = filename;
        document.body.appendChild(a);
        a.click();
        setTimeout(() => {
            document.body.removeChild(a);
            window.URL.revokeObjectURL(url);
        }, 100);
    }
}
