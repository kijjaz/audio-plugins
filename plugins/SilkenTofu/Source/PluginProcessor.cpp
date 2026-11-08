#include "PluginProcessor.h"
#include "PluginEditor.h"

SilkenTofuAudioProcessor::SilkenTofuAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withInput("Sidechain Key", juce::AudioChannelSet::stereo(), false)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    for (size_t i = 0; i < 4; ++i)
    {
        oversamplers_[i] = std::make_unique<juce::dsp::Oversampling<float>>(
            2, i, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true);
    }
    smStereoLink_.reset(80.0);
    applyPreset(0);
    abStates_[0] = apvts_.copyState();
    abStates_[1] = apvts_.copyState();
}

SilkenTofuAudioProcessor::~SilkenTofuAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout SilkenTofuAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addChannelBank = [&](const juce::String& suffix, const juce::String& namePrefix)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"peakReduction" + suffix, 1}, namePrefix + "Peak Reduction",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 42.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"gainDb" + suffix, 1}, namePrefix + "Makeup Gain",
            juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f), 1.5f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{"mode" + suffix, 1}, namePrefix + "Comp / Limit Mode",
            juce::StringArray{"Compress (3:1)", "Limit (inf:1)"}, 0));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"autoLevel" + suffix, 1}, namePrefix + "Auto-Level Link", true));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID{"crystalType" + suffix, 1}, namePrefix + "Crystal Chemistry",
            juce::StringArray{"Vintage CdS (LA-2A)", "CdSe Vactrol (LA-3A)", "Fast Silicon (FET-Opto)"}, 0));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"trapMemory" + suffix, 1}, namePrefix + "Trap Memory",
            juce::NormalisableRange<float>(-50.0f, 200.0f, 0.5f), 100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"topologyMorph" + suffix, 1}, namePrefix + "FB / FF Topology",
            juce::NormalisableRange<float>(0.0f, 125.0f, 0.5f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"punchHoldoff" + suffix, 1}, namePrefix + "Punch Holdoff",
            juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f), 2.5f,
            juce::AudioParameterFloatAttributes().withLabel("ms")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"biasGlow" + suffix, 1}, namePrefix + "T4B Pre-Warmth (Bias Glow)",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 20.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"cellAge" + suffix, 1}, namePrefix + "Phosphor Cell Age",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 15.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"tubeIronDrive" + suffix, 1}, namePrefix + "Tube & Iron Drive",
            juce::NormalisableRange<float>(-6.0f, 18.0f, 0.1f), 2.5f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"fatCathodeCap" + suffix, 1}, namePrefix + "V1 Fat Cathode Cap (Ck)", false));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"load600Ohm" + suffix, 1}, namePrefix + "600-Ohm Vintage Load", false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"bPlusSag" + suffix, 1}, namePrefix + "B+ Power Supply Sag",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 40.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"millerAir" + suffix, 1}, namePrefix + "Miller Impedance Air",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.5f), 15.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"r37Emphasis" + suffix, 1}, namePrefix + "R37 HF Emphasis",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 55.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"scHighPass" + suffix, 1}, namePrefix + "SC Warm-Cut HPF",
            juce::NormalisableRange<float>(20.0f, 350.0f, 1.0f, 0.5f), 65.0f,
            juce::AudioParameterFloatAttributes().withLabel("Hz")));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"extSidechain" + suffix, 1}, namePrefix + "External Key Ducking", false));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{"scListen" + suffix, 1}, namePrefix + "SC Listen (EL Drive)", false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"mix" + suffix, 1}, namePrefix + "Dry / Wet Mix",
            juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f,
            juce::AudioParameterFloatAttributes().withLabel("%")));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"trimDb" + suffix, 1}, namePrefix + "Master Trim",
            juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
            juce::AudioParameterFloatAttributes().withLabel("dB")));
    };

    // Bank 0: Main Stereo / MID Channel Parameters (preserves exact v0.0.2 parameter IDs)
    addChannelBank("", "");

    // Bank 1: Independent SIDE Channel Parameters (for Mid/Side Mastering & Sculpting)
    addChannelBank("_side", "Side ");

    // Global Oversampling, Stereo Link, Routing Mode, VU Meter Mode, M/S Solo & Soft Bypass
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"oversampling", 1}, "Oversampling",
        juce::StringArray{"1x (Off)", "2x Polyphase", "4x Polyphase", "8x Polyphase"}, 2));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"stereoLink", 1}, "Stereo / M-S Link",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 80.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"channelRouting", 1}, "Stereo / Mid-Side Routing",
        juce::StringArray{"Stereo (L/R)", "M/S: Edit MID", "M/S: Edit SIDE"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"vuMode", 1}, "VU Meter Mode",
        juce::StringArray{"GR (dB)", "Output VU", "Input VU"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"msSolo", 1}, "Solo Active M/S Channel", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"bypass", 1}, "Soft Bypass", false));

    return { params.begin(), params.end() };
}

