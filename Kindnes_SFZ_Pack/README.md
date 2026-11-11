# Kindnes 8-Bit Classic & Expansion 96kHz SFZ Sample Pack

A studio-grade, cycle-accurate chiptune sample library rendered at **96,000 Hz, 24-bit PCM**, capturing the classic **Ricoh 2A03 APU** and legendary **VRC6 & FDS 2C33** expansion audio architectures.

---

## ⚡ Quick Start: Compatible Samplers
This sound pack uses the standard open **SFZ format** and works out of the box in:
* **Plogue sforzando** (macOS, Windows — Recommended)
* **Decent Sampler** (macOS, Windows, iOS, Linux)
* **Logic Pro Sampler / Quick Sampler** (macOS)
* **Native Instruments Kontakt** (via SFZ import)
* **LinuxSampler / Carla** (Linux, macOS, Windows)

---

## 🎹 Melodic Waveform Instruments

### Standalone 1-Click Instruments (Full Keyboard)
Drag any of these files directly into sforzando to play the waveform across your full MIDI range:

| File | Architecture | Tone & Description |
| :--- | :--- | :--- |
| **`01_2A03_Pulse_12.5.sfz`** | 2A03 APU | **Pulse 12.5% (Duty 0)** — Hollow, reedy, cutting vintage lead |
| **`02_2A03_Pulse_25.0.sfz`** | 2A03 APU | **Pulse 25.0% (Duty 1)** — Bright, punchy chiptune melody |
| **`03_2A03_Pulse_50.0.sfz`** | 2A03 APU | **Pulse 50.0% (Duty 2)** — Classic round, hollow square wave |
| **`04_2A03_Pulse_75.0.sfz`** | 2A03 APU | **Pulse 75.0% (Duty 3)** — Inverted 25% phase duty cycle |
| **`05_2A03_Triangle_32Step.sfz`** | 2A03 APU | **32-Step Staircase Triangle** — 4-bit stepped DAC with pop attack |
| **`06_VRC6_Clean_Saw.sfz`** | VRC6 | **Clean 14-Step Sawtooth** — Iconic retro melodic saw |
| **`07_VRC6_Overdrive_Saw.sfz`** | VRC6 | **8-Bit Overdrive Wrap Saw** — Double-saw accumulator wrap bite |
| **`08_VRC6_Thin_Pulse.sfz`** | VRC6 | **1/16 Thin Pulse (6.25%)** — Ultra-narrow needle pulse lead |
| **`09_FDS_2C33_FM_Lead.sfz`** | FDS 2C33 | **Hardware FM Lead** — 64-step wavetable + 3-bit hardware FM |
| **`10_2A03_Noise_Metallic_Mode1.sfz`** | 2A03 APU | **Mode 1 Metallic Noise** — 93-step tonal buzz (16 hardware keys) |
| **`11_2A03_Noise_White_Mode0.sfz`** | 2A03 APU | **Mode 0 White Noise** — 15-bit Galois LFSR white noise loop |

---

### Master Keyswitched Instrument (`8Bit_Basic_Waveforms.sfz`)
Loads all waveforms into one single instance. Switch active sound engines on the fly using sub-octave keyswitches (`C-1` to `A#-1` / MIDI notes 12 to 22):

| Keyswitch Note | MIDI # | Architecture | Waveform / Sound Character |
| :--- | :---: | :--- | :--- |
| **C-1** | 12 | 2A03 APU | **Pulse 12.5% (Duty 0)** |
| **C#-1** | 13 | 2A03 APU | **Pulse 25.0% (Duty 1)** |
| **D-1** | 14 | 2A03 APU | **Pulse 50.0% (Duty 2 — Default)** |
| **D#-1** | 15 | 2A03 APU | **Pulse 75.0% (Duty 3)** |
| **E-1** | 16 | 2A03 APU | **32-Step Staircase Triangle** |
| **F-1** | 17 | 2A03 APU | **Mode 1 Metallic Noise** (16 rates) |
| **F#-1** | 18 | 2A03 APU | **Mode 0 White Noise** |
| **G-1** | 19 | **VRC6** | **Clean 14-Step Sawtooth** |
| **G#-1** | 20 | **VRC6** | **8-Bit Overdrive Wrap Saw** |
| **A-1** | 21 | **VRC6** | **1/16 Thin Pulse (6.25%)** |
| **A#-1** | 22 | **FDS 2C33** | **Hardware FM Lead** |

