#pragma once

#include <cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Supercharged 1-bit DPCM Sampler & Real-time Through-DPCM Crusher.
 * 
 * Features:
 * - 8 Built-in iconic vintage 1-bit ROM samples:
 *   0: Kick, 1: Snare, 2: Hi-Hat, 3: Slap Bass, 4: Tom, 5: Orchestra Hit, 6: Speech, 7: Laser
 * - 4-Voice Polyphonic DPCM Drum Engine (play Kick, Snare, and Hi-Hat simultaneously!)
 * - Velocity-to-Slew Modulation (Soft hits = fuzzy slope overload, Hard hits = punchy transient)
 * - Chromatic Keyboard Tracking (Play any 1-bit sample melodically across keys)
 * - Real-Time Through-DPCM Audio Input Mode (route external DAW tracks through 1-bit delta encoding)
 * - Live 1-bit staircase telemetry for visual display
 */
class DPCMChannel
{
public:
    static constexpr uint16_t kDPCMRatesNTSC[16] = {
        428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106, 84, 72, 54
    };

    struct SubVoice
    {
        int outputCounter{ 64 };
        size_t sampleAddress{ 0 };
        int bitIndex{ 0 };
        float playbackFreqHz{ 33144.0f };
        float phaseAccumulator{ 0.0f };
        int deltaStep{ 2 };
        float velocity{ 1.0f };
        bool isPlaying{ false };
        bool loop{ false };
        bool reverse{ false };
        int sampleIndex{ 0 };

        void reset()
        {
            outputCounter = 64;
            sampleAddress = 0;
            bitIndex = 0;
            phaseAccumulator = 0.0f;
            isPlaying = false;
        }
    };

    DPCMChannel()
    {
        generateBuiltinSampleLibrary();
        for (auto& v : voices_)
            v.reset();
        reset();
    }

    void reset() noexcept
    {
        for (auto& v : voices_)
            v.reset();

        rateIndex_ = 15;
        enabled_ = true;
        chromaticTracking_ = false;
        rootNote_ = 48; // C3
        deltaStep_ = 2; // Authentic 2A03 hardware default
        velToSlewAmount_ = 0.0f;
        volume_ = 15;
        throughDpcDac_ = 64;
        throughDpcmEnabled_ = false;
        padActivity_.fill(0.0f);
        demoBeatPhase_ = 0.0f;
        demoBeatStep_ = 0;
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(std::max(8000.0, sampleRate));
        updateBasePlaybackFrequency();
    }

    void setRateIndex(int index) noexcept
    {
        rateIndex_ = std::clamp(index, 0, 15);
        updateBasePlaybackFrequency();
    }

    [[nodiscard]] int getRateIndex() const noexcept { return rateIndex_; }

    void setChromaticTracking(bool enable, int rootNote = 48) noexcept
    {
        chromaticTracking_ = enable;
        rootNote_ = rootNote;
    }

    [[nodiscard]] bool isChromaticTracking() const noexcept { return chromaticTracking_; }

    void setDeltaStep(int step) noexcept
    {
        deltaStep_ = std::clamp(step, 1, 8);
    }

    [[nodiscard]] int getDeltaStep() const noexcept { return deltaStep_; }

    void setVelToSlew(float amount) noexcept
    {
        velToSlewAmount_ = std::clamp(amount, 0.0f, 1.0f);
    }

    void setVolume(int vol) noexcept { volume_ = std::clamp(vol, 0, 15); }
    [[nodiscard]] int getVolume() const noexcept { return volume_; }

    void setThroughDPCMEnabled(bool enable) noexcept { throughDpcmEnabled_ = enable; }
    [[nodiscard]] bool isThroughDPCMEnabled() const noexcept { return throughDpcmEnabled_; }

    void setDemoBeatEnabled(bool enable) noexcept
    {
        if (enable && !demoBeatEnabled_)
        {
            demoBeatPhase_ = 1.0f; // Trigger first step immediately
            demoBeatStep_ = 0;
        }
        demoBeatEnabled_ = enable;
    }
    [[nodiscard]] bool isDemoBeatEnabled() const noexcept { return demoBeatEnabled_; }

    [[nodiscard]] float getPadActivity(int padIdx) const noexcept
    {
        if (padIdx >= 0 && padIdx < 8)
            return padActivity_[static_cast<size_t>(padIdx)];
        return 0.0f;
    }

    const std::vector<float>& getReconstructedWaveform() const noexcept
    {
        return reconstructedWaveform_;
    }

