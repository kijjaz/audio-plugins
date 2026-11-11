#include "PluginProcessor.h"
#include "PluginEditor.h"

KindnesAudioProcessor::KindnesAudioProcessor()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    loadPreset(0);
}

KindnesAudioProcessor::~KindnesAudioProcessor() = default;

const juce::String KindnesAudioProcessor::getName() const { return "Kindnes"; }
bool KindnesAudioProcessor::acceptsMidi() const { return true; }
bool KindnesAudioProcessor::producesMidi() const { return false; }
bool KindnesAudioProcessor::isMidiEffect() const { return false; }
double KindnesAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int KindnesAudioProcessor::getNumPrograms()
{
    return static_cast<int>(KindnesAudio::getFactoryPresets().size());
}

int KindnesAudioProcessor::getCurrentProgram() { return currentProgram_; }

void KindnesAudioProcessor::setCurrentProgram(int index)
{
    loadPreset(index);
}

const juce::String KindnesAudioProcessor::getProgramName(int index)
{
    const auto& presets = KindnesAudio::getFactoryPresets();
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[static_cast<size_t>(index)].name;
    return {};
}

void KindnesAudioProcessor::changeProgramName(int, const juce::String&) {}

void KindnesAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    engine_.setSampleRate(sampleRate);
    engine_.reset();
}

void KindnesAudioProcessor::releaseResources()
{
    engine_.reset();
}

