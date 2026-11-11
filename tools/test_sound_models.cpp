#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <fstream>
#include <cstdint>

constexpr float kTwoPi = 6.28318530718f;

struct ScratchModel
{
    // Generates a 2-stroke "baby scratch" (forward rub + backward rip)
    static std::vector<float> generateBabyScratch(int numSamples, float fs)
    {
        std::vector<float> out(numSamples, 0.0f);
        uint32_t seed = 0x7654321;
        auto nextNoise = [&]() {
            seed = seed * 1664525u + 1013904223u;
            return static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
        };

        // Formants / bandpass state
        float bp1_y1 = 0.0f, bp1_y2 = 0.0f;
        float bp2_y1 = 0.0f, bp2_y2 = 0.0f;
        float vinylHissLP = 0.0f;

        // Playhead scrub position in source buffer
        float scrubPos = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float t = static_cast<float>(i) / fs;
            float totalDur = static_cast<float>(numSamples) / fs;
            float normT = t / totalDur; // 0..1

            // Hand scrub velocity curve v(t):
            // 0..0.45: Forward push (accelerate then decelerate) -> positive velocity
            // 0.45..0.52: Hand turnaround pause / reversal friction
            // 0.52..1.0: Backward pull (sharp zip back) -> high negative velocity
            float handSpeed = 0.0f;
            if (normT < 0.45f)
            {
                float p = normT / 0.45f;
                // bell curve push
                handSpeed = std::sin(p * 3.14159265f) * 1.8f;
            }
            else if (normT < 0.52f)
            {
                handSpeed = 0.08f; // momentary slow turnaround friction
            }
            else
            {
                float p = (normT - 0.52f) / 0.48f;
                // sharp fast reverse zip
                handSpeed = std::sin(p * 3.14159265f) * 2.6f;
            }

            // Scratched "Ahhh" horn/voice timbre pitch
            float baseToneFreq = 340.0f * std::max(0.1f, handSpeed);
            scrubPos += baseToneFreq / fs;
            if (scrubPos >= 1.0f) scrubPos -= std::floor(scrubPos);

            // Rich source sound on the record: buzzy brass/voice waveform with harmonics
            float src = std::sin(scrubPos * kTwoPi) 
                      + 0.6f * std::sin(scrubPos * 2.0f * kTwoPi) 
                      + 0.4f * std::sin(scrubPos * 3.0f * kTwoPi)
                      + 0.3f * std::sin(scrubPos * 4.0f * kTwoPi);

            // Vinyl surface noise & needle drag friction (groove friction increases with speed)
            float grooveFriction = nextNoise() * (0.35f + 0.65f * handSpeed);
            vinylHissLP += 0.4f * (grooveFriction - vinylHissLP);

            // Resonant filter tracking the needle cartridge resonance (900 Hz to 2.4 kHz)
            float fCartridge = 750.0f + 1600.0f * std::min(1.5f, handSpeed);
            float w0 = kTwoPi * fCartridge / fs;
            float q = 3.5f;
            float alpha = std::sin(w0) / (2.0f * q);
            float b0 = alpha;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * std::cos(w0);
            float a2 = 1.0f - alpha;

            float x = src * 0.7f + vinylHissLP * 0.45f;
            float y = (b0 / a0) * x - (a1 / a0) * bp1_y1 - (a2 / a0) * bp1_y2;
            bp1_y2 = bp1_y1;
            bp1_y1 = y;

            // Amplitude envelope with authentic crossfader gating click at start & end
            float xfader = 1.0f;
            if (normT < 0.04f) xfader = normT / 0.04f;
            else if (normT > 0.94f) xfader = (1.0f - normT) / 0.06f;

            // Turntable needle saturation / pre-amp clipper
            float outSample = std::tanh(y * 2.4f * std::max(0.2f, handSpeed)) * xfader;
            out[i] = outSample;
        }

        return out;
    }
};