    const std::vector<float>& getOriginalPcmWaveform() const noexcept
    {
        return originalPcmWaveform_;
    }

    void selectBuiltinSample(int index)
    {
        if (index >= 0 && index < static_cast<int>(builtinSamples_.size()))
        {
            activeSampleIndex_ = index;
            originalPcmWaveform_ = builtinSamples_[index].pcmPreview;
            reconstructedWaveform_ = builtinSamples_[index].deltaPreview;
        }
    }

    [[nodiscard]] int getActiveSampleIndex() const noexcept { return activeSampleIndex_; }

    // --- Note & Pad Triggering ---
    void triggerNote(int midiNote = 48, float velocity = 1.0f) noexcept
    {
        if (velocity <= 0.0f)
            return;

        int voiceIdx = findFreeVoice();
        auto& v = voices_[voiceIdx];
        v.reset();
        v.sampleIndex = activeSampleIndex_;
        v.velocity = velocity;

        // Dynamic Slew Modulation
        int effStep = deltaStep_;
        if (velToSlewAmount_ > 0.01f)
        {
            float mod = 1.0f + (velocity - 0.5f) * 2.0f * velToSlewAmount_;
            effStep = std::clamp(static_cast<int>(std::round(static_cast<float>(deltaStep_) * mod)), 1, 8);
        }
        v.deltaStep = effStep;

        if (chromaticTracking_)
        {
            float pitchRatio = std::pow(2.0f, static_cast<float>(midiNote - rootNote_) / 12.0f);
            v.playbackFreqHz = basePlaybackFreqHz_ * pitchRatio;
        }
        else
        {
            v.playbackFreqHz = basePlaybackFreqHz_;
        }

        v.isPlaying = true;
        padActivity_[static_cast<size_t>(activeSampleIndex_ & 7)] = 1.0f;
    }

    void triggerPad(int padIndex, float velocity = 1.0f) noexcept
    {
        if (padIndex < 0 || padIndex >= static_cast<int>(builtinSamples_.size()) || velocity <= 0.0f)
            return;

        if (volume_ == 0)
            volume_ = 13; // Ensure pad triggers are always audible

        int voiceIdx = findFreeVoice();
        auto& v = voices_[voiceIdx];
        v.reset();
        v.sampleIndex = padIndex;
        v.velocity = velocity;
        v.playbackFreqHz = basePlaybackFreqHz_;

        int effStep = deltaStep_;
        if (velToSlewAmount_ > 0.01f)
        {
            float mod = 1.0f + (velocity - 0.5f) * 2.0f * velToSlewAmount_;
            effStep = std::clamp(static_cast<int>(std::round(static_cast<float>(deltaStep_) * mod)), 1, 8);
        }
        v.deltaStep = effStep;
        v.isPlaying = true;

        // Open Hi-Hat Choke: triggering Closed Hi-Hat (2) chokes Open Hi-Hat (3)
        if (padIndex == 2)
        {
            for (auto& other : voices_)
            {
                if (&other != &v && other.isPlaying && other.sampleIndex == 3)
                    other.isPlaying = false;
            }
        }

        padActivity_[static_cast<size_t>(padIndex & 7)] = 1.0f;
    }

    void stopSample() noexcept
    {
        for (auto& v : voices_)
            v.reset();
    }

    void setLoop(bool loop) noexcept { loop_ = loop; }
    void setReverse(bool rev) noexcept { reverse_ = rev; }

    /**
     * Real-time Through-DPCM processor: takes audio input and crushes it through 1-bit delta modulation!
     */
    float processThroughAudio(float inSample) noexcept
    {
        if (!throughDpcmEnabled_)
            return inSample;

        int targetLevel = std::clamp(static_cast<int>((inSample + 1.0f) * 63.5f), 0, 126);
        if (targetLevel > throughDpcDac_)
            throughDpcDac_ = std::min(126, throughDpcDac_ + deltaStep_);
        else
            throughDpcDac_ = std::max(0, throughDpcDac_ - deltaStep_);

        return (static_cast<float>(throughDpcDac_) / 63.5f) - 1.0f;
    }

