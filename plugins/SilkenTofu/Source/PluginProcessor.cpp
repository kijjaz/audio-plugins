#include "PluginProcessor.h"
#include "PluginEditor.h"

SilkenTofuAudioProcessor::SilkenTofuAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withInput("Sidechain Key", juce::AudioChannelSet::stereo(), false) // #3: True External Key Input
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts_(*this, nullptr, "Parameters", createParameterLayout())
{
    for (size_t i = 0; i < 4; ++i)
    {
        oversamplers_[i] = std::make_unique<juce::dsp::Oversampling<float>>(
            2, i, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true);
    }
    applyPreset(0);
}

SilkenTofuAudioProcessor::~SilkenTofuAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout SilkenTofuAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // 1. Classic Teletronix LA-2A Front Panel
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"peakReduction", 1}, "Peak Reduction",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 46.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"gainDb", 1}, "Makeup Gain",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"mode", 1}, "Comp / Limit Mode",
        juce::StringArray{"Compress (3:1)", "Limit (inf:1)"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"autoLevel", 1}, "Auto-Level Link", true));

    // 2. "T4-Bean" Semiconductor & Electro-Optical Physics Lab
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"crystalType", 1}, "Crystal Chemistry",
        juce::StringArray{"Vintage CdS (LA-2A)", "CdSe Vactrol (LA-3A)", "Fast Silicon (FET-Opto)"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"trapMemory", 1}, "Trap Memory",
        juce::NormalisableRange<float>(-50.0f, 200.0f, 0.5f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"topologyMorph", 1}, "FB / FF Topology",
        juce::NormalisableRange<float>(0.0f, 125.0f, 0.5f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"punchHoldoff", 1}, "Punch Holdoff",
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f), 2.5f,
        juce::AudioParameterFloatAttributes().withLabel("ms")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"biasGlow", 1}, "T4B Pre-Warmth (Bias Glow)",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 20.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"cellAge", 1}, "Phosphor Cell Age",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // 3. Oversampled Analog Tube (12AX7A + 12BH7A) & Transformer (UTC HA-100X / A-24) Stage
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"oversampling", 1}, "Oversampling",
        juce::StringArray{"1x (Off)", "2x Polyphase", "4x Polyphase", "8x Polyphase"}, 2));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"tubeIronDrive", 1}, "Tube & Iron Drive",
        juce::NormalisableRange<float>(-6.0f, 18.0f, 0.1f), 2.5f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"fatCathodeCap", 1}, "V1 Fat Cathode Cap (Ck)", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"load600Ohm", 1}, "600-Ohm Vintage Load", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"bPlusSag", 1}, "B+ Power Supply Sag",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 40.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"millerAir", 1}, "Miller Impedance Air",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.5f), 15.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // 4. Sidechain & External Key Ducking (#3)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"r37Emphasis", 1}, "R37 HF Emphasis",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 55.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"scHighPass", 1}, "SC Warm-Cut HPF",
        juce::NormalisableRange<float>(20.0f, 350.0f, 1.0f, 0.5f), 65.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"extSidechain", 1}, "External Key Ducking", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{"scListen", 1}, "SC Listen (EL Drive)", false));

    // 5. Mix & Unity Trim (AGENTS.md Rule 2: 0.0 dB on all factory presets)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"mix", 1}, "Dry / Wet Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"trimDb", 1}, "Master Trim",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

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