struct OrchestraHitModel
{
    // The quintessential 1980s / Fairlight CMI / Stravinsky Firebird / Roland D-50 Orchestra Hit:
    // 1. Tutti fortissimo multi-octave brass & strings chord (Root, 5th, Octave, Maj/Min 3rd, 11th)
    // 2. Heavy explosive bass drum / orchestral bass impact
    // 3. Staccato brass bite with sharp attack transient
    // 4. Characteristic hall reverberation decay
    static std::vector<float> generateFairlightOrchHit(int numSamples, float fs, bool minor)
    {
        std::vector<float> out(numSamples, 0.0f);
        uint32_t seed = 0x19820412;
        auto nextNoise = [&]() {
            seed = seed * 1664525u + 1013904223u;
            return static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
        };

        // Chord frequencies based on low C2 (65.4 Hz) root for massive authoritative body:
        // C1 (32.7Hz sub), C2 (65.4Hz), G2 (98Hz), C3 (130.8Hz), E3/Eb3 (164.8/155.6Hz), G3 (196Hz), C4 (261.6Hz), E4/Eb4 (329.6/311.1Hz), G4 (392Hz)
        struct NoteOsc {
            float freq;
            float detune;
            float weight;
            float phase;
        };

        float third3 = minor ? 155.6f : 164.8f;
        float third4 = minor ? 311.1f : 329.6f;

        std::vector<NoteOsc> oscs = {
            { 65.4f,   0.0f,   1.0f,  0.0f }, // C2 Root
            { 65.4f,   0.4f,   0.8f,  0.2f }, // C2 detune
            { 98.0f,  -0.3f,   0.85f, 0.4f }, // G2 5th
            { 130.8f,  0.0f,   0.9f,  0.1f }, // C3 Octave
            { 130.8f,  0.5f,   0.7f,  0.6f }, // C3 detune
            { third3, -0.4f,   0.85f, 0.3f }, // 3rd (E3 or Eb3)
            { 196.0f,  0.3f,   0.75f, 0.5f }, // G3 5th
            { 261.6f,  0.0f,   0.65f, 0.2f }, // C4
            { third4,  0.3f,   0.60f, 0.7f }, // 3rd
            { 392.0f, -0.2f,   0.50f, 0.4f }, // G4
            { 523.2f,  0.0f,   0.35f, 0.1f }  // C5
        };

        float timpaniPhase = 0.0f;
        float drumLp = 0.0f;

        for (int i = 0; i < numSamples; ++i)
        {
            float t = static_cast<float>(i) / fs;
            float totalDur = static_cast<float>(numSamples) / fs;

            // 1. Initial Fortissimo Brass Slap Transient (< 35 ms)
            float brassBite = 0.0f;
            if (t < 0.035f)
            {
                float nz = nextNoise();
                brassBite = nz * (1.0f - t / 0.035f) * 0.9f;
            }

            // 2. Orchestral Bass Drum / Timpani impact transient (pitch dive 160 -> 48 Hz)
            float timpFreq = 48.0f + 112.0f * std::exp(-t * 35.0f);
            timpaniPhase += timpFreq / fs;
            if (timpaniPhase >= 1.0f) timpaniPhase -= std::floor(timpaniPhase);
            float timpani = std::sin(timpaniPhase * kTwoPi) * std::exp(-t * 9.0f) * 1.2f;

            // 3. Multi-voice ensemble saw & pulse staccato chords
            float chord = 0.0f;
            for (auto& o : oscs)
            {
                float f = o.freq + o.detune;
                o.phase += f / fs;
                if (o.phase >= 1.0f) o.phase -= std::floor(o.phase);
                
                // Sawtooth + narrow pulse brass harmonic
                float saw = 2.0f * o.phase - 1.0f;
                float pulse = (o.phase < 0.25f) ? 1.0f : -0.33f;
                chord += (saw * 0.6f + pulse * 0.4f) * o.weight;
            }

            // Orchestral envelope: Immediate full bite, punchy 120ms hold, then reverb tail decay
            float env = 0.0f;
            if (t < 0.005f) env = t / 0.005f; // 5ms attack
            else if (t < 0.14f) env = 1.0f - (t - 0.005f) * 1.5f; // staccato body
            else env = 0.7975f * std::exp(-(t - 0.14f) * 6.5f);  // dramatic hall tail

            float totalRaw = (chord * 0.25f + timpani * 0.6f + brassBite * 0.45f) * env;

            // Analog console transformer drive / tape saturation
            float sat = std::tanh(totalRaw * 2.2f);
            out[i] = sat;
        }

        return out;
    }
};

int main()
{
    std::cout << "Testing ScratchModel and OrchestraHitModel algorithms..." << std::endl;
    auto scratch = ScratchModel::generateBabyScratch(static_cast<int>(0.35f * 96000), 96000.0f);
    auto hitMaj = OrchestraHitModel::generateFairlightOrchHit(static_cast<int>(0.55f * 96000), 96000.0f, false);
    auto hitMin = OrchestraHitModel::generateFairlightOrchHit(static_cast<int>(0.55f * 96000), 96000.0f, true);

    std::cout << "Scratch size: " << scratch.size() << ", HitMaj size: " << hitMaj.size() << std::endl;
    return 0;
}