    /**
     * Encodes custom audio buffer into 1-bit DPCM.
     */
    void encodeAndLoadPCM(const float* pcmData, int numSamples, float inputSampleRate)
    {
        if (numSamples <= 0 || pcmData == nullptr)
            return;

        updateBasePlaybackFrequency();
        float resampleStep = inputSampleRate / basePlaybackFreqHz_;

        std::vector<uint8_t> encoded;
        originalPcmWaveform_.clear();
        reconstructedWaveform_.clear();

        int currentDac = 64;
        uint8_t currentByte = 0;
        int currentBit = 0;

        float srcIndex = 0.0f;
        while (static_cast<int>(srcIndex) < numSamples)
        {
            int idx = static_cast<int>(srcIndex);
            float frac = srcIndex - static_cast<float>(idx);
            float s0 = pcmData[idx];
            float s1 = (idx + 1 < numSamples) ? pcmData[idx + 1] : s0;
            float inputVal = s0 + frac * (s1 - s0);

            originalPcmWaveform_.push_back(inputVal);

            int targetLevel = std::clamp(static_cast<int>((inputVal + 1.0f) * 63.5f), 0, 126);

            int bit = 0;
            if (targetLevel > currentDac)
            {
                bit = 1;
                currentDac = std::min(126, currentDac + deltaStep_);
            }
            else
            {
                bit = 0;
                currentDac = std::max(0, currentDac - deltaStep_);
            }

            reconstructedWaveform_.push_back((static_cast<float>(currentDac) / 63.5f) - 1.0f);

            currentByte |= (bit << currentBit);
            currentBit++;
            if (currentBit == 8)
            {
                encoded.push_back(currentByte);
                currentByte = 0;
                currentBit = 0;
            }

            srcIndex += resampleStep;
        }

        if (currentBit > 0)
            encoded.push_back(currentByte);

        // Put in slot 32 (User Sample)
        if (builtinSamples_.size() <= 32)
        {
            BuiltinSample userSamp;
            userSamp.name = "33. User Sample";
            userSamp.data = std::move(encoded);
            userSamp.pcmPreview = originalPcmWaveform_;
            userSamp.deltaPreview = reconstructedWaveform_;
            builtinSamples_.push_back(std::move(userSamp));
        }
        else
        {
            builtinSamples_[32].data = std::move(encoded);
            builtinSamples_[32].pcmPreview = originalPcmWaveform_;
            builtinSamples_[32].deltaPreview = reconstructedWaveform_;
        }
        activeSampleIndex_ = 32;
    }

    /**
     * Advances polyphonic DPCM voices and returns mixed 7-bit output (0 to 127).
     */
    float processSample() noexcept
    {
        // Decay visual pad LED activity smoothly (sample-rate invariant ~120ms decay)
        float decayStep = 8.0f / sampleRate_;
        for (auto& act : padActivity_)
        {
            if (act > 0.0f)
                act = std::max(0.0f, act - decayStep);
        }

        if (volume_ == 0)
            return 64.0f;

        float sumCentered = 0.0f;
        int activeCount = 0;

        for (auto& v : voices_)
        {
            if (!v.isPlaying || v.sampleIndex >= static_cast<int>(builtinSamples_.size()))
                continue;

            const auto& data = builtinSamples_[v.sampleIndex].data;
            if (data.empty())
                continue;

            activeCount++;

            float step = v.playbackFreqHz / sampleRate_;
            v.phaseAccumulator += step;

            while (v.phaseAccumulator >= 1.0f)
            {
                v.phaseAccumulator -= 1.0f;
                stepVoiceBit(v, data);
            }

            sumCentered += static_cast<float>(v.outputCounter - 64) * v.velocity;
        }

        if (activeCount == 0)
            return 64.0f; // Neutral resting DC level

        // Scale by volume (0..15)
        float scaled = 64.0f + sumCentered * (static_cast<float>(volume_) / 15.0f);
        return std::clamp(scaled, 0.0f, 127.0f);
    }

private:
    int findFreeVoice() noexcept
    {
        for (size_t i = 0; i < voices_.size(); ++i)
        {
            if (!voices_[i].isPlaying)
                return static_cast<int>(i);
        }
        // Round robin steal
        rrIndex_ = (rrIndex_ + 1) % voices_.size();
        return static_cast<int>(rrIndex_);
    }

    void stepVoiceBit(SubVoice& v, const std::vector<uint8_t>& data) noexcept
    {
        if (v.sampleAddress >= data.size())
        {
            if (loop_ && !data.empty())
            {
                v.sampleAddress = 0;
                v.bitIndex = 0;
            }
            else
            {
                v.isPlaying = false;
                return;
            }
        }

        uint8_t byte = data[v.sampleAddress];
        int bit = (byte >> v.bitIndex) & 1;

        if (bit == 1)
            v.outputCounter = std::min(126, v.outputCounter + v.deltaStep);
        else
            v.outputCounter = std::max(0, v.outputCounter - v.deltaStep);

        v.bitIndex++;
        if (v.bitIndex >= 8)
        {
            v.bitIndex = 0;
            v.sampleAddress++;
        }
    }

