#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <cctype>
#include <cstdlib>
#include <algorithm>

namespace KindnesDSP
{

/**
 * Tracker-Style Frame Sequencer & Macro Arpeggiator.
 * 
 * Recreates the iconic 60 Hz / 50 Hz frame-rate modulation of vintage NES engines
 * (e.g., FamiTracker, Neil Baldwin's driver, Konami sound drivers):
 * - Pitch arpeggio sequence (semitone offsets, custom user patterns, or dynamic chord tracking)
 * - Rests support (represented by kRestOffset = -999)
 * - Duty cycle sequencing per tick (chiptune duty shimmer)
 * - Volume envelope macro
 * - Tempo sync (60 Hz NTSC, 50 Hz PAL, or DAW beat sync)
 */
class ArpeggiatorMacro
{
public:
    static constexpr int kRestOffset = -999;
    static constexpr int kFixedNoteBase = 1000; // Encodes fixed MIDI note n as (kFixedNoteBase + n)

    static inline bool isFixedNoteValue(int val) noexcept
    {
        return val >= kFixedNoteBase && val <= kFixedNoteBase + 127;
    }

    static inline int decodeFixedNoteValue(int val) noexcept
    {
        return std::clamp(val - kFixedNoteBase, 0, 127);
    }

    enum class RateMode
    {
        NTSC_60Hz = 0,
        PAL_50Hz = 1,
        HostSync_16th = 2,
        HostSync_32nd = 3,
        HostSync_64th = 4
    };

    ArpeggiatorMacro()
    {
        reset();
    }

    void reset() noexcept
    {
        stepIndex_ = 0;
        phaseAccumulator_ = 0.0f;
        active_ = false;
        loop_ = true;
        patternIndex_ = 0;
        heldNotes_.clear();

        // Default: Major arpeggio [0, 4, 7, 12] with duty shimmer
        pitchSequence_ = { 0, 4, 7, 12 };
        dutySequence_ = { 2, 1, 0, 1 }; // 50% -> 25% -> 12.5% -> 25%
        volumeSequence_ = { 15, 14, 15, 13 };
    }

    void setSampleRate(double sampleRate) noexcept
    {
        sampleRate_ = static_cast<float>(sampleRate);
        if (sampleRate_ < 8000.0f)
            sampleRate_ = 44100.0f;
    }

    void setRateMode(RateMode mode, double bpm = 120.0) noexcept
    {
        rateMode_ = mode;
        bpm_ = std::max(20.0, bpm);
    }

    void setPatternIndex(int pattern) noexcept
    {
        patternIndex_ = pattern;
    }

    [[nodiscard]] int getPatternIndex() const noexcept
    {
        return patternIndex_;
    }

    void setPitchSequence(const std::vector<int>& steps)
    {
        pitchSequence_ = steps;
        if (pitchSequence_.empty())
            pitchSequence_ = { 0 };
    }

    [[nodiscard]] const std::vector<int>& getPitchSequence() const noexcept
    {
        return pitchSequence_;
    }

    void setDutySequence(const std::vector<int>& duties)
    {
        dutySequence_ = duties;
        if (dutySequence_.empty())
            dutySequence_ = { 2 };
    }

    void setVolumeSequence(const std::vector<int>& volumes)
    {
        volumeSequence_ = volumes;
        if (volumeSequence_.empty())
            volumeSequence_ = { 15 };
    }

    void setPattern(int patternIndex,
                    const std::vector<int>& pitches,
                    const std::vector<int>& duties = {},
                    const std::vector<int>& volumes = {})
    {
        patternIndex_ = patternIndex;
        setPitchSequence(pitches);

        if (duties.empty())
        {
            std::vector<int> autoDuties(pitchSequence_.size());
            for (size_t i = 0; i < autoDuties.size(); ++i)
                autoDuties[i] = (i % 2 == 0) ? 2 : 1;
            setDutySequence(autoDuties);
        }
        else
        {
            setDutySequence(duties);
        }

        if (volumes.empty())
        {
            setVolumeSequence(std::vector<int>(pitchSequence_.size(), 15));
        }
        else
        {
            setVolumeSequence(volumes);
        }
    }

    void setHeldNotes(const std::vector<int>& notes)
    {
        heldNotes_ = notes;
    }

    void setEnabled(bool enabled) noexcept
    {
        active_ = enabled;
        if (!enabled)
            stepIndex_ = 0;
    }

