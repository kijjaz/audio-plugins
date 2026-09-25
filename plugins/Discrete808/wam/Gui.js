export class Discrete808Gui extends HTMLElement {
    constructor() {
        super();
        this.plugin = null;
        this.root = this.attachShadow({ mode: 'open' });
    }

    connectedCallback() {
        this.render();
    }

    render() {
        this.root.innerHTML = `
        <style>
            :host {
                display: block;
                width: 600px;
                height: 400px;
                background: #111;
                border: 2px solid #555;
                font-family: 'Helvetica Neue', sans-serif;
                color: #eee;
                padding: 20px;
                box-sizing: border-box;
            }
            .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #333; padding-bottom: 10px; margin-bottom: 20px; }
            h1 { font-size: 24px; margin: 0; color: gold; letter-spacing: 2px; }
            .grid { display: grid; grid-template-columns: repeat(6, 1fr); gap: 10px; }
            .channel { background: #1a1a1a; height: 250px; border: 1px solid #333; display: flex; flex-direction: column; align-items: center; justify-content: space-around; }
            .label { font-size: 10px; font-weight: bold; color: #888; }
            .knob { width: 40px; height: 40px; border-radius: 50%; border: 2px solid gold; background: #222; }
            .footer { margin-top: 20px; font-size: 10px; color: #666; text-align: right; }
        </style>
        <div class="header">
            <h1>DISCRETE 808</h1>
            <div class="label">WAM v2 PRODUCTION BUILD</div>
        </div>
        <div class="grid">
            ${['BD', 'SD', 'LT', 'MT', 'HT', 'RS'].map(name => `
                <div class="channel">
                    <div class="knob"></div>
                    <div class="knob" style="width:20px; height:20px; border-color:#888;"></div>
                    <div class="label">${name}</div>
                </div>
            `).join('')}
        </div>
        <div class="footer">ENGINEERING MANUAL LOADED | COMPONENT MODELING ACTIVE</div>
        `;
    }
}

customElements.define('discrete-808-gui', Discrete808Gui);