const juce::String SilkenTofuAudioProcessor::getName() const { return JucePlugin_Name; }
bool SilkenTofuAudioProcessor::acceptsMidi() const { return false; }
bool SilkenTofuAudioProcessor::producesMidi() const { return false; }
bool SilkenTofuAudioProcessor::isMidiEffect() const { return false; }
double SilkenTofuAudioProcessor::getTailLengthSeconds() const { return 2.0; }

int SilkenTofuAudioProcessor::getNumPrograms()
{
    return static_cast<int>(SilkenTofuDSP::getFactoryPresets().size());
}

int SilkenTofuAudioProcessor::getCurrentProgram() { return currentPresetIndex_; }

void SilkenTofuAudioProcessor::setCurrentProgram(int index)
{
    applyPreset(index);
}

const juce::String SilkenTofuAudioProcessor::getProgramName(int index)
{
    const auto& presets = SilkenTofuDSP::getFactoryPresets();
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[static_cast<size_t>(index)].name;
    return {};
}

void SilkenTofuAudioProcessor::changeProgramName(int, const juce::String&) {}

int SilkenTofuAudioProcessor::getVuMode() const
{
    return std::clamp(static_cast<int>(apvts_.getRawParameterValue("vuMode")->load()), 0, 2);
}

void SilkenTofuAudioProcessor::setVuMode(int mode)
{
    if (auto* param = apvts_.getParameter("vuMode"))
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(std::clamp(mode, 0, 2))));
}

int SilkenTofuAudioProcessor::getChannelRoutingMode() const
{
    return std::clamp(static_cast<int>(apvts_.getRawParameterValue("channelRouting")->load()), 0, 2);
}

void SilkenTofuAudioProcessor::setChannelRoutingMode(int mode)
{
    if (auto* param = apvts_.getParameter("channelRouting"))
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(std::clamp(mode, 0, 2))));
}

void SilkenTofuAudioProcessor::setActiveABSlot(int slot)
{
    slot = std::clamp(slot, 0, 1);
    if (slot == activeABSlot_)
        return;

    abStates_[activeABSlot_]      = apvts_.copyState();
    abPresetIndex_[activeABSlot_] = currentPresetIndex_;
    activeABSlot_ = slot;
    currentPresetIndex_ = abPresetIndex_[activeABSlot_];
    if (abStates_[activeABSlot_].isValid())
        apvts_.replaceState(abStates_[activeABSlot_].createCopy());
}

void SilkenTofuAudioProcessor::copyActiveSlotToOther()
{
    abStates_[activeABSlot_]          = apvts_.copyState();
    abPresetIndex_[activeABSlot_]     = currentPresetIndex_;
    abStates_[1 - activeABSlot_]      = abStates_[activeABSlot_].createCopy();
    abPresetIndex_[1 - activeABSlot_] = currentPresetIndex_;
}