    [[nodiscard]] bool isEnabled() const noexcept { return active_; }

    void trigger() noexcept
    {
        stepIndex_ = 0;
        phaseAccumulator_ = 0.0f;
    }

    [[nodiscard]] bool isCurrentStepRest() const noexcept
    {
        if (!active_ || pitchSequence_.empty())
            return false;
        return pitchSequence_[stepIndex_ % pitchSequence_.size()] == kRestOffset;
    }

    [[nodiscard]] int getCurrentPitchOffset() const noexcept
    {
        if (!active_)
            return 0;

        // If the pattern step itself is explicitly a rest, output rest
        if (!pitchSequence_.empty() && pitchSequence_[stepIndex_ % pitchSequence_.size()] == kRestOffset)
            return kRestOffset;

        // If multiple keys are held, dynamically arpeggiate across the chord notes!
        if (heldNotes_.size() > 1)
        {
            std::vector<int> sorted = heldNotes_;
            std::sort(sorted.begin(), sorted.end());
            int root = sorted.front();
            size_t count = sorted.size();

            if (patternIndex_ == 2) // Octaves mode across chord
            {
                size_t totalSteps = count * 2;
                size_t step = stepIndex_ % totalSteps;
                if (step < count)
                    return sorted[step] - root;
                else
                    return (sorted[step - count] - root) + 12;
            }
            else if (patternIndex_ == 5) // Chug mode: alternate root and chord tones
            {
                if ((stepIndex_ % 2) == 0)
                    return 0; // Root
                else
                {
                    size_t otherIndex = 1 + ((stepIndex_ / 2) % (count - 1));
                    return sorted[otherIndex] - root;
                }
            }
            else
            {
                // Ascending chord cycle
                size_t step = stepIndex_ % count;
                return sorted[step] - root;
            }
        }

        // Single note: tracker pitch macro sequence (supports loopPoint_ after transient steps)
        if (pitchSequence_.empty())
            return 0;
        return pitchSequence_[resolveSequenceIndex(stepIndex_, pitchSequence_.size(), loopPoint_)];
    }

    void setLoopPoint(int loopIdx) noexcept
    {
        loopPoint_ = loopIdx;
    }

    [[nodiscard]] int getLoopPoint() const noexcept { return loopPoint_; }

    [[nodiscard]] int getCurrentDuty() const noexcept
    {
        if (!active_ || dutySequence_.empty())
            return 2;
        return dutySequence_[stepIndex_ % dutySequence_.size()];
    }

    [[nodiscard]] int getCurrentVolume() const noexcept
    {
        if (!active_ || volumeSequence_.empty())
            return 15;
        if (isCurrentStepRest())
            return 0;
        return volumeSequence_[stepIndex_ % volumeSequence_.size()];
    }

    /**
     * Advances the macro sequencer by one audio sample.
     */
    void processSample() noexcept
    {
        if (!active_)
            return;

        float tickRateHz = 60.0f;
        switch (rateMode_)
        {
            case RateMode::NTSC_60Hz:
                tickRateHz = 60.0988f; // Exact NTSC frame rate
                break;
            case RateMode::PAL_50Hz:
                tickRateHz = 50.0070f; // Exact PAL frame rate
                break;
            case RateMode::HostSync_16th:
                tickRateHz = static_cast<float>(bpm_ * (4.0 / 60.0));
                break;
            case RateMode::HostSync_32nd:
                tickRateHz = static_cast<float>(bpm_ * (8.0 / 60.0));
                break;
            case RateMode::HostSync_64th:
                tickRateHz = static_cast<float>(bpm_ * (16.0 / 60.0));
                break;
        }

        phaseAccumulator_ += tickRateHz / sampleRate_;
        if (phaseAccumulator_ >= 1.0f)
        {
            phaseAccumulator_ -= 1.0f;
            stepIndex_++;
        }
    }

private:
    static inline size_t resolveSequenceIndex(size_t step, size_t len, int loopPt) noexcept
    {
        if (len == 0)
            return 0;
        if (step < len)
            return step;
        if (loopPt >= 0 && static_cast<size_t>(loopPt) < len)
        {
            size_t loopStart = static_cast<size_t>(loopPt);
            size_t loopSpan = len - loopStart;
            return loopStart + ((step - loopStart) % loopSpan);
        }
        return step % len;
    }