bool KindnesAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::disabled()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void KindnesAudioProcessor::updateEngineParameters()
{
    auto playMode = static_cast<int>(*apvts_.getRawParameterValue("playMode"));
    engine_.setPlayMode(static_cast<KindnesDSP::KindnesEngine::PlayMode>(playMode));

    engine_.setAuthenticDAC(*apvts_.getRawParameterValue("authenticDac") > 0.5f);
    engine_.setAnalogFilterEnabled(*apvts_.getRawParameterValue("analogFilter") > 0.5f);
    engine_.setFilterTone(*apvts_.getRawParameterValue("filterTone"));

    // Resonant Filter
    int fMode = static_cast<int>(*apvts_.getRawParameterValue("filterMode"));
    float fCutoff = *apvts_.getRawParameterValue("cutoff");
    float fReso = *apvts_.getRawParameterValue("resonance");
    engine_.setSvFilterParameters(fMode, fCutoff, fReso);

    // ADSR Envelope
    bool useAdsr = *apvts_.getRawParameterValue("useAdsr") > 0.5f;
    float att = *apvts_.getRawParameterValue("attack");
    float dec = *apvts_.getRawParameterValue("decay");
    float sus = *apvts_.getRawParameterValue("sustain");
    float rel = *apvts_.getRawParameterValue("release");
    engine_.setAdsrParameters(useAdsr, att, dec, sus, rel);

    // Modulation & Glide
    float lfoRate = *apvts_.getRawParameterValue("lfoRate");
    float vibrato = *apvts_.getRawParameterValue("vibrato");
    float pwmDepth = *apvts_.getRawParameterValue("lfoPwm");
    engine_.setLfo1(lfoRate, vibrato, pwmDepth);
    engine_.setGlideTime(*apvts_.getRawParameterValue("glide"));

    // Channels
    engine_.setPulse1Duty(static_cast<int>(*apvts_.getRawParameterValue("pulse1Duty")));
    engine_.setPulse2Duty(static_cast<int>(*apvts_.getRawParameterValue("pulse2Duty")));
    engine_.setPulseContinuousPWM(*apvts_.getRawParameterValue("continuousPWM"));

    engine_.setPulse1Volume(static_cast<int>(*apvts_.getRawParameterValue("pulse1Vol")));
    engine_.setPulse2Volume(static_cast<int>(*apvts_.getRawParameterValue("pulse2Vol")));
    engine_.setTriangleVolume(static_cast<int>(*apvts_.getRawParameterValue("triVol")));
    engine_.setNoiseVolume(static_cast<int>(*apvts_.getRawParameterValue("noiseVol")));

    engine_.setNoiseMode(static_cast<int>(*apvts_.getRawParameterValue("noiseMode")));
    engine_.setNoisePeriod(static_cast<int>(*apvts_.getRawParameterValue("noisePeriod")));

    // DPCM Sampler
    int dpSample = static_cast<int>(*apvts_.getRawParameterValue("dpcmSample"));
    bool dpChrom = *apvts_.getRawParameterValue("dpcmChromatic") > 0.5f;
    int dpStep = static_cast<int>(*apvts_.getRawParameterValue("dpcmDeltaStep"));
    int dpRate = static_cast<int>(*apvts_.getRawParameterValue("dpcmRate"));
    bool dpLoop = *apvts_.getRawParameterValue("dpcmLoop") > 0.5f;
    int dpVol = static_cast<int>(*apvts_.getRawParameterValue("dpcmVol"));
    engine_.setDPCMParameters(dpSample, dpChrom, dpStep, dpRate, dpLoop, false, dpVol);

    // Arpeggiator
    bool arpEn = *apvts_.getRawParameterValue("arpEnabled") > 0.5f;
    engine_.setArpEnabled(arpEn);
    auto arpRate = static_cast<int>(*apvts_.getRawParameterValue("arpRate"));
    double currentBpm = 120.0;
    if (auto* playHead = getPlayHead())
    {
        if (auto pos = playHead->getPosition())
        {
            if (pos->getBpm())
                currentBpm = *pos->getBpm();
        }
    }
    engine_.setArpRateMode(static_cast<KindnesDSP::ArpeggiatorMacro::RateMode>(arpRate), currentBpm);

    auto arpPattern = static_cast<int>(*apvts_.getRawParameterValue("arpPattern"));
    switch (arpPattern)
    {
        case 0: // Major
            engine_.setArpPattern(0, { 0, 4, 7, 12 }, { 2, 1, 0, 1 }, { 15, 14, 15, 13 });
            break;
        case 1: // Minor
            engine_.setArpPattern(1, { 0, 3, 7, 12 }, { 1, 2, 0, 2 }, { 15, 14, 15, 13 });
            break;
        case 2: // Octaves
            engine_.setArpPattern(2, { 0, 12, 24, 12 }, { 2, 2, 1, 1 }, { 15, 15, 14, 14 });
            break;
        case 3: // Sus4
            engine_.setArpPattern(3, { 0, 5, 7, 12 }, { 2, 1, 2, 0 }, { 15, 14, 15, 13 });
            break;
        case 4: // Fifth
            engine_.setArpPattern(4, { 0, 7, 12, 7 }, { 2, 0, 2, 0 }, { 15, 13, 15, 13 });
            break;
        case 5: // Chugg
            engine_.setArpPattern(5, { 0, 12, 0, 12, 3, 12, 7, 12 },
                                  { 2, 0, 2, 0, 1, 0, 2, 0 },
                                  { 15, 12, 15, 12, 15, 12, 15, 12 });
            break;
        case 6: // Custom
        default:
        {
            auto steps = KindnesDSP::parseArpPattern(customArpPattern_.toStdString());
            engine_.setArpPattern(6, steps);
            break;
        }
    }

    // VRC6
    engine_.setVRC6Enabled(*apvts_.getRawParameterValue("vrc6Enabled") > 0.5f);
    engine_.setVRC6SawVolume(static_cast<int>(*apvts_.getRawParameterValue("vrc6SawVol")));

    // Unison & Stereo
    engine_.setUnisonDetune(*apvts_.getRawParameterValue("unisonDetune"));
    engine_.setStereoSpread(*apvts_.getRawParameterValue("stereoSpread"));

    // Embellishments
    float scoopSemi = *apvts_.getRawParameterValue("scoopSemi");
    float scoopTime = *apvts_.getRawParameterValue("scoopTime");
    engine_.setScoopParameters(scoopSemi, scoopTime);

    float vibDelay = *apvts_.getRawParameterValue("delayedVibDelay");
    float vibDepth = *apvts_.getRawParameterValue("delayedVibDepth");
    engine_.setDelayedVibrato(vibDelay, 200.0f, vibDepth, 5.5f);

    bool dutyMorph = *apvts_.getRawParameterValue("dutyMorph") > 0.5f;
    engine_.setDutyMorph(dutyMorph, 0, 2, 3);

    bool pseudoEcho = *apvts_.getRawParameterValue("pseudoEcho") > 0.5f;
    int echoDelay = static_cast<int>(*apvts_.getRawParameterValue("pseudoEchoDelay"));
    float echoVol = *apvts_.getRawParameterValue("pseudoEchoVol");
    engine_.setPseudoEcho(pseudoEcho, echoDelay, echoVol, 0);

    // 1-Bit DPCM Expansions
    bool drumPads = *apvts_.getRawParameterValue("dpcmDrumPads") > 0.5f;
    engine_.setDpcmDrumPadMode(drumPads);
    float velToSlew = *apvts_.getRawParameterValue("dpcmVelToSlew");
    engine_.setDpcmVelToSlew(velToSlew);
    bool throughDpcm = *apvts_.getRawParameterValue("throughDpcm") > 0.5f;
    engine_.setThroughDPCMEnabled(throughDpcm);

    // FDS 2C33 Wavetable & FM
    bool fdsEn = *apvts_.getRawParameterValue("fdsEnabled") > 0.5f;
    engine_.setFDSEnabled(fdsEn);
    int fdsWave = static_cast<int>(*apvts_.getRawParameterValue("fdsWave"));
    engine_.setFDSPresetWave(fdsWave);
    int fdsVol = static_cast<int>(*apvts_.getRawParameterValue("fdsVol"));
    engine_.setFDSVolume(fdsVol);
    float fdsSpeed = *apvts_.getRawParameterValue("fdsModSpeed");
    float fdsDepth = *apvts_.getRawParameterValue("fdsModDepth");
    engine_.setFDSModulation(fdsSpeed, fdsDepth);
    int fdsModTable = static_cast<int>(*apvts_.getRawParameterValue("fdsModWave"));
    engine_.setFDSPresetMod(fdsModTable);
    bool fdsStepped = *apvts_.getRawParameterValue("fdsSteppedDac") > 0.5f;
    engine_.setFDSSteppedDAC(fdsStepped);

    // Advanced Algorithmic Drum Designer
    KindnesDSP::KindnesEngine::CustomDrumParams cDrum;
    cDrum.designMode = static_cast<int>(*apvts_.getRawParameterValue("drumDesignMode"));
    cDrum.templateId = static_cast<int>(*apvts_.getRawParameterValue("drumTemplate"));
    cDrum.midiMode = static_cast<int>(*apvts_.getRawParameterValue("drumMidiMode"));
    cDrum.macroPunch = *apvts_.getRawParameterValue("drumMacroPunch");
    cDrum.macroBody = *apvts_.getRawParameterValue("drumMacroBody");
    cDrum.macroSnappy = *apvts_.getRawParameterValue("drumMacroSnappy");
    cDrum.macroGrit = *apvts_.getRawParameterValue("drumMacroGrit");

    cDrum.pitchCurve = static_cast<int>(*apvts_.getRawParameterValue("drumPitchCurve"));
    cDrum.pitchTimeMs = *apvts_.getRawParameterValue("drumPitchTimeMs");
    cDrum.fmRateHz = *apvts_.getRawParameterValue("drumFmRateHz");
    cDrum.fmDepthHz = *apvts_.getRawParameterValue("drumFmDepthHz");

    cDrum.bodyStartPitchHz = *apvts_.getRawParameterValue("drumBodyStartPitch");
    cDrum.bodyEndPitchHz = *apvts_.getRawParameterValue("drumBodyEndPitch");
    cDrum.bodyDecayMs = *apvts_.getRawParameterValue("drumBodyDecayMs");
    cDrum.bodyWaveform = static_cast<int>(*apvts_.getRawParameterValue("drumBodyWave"));
    cDrum.bodyLevel = *apvts_.getRawParameterValue("drumBodyLevel");

    cDrum.clickType = static_cast<int>(*apvts_.getRawParameterValue("drumClickType"));
    cDrum.clickLevel = *apvts_.getRawParameterValue("drumClickLevel");

    cDrum.noiseMode = static_cast<int>(*apvts_.getRawParameterValue("drumNoiseMode"));
    cDrum.noisePeriod = static_cast<int>(*apvts_.getRawParameterValue("drumNoisePeriod"));
    cDrum.noiseDecayMs = *apvts_.getRawParameterValue("drumNoiseDecayMs");
    cDrum.noisePattern = static_cast<int>(*apvts_.getRawParameterValue("drumNoisePattern"));
    cDrum.noiseLevel = *apvts_.getRawParameterValue("drumNoiseLevel");

    cDrum.dpcmDeltaStep = static_cast<int>(*apvts_.getRawParameterValue("drumDpcmDelta"));
    cDrum.dpcmRateIndex = static_cast<int>(*apvts_.getRawParameterValue("drumDpcmRate"));
    cDrum.dpcmGrit = *apvts_.getRawParameterValue("drumDpcmGrit");
    cDrum.dpcmBlend = *apvts_.getRawParameterValue("drumDpcmBlend");
    cDrum.dpcmSampleVoice = static_cast<int>(*apvts_.getRawParameterValue("drumDpcmVoice"));
    engine_.setCustomDrumParams(cDrum);
}

void KindnesAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateEngineParameters();

    int numSamples = buffer.getNumSamples();
    float* channelL = buffer.getWritePointer(0);
    float* channelR = (totalNumOutputChannels > 1) ? buffer.getWritePointer(1) : channelL;

    const float* inL = (totalNumInputChannels > 0) ? buffer.getReadPointer(0) : nullptr;
    const float* inR = (totalNumInputChannels > 1) ? buffer.getReadPointer(1) : inL;

    // Sample-accurate sub-block MIDI slicing to eliminate dropped/missing notes
    int timePos = 0;
    for (const auto metadata : midiMessages)
    {
        const int msgSample = std::clamp(metadata.samplePosition, 0, numSamples);
        const int samplesToProcess = msgSample - timePos;

        if (samplesToProcess > 0)
        {
            const float* subInL = (inL != nullptr) ? (inL + timePos) : nullptr;
            const float* subInR = (inR != nullptr) ? (inR + timePos) : nullptr;
            engine_.processBlock(subInL, subInR, channelL + timePos, channelR + timePos, samplesToProcess);
            timePos = msgSample;
        }

        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            engine_.noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
        }
        else if (msg.isNoteOff())
        {
            engine_.noteOff(msg.getNoteNumber());
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            engine_.allNotesOff();
        }
        else if (msg.isPitchWheel())
        {
            float semitones = (static_cast<float>(msg.getPitchWheelValue() - 8192) / 8192.0f) * 2.0f;
            engine_.setPitchBend(semitones);
        }
        else if (msg.isController())
        {
            if (msg.getControllerNumber() == 64) // Sustain pedal
            {
                engine_.setSustainPedal(msg.getControllerValue() >= 64);
            }
            else if (msg.getControllerNumber() == 120 || msg.getControllerNumber() == 123)
            {
                engine_.allNotesOff();
            }
        }
    }

    if (timePos < numSamples)
    {
        const int remaining = numSamples - timePos;
        const float* subInL = (inL != nullptr) ? (inL + timePos) : nullptr;
        const float* subInR = (inR != nullptr) ? (inR + timePos) : nullptr;
        engine_.processBlock(subInL, subInR, channelL + timePos, channelR + timePos, remaining);
    }

    // Master Trim (default 0.0 dB)
    float trimDb = *apvts_.getRawParameterValue("masterTrim");
    float trimLinear = juce::Decibels::decibelsToGain(trimDb);

    if (std::abs(trimLinear - 1.0f) > 0.001f)
    {
        buffer.applyGain(trimLinear);
    }

    if (auto* editor = dynamic_cast<KindnesAudioProcessorEditor*>(getActiveEditor()))
    {
        editor->pushScopeSamples(channelL, numSamples);
    }
}

