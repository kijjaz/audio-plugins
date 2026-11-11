#!/usr/bin/env python3
"""
generate_game_speech.py

Generates 36 authentic vintage 8-bit / arcade game speech sound effects rendered
at 96,000 Hz, 24-bit PCM using cycle-accurate 1-Bit DPCM Delta Modulation
(modeled after vintage 8-bit APU / arcade sound chips like TMS5220 and 2A03).

Outputs:
  - Kindnes_SFZ_Pack/samples/speech/ (36 chromatic speech samples)
  - Kindnes_SFZ_Pack/8Bit_Game_Speech.sfz (SFZ mapping across 3 octaves C3-B5)
  - Updates Kindnes_SFZ_Pack/samples/drums/ with updated speech voices
"""

import os
import math
import wave
import struct
import subprocess

SAMPLE_RATE = 96000
DPCM_CLOCK = 33140  # Authentic 2A03 Rate Index $F (33.14 kHz)

SPEECH_ITEMS = [
    # (Filename, Voice, Rate, Text, Category)
    ("01_Game_Start.wav",           "Zarvox",   175, "Game start!",                 "Start"),
    ("02_Ready_Go.wav",             "Fred",     180, "Ready... Go!",                "Start"),
    ("03_Ready_Fight.wav",          "Zarvox",   175, "Ready... Fight!",             "Combat"),
    ("04_Round_One.wav",            "Fred",     170, "Round One!",                  "Combat"),
    ("05_Round_Two.wav",            "Fred",     170, "Round Two!",                  "Combat"),
    ("06_Final_Round.wav",          "Trinoids", 160, "Final Round!",                "Combat"),
    ("07_Fight.wav",                "Zarvox",   190, "Fight!",                      "Combat"),
    ("08_You_Win.wav",              "Fred",     175, "You win!",                    "Victory"),
    ("09_You_Lose.wav",             "Trinoids", 155, "You lose!",                   "Defeat"),
    ("10_Victory.wav",              "Fred",     165, "Victory!",                    "Victory"),
    ("11_Flawless_Victory.wav",     "Zarvox",   175, "Flawless victory!",           "Victory"),
    ("12_Perfect.wav",              "Fred",     180, "Perfect!",                    "Victory"),
    ("13_Stage_Clear.wav",          "Zarvox",   180, "Stage clear!",                "Stage"),
    ("14_Mission_Accomplished.wav", "Fred",     170, "Mission accomplished!",       "Stage"),
    ("15_Mission_Failed.wav",       "Trinoids", 150, "Mission failed!",             "Defeat"),
    ("16_Game_Over.wav",            "Fred",     155, "Game over!",                  "Defeat"),
    ("17_Continue.wav",             "Zarvox",   170, "Continue?",                   "Arcade"),
    ("18_Insert_Coin.wav",          "Ralph",    180, "Insert coin!",                "Arcade"),
    ("19_Player_One_Ready.wav",     "Fred",     175, "Player One, ready!",          "Start"),
    ("20_Select_Your_Hero.wav",     "Zarvox",   175, "Select your hero!",           "Arcade"),
    ("21_Level_Up.wav",             "Zarvox",   180, "Level up!",                   "Power"),
    ("22_Power_Up.wav",             "Zarvox",   180, "Power up!",                   "Power"),
    ("23_Extra_Life.wav",           "Fred",     175, "Extra life!",                 "Power"),
    ("24_Speed_Up.wav",             "Zarvox",   185, "Speed up!",                   "Power"),
    ("25_Shield_Activated.wav",     "Ralph",    175, "Shield activated!",           "Power"),
    ("26_Warning.wav",              "Trinoids", 150, "Warning! Warning!",           "Alert"),
    ("27_Danger.wav",               "Trinoids", 155, "Danger!",                     "Alert"),
    ("28_Intruder_Alert.wav",       "Fred",     175, "Intruder alert! Destroy!",    "Alert"),
    ("29_Boss_Approaching.wav",     "Trinoids", 145, "Boss approaching!",           "Alert"),
    ("30_Emergency_Evacuate.wav",   "Trinoids", 160, "Emergency! Evacuate!",        "Alert"),
    ("31_Time_Extended.wav",        "Fred",     175, "Time extended!",              "Arcade"),
    ("32_Hurry_Up.wav",             "Zarvox",   190, "Hurry up!",                   "Arcade"),
    ("33_Bonus_Stage.wav",          "Fred",     170, "Bonus stage!",                "Stage"),
    ("34_Hyper_Drive.wav",          "Ralph",    175, "Hyper drive engaged!",        "Power"),
    ("35_Kindnes_Online.wav",       "Zarvox",   170, "Kindnes online!",             "System"),
    ("36_Good_Luck.wav",            "Fred",     165, "Good luck, Commander!",       "Start"),
]