    void updateBasePlaybackFrequency() noexcept
    {
        constexpr float fCPU = 1789773.0f;
        basePlaybackFreqHz_ = fCPU / static_cast<float>(kDPCMRatesNTSC[rateIndex_]);
    }

    struct BuiltinSample
    {
        std::string name;
        std::vector<uint8_t> data;
        std::vector<float> pcmPreview;
        std::vector<float> deltaPreview;
    };

    void generateBuiltinSampleLibrary()
    {
        builtinSamples_.clear();
        constexpr float kTwoPi = 6.28318530718f;

        // =====================================================================
        // BANK 1 (00..07): CLASSIC NES MASTER DRUM KIT
        // =====================================================================
        // 00 (01). Sunsoft Punch Kick
        {
            std::vector<float> pcm(3400);
            float ph = 0.0f;
            uint32_t seed = 0x13579;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 230.0f * std::exp(-t * 14.0f) + 46.0f;
                ph += f / 44100.0f;
                seed = seed * 1664525u + 1013904223u;
                float click = (static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f) * std::exp(-t * 45.0f);
                float body = std::tanh(std::sin(ph * kTwoPi) * 2.3f) * std::exp(-t * 3.2f);
                pcm[i] = std::clamp(body * 0.95f + click * 0.45f, -1.0f, 1.0f);
            }
            addBuiltin("01. Sunsoft Punch Kick", pcm);
        }