void KindnesAudioProcessor::loadPreset(int index)
{
    const auto& presets = KindnesAudio::getFactoryPresets();
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return;

    currentProgram_ = index;
    const auto& p = presets[static_cast<size_t>(index)];

    auto setParam = [this](const juce::String& paramId, float value) {
        if (auto* param = apvts_.getParameter(paramId))
        {
            float norm = param->convertTo0to1(value);
            param->setValueNotifyingHost(norm);
        }
    };

    setParam("playMode", static_cast<float>(p.playMode));
    setParam("authenticDac", p.authenticDAC ? 1.0f : 0.0f);
    setParam("analogFilter", p.analogFilter ? 1.0f : 0.0f);
    setParam("filterTone", p.filterTone);

    setParam("filterMode", static_cast<float>(p.filterMode));
    setParam("cutoff", p.cutoffHz);
    setParam("resonance", p.resonance);

    setParam("useAdsr", p.useAdsr ? 1.0f : 0.0f);
    setParam("attack", p.attackMs);
    setParam("decay", p.decayMs);
    setParam("sustain", p.sustainLevel);
    setParam("release", p.releaseMs);

    setParam("lfoRate", p.lfoRateHz);
    setParam("vibrato", p.vibratoCents);
    setParam("lfoPwm", p.lfoPwmDepth);
    setParam("glide", p.glideMs);

    setParam("pulse1Duty", static_cast<float>(p.pulse1Duty));
    setParam("pulse2Duty", static_cast<float>(p.pulse2Duty));
    setParam("continuousPWM", p.continuousPWM);

    setParam("pulse1Vol", static_cast<float>(p.pulse1Vol));
    setParam("pulse2Vol", static_cast<float>(p.pulse2Vol));
    setParam("triVol", static_cast<float>(p.triVol));
    setParam("noiseVol", static_cast<float>(p.noiseVol));
    setParam("noiseMode", static_cast<float>(p.noiseMode));
    setParam("noisePeriod", static_cast<float>(p.noisePeriod));

    setParam("dpcmSample", static_cast<float>(p.dpcmSample));
    setParam("dpcmChromatic", p.dpcmChromatic ? 1.0f : 0.0f);
    setParam("dpcmDeltaStep", static_cast<float>(p.dpcmDeltaStep));
    setParam("dpcmRate", static_cast<float>(p.dpcmRateIndex));
    setParam("dpcmVol", static_cast<float>(p.dpcmVol));
    setParam("dpcmDrumPads", p.dpcmDrumPads ? 1.0f : 0.0f);
    setParam("dpcmVelToSlew", p.dpcmVelToSlew);
    setParam("throughDpcm", p.throughDpcm ? 1.0f : 0.0f);

    setParam("arpEnabled", p.arpEnabled ? 1.0f : 0.0f);
    setParam("arpRate", static_cast<float>(p.arpRateMode));
    setParam("arpPattern", static_cast<float>(p.arpPattern));
    customArpPattern_ = juce::String(KindnesDSP::formatArpPattern(p.arpSteps));

    setParam("vrc6Enabled", p.vrc6Enabled ? 1.0f : 0.0f);
    setParam("vrc6SawVol", static_cast<float>(p.vrc6SawVol));

    setParam("fdsEnabled", p.fdsEnabled ? 1.0f : 0.0f);
    setParam("fdsWave", static_cast<float>(p.fdsWave));
    setParam("fdsVol", static_cast<float>(p.fdsVol));
    setParam("fdsModSpeed", p.fdsModSpeed);
    setParam("fdsModDepth", p.fdsModDepth);
    setParam("fdsModWave", static_cast<float>(p.fdsModWave));
    setParam("fdsSteppedDac", p.fdsSteppedDac ? 1.0f : 0.0f);

    setParam("scoopSemi", p.scoopSemi);
    setParam("scoopTime", p.scoopTime);
    setParam("delayedVibDelay", p.delayedVibDelay);
    setParam("delayedVibDepth", p.delayedVibDepth);
    setParam("dutyMorph", p.dutyMorph ? 1.0f : 0.0f);
    setParam("pseudoEcho", p.pseudoEcho ? 1.0f : 0.0f);
    setParam("pseudoEchoDelay", static_cast<float>(p.pseudoEchoDelay));
    setParam("pseudoEchoVol", p.pseudoEchoVol);

    setParam("unisonDetune", p.unisonDetune);
    setParam("stereoSpread", p.stereoSpread);

    // MANDATORY STANDARD: Master trim strictly 0.0 dB
    setParam("masterTrim", 0.0f);
    setParam("drumMidiMode", 0.0f);

    // Sync DSP engine immediately
    engine_.getDPCM().setDemoBeatEnabled(false);
    updateEngineParameters();
}

