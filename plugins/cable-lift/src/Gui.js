import { WamElement } from '@webaudiomodules/sdk';

class CableLiftGui extends WamElement {
    constructor(plugin) {
        super(plugin);
        this.root = this.attachShadow({ mode: 'open' });
        this.plugin = plugin;
    }

    connectedCallback() {
        const style = document.createElement('style');
        style.textContent = `
			:host {
				display: block;
				width: 400px;
				height: 500px;
				background: linear-gradient(180deg, #1a1a1a 0%, #0d0d0d 100%);
				border: 1px solid #333;
				border-radius: 8px;
				font-family: 'Inter', sans-serif;
				color: #e0e0e0;
				position: relative;
				overflow: hidden;
				box-shadow: 0 20px 50px rgba(0,0,0,0.5);
			}
			
			.bg-image {
				position: absolute;
				top: 0;
				left: 0;
				width: 100%;
				height: 250px;
				background-image: url('./assets/cable_lift.png');
				background-size: cover;
				background-position: center;
				opacity: 0.8;
				mask-image: linear-gradient(to bottom, black 50%, transparent 100%);
				-webkit-mask-image: linear-gradient(to bottom, black 50%, transparent 100%);
			}

			.header {
				position: relative;
				z-index: 2;
				text-align: center;
				padding-top: 20px;
				text-transform: uppercase;
				letter-spacing: 2px;
				text-shadow: 0 2px 4px rgba(0,0,0,0.8);
			}

			.title {
				font-size: 18px;
				font-weight: 700;
				color: #FFD700; /* Gold */
				margin: 0;
			}
			
			.subtitle {
				font-size: 10px;
				color: #888;
				margin-top: 5px;
			}

			.controls {
				position: absolute;
				bottom: 0;
				width: 100%;
				height: 250px;
				padding: 20px;
				box-sizing: border-box;
				display: flex;
				flex-direction: column;
				align-items: center;
				background: linear-gradient(to bottom, transparent, #0a0a0a 40%);
			}

			.main-knob-container {
				display: flex;
				flex-direction: column;
				align-items: center;
				margin-bottom: 20px;
			}

			.knob-row {
				display: flex;
				justify-content: space-around;
				width: 100%;
			}

			.knob-wrapper {
				display: flex;
				flex-direction: column;
				align-items: center;
				width: 80px;
			}
			
			.knob-label {
				font-size: 10px;
				color: #aaa;
				margin-bottom: 5px;
				text-transform: uppercase;
			}

			input[type="range"] {
				-webkit-appearance: none;
				width: 100%;
				background: transparent;
			}
			
			/* Custom Range Slider Styling (Carbon & Gold) */
			input[type="range"]::-webkit-slider-thumb {
				-webkit-appearance: none;
				height: 16px;
				width: 16px;
				border-radius: 50%;
				background: #FFD700;
				cursor: pointer;
				margin-top: -6px;
				box-shadow: 0 0 10px rgba(255, 215, 0, 0.5);
			}
			
			input[type="range"]::-webkit-slider-runnable-track {
				width: 100%;
				height: 4px;
				cursor: pointer;
				background: #333;
				border-radius: 2px;
			}

            .current-level {
                margin-top: 20px;
                font-family: 'Inter', sans-serif;
                font-weight: bold;
                color:  #FFD700;
                font-size: 16px;
                text-shadow: -1px -1px 0 #000, 1px -1px 0 #000, -1px 1px 0 #000, 1px 1px 0 #000;
            }
			.value-display {
				font-family: 'Courier New', monospace;
				font-size: 10px;
				color: #FFD700;
				margin-top: 5px;
			}
			
			.level-display {
			    font-weight: bold;
			    font-size: 14px;
			    color: #FFD700;
			    margin-top: 8px;
                text-shadow: -1px -1px 0 #000, 1px -1px 0 #000, -1px 1px 0 #000, 1px 1px 0 #000;
			}

		`;

        this.root.innerHTML = `
			<div class="bg-image"></div>
			<div class="header">
				<h1 class="title">Cable Lift Refiner</h1>
				<div class="subtitle">Digital Resonance Dampener</div>
			</div>
			
			<div class="controls">
				
				<div class="main-knob-container" style="width: 100%;">
					<div class="knob-label" style="font-size: 12px; color: #fff;">Elevation Level</div>
					<input type="range" id="liftLevel" min="1" max="4" step="0.01" value="1" style="width: 80%;">
					<div class="level-display" id="liftLevelDisplay">ROUND 1: GROUND ZERO</div>
				</div>

				<div class="knob-row">
					<div class="knob-wrapper">
						<div class="knob-label">Sonic Purity</div>
						<input type="range" id="sonicPurity" min="0" max="1" step="0.01" value="0.5">
						<div class="value-display" id="sonicPurityDisplay">50%</div>
					</div>
					
					<div class="knob-wrapper">
						<div class="knob-label">Static Red.</div>
						<input type="range" id="staticReduction" min="0" max="1" step="0.01" value="0.0">
						<div class="value-display" id="staticReductionDisplay">0%</div>
					</div>
					
					<div class="knob-wrapper">
						<div class="knob-label">Focus</div>
						<input type="range" id="focusDetail" min="0" max="1" step="0.01" value="0.5">
						<div class="value-display" id="focusDetailDisplay">50%</div>
					</div>
				</div>
			</div>
		`;

        this.root.appendChild(style);

        // Bind Controls
        this.bindControl('liftLevel', 'liftLevel', (val) => {
            let text = "";
            if (val < 1.5) text = "LEVEL 1: GROUND ZERO";
            else if (val < 2.5) text = "LEVEL 2: MID-AIR ELEVATION";
            else if (val < 3.5) text = "LEVEL 3: HIGH-ALTITUDE LIFT";
            else text = "LEVEL 4: SKY-SCRAPER ZENITH";
            this.root.getElementById('liftLevelDisplay').innerText = text;
        });

        this.bindControl('sonicPurity', 'sonicPurity', (val) => {
            this.root.getElementById('sonicPurityDisplay').innerText = Math.round(val * 100) + "%";
        });

        this.bindControl('staticReduction', 'staticReduction', (val) => {
            this.root.getElementById('staticReductionDisplay').innerText = Math.round(val * 100) + "%";
        });

        this.bindControl('focusDetail', 'focusDetail', (val) => {
            this.root.getElementById('focusDetailDisplay').innerText = Math.round(val * 100) + "%";
        });
    }

    bindControl(id, paramId, displayCallback) {
        const slider = this.root.getElementById(id);
        if (!slider) return;

        slider.addEventListener('input', (e) => {
            const value = parseFloat(e.target.value);
            this.plugin.audioNode.parameters.get(paramId).value = value;
            if (displayCallback) displayCallback(value);
        });
    }
}

try {
    customElements.define('cable-lift-gui', CableLiftGui);
} catch (e) {
    // Already defined
}

export default function createGui(plugin) {
    return new CableLiftGui(plugin);
}