void SilkenTofuAudioProcessor::copyMidToSide()
{
    const auto midP = readChannelParameters(0);
    auto setSide = [this](const juce::String& id, float plainValue)
    {
        if (auto* param = apvts_.getParameter(id + "_side"))
            param->setValueNotifyingHost(param->convertTo0to1(plainValue));
    };

    setSide("peakReduction", midP.peakReduction);
    setSide("gainDb",        midP.gainDb);
    setSide("mode",          midP.limitMode ? 1.0f : 0.0f);
    setSide("autoLevel",     midP.autoLevel ? 1.0f : 0.0f);
    setSide("crystalType",   static_cast<float>(midP.crystalType));
    setSide("trapMemory",    midP.trapMemory);
    setSide("topologyMorph", midP.topologyMorph);
    setSide("punchHoldoff",  midP.punchHoldoffMs);
    setSide("biasGlow",      midP.biasGlow);
    setSide("cellAge",       midP.cellAge);
    setSide("tubeIronDrive", midP.tubeIronDriveDb);
    setSide("fatCathodeCap", midP.fatCathodeCap ? 1.0f : 0.0f);
    setSide("load600Ohm",    midP.load600Ohm ? 1.0f : 0.0f);
    setSide("bPlusSag",      midP.bPlusSag);
    setSide("millerAir",     midP.millerAir);
    setSide("r37Emphasis",   midP.r37Emphasis);
    setSide("scHighPass",    midP.scHighPassHz);
    setSide("extSidechain",  midP.extSidechain ? 1.0f : 0.0f);
    setSide("scListen",      midP.scListen ? 1.0f : 0.0f);
    setSide("mix",           midP.mix);
    setSide("trimDb",        midP.trimDb);
}

void SilkenTofuAudioProcessor::applyPreset(int presetIndex)
{
    const auto& presets = SilkenTofuDSP::getFactoryPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    currentPresetIndex_ = presetIndex;
    abPresetIndex_[activeABSlot_] = presetIndex;
    const auto& preset = presets[static_cast<size_t>(presetIndex)];

    auto applyBank = [this](const SilkenTofuDSP::Parameters& p, const juce::String& suffix)
    {
        auto setParam = [this, &suffix](const juce::String& id, float plainValue)
        {
            if (auto* param = apvts_.getParameter(id + suffix))
                param->setValueNotifyingHost(param->convertTo0to1(plainValue));
        };

        setParam("peakReduction", p.peakReduction);
        setParam("gainDb",        p.gainDb);
        setParam("mode",          p.limitMode ? 1.0f : 0.0f);
        setParam("autoLevel",     p.autoLevel ? 1.0f : 0.0f);

        setParam("crystalType",   static_cast<float>(p.crystalType));
        setParam("trapMemory",    p.trapMemory);
        setParam("topologyMorph", p.topologyMorph);
        setParam("punchHoldoff",  p.punchHoldoffMs);
        setParam("biasGlow",      p.biasGlow);
        setParam("cellAge",       p.cellAge);

        setParam("tubeIronDrive", p.tubeIronDriveDb);
        setParam("fatCathodeCap", p.fatCathodeCap ? 1.0f : 0.0f);
        setParam("load600Ohm",    p.load600Ohm ? 1.0f : 0.0f);
        setParam("bPlusSag",      p.bPlusSag);
        setParam("millerAir",     p.millerAir);

        setParam("r37Emphasis",   p.r37Emphasis);
        setParam("scHighPass",    p.scHighPassHz);
        setParam("extSidechain",  p.extSidechain ? 1.0f : 0.0f);
        setParam("scListen",      p.scListen ? 1.0f : 0.0f);

        setParam("mix",           p.mix);
        setParam("trimDb",        0.0f); // Strictly 0.0 dB Unity Trim on all factory presets (AGENTS.md Rule 2)
    };

    applyBank(preset.params, "");
    applyBank(preset.isMidSide ? preset.sideParams : preset.params, "_side");

    if (auto* linkParam = apvts_.getParameter("stereoLink"))
        linkParam->setValueNotifyingHost(linkParam->convertTo0to1(preset.stereoLink));

    if (auto* routeParam = apvts_.getParameter("channelRouting"))
        routeParam->setValueNotifyingHost(routeParam->convertTo0to1(preset.isMidSide ? 1.0f : 0.0f));

    if (auto* soloParam = apvts_.getParameter("msSolo"))
        soloParam->setValueNotifyingHost(0.0f);

    if (auto* bypassParam = apvts_.getParameter("bypass"))
        bypassParam->setValueNotifyingHost(0.0f);
}