    float sampleRate_{ 44100.0f };
    RateMode rateMode_{ RateMode::NTSC_60Hz };
    double bpm_{ 120.0 };
    int patternIndex_{ 0 };
    int loopPoint_{ -1 };

    bool active_{ false };
    bool loop_{ true };
    size_t stepIndex_{ 0 };
    float phaseAccumulator_{ 0.0f };

    std::vector<int> heldNotes_;
    std::vector<int> pitchSequence_;
    std::vector<int> dutySequence_;
    std::vector<int> volumeSequence_;
};

/**
 * Flexible text parser for user-defined Arp patterns:
 * Supports integers (-12, 0, 4, 7, 12), note names (C, D#, Eb, G, C1),
 * and rests ('r', 'R', '-', '_', '.', 'rest', 'off', 'cut').
 */
inline std::vector<int> parseArpPattern(const std::string& input)
{
    std::vector<int> steps;
    std::string s = input;
    for (char& c : s)
    {
        if (c == ',' || c == ';' || c == '|' || c == '\t' || c == '\n' || c == '\r')
            c = ' ';
    }

    auto isRestToken = [](const std::string& tok) {
        if (tok == "r" || tok == "rest" || tok == "-" || tok == "_" || 
            tok == "." || tok == "x" || tok == "off" || tok == "cut")
            return true;
        return false;
    };

    auto parseNoteToken = [](const std::string& tok, int& outOffset) -> bool {
        // 1. Integer check
        char* endPtr = nullptr;
        long val = std::strtol(tok.c_str(), &endPtr, 10);
        if (endPtr != tok.c_str() && *endPtr == '\0')
        {
            outOffset = static_cast<int>(val);
            return true;
        }

        // 2. Note name check (e.g. C, C#, Db, D, Eb, E, F, F#, G, Ab, A, Bb, B)
        if (tok.empty()) return false;
        char noteLetter = static_cast<char>(std::toupper(tok[0]));
        int base = -1;
        switch (noteLetter)
        {
            case 'C': base = 0; break;
            case 'D': base = 2; break;
            case 'E': base = 4; break;
            case 'F': base = 5; break;
            case 'G': base = 7; break;
            case 'A': base = 9; break;
            case 'B': base = 11; break;
            default: return false;
        }

        size_t idx = 1;
        if (idx < tok.size())
        {
            if (tok[idx] == '#' || tok[idx] == '+') { base += 1; idx++; }
            else if (tok[idx] == 'b' || tok[idx] == '-') { base -= 1; idx++; }
        }

        if (idx < tok.size())
        {
            char* octEnd = nullptr;
            long oct = std::strtol(tok.c_str() + idx, &octEnd, 10);
            if (octEnd != tok.c_str() + idx)
                base += static_cast<int>(oct * 12);
        }

        outOffset = base;
        return true;
    };

    std::istringstream iss(s);
    std::string token;
    while (iss >> token)
    {
        std::string lower = token;
        for (char& c : lower) c = static_cast<char>(std::tolower(c));

        if (isRestToken(lower))
        {
            steps.push_back(ArpeggiatorMacro::kRestOffset);
        }
        else if (!token.empty() && token[0] == '@' && token.size() > 1)
        {
            // Dn-FamiTracker fixed-note step token (e.g. @60 or @36 for 1-channel drum/pluck transient)
            int fixedNote = std::clamp(std::atoi(token.c_str() + 1), 0, 127);
            steps.push_back(ArpeggiatorMacro::kFixedNoteBase + fixedNote);
        }
        else
        {
            int offset = 0;
            if (parseNoteToken(token, offset))
            {
                steps.push_back(offset);
            }
        }
    }

    if (steps.empty())
        steps.push_back(0);

    return steps;
}

inline std::string formatArpPattern(const std::vector<int>& steps)
{
    std::string out;
    for (size_t i = 0; i < steps.size(); ++i)
    {
        if (i > 0) out += ", ";
        if (steps[i] == ArpeggiatorMacro::kRestOffset)
            out += "r";
        else if (ArpeggiatorMacro::isFixedNoteValue(steps[i]))
            out += "@" + std::to_string(ArpeggiatorMacro::decodeFixedNoteValue(steps[i]));
        else
            out += std::to_string(steps[i]);
    }
    return out;
}

} // namespace KindnesDSP