---

## 🗣️ Vintage Arcade Game Speech Instrument: `8Bit_Game_Speech.sfz`

A dedicated library of **36 authentic 8-bit arcade speech samples** rendered through cycle-accurate 1-bit DPCM delta modulation and non-linear DAC emulation. Mapped chromatically across 3 octaves from **Note 48 (C3) to Note 83 (B5)**:

| Note | Name | Text | Voice Model | Category |
| :---: | :--- | :--- | :--- | :--- |
| **48 (C3)** | `01_Game_Start.wav` | *"Game start!"* | Zarvox | Start |
| **49 (C#3)** | `02_Ready_Go.wav` | *"Ready... Go!"* | Fred | Start |
| **50 (D3)** | `03_Ready_Fight.wav` | *"Ready... Fight!"* | Zarvox | Combat |
| **51 (D#3)** | `04_Round_One.wav` | *"Round One!"* | Fred | Combat |
| **52 (E3)** | `05_Round_Two.wav` | *"Round Two!"* | Fred | Combat |
| **53 (F3)** | `06_Final_Round.wav` | *"Final Round!"* | Trinoids | Combat |
| **54 (F#3)** | `07_Fight.wav` | *"Fight!"* | Zarvox | Combat |
| **55 (G3)** | `08_You_Win.wav` | *"You win!"* | Fred | Victory |
| **56 (G#3)** | `09_You_Lose.wav` | *"You lose!"* | Trinoids | Defeat |
| **57 (A3)** | `10_Victory.wav` | *"Victory!"* | Fred | Victory |
| **58 (A#3)** | `11_Flawless_Victory.wav` | *"Flawless victory!"* | Zarvox | Victory |
| **59 (B3)** | `12_Perfect.wav` | *"Perfect!"* | Fred | Victory |
| **60 (C4)** | `13_Stage_Clear.wav` | *"Stage clear!"* | Zarvox | Stage |
| **61 (C#4)** | `14_Mission_Accomplished.wav` | *"Mission accomplished!"* | Fred | Stage |
| **62 (D4)** | `15_Mission_Failed.wav` | *"Mission failed!"* | Trinoids | Defeat |
| **63 (D#4)** | `16_Game_Over.wav` | *"Game over!"* | Fred | Defeat |
| **64 (E4)** | `17_Continue.wav` | *"Continue?"* | Zarvox | Arcade |
| **65 (F4)** | `18_Insert_Coin.wav` | *"Insert coin!"* | Ralph | Arcade |
| **66 (F#4)** | `19_Player_One_Ready.wav` | *"Player One, ready!"* | Fred | Start |
| **67 (G4)** | `20_Select_Your_Hero.wav` | *"Select your hero!"* | Zarvox | Arcade |
| **68 (G#4)** | `21_Level_Up.wav` | *"Level up!"* | Zarvox | Power |
| **69 (A4)** | `22_Power_Up.wav` | *"Power up!"* | Zarvox | Power |
| **70 (A#4)** | `23_Extra_Life.wav` | *"Extra life!"* | Fred | Power |
| **71 (B4)** | `24_Speed_Up.wav` | *"Speed up!"* | Zarvox | Power |
| **72 (C5)** | `25_Shield_Activated.wav` | *"Shield activated!"* | Ralph | Power |
| **73 (C#5)** | `26_Warning.wav` | *"Warning! Warning!"* | Trinoids | Alert |
| **74 (D5)** | `27_Danger.wav` | *"Danger!"* | Trinoids | Alert |
| **75 (D#5)** | `28_Intruder_Alert.wav` | *"Intruder alert! Destroy!"* | Fred | Alert |
| **76 (E5)** | `29_Boss_Approaching.wav` | *"Boss approaching!"* | Trinoids | Alert |
| **77 (F5)** | `30_Emergency_Evacuate.wav` | *"Emergency! Evacuate!"* | Trinoids | Alert |
| **78 (F#5)** | `31_Time_Extended.wav` | *"Time extended!"* | Fred | Arcade |
| **79 (G5)** | `32_Hurry_Up.wav` | *"Hurry up!"* | Zarvox | Arcade |
| **80 (G#5)** | `33_Bonus_Stage.wav` | *"Bonus stage!"* | Fred | Stage |
| **81 (A5)** | `34_Hyper_Drive.wav` | *"Hyper drive engaged!"* | Ralph | Power |
| **82 (A#5)** | `35_Kindnes_Online.wav` | *"Kindnes online!"* | Zarvox | System |
| **83 (B5)** | `36_Good_Luck.wav` | *"Good luck, Commander!"* | Fred | Start |

---

## 🥁 Master Drum Instrument: `8Bit_Drums.sfz` (61 Chromatic Voices, C1 to C6)

Mapped across 5 octaves (MIDI notes **24 to 84**, $C1\dots C6$). Features kicks with diverse pitch profiles, varied snares, hi-hats with choking, toms with creative pitch sweeps (drops, dives, FM space modulations, and comic rises), layered attack hits, lasers, and **explosions spanning short pops, medium arcade bombs, large sub-rumbles, and dramatic 2.6s & 3.6s boss cataclysms**:

### 1. Kicks & Sub Booms (Notes 24 – 36 / C1 – C2)
* **24 (C1)**: **Sub 808 2A03 Triangle Boom** (1.2s prolonged 32Hz sub body)
* **25 (C#1)**: **Dn-Tracker Deep Sub Kick** (Dual-stage dive, punchy attack)
* **26 (D1)**: **VRC6 Overdrive Heavy Kick** (8-bit saw wrap saturation)
* **27 (D#1)**: **FDS 2C33 FM Modulated Kick** (Chirp FM modulation)
* **28 (E1)**: **Fast Laser-Dive Electro Kick** (Linear sweep transient)
* **29 (F1)**: **Vintage Punch Kick (Tight)** (Classic arcade punch)
* **30 (F#1)**: **Vintage Punch Kick (Long)** (Extended low-end release)
* **31 (G1)**: **1-Bit Slope Overload Fuzz Kick** (DPCM slew distortion)
* **32 (G#1)**: **Lo-Fi 4kHz DPCM Crunch Kick** (Authentic rate-divider crunch)
* **33 (A1)**: **Double Pitch Drop Kick** (Dual-stage transient bite)
* **34 (A#1)**: **Acoustic Style 8-Bit Kick** (Mellow rounded tone)
* **35 (B1)**: **General MIDI Sub Bass Drum** (Standard GM mapping)
* **36 (C2)**: **Main Arcade Electric Punch Kick** (General MIDI primary kick)

### 2. Snares, Claps & Rimshots (Notes 37 – 44 / C#2 – G#2)
* **37 (C#2)**: **Chiptune Side Stick / Rimshot** (High resonant click)
* **38 (D2)**: **Mega Crisp Snare (Short Pop)** (Fast decay pop)
* **39 (D#2)**: **8-Bit Hand Clap** (Multi-flam 3-burst envelope)
* **40 (E2)**: **8-Bit Delta Snappy Snare** (Classic 2A03 snare body)
* **41 (F2)**: **Gated Boss Battle Snare** (Plateau gate cut)
* **42 (F#2)**: **Metallic 93-Step Snare** (Mode 1 metallic LFSR timbre)
* **43 (G2)**: **1-Bit Slope Overload Fuzz Snare** (Heavy non-linear saturation)
* **44 (G#2)**: **Glitch Stutter Frame Snare** (Frame-synced stutter)

### 3. Hi-Hats & Cymbals (Notes 45 – 50 / A2 – D3)
* **45 (A2)**: **Closed Hi-Hat (Tight 25ms)** *(Choke Group 1)*
* **46 (A#2)**: **Closed Hi-Hat (Crisp 55ms)** *(Choke Group 1)*
* **47 (B2)**: **Pedal Hi-Hat (Choked Tone)** *(Choke Group 1)*
* **48 (C3)**: **Open Hi-Hat (Medium 260ms)** *(Choked by notes 45, 46, 47)*
* **49 (C#3)**: **Open Hi-Hat (Sizzle Long 480ms)** *(Choked by notes 45, 46, 47)*
* **50 (D3)**: **Arcade Crash Cymbal (Long 850ms)** (Full decay burst)

### 4. Toms & Tuned Percussion with Diverse Pitch Modulation (Notes 51 – 60 / D#3 – C4)
* **51 (D#3)**: **Low Floor Tom (Deep Drop)** (Exponential downward dive)
* **52 (E3)**: **Low Tom (Fast Dive Punch)** (Steep initial drop)
* **53 (F3)**: **Mid Tom (Gothic Quest)** (Iconic arcade tom tone)
* **54 (F#3)**: **High Rack Tom (Quick Pitch Fall)** (Fast upper tom)
* **55 (G3)**: **FDS 2C33 FM Space Simmons Tom** (Wavetable FM modulated pitch)
* **56 (G#3)**: **VRC6 Sawtooth Synth Tom** (Aggressive 14-step saw sweep)
* **57 (A3)**: **Dramatic 1-Bit Timpani Hit** (Dramatic kettle drum)
* **58 (A#3)**: **Comic Spring Tom (Pitch Scoop Rise)** (Upward rising pitch scoop!)
* **59 (B3)**: **8-Bit Arcade Cowbell** (540 Hz dual pulse tone)
* **60 (C4)**: **FDS Metallic Ride Bell** (High bell overtone)

### 5. Explosions & Attack Sounds: Short, Medium, Long & Dramatic Boss (Notes 61 – 68 / C#4 – G#4)
* **61 (C#4)**: **Explosion: Micro Hit / Grenade Pop** (Short 120ms snap)
* **62 (D4)**: **Explosion: Tank Cannon Punch** (Medium 280ms punch)
* **63 (D#4)**: **Explosion: Arcade Bomb Burst** (Medium 450ms boom)
* **64 (E4)**: **Explosion: Large Sub Rumble** (Long 950ms rumble)
* **65 (F4)**: **Explosion: Boss Defeat Dramatic (2.6s Extended)** (Multi-stage rumbling decay for dramatic boss kills)
* **66 (F#4)**: **Explosion: Boss Super Cataclysm (3.6s Ultra Long)** (Epic screen-shaking finale explosion)
* **67 (G4)**: **Impact Attack: Physical Melee Punch** (Fist/kick impact)
* **68 (G#4)**: **Impact Attack: Ninja Sword Slash** (Sharp blade slice transient)

### 6. Sci-Fi Lasers, Zappers & Warp SFX (Notes 69 – 76 / A4 – E5)
* **69 (A4)**: **Laser: Fast Pistol Zapper (Short 80ms)** (High-frequency drop)
* **70 (A#4)**: **Laser: Arcade Blaster Pew (Medium 160ms)** (Classic space pew)
* **71 (B4)**: **Laser: Heavy Plasma Cannon (Long 320ms)** (Heavy energy blast)
* **72 (C5)**: **Sci-Fi: UFO Warp Siren** (Continuous FM chirp modulation)
* **73 (C#5)**: **Sci-Fi: Forcefield Shield Deflect** (High resonant shield ricochet)
* **74 (D5)**: **Reward: Coin 1-Up Chime (B5 to E6)** (Iconic ascending chime)
* **75 (D#5)**: **Movement: Jump Spring Boing** (Dramatic upward pitch scoop)
* **76 (E5)**: **Status: Power-Down Glitch Fall** (Descending chromatic stutter)

### 7. Vintage 1-Bit Slap Bass, Turntable Scratch, Maraca & Orchestra Hits (Notes 77 – 84 / F5 – C6)
* **77 (F5)**: **Vintage 1-Bit Slap Bass Pop** (Crisp funk pop)
* **78 (F#5)**: **Vintage 1-Bit Slap Bass Thumb** (Deep thumb sub)
* **79 (G5)**: **Orchestra Hit Major (Arcade Tutti Blast)** (Fairlight CMI tutti orchestra hit with bass drum punch)
* **80 (G#5)**: **Orchestra Hit Minor (Gothic Castle Stab)** (Gothic minor orchestra stab)
* **81 (A5)**: **1-Bit Turntable Vinyl Scratch Chirp** (Physical 2-stroke baby scratch with speed-scaled needle friction)
* **82 (A#5)**: **Chiptune Maraca Shaker** (Crisp, resonant bead flick rattle at 6.2 kHz)
* **83 (B5)**: **Speech: "KINDNES"** (1-bit robotic voice synthesis)
* **84 (C6)**: **Speech: "POWER UP"** (1-bit robotic voice synthesis)

---

## 🛠️ Technical Specifications
* **Sample Rate**: 96,000 Hz (96 kHz)
* **Bit Depth**: 24-bit PCM mono
* **Anti-Aliasing**: Bandlimited PolyBLEP synthesis (0 aliasing foldover at Nyquist)
* **Loop Encoding**: Both internal WAV `smpl` chunk and external SFZ `loop_start` / `loop_end` zero-crossing loop markers
* **Choke Logic**: Authentic single-channel noise emulation via SFZ `group=1 off_by=1`
