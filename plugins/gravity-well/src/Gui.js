
export default class GravityGui extends HTMLElement {
    constructor(plugin) {
        super();
        this.plugin = plugin;
        this.root = this.attachShadow({ mode: 'open' });
        this.readHead = 0;

        // Load Data for visualization
        import('./gravity_data.js').then(module => {
            this.gravityTable = module.GRAVITY_TABLE;
            this.startVis();
        });
    }

    connectedCallback() {
        this.root.innerHTML = `
        <style>
            :host {
                display: block;
                width: 400px;
                height: 350px;
                background: url('./src/assets/gravity_well_bg_no_text.png') no-repeat center center;
                background-size: cover;
                border: 2px solid #333;
                border-radius: 8px;
                font-family: 'Inter', sans-serif;
                color: #e0dacc;
                position: relative;
                overflow: hidden;
            }
            .canvas-container {
                width: 100%;
                height: 200px;
                position: relative;
                /* Transparent to show BG */
                background: rgba(0,0,0,0.3); 
            }
            canvas {
                width: 100%;
                height: 100%;
                display: block;
            }
            .controls {
                padding: 15px;
                display: grid;
                grid-template-columns: 1fr 1fr;
                gap: 10px;
            }
            .slider-group {
                display: flex;
                flex-direction: column;
            }
            label {
                font-size: 10px;
                text-transform: uppercase;
                letter-spacing: 1px;
                color: #888;
                margin-bottom: 4px;
            }
            input[type="range"] {
                width: 100%;
                accent-color: #ffd700;
            }
            h3 {
                position: absolute;
                top: 10px;
                left: 15px;
                margin: 0;
                font-weight: 400;
                color: #ffd700;
                text-shadow: 0 0 10px rgba(255, 215, 0, 0.3);
                pointer-events: none;
            }
            .help-btn {
                position: absolute;
                top: 10px;
                right: 15px;
                background: none;
                border: 1px solid #ffd700;
                color: #ffd700;
                border-radius: 50%;
                width: 25px;
                height: 25px;
                cursor: pointer;
                font-size: 14px;
                display: flex;
                align-items: center;
                justify-content: center;
                z-index: 10;
            }
            .help-overlay {
                position: absolute;
                top: 0;
                left: 0;
                width: 100%;
                height: 100%;
                background: rgba(0, 0, 0, 0.9);
                color: #fff;
                z-index: 20;
                display: none; /* Hidden by default */
                flex-direction: column;
                justify-content: center;
                align-items: center;
                padding: 20px;
                box-sizing: border-box;
                text-align: center;
                font-size: 13px;
                line-height: 1.5;
            }
            .help-overlay h4 {
                color: #ffd700;
                margin-top: 0;
            }
            .close-help {
                margin-top: 20px;
                background: #ffd700;
                border: none;
                color: #000;
                padding: 5px 15px;
                cursor: pointer;
                border-radius: 4px;
                font-weight: bold;
            }
        </style>
        
        <h3>GRAVITY WELL</h3>
        <button id="helpBtn" class="help-btn">?</button>

        <div id="helpOverlay" class="help-overlay">
            <h4>GRAVITY WELL v2.01</h4>
            <p>
                Physics Engine: 3-Body Simulation (Sun, Planet, Moon) modulates
                FM Synthesis indices and oscillator detuning.
            </p>
            <div style="text-align: left; display: inline-block;">
                Controls:<br>
                - Speed: Simulation time rate.<br>
                - Gravity: Modulation depth.<br>
                - Dust Amt: Granular synthesis mix.<br>
                - Dust Dens: Granular density/jitter.
            </div>
            <p style="margin-top:15px; font-size: 11px; opacity: 0.7;">Created by Kijjaz</p>
            <button id="closeHelp" class="close-help">CLOSE</button>
        </div>

        <div class="canvas-container">
            <canvas id="orbitCanvas"></canvas>
        </div>
        
        <div class="controls">
            <div class="slider-group">
                <label>Sim Speed</label>
                <input type="range" id="speed" min="0" max="5" step="0.1" value="1.0">
            </div>
            <div class="slider-group">
                <label>Gravity Amt</label>
                <input type="range" id="gravity" min="0" max="1" step="0.01" value="1.0">
            </div>
             <div class="slider-group" style="grid-column: span 2;">
                <label>Base Pitch</label>
                <input type="range" id="pitch" min="24" max="84" step="1" value="48">
            </div>
        </div>
        `;

        this.canvas = this.root.querySelector("#orbitCanvas");
        this.ctx = this.canvas.getContext('2d');

        // Help Logic
        const helpBtn = this.root.getElementById('helpBtn');
        const overlay = this.root.getElementById('helpOverlay');
        const closeHelp = this.root.getElementById('closeHelp');

        const toggleHelp = () => {
            const isHidden = overlay.style.display === 'none' || overlay.style.display === '';
            overlay.style.display = isHidden ? 'flex' : 'none';
        };

        helpBtn.addEventListener('click', toggleHelp);
        closeHelp.addEventListener('click', toggleHelp);

        this.bindControl('speed', 'simulationSpeed');
        this.bindControl('gravity', 'gravityAmount');
        this.bindControl('pitch', 'basePitch');

        // Resize observer
        const ro = new ResizeObserver(() => {
            this.canvas.width = this.canvas.clientWidth;
            this.canvas.height = this.canvas.clientHeight;
        });
        ro.observe(this.canvas);
    }