void SilkenTofuAudioProcessor::applyPreset(int presetIndex)
{
    const auto& presets = SilkenTofuDSP::getFactoryPresets();
    if (presetIndex < 0 || presetIndex >= static_cast<int>(presets.size()))
        return;

    currentPresetIndex_ = presetIndex;
    const auto& p = presets[static_cast<size_t>(presetIndex)].params;

    auto setParam = [this](const char* id, float plainValue)
    {
        if (auto* param = apvts_.getParameter(id))
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
}

SilkenTofuDSP::Parameters SilkenTofuAudioProcessor::readCurrentParameters() const
{
    SilkenTofuDSP::Parameters p;
    p.peakReduction  = apvts_.getRawParameterValue("peakReduction")->load();
    p.gainDb         = apvts_.getRawParameterValue("gainDb")->load();
    p.limitMode      = apvts_.getRawParameterValue("mode")->load() > 0.5f;
    p.autoLevel      = apvts_.getRawParameterValue("autoLevel")->load() > 0.5f;

    p.crystalType    = static_cast<int>(apvts_.getRawParameterValue("crystalType")->load());
    p.trapMemory     = apvts_.getRawParameterValue("trapMemory")->load();
    p.topologyMorph  = apvts_.getRawParameterValue("topologyMorph")->load();
    p.punchHoldoffMs = apvts_.getRawParameterValue("punchHoldoff")->load();
    p.biasGlow       = apvts_.getRawParameterValue("biasGlow")->load();
    p.cellAge        = apvts_.getRawParameterValue("cellAge")->load();

    p.tubeIronDriveDb= apvts_.getRawParameterValue("tubeIronDrive")->load();
    p.fatCathodeCap  = apvts_.getRawParameterValue("fatCathodeCap")->load() > 0.5f;
    p.load600Ohm     = apvts_.getRawParameterValue("load600Ohm")->load() > 0.5f;
    p.bPlusSag       = apvts_.getRawParameterValue("bPlusSag")->load();
    p.millerAir      = apvts_.getRawParameterValue("millerAir")->load();

    p.r37Emphasis    = apvts_.getRawParameterValue("r37Emphasis")->load();
    p.scHighPassHz   = apvts_.getRawParameterValue("scHighPass")->load();
    p.extSidechain   = apvts_.getRawParameterValue("extSidechain")->load() > 0.5f;
    p.scListen       = apvts_.getRawParameterValue("scListen")->load() > 0.5f;

    p.mix            = apvts_.getRawParameterValue("mix")->load();
    p.trimDb         = apvts_.getRawParameterValue("trimDb")->load();
    return p;
}

void SilkenTofuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate_ = std::max(22050.0, sampleRate);
    engines_[0].prepare(currentSampleRate_, -0.35f); // Left canister micro-tolerance
    engines_[1].prepare(currentSampleRate_, +0.35f); // Right canister micro-tolerance

    const int maxBlock = std::max(64, samplesPerBlock);
    for (auto& os : oversamplers_)
    {
        os->initProcessing(static_cast<size_t>(maxBlock));
        os->reset();
    }

    dryBuffer_.setSize(2, maxBlock);
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
    auto scBus   = (getBusCount(true) > 1) ? getBusBuffer(buffer, true, 1) : juce::AudioBuffer<float>();

    const int numChannels = std::min(2, mainBus.getNumChannels());
    const int numSamples  = mainBus.getNumSamples();
    if (numChannels == 0 || numSamples == 0)
        return;

    if (dryBuffer_.getNumSamples() < numSamples)
    {
        dryBuffer_.setSize(2, numSamples, false, false, true);
        t4bGainBuffer_.setSize(2, numSamples, false, false, true);
        scMonitorBuffer_.setSize(2, numSamples, false, false, true);
    }

    const auto p = readCurrentParameters();
    const int osChoice = std::clamp(static_cast<int>(apvts_.getRawParameterValue("oversampling")->load()), 0, 3);
    const bool hasValidScBus = (scBus.getNumChannels() > 0 && scBus.getNumSamples() >= numSamples);

    float inPeak = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        dryBuffer_.copyFrom(ch, 0, mainBus, ch, 0, numSamples);
        inPeak = std::max(inPeak, mainBus.getMagnitude(ch, 0, numSamples));
    }

    // --- PASS 1: Advance T4B Electro-Optical Cell & Sidechain at Base Sample Rate ---
    // Stereo-linked optical photon sharing (75% stereo link like dual LA-2A Pin 6 interconnect)
    for (int i = 0; i < numSamples; ++i)
    {
        float gCh[2] = { 1.0f, 1.0f };
        float scMon[2] = { 0.0f, 0.0f };

        for (int ch = 0; ch < numChannels; ++ch)
        {
            const float xIn = mainBus.getSample(ch, i);
            const float kIn = (p.extSidechain && hasValidScBus)
                ? scBus.getSample(std::min(ch, scBus.getNumChannels() - 1), i)
                : xIn;
            gCh[ch] = engines_[ch].processT4BSidechain(xIn, kIn, p, scMon[ch]);
            scMonitorBuffer_.setSample(ch, i, scMon[ch]);
        }

        if (numChannels == 2)
        {
            const float avgG = 0.5f * (gCh[0] + gCh[1]);
            t4bGainBuffer_.setSample(0, i, 0.80f * avgG + 0.20f * gCh[0]);
            t4bGainBuffer_.setSample(1, i, 0.80f * avgG + 0.20f * gCh[1]);
        }
        else
        {
            t4bGainBuffer_.setSample(0, i, gCh[0]);
        }
    }

    // If SC Listen is active, route the filtered EL Panel drive directly to the output
    if (p.scListen)
    {
        const float trimLin = std::pow(10.0f, p.trimDb / 20.0f);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* outPtr = mainBus.getWritePointer(ch);
            const auto* scPtr = scMonitorBuffer_.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
                outPtr[i] = scPtr[i] * trimLin;
        }
        return;
    }

    // --- PASS 2: Oversampled Non-Linear Analog Stage (HA-100X -> T4B -> 12AX7A -> 12BH7A -> A-24) ---
    auto& os = *oversamplers_[static_cast<size_t>(osChoice)];
    const int osFactor = 1 << osChoice;
    const double osRate = currentSampleRate_ * static_cast<double>(osFactor);

    juce::dsp::AudioBlock<float> baseBlock(mainBus);
    auto osBlock = os.processSamplesUp(baseBlock);
    const int osNumSamples = static_cast<int>(osBlock.getNumSamples());

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* osSamples = osBlock.getChannelPointer(static_cast<size_t>(ch));
        const auto* gBase = t4bGainBuffer_.getReadPointer(ch);

        for (int k = 0; k < osNumSamples; ++k)
        {
            // Linearly interpolate smooth T4B conductance across oversampled sub-steps
            const int baseIdx = std::min(numSamples - 1, k / osFactor);
            const float gVal = gBase[baseIdx];
            osSamples[k] = engines_[ch].processAnalogStageSample(osSamples[k], gVal, p, osRate);
        }
    }

    os.processSamplesDown(baseBlock);

    // --- PASS 3: Makeup Gain (with Auto-Level Link), Parallel Dry/Wet Mix & Master Unity Trim ---
    const float makeupLin = SilkenTofuDSP::ChannelEngine::computeMakeupGainLinear(p);
    const float trimLin   = std::pow(10.0f, p.trimDb / 20.0f);
    const float wetMix    = std::clamp(p.mix * 0.01f, 0.0f, 1.0f);
    const float dryMix    = 1.0f - wetMix;

    float outPeak = 0.0f;
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wetPtr = mainBus.getWritePointer(ch);
        const auto* dryPtr = dryBuffer_.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            const float wetSample = wetPtr[i] * makeupLin;
            const float blended   = (dryMix * dryPtr[i] + wetMix * wetSample) * trimLin;
            wetPtr[i] = blended;
            outPeak = std::max(outPeak, std::abs(blended));
        }
    }

    // Update UI Telemetry Atomics
    const auto tel = engines_[0].getTelemetry();
    telemetryGrDb_.store(tel.gainReductionDb, std::memory_order_relaxed);
    telemetryLum_.store(tel.elLuminescence, std::memory_order_relaxed);
    telemetryN1_.store(tel.shallowTrapN1, std::memory_order_relaxed);
    telemetryN2_.store(tel.deepTrapN2, std::memory_order_relaxed);
    telemetryBPlus_.store(tel.bPlusVolts, std::memory_order_relaxed);
    telemetryInDb_.store(20.0f * std::log10(std::max(inPeak, 1e-5f)), std::memory_order_relaxed);
    telemetryOutDb_.store(20.0f * std::log10(std::max(outPeak, 1e-5f)), std::memory_order_relaxed);
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
        apvts_.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SilkenTofuAudioProcessor();
}