        // 01 (02). Mega Crisp Snare
        {
            std::vector<float> pcm(3800);
            uint32_t seed = 0x54321;
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                float f = 250.0f * std::exp(-t * 10.0f) + 115.0f;
                ph += f / 44100.0f;
                float tone = std::sin(ph * kTwoPi) * std::exp(-t * 8.0f);
                pcm[i] = std::clamp((nz * 0.85f * std::exp(-t * 3.4f) + tone * 0.65f), -1.0f, 1.0f);
            }
            addBuiltin("02. Mega Crisp Snare", pcm);
        }

        // 02 (03). Closed Hi-Hat
        {
            std::vector<float> pcm(1600);
            uint32_t seed = 0x98765;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                float ring = ((i % 9) < 4) ? 0.5f : -0.5f;
                pcm[i] = std::clamp((nz * 0.8f + ring * 0.45f) * std::exp(-t * 9.5f), -1.0f, 1.0f);
            }
            addBuiltin("03. Closed Hi-Hat", pcm);
        }

        // 03 (04). Open Hi-Hat (Choked by Closed Hat)
        {
            std::vector<float> pcm(5200);
            uint32_t seed = 0xABCDE;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                float sizzle = std::sin(static_cast<float>(i) * 1.35f) * std::sin(static_cast<float>(i) * 0.83f);
                pcm[i] = std::clamp((nz * 0.75f + sizzle * 0.5f) * std::exp(-t * 3.2f), -1.0f, 1.0f);
            }
            addBuiltin("04. Open Hi-Hat", pcm);
        }

        // 04 (05). 8-Bit Hand Clap (Multi-Burst 60Hz Tracker Clap)
        {
            std::vector<float> pcm(4000);
            uint32_t seed = 0x2468A;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                // 3 micro-flam bursts in the first 30 ms followed by reverb tail
                float env = 0.0f;
                if (t < 0.08f) env = 1.0f - (t / 0.08f) * 0.7f;
                else if (t < 0.16f) env = 1.0f - ((t - 0.08f) / 0.08f) * 0.7f;
                else if (t < 0.24f) env = 1.0f - ((t - 0.16f) / 0.08f) * 0.5f;
                else env = std::exp(-(t - 0.24f) * 5.0f);
                pcm[i] = std::clamp(nz * env * 0.95f, -1.0f, 1.0f);
            }
            addBuiltin("05. 8-Bit Hand Clap", pcm);
        }

        // 05 (06). High Rack Tom
        {
            std::vector<float> pcm(3200);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 330.0f * std::exp(-t * 4.8f) + 92.0f;
                ph += f / 44100.0f;
                pcm[i] = std::tanh(std::sin(ph * kTwoPi) * 1.8f) * std::exp(-t * 2.8f);
            }
            addBuiltin("06. High Rack Tom", pcm);
        }

        // 06 (07). Low Floor Tom
        {
            std::vector<float> pcm(3800);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 210.0f * std::exp(-t * 4.2f) + 56.0f;
                ph += f / 44100.0f;
                pcm[i] = std::tanh(std::sin(ph * kTwoPi) * 2.0f) * std::exp(-t * 2.5f);
            }
            addBuiltin("07. Low Floor Tom", pcm);
        }

        // 07 (08). Arcade Crash Cymbal
        {
            std::vector<float> pcm(8400);
            uint32_t seed = 0x77777;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                float metal = std::sin(static_cast<float>(i) * 0.91f) * std::cos(static_cast<float>(i) * 1.47f);
                pcm[i] = std::clamp((nz * 0.7f + metal * 0.55f) * std::exp(-t * 2.8f), -1.0f, 1.0f);
            }
            addBuiltin("08. Arcade Crash Cymbal", pcm);
        }

        // =====================================================================
        // BANK 2 (08..15): KONAMI VRC6 & FDS FM SYNTH PERCUSSION KIT
        // =====================================================================
        // 08 (09). VRC6 Overdriven Synth Kick
        {
            std::vector<float> pcm(3600);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 310.0f * std::exp(-t * 16.0f) + 42.0f;
                ph += f / 44100.0f;
                float saw = 2.0f * (ph - std::floor(ph)) - 1.0f;
                pcm[i] = std::tanh((std::sin(ph * kTwoPi) + 0.65f * saw) * 2.5f) * std::exp(-t * 3.0f);
            }
            addBuiltin("09. VRC6 Overdriven Kick", pcm);
        }

        // 09 (10). 93-Step Metallic Robo-Snare
        {
            std::vector<float> pcm(3600);
            uint16_t lfsr = 1;
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                if ((i % 3) == 0)
                {
                    uint16_t fb = (lfsr & 1u) ^ ((lfsr >> 6) & 1u); // 2A03 Mode 1 (93-step)
                    lfsr = (lfsr >> 1) | (fb << 14);
                }
                float mNoise = ((lfsr & 1u) == 0u) ? 0.85f : -0.85f;
                ph += (195.0f * std::exp(-t * 8.0f) + 90.0f) / 44100.0f;
                pcm[i] = std::clamp((mNoise * 0.75f + std::sin(ph * kTwoPi) * 0.6f) * std::exp(-t * 3.5f), -1.0f, 1.0f);
            }
            addBuiltin("10. 93-Step Metallic Snare", pcm);
        }

        // 10 (11). Rimshot / Chiptune Side-Stick
        {
            std::vector<float> pcm(1400);
            float ph1 = 0.0f, ph2 = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                ph1 += 480.0f / 44100.0f;
                ph2 += 1180.0f / 44100.0f;
                float click = (std::sin(ph1 * kTwoPi) + 0.8f * std::sin(ph2 * kTwoPi));
                pcm[i] = std::tanh(click * 2.4f) * std::exp(-t * 11.0f);
            }
            addBuiltin("11. Chiptune Rimshot", pcm);
        }

        // 11 (12). 8-Bit Arcade Cowbell (540 Hz + 800 Hz Dual Pulse)
        {
            std::vector<float> pcm(3200);
            float ph1 = 0.0f, ph2 = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                ph1 += 540.0f / 44100.0f;
                ph2 += 800.0f / 44100.0f;
                float p1 = (std::fmod(ph1, 1.0f) < 0.25f) ? 1.0f : -0.33f;
                float p2 = (std::fmod(ph2, 1.0f) < 0.25f) ? 1.0f : -0.33f;
                pcm[i] = std::clamp((p1 + p2) * 0.65f * std::exp(-t * 4.5f), -1.0f, 1.0f);
            }
            addBuiltin("12. 8-Bit Arcade Cowbell", pcm);
        }

        // 12 (13). FDS 2C33 FM Simmons Tom
        {
            std::vector<float> pcm(3800);
            float phC = 0.0f, phM = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 260.0f * std::exp(-t * 5.5f) + 72.0f;
                phM += (f * 2.4f) / 44100.0f;
                float mod = std::sin(phM * kTwoPi) * 1.8f * std::exp(-t * 6.0f);
                phC += f / 44100.0f;
                pcm[i] = std::sin(phC * kTwoPi + mod) * std::exp(-t * 2.8f);
            }
            addBuiltin("13. FDS 2C33 FM Simmons Tom", pcm);
        }

        // 13 (14). Gated Boss Battle Snare
        {
            std::vector<float> pcm(5200);
            uint32_t seed = 0x31415;
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                ph += (210.0f * std::exp(-t * 12.0f) + 100.0f) / 44100.0f;
                // Gated plateau until t=0.65, then steep gate close
                float gate = (t < 0.65f) ? (1.0f - t * 0.25f) : std::exp(-(t - 0.65f) * 14.0f);
                pcm[i] = std::clamp((nz * 0.8f + std::sin(ph * kTwoPi) * 0.6f * std::exp(-t * 6.0f)) * gate, -1.0f, 1.0f);
            }
            addBuiltin("14. Gated Boss Battle Snare", pcm);
        }

        // 14 (15). Chiptune Shaker / Maraca
        {
            std::vector<float> pcm(2200);
            uint32_t seed = 0x65432;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                float swell = std::sin(std::min(1.0f, t * 2.2f) * 3.14159265f) * std::exp(-t * 3.5f);
                pcm[i] = std::clamp(nz * swell * 0.95f, -1.0f, 1.0f);
            }
            addBuiltin("15. Chiptune Shaker", pcm);
        }

        // 15 (16). FDS Metallic Ride Bell
        {
            std::vector<float> pcm(5400);
            float p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                p1 += 620.0f / 44100.0f;
                p2 += 1470.0f / 44100.0f;
                p3 += 2680.0f / 44100.0f;
                float bell = 0.5f * std::sin(p1 * kTwoPi) + 0.4f * std::sin(p2 * kTwoPi) + 0.35f * std::sin(p3 * kTwoPi);
                pcm[i] = std::clamp(bell * std::exp(-t * 3.6f), -1.0f, 1.0f);
            }
            addBuiltin("16. FDS Metallic Ride Bell", pcm);
        }

        // =====================================================================
        // BANK 3 (16..23): SUNSOFT & KONAMI 1-BIT SAMPLE ROM HITS & BASSES
        // =====================================================================
        // 16 (17). 1-Bit Slap Bass (Pop)
        {
            std::vector<float> pcm(4200);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 110.0f + 55.0f * std::exp(-t * 28.0f);
                ph += f / 44100.0f;
                float harm = std::sin(ph * kTwoPi)
                           + 0.75f * std::sin(ph * 2.0f * kTwoPi) * std::exp(-t * 6.0f)
                           + 0.45f * std::sin(ph * 3.0f * kTwoPi) * std::exp(-t * 14.0f);
                pcm[i] = std::clamp(std::tanh(harm * 1.6f) * std::exp(-t * 2.4f), -1.0f, 1.0f);
            }
            addBuiltin("17. 1-Bit Slap Bass (Pop)", pcm);
        }

        // 17 (18). 1-Bit Slap Bass (Thumb Sub)
        {
            std::vector<float> pcm(4400);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 55.0f + 110.0f * std::exp(-t * 35.0f);
                ph += f / 44100.0f;
                float harm = std::sin(ph * kTwoPi) + 0.6f * std::sin(ph * 2.0f * kTwoPi) * std::exp(-t * 5.0f);
                pcm[i] = std::clamp(std::tanh(harm * 1.9f) * std::exp(-t * 2.2f), -1.0f, 1.0f);
            }
            addBuiltin("18. 1-Bit Slap Bass (Thumb)", pcm);
        }

        // 18 (19). Orchestra Hit (Major Brass Stab)
        {
            std::vector<float> pcm(4200);
            float ph1 = 0.0f, ph2 = 0.0f, ph3 = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                ph1 += 220.0f / 44100.0f;
                ph2 += 277.18f / 44100.0f;
                ph3 += 329.63f / 44100.0f;
                float s = (std::sin(ph1 * kTwoPi) + std::sin(ph2 * kTwoPi) + std::sin(ph3 * kTwoPi)
                         + 0.4f * std::sin(ph1 * 2.0f * kTwoPi)) * 0.42f;
                pcm[i] = std::tanh(s * 1.8f) * std::exp(-t * 2.5f);
            }
            addBuiltin("19. Orchestra Hit (Major)", pcm);
        }

        // 19 (20). Orchestra Hit (Minor Castlevania Stab)
        {
            std::vector<float> pcm(4200);
            float ph1 = 0.0f, ph2 = 0.0f, ph3 = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                ph1 += 220.0f / 44100.0f;
                ph2 += 261.63f / 44100.0f;
                ph3 += 329.63f / 44100.0f;
                float s = (std::sin(ph1 * kTwoPi) + std::sin(ph2 * kTwoPi) + std::sin(ph3 * kTwoPi)
                         + 0.5f * std::sin(ph3 * 2.0f * kTwoPi)) * 0.42f;
                pcm[i] = std::tanh(s * 1.9f) * std::exp(-t * 2.4f);
            }
            addBuiltin("20. Orchestra Hit (Minor)", pcm);
        }

        // 20 (21). Konami 1-Bit Timpani Strike
        {
            std::vector<float> pcm(4800);
            float ph = 0.0f;
            uint32_t seed = 0x11223;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 145.0f * std::exp(-t * 6.0f) + 98.0f;
                ph += f / 44100.0f;
                seed = seed * 1664525u + 1013904223u;
                float mallet = (static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f) * std::exp(-t * 25.0f);
                float kettle = (std::sin(ph * kTwoPi) + 0.35f * std::sin(ph * 1.51f * kTwoPi)) * std::exp(-t * 2.2f);
                pcm[i] = std::clamp(kettle * 0.85f + mallet * 0.4f, -1.0f, 1.0f);
            }
            addBuiltin("21. Konami 1-Bit Timpani", pcm);
        }

        // 21 (22). 2A03 Deep Sub-Boom Kick
        {
            std::vector<float> pcm(4600);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 160.0f * std::exp(-t * 18.0f) + 38.0f;
                ph += f / 44100.0f;
                // Quantize to 16 steps for 4-bit triangle sub grit
                float tri = 2.0f * std::fabs(2.0f * (ph - std::floor(ph + 0.5f))) - 1.0f;
                tri = std::round(tri * 7.5f) / 7.5f;
                pcm[i] = tri * std::exp(-t * 2.3f);
            }
            addBuiltin("22. 2A03 Deep Sub-Boom Kick", pcm);
        }

        // 22 (23). Slope-Overload Fuzzy Crunch Snare
        {
            std::vector<float> pcm(3800);
            uint32_t seed = 0x88888;
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                ph += (180.0f * std::exp(-t * 7.0f) + 85.0f) / 44100.0f;
                pcm[i] = std::tanh((nz * 1.4f + std::sin(ph * kTwoPi) * 1.2f) * 2.0f) * std::exp(-t * 3.0f);
            }
            addBuiltin("23. Slope-Overload Fuzz Snare", pcm);
        }

        // 23 (24). 1-Bit Vinyl Record Scratch
        {
            std::vector<float> pcm(4000);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 220.0f + 580.0f * std::sin(t * 9.42477f);
                ph += std::fabs(f) / 44100.0f;
                float wave = std::sin(ph * kTwoPi) + 0.5f * std::sin(ph * 3.0f * kTwoPi);
                pcm[i] = std::tanh(wave * 1.6f) * std::sin(t * 3.14159265f);
            }
            addBuiltin("24. 1-Bit Vinyl Scratch", pcm);
        }

        // =====================================================================
        // BANK 4 (24..31): RETRO ARCADE VOCALS & SFX KIT
        // =====================================================================
        // 24 (25). Speech: "KINDNES"
        {
            std::vector<float> pcm(4400);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f0 = 120.0f + 30.0f * std::sin(t * 3.1415f);
                ph += f0 / 44100.0f;
                float form1 = std::sin(ph * kTwoPi * 5.0f) * 0.55f;
                float form2 = std::sin(ph * kTwoPi * 14.0f) * 0.35f;
                pcm[i] = (form1 + form2) * std::sin(t * 3.14159f);
            }
            addBuiltin("25. Speech: KINDNES", pcm);
        }

        // 25 (26). Speech: "POWER UP"
        {
            std::vector<float> pcm(4800);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f0 = 105.0f + 65.0f * t;
                ph += f0 / 44100.0f;
                float f1 = std::sin(ph * kTwoPi * (4.0f + 3.0f * std::sin(t * 6.28f))) * 0.55f;
                float f2 = std::sin(ph * kTwoPi * 11.0f) * 0.35f;
                pcm[i] = (f1 + f2) * std::sin(t * 3.14159f);
            }
            addBuiltin("26. Speech: POWER UP", pcm);
        }

        // 26 (27). Retro Laser Zapper
        {
            std::vector<float> pcm(2600);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 1500.0f * std::exp(-t * 6.5f) + 75.0f;
                ph += f / 44100.0f;
                pcm[i] = ((std::fmod(ph, 1.0f) > 0.5f) ? 0.85f : -0.85f) * std::exp(-t * 2.5f);
            }
            addBuiltin("27. Retro Laser Zapper", pcm);
        }

        // 27 (28). Coin / 1-Up Chime (B5 -> E6)
        {
            std::vector<float> pcm(4400);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = (t < 0.22f) ? 987.77f : 1318.51f; // B5 -> E6
                ph += f / 44100.0f;
                float sq = (std::fmod(ph, 1.0f) < 0.5f) ? 0.8f : -0.8f;
                float env = (t < 0.22f) ? 0.9f : std::exp(-(t - 0.22f) * 3.2f);
                pcm[i] = sq * env;
            }
            addBuiltin("28. Coin Chime (B5-E6)", pcm);
        }

        // 28 (29). Jump Spring Boing
        {
            std::vector<float> pcm(3800);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 160.0f * std::pow(4.2f, t) + 25.0f * std::sin(t * 50.0f);
                ph += f / 44100.0f;
                pcm[i] = ((std::fmod(ph, 1.0f) < 0.25f) ? 0.9f : -0.45f) * (1.0f - t * 0.85f);
            }
            addBuiltin("29. Jump Spring Boing", pcm);
        }

        // 29 (30). Boss Explosion Rumble
        {
            std::vector<float> pcm(7200);
            uint32_t seed = 0x99999;
            float lp = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                if ((i % 6) == 0)
                    seed = seed * 1664525u + 1013904223u;
                float nz = static_cast<float>(static_cast<int32_t>(seed)) / 2147483648.0f;
                lp += 0.18f * (nz - lp);
                pcm[i] = std::tanh(lp * 2.5f) * std::exp(-t * 2.4f);
            }
            addBuiltin("30. Boss Explosion Rumble", pcm);
        }

        // 30 (31). UFO Warp Siren
        {
            std::vector<float> pcm(4200);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                float f = 640.0f + 320.0f * std::sin(t * 37.7f);
                ph += f / 44100.0f;
                pcm[i] = std::sin(ph * kTwoPi) * std::exp(-t * 2.2f);
            }
            addBuiltin("31. UFO Warp Siren", pcm);
        }

        // 31 (32). Power-Down Glitch
        {
            std::vector<float> pcm(3800);
            float ph = 0.0f;
            for (size_t i = 0; i < pcm.size(); ++i)
            {
                float t = static_cast<float>(i) / static_cast<float>(pcm.size());
                int step = static_cast<int>(t * 8.0f);
                float f = 520.0f * std::pow(0.78f, static_cast<float>(step));
                ph += f / 44100.0f;
                pcm[i] = ((std::fmod(ph, 1.0f) < 0.125f) ? 0.9f : -0.35f) * (1.0f - t * 0.7f);
            }
            addBuiltin("32. Power-Down Glitch", pcm);
        }

        if (!builtinSamples_.empty())
            selectBuiltinSample(0);
    }

    void addBuiltin(const std::string& name, const std::vector<float>& pcm)
    {
        BuiltinSample bs;
        bs.name = name;
        bs.pcmPreview = pcm;

        int curDac = 64;
        uint8_t curByte = 0;
        int curBit = 0;

        for (float val : pcm)
        {
            int target = std::clamp(static_cast<int>((val + 1.0f) * 63.5f), 0, 126);
            int bit = (target > curDac) ? 1 : 0;
            if (bit == 1) curDac = std::min(126, curDac + 2);
            else curDac = std::max(0, curDac - 2);

            bs.deltaPreview.push_back((static_cast<float>(curDac) / 63.5f) - 1.0f);

            curByte |= (bit << curBit);
            curBit++;
            if (curBit == 8)
            {
                bs.data.push_back(curByte);
                curByte = 0;
                curBit = 0;
            }
        }
        if (curBit > 0)
            bs.data.push_back(curByte);

        builtinSamples_.push_back(std::move(bs));
    }

    float sampleRate_{ 44100.0f };
    int rateIndex_{ 15 };
    float basePlaybackFreqHz_{ 33144.0f };

    int deltaStep_{ 2 };
    int volume_{ 15 };
    float velToSlewAmount_{ 0.0f };
    bool chromaticTracking_{ false };
    int rootNote_{ 48 };

    bool loop_{ false };
    bool reverse_{ false };
    bool enabled_{ true };

    int throughDpcDac_{ 64 };
    bool throughDpcmEnabled_{ false };
    bool demoBeatEnabled_{ false };
    float demoBeatPhase_{ 0.0f };
    int demoBeatStep_{ 0 };
    std::array<float, 8> padActivity_{};

    std::array<SubVoice, 4> voices_;
    size_t rrIndex_{ 0 };

    int activeSampleIndex_{ 0 };
    std::vector<BuiltinSample> builtinSamples_;
    std::vector<float> originalPcmWaveform_;
    std::vector<float> reconstructedWaveform_;
};

} // namespace KindnesDSP