def render_dpcm_speech(voice, rate, text, out_filepath):
    tmp_aiff = "/tmp/speech_gen.aiff"
    tmp_wav = "/tmp/speech_gen.wav"
    
    # 1. Synthesize text with designated macOS voice & cadence
    subprocess.run(["say", "-v", voice, "-r", str(rate), text, "-o", tmp_aiff], check=True)
    # 2. Convert to 96kHz 24-bit mono WAV
    subprocess.run(["afconvert", "-f", "WAVE", "-d", "LEI24@96000", "-c", "1", tmp_aiff, tmp_wav], check=True)
    
    # 3. Read input audio
    w = wave.open(tmp_wav, 'rb')
    n = w.getnframes()
    frames = w.readframes(n)
    w.close()
    
    raw = []
    for i in range(n):
        val = int.from_bytes(frames[i*3:i*3+3], byteorder='little', signed=True)
        raw.append(val / 8388607.0)
        
    # 4. Cycle-accurate 1-bit DPCM Delta Modulation at DPCM_CLOCK (33.14 kHz)
    # Hardware DAC: 7-bit signed accumulator (0..127) centered at 64
    dac = 64
    dpcm_phase = 0.0
    dpcm_step = DPCM_CLOCK / SAMPLE_RATE
    out_samples = []
    
    # Pre-emphasis filter to boost speech clarity prior to 1-bit delta quantization
    pre_z = 0.0
    
    for x in raw:
        # Pre-emphasis
        x_filt = x - 0.72 * pre_z
        pre_z = x
        
        target_dac = int(64 + x_filt * 62.0)
        target_dac = max(0, min(127, target_dac))
        
        dpcm_phase += dpcm_step
        if dpcm_phase >= 1.0:
            dpcm_phase -= 1.0
            # 1-bit delta step (2A03 hardware step is +/- 2)
            if target_dac > dac:
                dac = min(127, dac + 2)
            elif target_dac < dac:
                dac = max(0, dac - 2)
                
        # Non-linear DAC ladder transfer function
        norm_dac = (dac - 64) / 64.0
        # Non-linear MOS curve
        v_out = norm_dac / (1.0 + 0.18 * abs(norm_dac))
        out_samples.append(v_out)
        
    # Trim initial leading silence & trailing silence with 5ms fade-in/out
    start_idx = 0
    while start_idx < len(out_samples) and abs(out_samples[start_idx]) < 0.02:
        start_idx += 1
    start_idx = max(0, start_idx - int(SAMPLE_RATE * 0.015))  # Keep 15ms lead-in
    
    end_idx = len(out_samples) - 1
    while end_idx > start_idx and abs(out_samples[end_idx]) < 0.02:
        end_idx -= 1
    end_idx = min(len(out_samples), end_idx + int(SAMPLE_RATE * 0.05)) # Keep 50ms decay
    
    trimmed = out_samples[start_idx:end_idx]
    if len(trimmed) < 100:
        trimmed = out_samples
        
    # 5ms fade in / 10ms fade out to avoid clicks
    fade_in_len = min(len(trimmed), int(SAMPLE_RATE * 0.005))
    fade_out_len = min(len(trimmed), int(SAMPLE_RATE * 0.012))
    for i in range(fade_in_len):
        trimmed[i] *= (i / float(fade_in_len))
    for i in range(fade_out_len):
        trimmed[len(trimmed) - 1 - i] *= (i / float(fade_out_len))
        
    # Normalize peak cleanly to -0.7 dBFS
    max_peak = max(abs(s) for s in trimmed) if trimmed else 1.0
    scale = (0.92 / max(1e-5, max_peak))
    final_samples = [s * scale for s in trimmed]
    
    # Write 24-bit 96k WAV
    os.makedirs(os.path.dirname(out_filepath), exist_ok=True)
    out_w = wave.open(out_filepath, 'wb')
    out_w.setnchannels(1)
    out_w.setsampwidth(3)
    out_w.setframerate(SAMPLE_RATE)
    
    out_bytes = bytearray()
    for s in final_samples:
        clamped = max(-1.0, min(1.0, s))
        ival = int(clamped * 8388607.0)
        out_bytes.extend(ival.to_bytes(3, byteorder='little', signed=True))
        
    out_w.writeframes(out_bytes)
    out_w.close()
    
    dur = len(final_samples) / SAMPLE_RATE
    print(f"  [OK] {os.path.basename(out_filepath):<30} | {voice:<8} | {dur:.2f}s | \"{text}\"")

