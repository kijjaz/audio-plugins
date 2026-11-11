#pragma once

#include <string>
#include <vector>

namespace KindnesAudio
{

struct Preset
{
    std::string name;
    std::string category;
    
    // Core parameters
    int playMode{ 1 };          // 0: Classic APU, 1: Poly, 2: Unison Stereo
    bool authenticDAC{ true };   // True: Non-linear resistive DAC
    bool analogFilter{ true };   // True: Motherboard HPF + LPF
    float filterTone{ 0.5f };    // 0: Bright, 0.5: Authentic 8-Bit APU, 1: Dark CRT

    // Resonant Filter
    int filterMode{ 0 };        // 0: LP, 1: HP, 2: BP, 3: Notch, 4: Bypass
    float cutoffHz{ 16000.0f };
    float resonance{ 0.707f };

    // ADSR Envelope
    bool useAdsr{ true };
    float attackMs{ 5.0f };
    float decayMs{ 150.0f };
    float sustainLevel{ 0.8f };
    float releaseMs{ 200.0f };

    // Modulation
    float lfoRateHz{ 5.0f };
    float vibratoCents{ 0.0f };
    float lfoPwmDepth{ 0.0f };
    float glideMs{ 0.0f };

    // Pulse Channels
    int pulse1Duty{ 2 };        // 0: 12.5%, 1: 25%, 2: 50%, 3: 75%, 4: PWM
    int pulse2Duty{ 1 };        // 25%
    float continuousPWM{ 0.5f };
    int pulse1Vol{ 15 };
    int pulse2Vol{ 12 };
    int triVol{ 15 };
    int noiseVol{ 0 };
    int noiseMode{ 0 };         // 0: White, 1: Metallic 93-step
    int noisePeriod{ 0 };

    // DPCM 1-Bit Sampler
    int dpcmSample{ 0 };        // 0..7 built-in, 8 user
    bool dpcmChromatic{ false };
    int dpcmDeltaStep{ 2 };     // 1: fuzz/slope overload, 2: 2A03, 4: punch
    int dpcmRateIndex{ 15 };    // 0..15 (4.18 kHz to 33.14 kHz)
    int dpcmVol{ 12 };
    bool dpcmDrumPads{ false };
    float dpcmVelToSlew{ 0.0f };
    bool throughDpcm{ false };

    // Arpeggiator Macro
    bool arpEnabled{ false };
    int arpRateMode{ 0 };       // 0: 60Hz NTSC, 1: 50Hz PAL, 2: 1/16, 3: 1/32, 4: 1/64
    int arpPattern{ 0 };        // 0: Major, 1: Minor, 2: Octaves, 3: Sus4, 4: Fifth, 5: Chug
    std::vector<int> arpSteps{ 0, 4, 7, 12 };

    // VRC6 Expansion
    bool vrc6Enabled{ false };
    int vrc6SawVol{ 0 };

    // FDS 2C33 Wavetable & FM Unit
    bool fdsEnabled{ false };
    int fdsWave{ 0 };           // 0..7 presets
    int fdsVol{ 15 };
    float fdsModSpeed{ 6.0f };
    float fdsModDepth{ 0.0f };
    int fdsModWave{ 0 };        // 0: Vibrato, 1: Metallic FM, 2: Laser
    bool fdsSteppedDac{ true };

    // Chiptune Embellishments
    float scoopSemi{ 0.0f };
    float scoopTime{ 50.0f };
    float delayedVibDelay{ 150.0f };
    float delayedVibDepth{ 0.0f };
    bool dutyMorph{ false };
    bool pseudoEcho{ false };
    int pseudoEchoDelay{ 4 };
    float pseudoEchoVol{ 0.5f };

    // Unison & Stereo
    float unisonDetune{ 6.0f }; // Cents
    float stereoSpread{ 0.5f };

