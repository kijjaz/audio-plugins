import wave
import struct
import math
import os
import subprocess

kSampleRate = 96000
kTwoPi = 6.28318530718

def write_wav_24(path, samples):
    if not samples:
        return
    
    # 1. Remove DC offset
    mean = sum(samples) / len(samples)
    samples = [s - mean for s in samples]

    # 2. Smooth cosine attack fade-in (1 ms / 96 samples at 96 kHz) to prevent start click
    att_len = min(len(samples) // 4, int(0.001 * kSampleRate))
    for i in range(att_len):
        gain = 0.5 * (1.0 - math.cos(i / att_len * math.pi))
        samples[i] *= gain

    # 3. Smooth cosine release fade-out (10 ms / 960 samples at 96 kHz)
    rel_len = min(len(samples) // 4, int(0.010 * kSampleRate))
    rel_start = len(samples) - rel_len
    for i in range(rel_len):
        gain = 0.5 * (1.0 + math.cos(i / rel_len * math.pi))
        samples[rel_start + i] *= gain
    samples[-1] = 0.0

    wo = wave.open(path, 'wb')
    wo.setnchannels(1)
    wo.setsampwidth(3)
    wo.setframerate(kSampleRate)
    
    # normalize to 0.92
    max_peak = max(abs(s) for s in samples) if samples else 1.0
    scale = 0.92 / max_peak if max_peak > 1e-5 else 1.0
    
    bytes_out = bytearray()
    for s in samples:
        val = int(s * scale * 8388607.0)
        val = max(-8388608, min(8388607, val))
        bytes_out.extend(val.to_bytes(3, byteorder='little', signed=True))
    wo.writeframes(bytes_out)
    wo.close()
    print(f"Generated {path}: {len(samples)} frames ({len(samples)/kSampleRate:.3f}s)")

# -----------------------------------------------------------------------------
# 1. Authentic DJ Turntable Vinyl Scratch (Forward push + Reverse rip + Cartridge tone)
# -----------------------------------------------------------------------------
def generate_vinyl_scratch():
    num_samples = int(0.38 * kSampleRate)
    samples = [0.0] * num_samples
    seed = 0x8273645
    
    def next_noise():
        nonlocal seed
        seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF
        val = seed if seed < 0x80000000 else seed - 0x100000000
        return val / 2147483648.0

    scrub_phase = 0.0
    bp1_y1 = 0.0
    bp1_y2 = 0.0
    vinyl_hiss = 0.0

    for i in range(num_samples):
        t = i / kSampleRate
        norm_t = t / 0.38

        # 2-stroke baby scratch velocity curve v(t):
        # 0.00..0.45: Forward push (accelerate up to 1.8x, then slow down)
        # 0.45..0.50: Turnaround hand pause
        # 0.50..0.95: Backward rip (sharp zip back up to 2.5x speed)
        # 0.95..1.00: Fade out / stop
        if norm_t < 0.45:
            p = norm_t / 0.45
            hand_speed = math.sin(p * math.pi) * 1.85
        elif norm_t < 0.50:
            hand_speed = 0.06
        elif norm_t < 0.95:
            p = (norm_t - 0.50) / 0.45
            hand_speed = math.sin(p * math.pi) * 2.45
        else:
            hand_speed = 0.05

        # Scratched sound on the vinyl record (famous 80s "Ahhh" / "Fresh" vocal-horn sample)
        base_tone = 360.0 * max(0.1, hand_speed)
        scrub_phase += base_tone / kSampleRate
        if scrub_phase >= 1.0:
            scrub_phase -= math.floor(scrub_phase)

        # Multi-harmonic rich brass/formant source on vinyl
        src = (math.sin(scrub_phase * kTwoPi) 
             + 0.65 * math.sin(scrub_phase * 2.0 * kTwoPi) 
             + 0.45 * math.sin(scrub_phase * 3.0 * kTwoPi)
             + 0.30 * math.sin(scrub_phase * 4.0 * kTwoPi))

        # Turntable needle surface hiss & friction modulated by scrub velocity
        hiss = next_noise() * (0.35 + 0.65 * hand_speed)
        vinyl_hiss += 0.35 * (hiss - vinyl_hiss)

        # Resonant cartridge filter tracking needle friction speed (800Hz to 2.6kHz)
        fc = 750.0 + 1750.0 * min(1.4, hand_speed)
        w0 = kTwoPi * fc / kSampleRate
        q = 3.6
        alpha = math.sin(w0) / (2.0 * q)
        b0 = alpha
        a0 = 1.0 + alpha
        a1 = -2.0 * math.cos(w0)
        a2 = 1.0 - alpha

        x = src * 0.75 + vinyl_hiss * 0.4
        y = (b0 / a0) * x - (a1 / a0) * bp1_y1 - (a2 / a0) * bp1_y2
        bp1_y2 = bp1_y1
        bp1_y1 = y

        # Crossfader envelope gating
        xfader = 1.0
        if norm_t < 0.03:
            xfader = norm_t / 0.03
        elif norm_t > 0.94:
            xfader = (1.0 - norm_t) / 0.06

        # Cartridge preamp saturation
        out_s = math.tanh(y * 2.2 * max(0.2, hand_speed)) * xfader
        samples[i] = out_s

    return samples

# -----------------------------------------------------------------------------
# 2. Classic 80s Fairlight Orchestra Hits (Major & Minor)
# -----------------------------------------------------------------------------
def generate_orchestra_hit(minor=False):
    num_samples = int(0.60 * kSampleRate)
    samples = [0.0] * num_samples
    seed = 0x19830501
    
    def next_noise():
        nonlocal seed
        seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF
        val = seed if seed < 0x80000000 else seed - 0x100000000
        return val / 2147483648.0

    # Multi-octave tutti brass + string section over solid C2 root (65.4 Hz):
    # C2 (65.4), G2 (98.0), C3 (130.8), E3/Eb3 (164.8/155.6), G3 (196.0), C4 (261.6), E4/Eb4 (329.6/311.1), G4 (392.0), C5 (523.2)
    third3 = 155.6 if minor else 164.8
    third4 = 311.1 if minor else 329.6

    oscs = [
        {'freq': 65.41,  'detune':  0.0, 'weight': 1.1, 'phase': 0.0},
        {'freq': 65.41,  'detune':  0.4, 'weight': 0.8, 'phase': 0.2},
        {'freq': 98.00,  'detune': -0.3, 'weight': 0.85, 'phase': 0.5},
        {'freq': 130.81, 'detune':  0.0, 'weight': 0.9, 'phase': 0.1},
        {'freq': 130.81, 'detune':  0.5, 'weight': 0.7, 'phase': 0.6},
        {'freq': third3, 'detune': -0.4, 'weight': 0.9, 'phase': 0.3},
        {'freq': 196.00, 'detune':  0.3, 'weight': 0.75, 'phase': 0.4},
        {'freq': 261.63, 'detune':  0.0, 'weight': 0.7, 'phase': 0.2},
        {'freq': third4, 'detune':  0.4, 'weight': 0.65, 'phase': 0.7},
        {'freq': 392.00, 'detune': -0.3, 'weight': 0.55, 'phase': 0.3},
        {'freq': 523.25, 'detune':  0.0, 'weight': 0.40, 'phase': 0.1},
    ]

    timp_phase = 0.0
    for i in range(num_samples):
        t = i / kSampleRate

        # 1. Fortissimo Brass Attack Crunch (< 35 ms)
        brass_attack = 0.0
        if t < 0.035:
            nz = next_noise()
            brass_attack = nz * (1.0 - t / 0.035) * 0.95

        # 2. Orchestral Bass Drum / Timpani Pitch Dive (170 Hz down to 48 Hz)
        timp_freq = 48.0 + 122.0 * math.exp(-t * 36.0)
        timp_phase += timp_freq / kSampleRate
        if timp_phase >= 1.0:
            timp_phase -= math.floor(timp_phase)
        timp_body = math.sin(timp_phase * kTwoPi) * math.exp(-t * 8.5) * 1.3

        # 3. Tutti Staccato Ensemble (multi-saw + pulse)
        ensemble = 0.0
        for o in oscs:
            f = o['freq'] + o['detune']
            o['phase'] += f / kSampleRate
            if o['phase'] >= 1.0:
                o['phase'] -= math.floor(o['phase'])
            # saw + narrow pulse
            saw = 2.0 * o['phase'] - 1.0
            pulse = 1.0 if o['phase'] < 0.25 else -0.33
            ensemble += (saw * 0.65 + pulse * 0.35) * o['weight']

        # 4. Characteristic envelope: Instant punch, 140ms dense body, dramatic hall decay
        if t < 0.004:
            env = t / 0.004
        elif t < 0.13:
            env = 1.0 - (t - 0.004) * 1.4
        else:
            env = 0.82 * math.exp(-(t - 0.13) * 6.0)

        total = (ensemble * 0.26 + timp_body * 0.55 + brass_attack * 0.45) * env
        # Analog transformer saturation
        samples[i] = math.tanh(total * 2.3)

    return samples

# -----------------------------------------------------------------------------
# 3. Chiptune Maraca / Shaker (Crisp high-frequency resonant rattle)
# -----------------------------------------------------------------------------
def generate_chiptune_maraca():
    num_samples = int(0.22 * kSampleRate)
    samples = [0.0] * num_samples
    seed = 0x54321
    
    def next_noise():
        nonlocal seed
        seed = (seed * 1664525 + 1013904223) & 0xFFFFFFFF
        val = seed if seed < 0x80000000 else seed - 0x100000000
        return val / 2147483648.0

    # 4-stage resonant bandpass filter centered around 6.5 kHz with 12 kHz sparkle
    bp_y1 = 0.0
    bp_y2 = 0.0
    fc = 6200.0
    w0 = kTwoPi * fc / kSampleRate
    q = 2.2
    alpha = math.sin(w0) / (2.0 * q)
    b0 = alpha
    a0 = 1.0 + alpha
    a1 = -2.0 * math.cos(w0)
    a2 = 1.0 - alpha

    for i in range(num_samples):
        t = i / kSampleRate
        # Characteristic swell & fast snap (80ms forward flick)
        # Beads accelerate into shell then bounce
        if t < 0.025:
            swell = t / 0.025
        elif t < 0.08:
            swell = 1.0 - (t - 0.025) * 6.0
        else:
            swell = 0.67 * math.exp(-(t - 0.08) * 22.0)

        nz = next_noise()
        # filter
        y = (b0 / a0) * nz - (a1 / a0) * bp_y1 - (a2 / a0) * bp_y2
        bp_y2 = bp_y1
        bp_y1 = y

        # Symmetrical 4-bit staircase DAC quantizing for authentic 2A03 grit without DC bias
        grit = round(y * swell * 0.95 * 7.5) / 7.5
        samples[i] = grit

    return samples

# -----------------------------------------------------------------------------
# 4. Authentic 1-Bit DPCM Robot Speech ("KINDNES" & "POWER UP")
# -----------------------------------------------------------------------------
def render_mac_speech(text, out_path):
    tmp_aiff = "/tmp/speech_tmp.aiff"
    tmp_wav = "/tmp/speech_tmp.wav"
    # Use Zarvox for quintessential 80s arcade robot voice
    subprocess.run(["say", "-v", "Zarvox", text, "-o", tmp_aiff], check=True)
    subprocess.run(["afconvert", "-f", "WAVE", "-d", "LEI24@96000", "-c", "1", tmp_aiff, tmp_wav], check=True)

    w = wave.open(tmp_wav, 'rb')
    n = w.getnframes()
    frames = w.readframes(n)
    w.close()
    
    # Unpack 24-bit samples
    raw_s = []
    for i in range(n):
        chunk = frames[i*3:i*3+3]
        val = int.from_bytes(chunk, byteorder='little', signed=True)
        raw_s.append(val / 8388607.0)

    # Encode with 1-bit DPCM delta modulation at ~32 kHz rate divider
    dac = 64
    dpcm_out = []
    clock_div = 3 # 96k / 3 = 32 kHz
    cur_step = 2

    for i, s in enumerate(raw_s):
        if (i % clock_div) == 0:
            target = max(0, min(126, int((s + 1.0) * 63.5)))
            if target > dac:
                dac = min(126, dac + cur_step)
            else:
                dac = max(0, dac - cur_step)
        dpcm_out.append((dac / 63.5) - 1.0)

    # Trim leading silence and apply 5ms fade
    # find first frame > 0.05
    start_idx = 0
    for idx, v in enumerate(dpcm_out):
        if abs(v) > 0.05:
            start_idx = max(0, idx - 480) # 5ms head
            break
    
    end_idx = len(dpcm_out) - 1
    for idx in range(len(dpcm_out)-1, -1, -1):
        if abs(dpcm_out[idx]) > 0.05:
            end_idx = min(len(dpcm_out), idx + 2400) # 25ms tail
            break
            
    trimmed = dpcm_out[start_idx:end_idx]
    
    write_wav_24(out_path, trimmed)

if __name__ == "__main__":
    drums_dir = "Kindnes_SFZ_Pack/samples/drums"
    os.makedirs(drums_dir, exist_ok=True)

    print(">>> 1. Generating Authentic Vinyl Scratch...")
    scratch = generate_vinyl_scratch()
    write_wav_24(f"{drums_dir}/58_1Bit_Vinyl_Scratch_Chirp.wav", scratch)

    print(">>> 2. Generating Classic Fairlight Orchestra Hits (Major & Minor)...")
    orch_maj = generate_orchestra_hit(minor=False)
    write_wav_24(f"{drums_dir}/56_Orchestra_Hit_Major_Blast.wav", orch_maj)

    orch_min = generate_orchestra_hit(minor=True)
    write_wav_24(f"{drums_dir}/57_Orchestra_Hit_Minor_Gothic.wav", orch_min)

    print(">>> 3. Generating Chiptune Maraca / Shaker...")
    maraca = generate_chiptune_maraca()
    write_wav_24(f"{drums_dir}/59_Chiptune_Maraca_Shaker.wav", maraca)

    print(">>> 4. Rendering Authentic 1-Bit DPCM Speech ('KINDNES' & 'POWER UP')...")
    render_mac_speech("kindness", f"{drums_dir}/60_Speech_1Bit_KINDNES.wav")
    render_mac_speech("power up", f"{drums_dir}/61_Speech_1Bit_POWER_UP.wav")

    print(">>> ALL SOUNDS GENERATED SUCCESSFULLY! <<<")