def build_sfz_file(sfz_path, speech_dir):
    """
    Creates an SFZ mapping for all 36 speech samples mapped chromatically
    from MIDI Note 48 (C3) through Note 83 (B5).
    """
    lines = [
        "// ====================================================================",
        "// Kindnes 8-Bit Vintage Arcade Game Speech Library (96kHz, 24-bit)",
        "// Cycle-accurate 1-Bit DPCM Delta Modulation & Hardware Speech Silicon",
        "// Key Range: C3 (Note 48) - B5 (Note 83) across 36 chromatic pads",
        "// ====================================================================",
        "",
        "<control>",
        "default_path=samples/speech/",
        "",
        "<global>",
        "loop_mode=one_shot",
        "ampeg_attack=0.001",
        "ampeg_decay=0.0",
        "ampeg_sustain=100",
        "ampeg_release=0.1",
        "",
    ]
    
    base_midi = 48  # C3
    for i, (fn, voice, rate, text, cat) in enumerate(SPEECH_ITEMS):
        midi_note = base_midi + i
        lines.append(f"// Note {midi_note}: {text} ({cat})")
        lines.append("<region>")
        lines.append(f"sample={fn}")
        lines.append(f"key={midi_note}")
        lines.append(f"pitch_keycenter={midi_note}")
        lines.append("")
        
    with open(sfz_path, "w") as f:
        f.write("\n".join(lines) + "\n")
    print(f"\n[OK] Generated SFZ Instrument: {sfz_path}")

def main():
    repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    speech_dir = os.path.join(repo_root, "Kindnes_SFZ_Pack", "samples", "speech")
    drums_dir = os.path.join(repo_root, "Kindnes_SFZ_Pack", "samples", "drums")
    sfz_path = os.path.join(repo_root, "Kindnes_SFZ_Pack", "8Bit_Game_Speech.sfz")
    
    print(f">>> Generating {len(SPEECH_ITEMS)} Retro Game Speech Samples (96 kHz, 24-bit DPCM)...")
    for fn, voice, rate, text, cat in SPEECH_ITEMS:
        out_fp = os.path.join(speech_dir, fn)
        render_dpcm_speech(voice, rate, text, out_fp)
        
    # Also update/refresh the drums folder speech samples:
    # 60: KINDNES
    render_dpcm_speech("Zarvox", 170, "Kindnes", os.path.join(drums_dir, "60_Speech_1Bit_KINDNES.wav"))
    render_dpcm_speech("Zarvox", 170, "Kindnes", os.path.join(drums_dir, "25_Bank4_Speech_KINDNES.wav"))
    # 61: POWER UP
    render_dpcm_speech("Zarvox", 180, "Power up!", os.path.join(drums_dir, "61_Speech_1Bit_POWER_UP.wav"))
    render_dpcm_speech("Zarvox", 180, "Power up!", os.path.join(drums_dir, "26_Bank4_Speech_POWER_UP.wav"))
    
    # Generate dedicated SFZ instrument
    build_sfz_file(sfz_path, speech_dir)
    print("\n>>> ALL 36 GAME SPEECH SAMPLES SUCCESSFULLY GENERATED! <<<")

if __name__ == "__main__":
    main()