    // MANDATORY STANDARD (AGENTS.md): Trim is strictly 0.0 dB for all factory presets
    float masterTrimDb{ 0.0f };
};

inline const std::vector<Preset>& getFactoryPresets()
{
    static const std::vector<Preset> kPresets = {
        // =====================================================================
        // CATEGORY 1: DRUM KITS & RHYTHM GROOVES (01 - 04)
        // =====================================================================
        {
            "01. [DRUMS] Master Arcade Beat (Hold Key)", "Drums",
            0, true, true, 0.45f,
            0, 18000.0f, 0.707f,
            false, 1.0f, 150.0f, 1.0f, 50.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            2, 1, 0.5f, 0, 0, 0, 0, 0, 0, // Pitched oscs muted -> Pure Drum Kit across all keys!
            0, false, 2, 15, 15, true, 0.4f, false,
            true, 2, 0, { 0 }, // Arp enabled in Pure Drum Kit -> holding any key runs 16th-note 8-Bit Beat!
            false, 0,
            false, 0, 0, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "02. [DRUMS] VRC6 & FDS Synth Kit (Bank 2)", "Drums",
            0, true, true, 0.40f,
            0, 19000.0f, 0.75f,
            false, 1.0f, 180.0f, 1.0f, 80.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            2, 1, 0.5f, 0, 0, 0, 0, 0, 0,
            8, false, 3, 15, 15, true, 0.5f, false, // Sample 8 selects Bank 2 (09..16: VRC6 Kick, 93-Step Snare, Rimshot, Cowbell, FDS Tom...)!
            true, 2, 0, { 0 },
            false, 0,
            false, 0, 0, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "03. [DRUMS] Split Kit (C1-B1 Drums / C2+ Lead)", "Drums",
            0, true, true, 0.45f,
            0, 18000.0f, 0.8f,
            true, 2.0f, 140.0f, 0.85f, 120.0f,
            5.5f, 10.0f, 0.0f, 12.0f,
            2, 1, 0.5f, 14, 11, 13, 0, 0, 0,
            0, false, 2, 15, 15, true, 0.4f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 0, 6.0f, 0.0f, 0, true,
            -1.5f, 35.0f, 120.0f, 10.0f, true, false, 4, 0.5f,
            4.0f, 0.3f, 0.0f
        },
        {
            "04. [DRUMS] Vintage 1-Bit ROM Kit (Bank 3)", "Drums",
            0, true, true, 0.45f,
            0, 17000.0f, 1.0f,
            false, 1.0f, 180.0f, 1.0f, 80.0f,
            6.0f, 0.0f, 0.0f, 0.0f,
            0, 1, 0.5f, 0, 0, 0, 0, 0, 0,
            16, false, 2, 15, 15, true, 0.6f, false, // Sample 16 selects Bank 3 (17..24: Slap Bass, Orch Hits, Timpani, Sub-Boom Kick, Fuzz Snare, Scratch)!
            true, 2, 0, { 0 },
            false, 0,
            false, 0, 0, 6.0f, 0.0f, 0, true,
            0.0f, 45.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },

        // =====================================================================
        // CATEGORY 2: CHIPTUNE LEADS (05 - 11)
        // =====================================================================
        {
            "05. [LEAD] Mega Blue Bomber (1987)", "Leads",
            1, true, true, 0.45f,
            0, 18000.0f, 0.707f,
            true, 4.0f, 120.0f, 0.85f, 150.0f,
            5.5f, 12.0f, 0.2f, 0.0f,
            2, 1, 0.5f, 15, 12, 10, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0, 4, 7, 12 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -2.0f, 40.0f, 150.0f, 12.0f, true, false, 4, 0.5f,
            6.0f, 0.4f, 0.0f
        },
        {
            "06. [LEAD] 8-Bit Heroic Mono Legato", "Leads",
            0, true, true, 0.45f,
            0, 18000.0f, 0.707f,
            true, 1.5f, 120.0f, 0.85f, 100.0f,
            6.0f, 8.0f, 0.0f, 22.0f, // 22ms portamento legato & last-note priority trills
            2, 1, 0.5f, 15, 12, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -2.0f, 35.0f, 120.0f, 14.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "07. [LEAD] Gothic VRC6 Saw Lead", "Leads",
            0, true, true, 0.40f,
            0, 17500.0f, 0.9f,
            true, 2.0f, 160.0f, 0.8f, 140.0f,
            5.8f, 10.0f, 0.0f, 8.0f,
            0, 1, 0.5f, 11, 8, 10, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 13, // Clean 14-step VRC6 sawtooth + 1/16 thin pulse
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -1.0f, 25.0f, 140.0f, 12.0f, false, true, 5, 0.45f,
            4.0f, 0.35f, 0.0f
        },
        {
            "08. [LEAD] VRC6 Overdriven Double-Saw", "Leads",
            0, true, true, 0.35f,
            0, 16500.0f, 1.35f,
            true, 1.0f, 200.0f, 0.85f, 120.0f,
            6.2f, 14.0f, 0.0f, 10.0f,
            3, 1, 0.5f, 10, 8, 12, 0, 0, 0, // Duty 3 (inverted 25%) + VRC6 wrap distortion
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 15, // Vol 15 drives 8-bit accumulator wrap-around (>42) for double-saw bite!
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -3.0f, 35.0f, 100.0f, 15.0f, true, false, 4, 0.5f,
            7.0f, 0.5f, 0.0f
        },
        {
            "09. [LEAD] 2C33 Heroic Fantasy Synth", "Leads",
            0, true, true, 0.50f,
            0, 16000.0f, 0.8f,
            true, 3.0f, 160.0f, 0.78f, 130.0f,
            4.8f, 8.0f, 0.0f, 10.0f,
            2, 1, 0.5f, 10, 8, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            true, 7, 15, 5.5f, 0.22f, 0, true, // Wave 7 (Heroic Fantasy Title) + 2C33 hardware vibrato
            -1.0f, 30.0f, 100.0f, 10.0f, false, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "10. [LEAD] 8-Bit Gotham Screamer", "Leads",
            0, true, true, 0.35f,
            0, 15000.0f, 1.8f, // Resonant bite
            true, 1.0f, 180.0f, 0.75f, 110.0f,
            6.5f, 16.0f, 0.0f, 18.0f,
            0, 0, 0.5f, 15, 14, 12, 0, 0, 0, // Twin 12.5% narrow pulses
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -5.0f, 55.0f, 110.0f, 18.0f, true, true, 4, 0.5f,
            9.0f, 0.4f, 0.0f
        },
        {
            "11. [LEAD] Overworld Stereo Unison", "Leads",
            2, true, true, 0.35f,
            0, 20000.0f, 0.8f,
            true, 6.0f, 150.0f, 0.8f, 180.0f,
            6.0f, 15.0f, 0.3f, 20.0f,
            2, 1, 0.5f, 15, 14, 10, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            -1.5f, 35.0f, 120.0f, 10.0f, true, false, 4, 0.5f,
            14.0f, 0.85f, 0.0f
        },

        // =====================================================================
        // CATEGORY 3: BASSES (12 - 16)
        // =====================================================================
        {
            "12. [BASS] 2A03 Stepped Triangle Sub", "Bass",
            0, true, true, 0.55f,
            0, 12000.0f, 1.1f,
            true, 1.0f, 200.0f, 0.9f, 80.0f,
            4.0f, 0.0f, 0.0f, 12.0f,
            0, 0, 0.5f, 4, 0, 15, 0, 0, 0, // Pure 32-step staircase triangle + subtle 12.5% transient
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            true, 1, 13, 4.0f, 0.12f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "13. [BASS] 1-Bit DPCM + Stepped Tri Bass", "Bass",
            0, true, true, 0.45f,
            0, 16000.0f, 0.9f,
            true, 1.5f, 180.0f, 0.65f, 100.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            0, 1, 0.5f, 8, 0, 15, 0, 0, 0,
            16, true, 2, 15, 15, false, 0.5f, false, // Sample 16 (17. 1-Bit Slap Bass Pop) tracked chromatically + 2A03 Triangle!
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "14. [BASS] VRC6 Sawtooth Bass", "Bass",
            0, true, true, 0.45f,
            0, 11000.0f, 1.3f,
            true, 1.0f, 140.0f, 0.55f, 70.0f,
            5.0f, 0.0f, 0.0f, 6.0f,
            0, 1, 0.5f, 9, 0, 14, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 15, // Full VRC6 Sawtooth bass punch
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "15. [BASS] FDS 2C33 Hardware FM Growl Bass", "Bass",
            0, true, true, 0.40f,
            0, 14000.0f, 1.4f,
            true, 1.5f, 170.0f, 0.6f, 90.0f,
            5.0f, 0.0f, 0.0f, 8.0f,
            1, 3, 0.5f, 8, 6, 14, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            true, 2, 15, 24.0f, 0.65f, 1, true, // FDS Saw + Auto-FM Metallic Modulator growl!
            -2.0f, 20.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },
        {
            "16. [BASS] Dn-Tracker Click-Transient Bass", "Bass",
            0, true, true, 0.45f,
            0, 15000.0f, 1.0f,
            true, 1.0f, 160.0f, 0.7f, 80.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            1, 2, 0.5f, 14, 8, 15, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 0, 6, { 1072, 0, 0, 0, 0, 0, 0, 0 }, // Fixed @72 click on frame 0 then bass fundamental!
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            0.0f, 0.0f, 0.0f
        },

        // =====================================================================
        // CATEGORY 4: TRACKER ARPEGGIOS (17 - 21)
        // =====================================================================
        {
            "17. [ARP] Castle VRC6 Minor Arp (60Hz NTSC)", "Arpeggios",
            1, true, true, 0.5f,
            0, 20000.0f, 0.8f,
            true, 2.0f, 80.0f, 0.7f, 80.0f,
            6.0f, 0.0f, 0.0f, 0.0f,
            1, 2, 0.5f, 14, 10, 8, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 0, 1, { 0, 3, 7, 12 }, // 60Hz Minor arpeggio
            true, 12,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, true, 4, 0.45f,
            8.0f, 0.6f, 0.0f
        },
        {
            "18. [ARP] European PAL 50Hz Commando Arp", "Arpeggios",
            1, true, true, 0.40f,
            0, 19000.0f, 0.85f,
            true, 1.5f, 140.0f, 0.75f, 110.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            0, 2, 0.5f, 15, 12, 11, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 1, 2, { 0, 12, 24, 12 }, // 50Hz PAL Octaves
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, true, 5, 0.40f,
            10.0f, 0.65f, 0.0f
        },
        {
            "19. [ARP] Syncopated Crystal Rest Arp", "Arpeggios",
            1, true, true, 0.4f,
            0, 20000.0f, 0.9f,
            true, 2.0f, 100.0f, 0.6f, 100.0f,
            4.0f, 0.0f, 0.0f, 0.0f,
            2, 0, 0.5f, 15, 11, 10, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 1, 6, { 0, -999, 7, -999, 12, 10, 7, -999 }, // Custom pattern with rests!
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, true, 4, 0.4f,
            4.0f, 0.5f, 0.0f
        },
        {
            "20. [ARP] Boss Battle Octave Chugg (1/32)", "Arpeggios",
            1, true, true, 0.5f,
            0, 18000.0f, 1.1f,
            true, 2.0f, 100.0f, 0.5f, 60.0f,
            8.0f, 0.0f, 0.0f, 0.0f,
            2, 0, 0.5f, 15, 12, 15, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 3, 5, { 0, 12, 0, 12, 3, 12, 7, 12 }, // Chug pattern
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            9.0f, 0.4f, 0.0f
        },
        {
            "21. [ARP] Starlight Major 7th Shimmer", "Arpeggios",
            2, true, true, 0.35f,
            0, 19000.0f, 0.8f,
            true, 8.0f, 220.0f, 0.75f, 260.0f,
            5.0f, 8.0f, 0.25f, 0.0f,
            2, 1, 0.5f, 13, 11, 10, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            true, 0, 6, { 0, 4, 7, 11, 12, 11, 7, 4 }, // Lush Maj7 arpeggio
            false, 0,
            true, 0, 11, 6.0f, 0.15f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, true, 6, 0.45f,
            12.0f, 0.8f, 0.0f
        },

        // =====================================================================
        // CATEGORY 5: KEYS, BELLS & PLUCKS (22 - 25)
        // =====================================================================
        {
            "22. [KEYS] FDS 2C33 Chromatic FM Bells", "Keys",
            1, true, true, 0.4f,
            0, 16000.0f, 1.2f,
            true, 2.0f, 380.0f, 0.18f, 320.0f,
            7.0f, 0.0f, 0.0f, 0.0f,
            0, 3, 0.5f, 12, 10, 8, 0, 1, 1,
            5, true, 3, 14, 10, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 8,
            true, 5, 15, 18.0f, 0.55f, 1, true, // FDS Bell + Auto-FM Metallic Modulator!
            0.0f, 50.0f, 0.0f, 0.0f, false, true, 6, 0.4f,
            14.0f, 0.55f, 0.0f
        },
        {
            "23. [KEYS] 2C33 Disk Funk Clavinet", "Keys",
            1, true, true, 0.35f,
            2, 2400.0f, 1.6f, // Bandpass funkbite
            true, 1.0f, 150.0f, 0.35f, 90.0f,
            5.5f, 0.0f, 0.0f, 0.0f,
            0, 1, 0.5f, 15, 13, 0, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 12,
            true, 3, 14, 8.0f, 0.15f, 1, true,
            -1.0f, 15.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            6.0f, 0.4f, 0.0f
        },
        {
            "24. [KEYS] Neo-Tokyo Poly Chiptune Stabs", "Keys",
            1, true, true, 0.35f,
            0, 15000.0f, 1.4f,
            true, 1.0f, 220.0f, 0.45f, 140.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            1, 2, 0.5f, 15, 13, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            true, 11,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, false, 4, 0.5f,
            8.0f, 0.6f, 0.0f
        },
        {
            "25. [KEYS] Haunted Castle Pipe Organ (FDS)", "Keys",
            1, true, true, 0.45f,
            0, 14000.0f, 0.8f,
            true, 15.0f, 300.0f, 0.9f, 220.0f,
            5.2f, 6.0f, 0.0f, 0.0f,
            2, 0, 0.5f, 11, 10, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            true, 4, 15, 5.5f, 0.20f, 0, true, // FDS Organ wavetable
            0.0f, 50.0f, 120.0f, 10.0f, false, true, 5, 0.35f,
            10.0f, 0.6f, 0.0f
        },

        // =====================================================================
        // CATEGORY 6: PADS & VINTAGE ATMOSPHERES (26 - 29)
        // =====================================================================
        {
            "26. [PAD] Solar 8-Voice Poly APU Pad", "Pads & Poly",
            1, true, true, 0.4f,
            0, 14000.0f, 0.8f,
            true, 120.0f, 350.0f, 0.75f, 450.0f,
            3.5f, 6.0f, 0.4f, 0.0f,
            2, 2, 0.5f, 13, 11, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 200.0f, 8.0f, false, false, 4, 0.5f,
            5.0f, 0.7f, 0.0f
        },
        {
            "27. [PAD] Alien Space 2C33 Choir", "Pads & Poly",
            1, true, true, 0.50f,
            0, 12000.0f, 1.1f,
            true, 90.0f, 400.0f, 0.80f, 400.0f,
            4.2f, 10.0f, 0.0f, 0.0f,
            1, 2, 0.5f, 10, 9, 12, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            true, 6, 15, 6.2f, 0.30f, 0, true, // Wave 6 (Formant Aah) + 2C33 vibrato
            0.0f, 50.0f, 180.0f, 12.0f, false, true, 6, 0.4f,
            11.0f, 0.75f, 0.0f
        },
        {
            "28. [PAD] Cybernetic Continuous PWM Sweep", "Pads & Poly",
            1, false, false, 0.2f,
            0, 14000.0f, 0.9f,
            true, 80.0f, 400.0f, 0.7f, 350.0f,
            2.0f, 0.0f, 0.65f, 0.0f,
            4, 4, 0.5f, 12, 12, 8, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            12.0f, 0.9f, 0.0f
        },
        {
            "29. [VINTAGE] CRT Nostalgia (Warm 1983 TV)", "Vintage",
            1, true, true, 0.85f,
            0, 10000.0f, 1.0f,
            true, 10.0f, 200.0f, 0.8f, 250.0f,
            2.5f, 8.0f, 0.2f, 0.0f,
            2, 1, 0.5f, 14, 11, 14, 0, 0, 0,
            0, false, 2, 15, 13, false, 0.0f, false,
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            7.0f, 0.3f, 0.0f
        },

        // =====================================================================
        // CATEGORY 7: DPCM SAMPLER, PURE 2A03 & RETRO SFX (30 - 32)
        // =====================================================================
        {
            "30. [DPCM] 1-Bit Robotic Speech (KINDNES)", "DPCM Sampler",
            1, true, true, 0.4f,
            0, 16000.0f, 0.9f,
            true, 1.0f, 500.0f, 1.0f, 200.0f,
            4.0f, 0.0f, 0.0f, 0.0f,
            2, 1, 0.5f, 8, 8, 8, 0, 0, 0,
            24, true, 1, 15, 15, true, 0.7f, false, // Sample 24 (25. Speech: KINDNES)
            false, 0, 0, { 0 },
            false, 0,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, false, false, 4, 0.5f,
            8.0f, 0.5f, 0.0f
        },
        {
            "31. [DPCM] 8-Bit Orchestra Hit Stabs", "DPCM Sampler",
            1, true, true, 0.35f,
            0, 17500.0f, 1.1f,
            true, 1.0f, 260.0f, 0.5f, 180.0f,
            5.0f, 0.0f, 0.0f, 0.0f,
            1, 2, 0.5f, 11, 9, 10, 0, 0, 0,
            18, true, 3, 15, 15, false, 0.4f, false, // Sample 18 (19. Orchestra Hit Major) tracked chromatically
            false, 0, 0, { 0 },
            true, 10,
            false, 0, 15, 6.0f, 0.0f, 0, true,
            0.0f, 50.0f, 0.0f, 0.0f, true, true, 4, 0.4f,
            10.0f, 0.6f, 0.0f
        },
        {
            "32. [SFX] Arcade Laser & Coin Cascade", "SFX",
            0, true, true, 0.40f,
            0, 19000.0f, 1.2f,
            true, 1.0f, 220.0f, 0.6f, 150.0f,
            8.0f, 0.0f, 0.0f, 0.0f,
            0, 2, 0.5f, 14, 10, 12, 8, 1, 3, // Mode 1 93-step metallic ring
            26, true, 2, 15, 14, false, 0.5f, false, // Sample 26 (27. Retro Laser Zapper)
            true, 0, 6, { 0, 7, 12, 19, 24 },
            false, 0,
            true, 2, 14, 16.0f, 0.70f, 2, true, // FDS Laser Sweep Modulator!
            12.0f, 80.0f, 0.0f, 0.0f, false, true, 3, 0.5f,
            6.0f, 0.4f, 0.0f
        }
    };
    return kPresets;
}

} // namespace KindnesAudio