SilkenTofuDSP::Parameters SilkenTofuAudioProcessor::readChannelParameters(int channelIndex) const
{
    const juce::String s = (channelIndex == 1) ? "_side" : "";
    SilkenTofuDSP::Parameters p;
    p.peakReduction  = apvts_.getRawParameterValue("peakReduction" + s)->load();
    p.gainDb         = apvts_.getRawParameterValue("gainDb" + s)->load();
    p.limitMode      = apvts_.getRawParameterValue("mode" + s)->load() > 0.5f;
    p.autoLevel      = apvts_.getRawParameterValue("autoLevel" + s)->load() > 0.5f;

    p.crystalType    = static_cast<int>(apvts_.getRawParameterValue("crystalType" + s)->load());
    p.trapMemory     = apvts_.getRawParameterValue("trapMemory" + s)->load();
    p.topologyMorph  = apvts_.getRawParameterValue("topologyMorph" + s)->load();
    p.punchHoldoffMs = apvts_.getRawParameterValue("punchHoldoff" + s)->load();
    p.biasGlow       = apvts_.getRawParameterValue("biasGlow" + s)->load();
    p.cellAge        = apvts_.getRawParameterValue("cellAge" + s)->load();

    p.tubeIronDriveDb= apvts_.getRawParameterValue("tubeIronDrive" + s)->load();
    p.fatCathodeCap  = apvts_.getRawParameterValue("fatCathodeCap" + s)->load() > 0.5f;
    p.load600Ohm     = apvts_.getRawParameterValue("load600Ohm" + s)->load() > 0.5f;
    p.bPlusSag       = apvts_.getRawParameterValue("bPlusSag" + s)->load();
    p.millerAir      = apvts_.getRawParameterValue("millerAir" + s)->load();

    p.r37Emphasis    = apvts_.getRawParameterValue("r37Emphasis" + s)->load();
    p.scHighPassHz   = apvts_.getRawParameterValue("scHighPass" + s)->load();
    p.extSidechain   = apvts_.getRawParameterValue("extSidechain" + s)->load() > 0.5f;
    p.scListen       = apvts_.getRawParameterValue("scListen" + s)->load() > 0.5f;

    p.mix            = apvts_.getRawParameterValue("mix" + s)->load();
    p.trimDb         = apvts_.getRawParameterValue("trimDb" + s)->load();
    p.bypass         = apvts_.getRawParameterValue("bypass")->load() > 0.5f;
    return p;
}

void SilkenTofuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate_ = std::max(22050.0, sampleRate);
    engines_[0].prepare(currentSampleRate_, -0.35f); // Left / Mid canister micro-tolerance
    engines_[1].prepare(currentSampleRate_, +0.35f); // Right / Side canister micro-tolerance

    const int maxBlock = std::max(64, samplesPerBlock);
    for (auto& os : oversamplers_)
    {
        os->initProcessing(static_cast<size_t>(maxBlock));
        os->reset();
    }

    const int osChoice = std::clamp(static_cast<int>(apvts_.getRawParameterValue("oversampling")->load()), 0, 3);
    lastReportedLatency_ = juce::roundToInt(oversamplers_[static_cast<size_t>(osChoice)]->getLatencyInSamples());
    setLatencySamples(lastReportedLatency_);

    smStereoLink_.reset(apvts_.getRawParameterValue("stereoLink")->load());

    t4bGainBuffer_.setSize(2, maxBlock);
    scMonitorBuffer_.setSize(2, maxBlock);
}

void SilkenTofuAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SilkenTofuAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto mainOut = layouts.getMainOutputChannelSet();
    const auto mainIn  = layouts.getMainInputChannelSet();
    if (mainOut != juce::AudioChannelSet::mono() && mainOut != juce::AudioChannelSet::stereo())
        return false;
    if (mainOut != mainIn)
        return false;
    return true;
}
#endif

void SilkenTofuAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto mainBus = getBusBuffer(buffer, true, 0);
    const bool scBusReady = (getBusCount(true) > 1 && getBus(true, 1) != nullptr &&
                             getBus(true, 1)->isEnabled() &&
                             buffer.getNumChannels() > mainBus.getNumChannels());
    auto scBus = scBusReady ? getBusBuffer(buffer, true, 1) : juce::AudioBuffer<float>();

    const int numChannels = std::min(2, mainBus.getNumChannels());
    const int numSamples  = mainBus.getNumSamples();
    if (numChannels == 0 || numSamples == 0)
        return;

    if (t4bGainBuffer_.getNumSamples() < numSamples)
    {
        t4bGainBuffer_.setSize(2, numSamples, false, false, true);
        scMonitorBuffer_.setSize(2, numSamples, false, false, true);
    }

    const int osChoice = std::clamp(static_cast<int>(apvts_.getRawParameterValue("oversampling")->load()), 0, 3);
    auto& os = *oversamplers_[static_cast<size_t>(osChoice)];
    const int currentLatency = juce::roundToInt(os.getLatencyInSamples());
    if (currentLatency != lastReportedLatency_)
    {
        lastReportedLatency_ = currentLatency;
        setLatencySamples(currentLatency);
    }

    const int routingMode = getChannelRoutingMode();
    const bool isMidSide  = (routingMode > 0 && numChannels == 2);

    // In Stereo (L/R) mode, both channels share Bank 0; in M/S mode, Ch 0 = MID (Bank 0), Ch 1 = SIDE (Bank 1)
    const SilkenTofuDSP::Parameters pCh[2] = {
        readChannelParameters(0),
        isMidSide ? readChannelParameters(1) : readChannelParameters(0)
    };

    engines_[0].setTargets(pCh[0]);
    engines_[1].setTargets(pCh[1]);
    smStereoLink_.target = static_cast<double>(apvts_.getRawParameterValue("stereoLink")->load());
    const double linkSmoothAlpha = 1.0 - std::exp(-1.0 / (currentSampleRate_ * 0.012));

    const bool hasValidScBus = (scBus.getNumChannels() > 0 && scBus.getNumSamples() >= numSamples);

    // Optional Mid/Side Matrix Encode: M = 0.5*(L+R), S = 0.5*(L-R)
    // Preserves exact 0.0 dB threshold for centered mono material and lossless L = M+S, R = M-S reconstruction!
    if (isMidSide)
    {
        auto* leftPtr  = mainBus.getWritePointer(0);
        auto* rightPtr = mainBus.getWritePointer(1);
        for (int i = 0; i < numSamples; ++i)
        {
            const float L = leftPtr[i];
            const float R = rightPtr[i];
            leftPtr[i]  = 0.5f * (L + R); // Ch 0 = Mid
            rightPtr[i] = 0.5f * (L - R); // Ch 1 = Side
        }
    }

    // --- PASS 1: Advance T4B Electro-Optical Cell & Sidechain at Base Sample Rate ---
    for (int i = 0; i < numSamples; ++i)
    {
        const float linkFrac = static_cast<float>(std::clamp(smStereoLink_.step(linkSmoothAlpha) * 0.01, 0.0, 1.0));
        float gCh[2] = { 1.0f, 1.0f };
        float scMon[2] = { 0.0f, 0.0f };

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float xIn = mainBus.getSample(ch, i);
            float kIn = xIn;
            if (pCh[ch].extSidechain && hasValidScBus)
            {
                if (isMidSide && scBus.getNumChannels() >= 2)
                {
                    const float scL = scBus.getSample(0, i);
                    const float scR = scBus.getSample(1, i);
                    kIn = (ch == 0) ? (0.5f * (scL + scR)) : (0.5f * (scL - scR));
                }
                else
                {
                    kIn = scBus.getSample(std::min(ch, scBus.getNumChannels() - 1), i);
                }
            }
            gCh[ch] = engines_[ch].processT4BSidechain(xIn, kIn, pCh[ch], scMon[ch]);
            scMonitorBuffer_.setSample(ch, i, scMon[ch]);
        }

        if (numChannels == 2)
        {
            const float avgG = 0.5f * (gCh[0] + gCh[1]);
            const float g0 = (1.0f - linkFrac) * gCh[0] + linkFrac * avgG;
            const float g1 = (1.0f - linkFrac) * gCh[1] + linkFrac * avgG;
            t4bGainBuffer_.setSample(0, i, g0);
            t4bGainBuffer_.setSample(1, i, g1);
            engines_[0].setEffectiveGainReductionLin(g0);
            engines_[1].setEffectiveGainReductionLin(g1);
        }
        else
        {
            t4bGainBuffer_.setSample(0, i, gCh[0]);
            engines_[0].setEffectiveGainReductionLin(gCh[0]);
        }
    }

    // If SC Listen is active on the currently monitored bank, route the filtered EL Panel drive to output
    const int activeBank = (routingMode == 2) ? 1 : 0;
    if (pCh[activeBank].scListen && !pCh[0].bypass)
    {
        const float trimLin = std::pow(10.0f, pCh[activeBank].trimDb / 20.0f);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* outPtr = mainBus.getWritePointer(ch);
            const auto* scPtr = scMonitorBuffer_.getReadPointer(isMidSide ? std::min(activeBank, numChannels - 1) : ch);
            for (int i = 0; i < numSamples; ++i)
                outPtr[i] = scPtr[i] * trimLin;
        }
        return;
    }

    // --- PASS 2: Oversampled Non-Linear Analog Stage & Phase-Locked Dry/Wet Blend ---
    const int osFactor = 1 << osChoice;
    const double osRate = currentSampleRate_ * static_cast<double>(osFactor);

    SilkenTofuDSP::AnalogStageCoeffs osCoeffs;
    osCoeffs.update(osRate);

    juce::dsp::AudioBlock<float> baseBlock(mainBus);
    auto osBlock = os.processSamplesUp(baseBlock);
    const int osNumSamples = static_cast<int>(osBlock.getNumSamples());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* osSamples = osBlock.getChannelPointer(static_cast<size_t>(ch));
        const auto* gBase = t4bGainBuffer_.getReadPointer(ch);

        for (int k = 0; k < osNumSamples; ++k)
        {
            const int baseIdx = std::min(numSamples - 1, k / osFactor);
            const int nextIdx = std::min(numSamples - 1, baseIdx + 1);
            const float frac  = static_cast<float>(k % osFactor) / static_cast<float>(osFactor);
            const float gVal  = gBase[baseIdx] + frac * (gBase[nextIdx] - gBase[baseIdx]);

            osSamples[k] = engines_[ch].processAnalogAndMixOsSample(osSamples[k], gVal, pCh[ch], osCoeffs);
        }
    }

    os.processSamplesDown(baseBlock);

    // --- PASS 3: Post-Mix 5 Hz Output Transformer DC Blocker, Smoothed Master Trim & M/S Decode ---
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* outPtr = mainBus.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
            outPtr[i] = engines_[ch].processPostMixDcBlockAndTrim(outPtr[i]);
    }

    if (isMidSide)
    {
        const bool msSolo = !pCh[0].bypass && (apvts_.getRawParameterValue("msSolo")->load() > 0.5f);
        auto* midPtr  = mainBus.getWritePointer(0);
        auto* sidePtr = mainBus.getWritePointer(1);
        if (msSolo)
        {
            if (routingMode == 1) // Solo MID -> centered mono M on Left and Right
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    const float M = midPtr[i];
                    midPtr[i]  = M;
                    sidePtr[i] = M;
                }
            }
            else // Solo SIDE (routingMode == 2) -> stereo difference +S on Left, -S on Right
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    const float S = sidePtr[i];
                    midPtr[i]  = S;
                    sidePtr[i] = -S;
                }
            }
        }
        else
        {
            for (int i = 0; i < numSamples; ++i)
            {
                const float M = midPtr[i];
                const float S = sidePtr[i];
                midPtr[i]  = M + S; // Left  = Mid + Side
                sidePtr[i] = M - S; // Right = Mid - Side
            }
        }
    }

    // Update UI Telemetry Atomics per channel (0 = Left/Mid, 1 = Right/Side)
    for (int ch = 0; ch < 2; ++ch)
    {
        const auto tel = engines_[std::min(ch, numChannels - 1)].getTelemetry();
        telemetryGrDb_[ch].store(tel.gainReductionDb, std::memory_order_relaxed);
        telemetryLum_[ch].store(tel.elLuminescence,   std::memory_order_relaxed);
        telemetryN1_[ch].store(tel.shallowTrapN1,     std::memory_order_relaxed);
        telemetryN2_[ch].store(tel.deepTrapN2,        std::memory_order_relaxed);
        telemetryBPlus_[ch].store(tel.bPlusVolts,     std::memory_order_relaxed);
        telemetryInDb_[ch].store(tel.inputVuDb,       std::memory_order_relaxed);
        telemetryOutDb_[ch].store(tel.outputVuDb,     std::memory_order_relaxed);
    }
}

bool SilkenTofuAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* SilkenTofuAudioProcessor::createEditor()
{
    return new SilkenTofuAudioProcessorEditor(*this);
}

void SilkenTofuAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SilkenTofuAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts_.state.getType()))
    {
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
        abStates_[activeABSlot_] = apvts_.copyState();
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SilkenTofuAudioProcessor();
}