    bindControl(id, paramId) {
        const el = this.root.getElementById(id);
        el.addEventListener('input', (e) => {
            if (this.plugin && this.plugin.audioNode) {
                const param = this.plugin.audioNode.parameters.get(paramId);
                if (param) param.value = parseFloat(e.target.value);
            }
        });
    }

    startVis() {
        const draw = () => {
            if (!this.gravityTable) return;

            const w = this.canvas.width;
            const h = this.canvas.height;
            const ctx = this.ctx;

            ctx.fillStyle = 'rgba(0, 0, 0, 0.1)'; // Trail effect
            ctx.fillRect(0, 0, w, h);

            // We need to know where the read head is.
            // But we can't easily get it from AudioWorklet without messaging.
            // For now, let's just simulate the visual speed roughly matching.
            // Or better: Just animate the whole system in a loop for visualization?
            // "Scrum" functionality requires bidirectional comms which is complex for this fast demo.
            // Let's just animate an "Approximation" using our own read head.

            this.readHead += (this.root.getElementById('speed').value * 100 / 60); // approx
            if (this.readHead >= this.gravityTable.length) this.readHead = 0;

            const idx = Math.floor(this.readHead);
            const row = this.gravityTable[idx];
            // x1,y1,x2,y2,x3,y3 are -1.0 to 1.0 (Centered)

            // Helper: Map -1..1 to 0..size
            const mapX = (x) => (x + 1) * 0.5 * w;
            const mapY = (y) => (y + 1) * 0.5 * h;

            // Sun (Yellow)
            const sx = mapX(row[0]);
            const sy = mapY(row[1]);
            ctx.beginPath();
            ctx.arc(sx, sy, 8, 0, Math.PI * 2);
            ctx.fillStyle = '#ffd700'; // Gold
            ctx.shadowBlur = 15;
            ctx.shadowColor = '#ffd700';
            ctx.fill();
            ctx.shadowBlur = 0;

            // Planet (Blue)
            const px = mapX(row[2]);
            const py = mapY(row[3]);
            ctx.beginPath();
            ctx.arc(px, py, 5, 0, Math.PI * 2);
            ctx.fillStyle = '#00aaff'; // Cyan
            ctx.fill();

            // Moon (White)
            const mx = mapX(row[4]);
            const my = mapY(row[5]);
            ctx.beginPath();
            ctx.arc(mx, my, 2, 0, Math.PI * 2);
            ctx.fillStyle = '#ffffff';
            ctx.fill();

            // --- Goniometer (Stereo Scope) ---
            // Draw a small overlay in bottom right or center?
            // User asked for "simple and not too big".
            // Let's put it at bottom center, semi-transparent.

            if (this.analyserL && this.analyserR) {
                if (!this.waveL) this.waveL = new Float32Array(this.analyserL.frequencyBinCount);
                if (!this.waveR) this.waveR = new Float32Array(this.analyserR.frequencyBinCount);

                this.analyserL.getFloatTimeDomainData(this.waveL);
                this.analyserR.getFloatTimeDomainData(this.waveR);

                // Scope Box
                const scopeSize = 80;
                const ScopeX = w - scopeSize - 10; // Bottom Right
                const ScopeY = h - scopeSize - 10;
                const center = scopeSize / 2;

                // BG for Scope
                ctx.fillStyle = 'rgba(0, 0, 0, 0.5)';
                ctx.fillRect(ScopeX, ScopeY, scopeSize, scopeSize);
                ctx.strokeStyle = '#333';
                ctx.strokeRect(ScopeX, ScopeY, scopeSize, scopeSize);

                ctx.save();
                ctx.translate(ScopeX + center, ScopeY + center);
                // Rotate 45 deg for standard Stereo phase plot
                ctx.rotate(-Math.PI / 4);

                ctx.beginPath();
                ctx.strokeStyle = 'rgba(0, 255, 100, 0.8)'; // Sci-fi Green
                ctx.lineWidth = 1;

                // Downsample for performance? 
                const step = 2;
                for (let i = 0; i < this.waveL.length; i += step) {
                    const l = this.waveL[i];
                    const r = this.waveR[i];

                    // Map L to X, R to Y? 
                    // Lissajous: X = L, Y = R.
                    // Rotated 45 deg: M/S (Mid/Side).
                    // We rotated canvas, so drawing pure L/R gives diamond.

                    const x = l * (scopeSize * 0.4);
                    const y = r * (scopeSize * 0.4);

                    if (i === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.stroke();
                ctx.restore();
            }

            requestAnimationFrame(draw);
        };
        draw();
    }
}

customElements.define('gravity-gui', GravityGui);
