import { WamProcessor } from '@webaudiomodules/sdk';

/**
 * Cable Lift Processor
 * Simulates the "benefits" of cable lifters via physical modeling of bad cables.
 */
class CableLiftProcessor extends WamProcessor {
    static get parameterDescriptors() {
        return [
            {
                name: 'liftLevel',
                defaultValue: 1.0,
                minValue: 1.0,
                maxValue: 4.0,
            },
            {
                name: 'sonicPurity',
                defaultValue: 0.5,
                minValue: 0.0,
                maxValue: 1.0,
            },
            {
                name: 'staticReduction',
                defaultValue: 0.0,
                minValue: 0.0,
                maxValue: 1.0,
            },
            {
                name: 'focusDetail',
                defaultValue: 0.5,
                minValue: 0.0,
                maxValue: 1.0,
            }
        ];
    }

    constructor(options) {
        super(options);

        // Jitter state
        this.time = 0;
        this.jitterPhase = 0;

        // Mains hum state
        this.humPhase = 0;

        // Capacitance filter state
        this.lastSampleL = 0;
        this.lastSampleR = 0;
    }

    _process(startSample, endSample, inputs, outputs, parameters) {
        const input = inputs[0];
        const output = outputs[0];

        if (!input || !input[0]) return;
        if (input.length < 2) {
            // Mono to Stereo copy if needed, but usually host handles channel count
        }

        // if (input.length < 2) { // This block is no longer needed as per new code structure
        //     // Mono to Stereo copy if needed, but usually host handles channel count
        // }

        const sonicPurity = parameters.sonicPurity ? parameters.sonicPurity[0] : 0.5;
        const staticReduction = parameters.staticReduction ? parameters.staticReduction[0] : 0.0;
        const focusDetail = parameters.focusDetail ? parameters.focusDetail[0] : 0.5;

        // Inverse Lift: 1.0 = Max Badness, 4.0 = Min Badness

        let lpOut = last + alpha * (input[ch][i] - last);

        // Update state
        if (ch === 0) this.lastSampleL = lpOut;
        else this.lastSampleR = lpOut;

        // Jitter injection (Amplitute Modulation artifacts)
        // Real jitter is timing, but hard to do without interpolation buffer
        // We'll simulate "Loose Contact" crackle via AM
        if (Math.random() < jitterAmt) {
            lpOut *= (1.0 - Math.random() * 0.5);
        }

        // 3. Saturation (Transformer / Tape) - "Focus & Detail"
        // Hyperbolic Tangent soft clip
        if (saturationAmt > 0) {
            lpOut = Math.tanh(lpOut * (1.0 + saturationAmt));
            // Compensate gain slighly? No, loudness is quality.
        }

        // 4. Air Boost (High Shelf approximation)
        // Simple trick: differentiate and add back
        if (airBoost > 0) {
            const diff = input[ch][i] - last; // Highs
            lpOut += diff * airBoost * 0.5;
        }

        // Output
        output[ch][i] = lpOut;
    }
}
    }
}

try {
    registerProcessor('CableLiftProcessor', CableLiftProcessor);
} catch (e) {
    // Already registered
}