juce::AudioProcessorValueTreeState::ParameterLayout KindnesAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Modes & Switches
    params.push_back(std::make_unique<juce::AudioParameterChoice>("playMode", "Play Mode",
        juce::StringArray{ "Classic APU", "Polyphonic", "Unison Stereo" }, 1));

    params.push_back(std::make_unique<juce::AudioParameterBool>("authenticDac", "Authentic 2A03 DAC", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("analogFilter", "Motherboard Filter", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("filterTone", "Filter Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // Modern Resonant SVF
    params.push_back(std::make_unique<juce::AudioParameterChoice>("filterMode", "Filter Type",
        juce::StringArray{ "LowPass", "HighPass", "BandPass", "Notch", "Bypass" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("cutoff", "Cutoff Hz",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.35f), 18000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("resonance", "Resonance Q",
        juce::NormalisableRange<float>(0.1f, 8.0f, 0.01f), 0.707f));

    // ADSR Envelope
    params.push_back(std::make_unique<juce::AudioParameterBool>("useAdsr", "ADSR Envelope", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("attack", "Attack",
        juce::NormalisableRange<float>(0.5f, 1000.0f, 0.1f, 0.35f), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("decay", "Decay",
        juce::NormalisableRange<float>(1.0f, 2000.0f, 1.0f, 0.35f), 150.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("release", "Release",
        juce::NormalisableRange<float>(1.0f, 3000.0f, 1.0f, 0.35f), 200.0f));

    // Modulation & Glide
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lfoRate", "LFO Speed",
        juce::NormalisableRange<float>(0.1f, 20.0f, 0.1f, 0.5f), 5.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("vibrato", "Vibrato Depth",
        juce::NormalisableRange<float>(0.0f, 50.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lfoPwm", "LFO PWM Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("glide", "Portamento Glide",
        juce::NormalisableRange<float>(0.0f, 250.0f), 0.0f));

    // Pulse 1 & 2
    params.push_back(std::make_unique<juce::AudioParameterChoice>("pulse1Duty", "Pulse 1 Duty",
        juce::StringArray{ "12.5%", "25.0%", "50.0%", "75.0%", "Continuous PWM" }, 2));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("pulse2Duty", "Pulse 2 Duty",
        juce::StringArray{ "12.5%", "25.0%", "50.0%", "75.0%", "Continuous PWM" }, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("continuousPWM", "Continuous PWM",
        juce::NormalisableRange<float>(0.01f, 0.99f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterInt>("pulse1Vol", "Pulse 1 Vol", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterInt>("pulse2Vol", "Pulse 2 Vol", 0, 15, 12));
    params.push_back(std::make_unique<juce::AudioParameterInt>("triVol", "Triangle Vol", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterInt>("noiseVol", "Noise Vol", 0, 15, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>("noiseMode", "Noise Mode",
        juce::StringArray{ "White (Mode 0)", "Metallic (Mode 1)" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>("noisePeriod", "Noise Period", 0, 15, 0));

    // DPCM Sampler & 32-Voice Drum ROM
    params.push_back(std::make_unique<juce::AudioParameterChoice>("dpcmSample", "DPCM Sample",
        juce::StringArray{
            "01. Punch Bass Drum", "02. Mega Crisp Snare", "03. Closed Hi-Hat", "04. Open Hi-Hat",
            "05. 8-Bit Hand Clap", "06. High Rack Tom", "07. Low Floor Tom", "08. Arcade Crash Cymbal",
            "09. VRC6 Overdriven Kick", "10. 93-Step Metallic Snare", "11. Chiptune Rimshot", "12. 8-Bit Arcade Cowbell",
            "13. FDS 2C33 FM Simmons Tom", "14. Gated Boss Battle Snare", "15. Chiptune Shaker", "16. FDS Metallic Ride Bell",
            "17. 1-Bit Slap Bass (Pop)", "18. 1-Bit Slap Bass (Thumb)", "19. Orchestra Hit (Major)", "20. Orchestra Hit (Minor)",
            "21. Dramatic 1-Bit Timpani", "22. 2A03 Deep Sub-Boom Kick", "23. Slope-Overload Fuzz Snare", "24. 1-Bit Vinyl Scratch",
            "25. Speech: KINDNES", "26. Speech: POWER UP", "27. Retro Laser Zapper", "28. Coin Chime (B5-E6)",
            "29. Jump Spring Boing", "30. Boss Explosion Rumble", "31. UFO Warp Siren", "32. Power-Down Glitch",
            "33. User Sample"
        }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>("dpcmChromatic", "DPCM Chromatic Pitch", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>("dpcmDeltaStep", "DPCM Delta Slew", 1, 8, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>("dpcmRate", "DPCM Rate Index", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterBool>("dpcmLoop", "DPCM Loop", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>("dpcmVol", "DPCM Vol", 0, 15, 12));

    // Arpeggiator Macro
    params.push_back(std::make_unique<juce::AudioParameterBool>("arpEnabled", "Arp Enabled", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("arpRate", "Arp Rate",
        juce::StringArray{ "60Hz (NTSC)", "50Hz (PAL)", "1/16 Beat", "1/32 Beat", "1/64 Beat" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("arpPattern", "Arp Pattern",
        juce::StringArray{ "Major", "Minor", "Octaves", "Sus4", "Fifth", "Chug", "Custom" }, 0));

    // VRC6
    params.push_back(std::make_unique<juce::AudioParameterBool>("vrc6Enabled", "VRC6 Expansion", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>("vrc6SawVol", "VRC6 Saw Vol", 0, 15, 0));

    // Unison & Stereo
    params.push_back(std::make_unique<juce::AudioParameterFloat>("unisonDetune", "Unison Detune",
        juce::NormalisableRange<float>(0.0f, 30.0f), 6.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("stereoSpread", "Stereo Spread",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    // Chiptune Embellishments
    params.push_back(std::make_unique<juce::AudioParameterFloat>("scoopSemi", "Pitch Scoop",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("scoopTime", "Scoop Time Ms",
        juce::NormalisableRange<float>(5.0f, 150.0f, 1.0f), 50.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayedVibDelay", "Delayed Vib Delay",
        juce::NormalisableRange<float>(0.0f, 500.0f, 1.0f), 150.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("delayedVibDepth", "Delayed Vib Depth",
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("dutyMorph", "Duty Attack Morph", false));
    params.push_back(std::make_unique<juce::AudioParameterBool>("pseudoEcho", "Chiptune Pseudo-Echo", false));
    params.push_back(std::make_unique<juce::AudioParameterInt>("pseudoEchoDelay", "Echo Delay Frames", 1, 16, 4));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("pseudoEchoVol", "Echo Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // 1-Bit DPCM Sampler Expansion
    params.push_back(std::make_unique<juce::AudioParameterBool>("dpcmDrumPads", "DPCM Drum Pads Mode", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("dpcmVelToSlew", "Vel to Slew Grit",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("throughDpcm", "Real-Time Through DPCM", false));

    // FDS 2C33 Wavetable & FM
    params.push_back(std::make_unique<juce::AudioParameterBool>("fdsEnabled", "FDS 2C33 Enable", false));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("fdsWave", "FDS Wavetable",
        juce::StringArray{
            "01. Sine", "02. Triangle", "03. Sawtooth", "04. 25% Pulse",
            "05. FDS Organ", "06. FDS Bell", "07. Formant Aah", "08. Heroic Fantasy Lead"
        }, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>("fdsVol", "FDS Vol", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fdsModSpeed", "FDS FM Speed",
        juce::NormalisableRange<float>(0.1f, 50.0f, 0.1f), 6.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("fdsModDepth", "FDS FM Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("fdsModWave", "FDS FM Table",
        juce::StringArray{ "01. Vibrato", "02. Metallic FM", "03. Laser Sweep" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterBool>("fdsSteppedDac", "FDS Stepped DAC", true));

    // Master Trim (default strictly 0.0 dB)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("masterTrim", "Master Trim",
        juce::NormalisableRange<float>(-24.0f, 12.0f), 0.0f));

    // =========================================================================
    // Advanced Drum Sound Design Parameters (Easy Macros vs Flexible Deep)
    // =========================================================================
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumDesignMode", "Drum Design Mode",
        juce::StringArray{ "Easy (Macro)", "Flexible (Deep)" }, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumTemplate", "Drum Template",
        juce::StringArray{
            "Custom User Drum",
            "01. Punch Kick", "02. Mega Snare", "03. Closed Hi-Hat", "04. Open Hi-Hat",
            "05. 8-Bit Hand Clap", "06. High Rack Tom", "07. Low Floor Tom", "08. Arcade Crash Cymbal",
            "09. VRC6 Overdriven Kick", "10. 93-Step Metallic Snare", "11. Chiptune Rimshot", "12. 8-Bit Arcade Cowbell",
            "13. FDS FM Simmons Tom", "14. Gated Boss Battle Snare", "15. Laser Zapper FX",
            "16. Boss Defeat Cataclysm", "17. Comic Spring Boing"
        }, 1));

    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumMidiMode", "Drum MIDI Play Mode",
        juce::StringArray{ "Audition Button Only", "MIDI Note 36 (C1 Kick / D1 Snare)", "Tuned Chromatic Synth" }, 0));

    // Easy Macros (0.0 to 1.0)
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumMacroPunch", "Punch (Pitch Drop)",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.70f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumMacroBody", "Body Length (Decay)",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.50f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumMacroSnappy", "Snappy (Noise Burst)",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumMacroGrit", "1-Bit Grit (Slew Distortion)",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.20f));

    // Flexible / Deep Controls
    // Tonal Body & Pitch Curves
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumPitchCurve", "Pitch Modulation Curve",
        juce::StringArray{ "Exponential Drop", "Dual-Stage Click Dive", "Laser Linear Sweep", "Pitch Scoop Rise", "FM Chirp Modulation" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumPitchTimeMs", "Pitch Sweep Time",
        juce::NormalisableRange<float>(5.0f, 300.0f, 1.0f, 0.5f), 60.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumFmRateHz", "FM Modulation Rate",
        juce::NormalisableRange<float>(10.0f, 300.0f, 1.0f, 0.5f), 120.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumFmDepthHz", "FM Modulation Depth",
        juce::NormalisableRange<float>(0.0f, 400.0f, 1.0f, 0.5f), 150.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumBodyStartPitch", "Body Start Freq",
        juce::NormalisableRange<float>(30.0f, 800.0f, 1.0f, 0.5f), 215.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumBodyEndPitch", "Body End Freq",
        juce::NormalisableRange<float>(20.0f, 300.0f, 1.0f, 0.5f), 47.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumBodyDecayMs", "Body Decay Time",
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.4f), 120.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumBodyWave", "Body Waveform",
        juce::StringArray{ "2A03 4-Bit Triangle", "Saturated Sine", "VRC6 Saw Overdrive", "2A03 25% Pulse" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumBodyLevel", "Body Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.95f));

    // Transient Click Layer
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumClickType", "Transient Click Type",
        juce::StringArray{ "Off", "2A03 Impulse Spike", "Square Pop", "Noise Snap" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumClickLevel", "Click Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));

    // Noise Layer
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumNoiseMode", "Noise LFSR Mode",
        juce::StringArray{ "Mode 0: White Noise (32767-step)", "Mode 1: Metallic Timbre (93-step)" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterInt>("drumNoisePeriod", "Noise Pitch Period", 0, 15, 3));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumNoiseDecayMs", "Noise Decay Time",
        juce::NormalisableRange<float>(5.0f, 4000.0f, 1.0f, 0.4f), 80.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("drumNoisePattern", "Noise Envelope Pattern",
        juce::StringArray{ "Single Fast Burst", "Multi-Flam Clap (3 Bursts)", "Gated Snare Plateau", "Low-Rumble Swell (Explosion)", "Glitch Frame Stutter" }, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumNoiseLevel", "Noise Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.50f));

    // DPCM Hardware Crusher & Blend
    params.push_back(std::make_unique<juce::AudioParameterInt>("drumDpcmDelta", "DPCM Delta Step", 1, 8, 2));
    params.push_back(std::make_unique<juce::AudioParameterInt>("drumDpcmRate", "DPCM Rate Index", 0, 15, 15));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumDpcmGrit", "DPCM Slope Overload Grit",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.20f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drumDpcmBlend", "Synth vs DPCM Blend",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.50f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("drumDpcmVoice", "DPCM Paired ROM Voice", 0, 31, 0));

    return { params.begin(), params.end() };
}

void KindnesAudioProcessor::setCustomArpPatternString(const juce::String& pattern)
{
    customArpPattern_ = pattern.trim();
    if (auto* param = apvts_.getParameter("arpPattern"))
    {
        param->setValueNotifyingHost(param->convertTo0to1(6.0f));
    }
}

juce::String KindnesAudioProcessor::getCustomArpPatternString() const
{
    return customArpPattern_;
}

juce::String KindnesAudioProcessor::getArpPatternStringForCurrentSetting() const
{
    auto patternIdx = static_cast<int>(*apvts_.getRawParameterValue("arpPattern"));
    switch (patternIdx)
    {
        case 0: return "0, 4, 7, 12";
        case 1: return "0, 3, 7, 12";
        case 2: return "0, 12, 24, 12";
        case 3: return "0, 5, 7, 12";
        case 4: return "0, 7, 12, 7";
        case 5: return "0, 12, 0, 12, 3, 12, 7, 12";
        case 6:
        default: return customArpPattern_;
    }
}

void KindnesAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    state.setProperty("customArpPattern", customArpPattern_, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KindnesAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts_.state.getType()))
    {
        auto vt = juce::ValueTree::fromXml(*xmlState);
        apvts_.replaceState(vt);
        if (vt.hasProperty("customArpPattern"))
            customArpPattern_ = vt.getProperty("customArpPattern").toString();
    }
}

juce::AudioProcessorEditor* KindnesAudioProcessor::createEditor()
{
    return new KindnesAudioProcessorEditor(*this);
}

bool KindnesAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KindnesAudioProcessor();
}
