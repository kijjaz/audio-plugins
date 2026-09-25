#include "PluginProcessor.h"
#include "PluginEditor.h"

FriedTofuMBAudioProcessor::FriedTofuMBAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    masterDriveParam   = apvts.getRawParameterValue("masterDrive");
    masterTrimParam    = apvts.getRawParameterValue("masterTrim");
    masterMixParam     = apvts.getRawParameterValue("masterMix");
    crossoverLowParam  = apvts.getRawParameterValue("crossoverLow");
    crossoverHighParam = apvts.getRawParameterValue("crossoverHigh");

    const juce::String bandPrefixes[3] = { "low_", "mid_", "high_" };
    for (int b = 0; b < 3; ++b)
    {
        bandParams[b].drive    = apvts.getRawParameterValue(bandPrefixes[b] + "drive");
        bandParams[b].networkZ = apvts.getRawParameterValue(bandPrefixes[b] + "networkZ");
        bandParams[b].diodeA   = apvts.getRawParameterValue(bandPrefixes[b] + "diodeA");
        bandParams[b].diodeB   = apvts.getRawParameterValue(bandPrefixes[b] + "diodeB");
        bandParams[b].gain     = apvts.getRawParameterValue(bandPrefixes[b] + "gain");
        bandParams[b].mute     = apvts.getRawParameterValue(bandPrefixes[b] + "mute");
        bandParams[b].solo     = apvts.getRawParameterValue(bandPrefixes[b] + "solo");
        bandParams[b].bypass   = apvts.getRawParameterValue(bandPrefixes[b] + "bypass");
    }
}

FriedTofuMBAudioProcessor::~FriedTofuMBAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout FriedTofuMBAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Master Parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "masterDrive", 1 }, "Master Drive",
        juce::NormalisableRange<float>(-12.0f, 24.0f, 0.1f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "masterTrim", 1 }, "Master Trim",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "masterMix", 1 }, "Master Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f));

    // Crossover Frequencies
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crossoverLow", 1 }, "Crossover Low-Mid",
        juce::NormalisableRange<float>(60.0f, 1200.0f, 1.0f, 0.35f), 250.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crossoverHigh", 1 }, "Crossover Mid-High",
        juce::NormalisableRange<float>(1200.0f, 12000.0f, 1.0f, 0.35f), 3000.0f));

    // Diode Choices list
    juce::StringArray diodeChoices = {
        "Si 1N4148 (Crisp)",
        "Ge 1N34A (Silky)",
        "BAT41 (Flash Fried)",
        "Red LED (Deep Fried)"
    };

    // Per-Band Parameters
    const juce::String bandPrefixes[3] = { "low_", "mid_", "high_" };
    const juce::String bandNames[3] = { "Low", "Mid", "High" };
    const float defaultDrives[3] = { 6.0f, 12.0f, 8.0f };
    const float defaultZ[3] = { 1000.0f, 2200.0f, 4700.0f };

    for (int b = 0; b < 3; ++b)
    {
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { bandPrefixes[b] + "drive", 1 }, bandNames[b] + " Drive",
            juce::NormalisableRange<float>(-12.0f, 36.0f, 0.1f), defaultDrives[b]));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { bandPrefixes[b] + "networkZ", 1 }, bandNames[b] + " Network Z",
            juce::NormalisableRange<float>(200.0f, 22000.0f, 1.0f, 0.25f), defaultZ[b]));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { bandPrefixes[b] + "diodeA", 1 }, bandNames[b] + " Diode A (+)",
            diodeChoices, 0));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID { bandPrefixes[b] + "diodeB", 1 }, bandNames[b] + " Diode B (-)",
            diodeChoices, (b == 1 ? 1 : 0))); // Default asymmetric on Mid band

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID { bandPrefixes[b] + "gain", 1 }, bandNames[b] + " Level",
            juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { bandPrefixes[b] + "mute", 1 }, bandNames[b] + " Mute",
            false));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { bandPrefixes[b] + "solo", 1 }, bandNames[b] + " Solo",
            false));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID { bandPrefixes[b] + "bypass", 1 }, bandNames[b] + " Bypass",
            false));
    }

    return { params.begin(), params.end() };
}

const juce::String FriedTofuMBAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FriedTofuMBAudioProcessor::acceptsMidi() const { return false; }
bool FriedTofuMBAudioProcessor::producesMidi() const { return false; }
bool FriedTofuMBAudioProcessor::isMidiEffect() const { return false; }
double FriedTofuMBAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int FriedTofuMBAudioProcessor::getNumPrograms() { return 1; }
int FriedTofuMBAudioProcessor::getCurrentProgram() { return 0; }
void FriedTofuMBAudioProcessor::setCurrentProgram(int) {}
const juce::String FriedTofuMBAudioProcessor::getProgramName(int) { return {}; }
void FriedTofuMBAudioProcessor::changeProgramName(int, const juce::String&) {}

void FriedTofuMBAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    mbEngine.prepare(spec);

    int latency = mbEngine.getLatencySamples();
    setLatencySamples(latency);

    // Prepare dry delay line to match linear-phase crossover latency
    dryDelayBuffer.resize(spec.numChannels);
    for (size_t ch = 0; ch < spec.numChannels; ++ch)
        dryDelayBuffer[ch].resize(static_cast<size_t>(latency + samplesPerBlock + 128), 0.0f);
    dryDelayIndex = 0;

    scopeBuffer.fill(0.0f);
    scopeFifo.reset();
}

void FriedTofuMBAudioProcessor::releaseResources()
{
    mbEngine.reset();
}

bool FriedTofuMBAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void FriedTofuMBAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (numSamples == 0)
        return;

    // Read Master parameters
    const float masterDriveDb = masterDriveParam ? masterDriveParam->load() : 0.0f;
    const float masterDriveLin = juce::Decibels::decibelsToGain(masterDriveDb);
    const float masterTrimDb = masterTrimParam ? masterTrimParam->load() : 0.0f;
    const float masterTrimLin = juce::Decibels::decibelsToGain(masterTrimDb);
    const float masterMix = masterMixParam ? (masterMixParam->load() / 100.0f) : 1.0f;

    // Update crossover
    const float fc1 = crossoverLowParam ? crossoverLowParam->load() : 250.0f;
    const float fc2 = crossoverHighParam ? crossoverHighParam->load() : 3000.0f;
    mbEngine.setCrossoverFrequencies(fc1, fc2);

    // Update per-band parameters
    for (int b = 0; b < 3; ++b)
    {
        FriedTofuDSP::BandSettings s;
        s.driveDb  = bandParams[b].drive->load();
        s.networkZ = bandParams[b].networkZ->load();
        s.diodeA   = static_cast<int>(bandParams[b].diodeA->load());
        s.diodeB   = static_cast<int>(bandParams[b].diodeB->load());
        s.gainDb   = bandParams[b].gain->load();
        s.mute     = bandParams[b].mute->load() > 0.5f;
        s.solo     = bandParams[b].solo->load() > 0.5f;
        s.bypass   = bandParams[b].bypass->load() > 0.5f;
        mbEngine.setBandSettings(b, s);
    }

    // Keep dry delayed copy for dry/wet mix
    const int latency = mbEngine.getLatencySamples();
    juce::AudioBuffer<float> dryAligned;
    if (masterMix < 1.0f)
    {
        dryAligned.setSize(totalNumInputChannels, numSamples, false, false, true);
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            const float* inData = buffer.getReadPointer(ch);
            float* dryOut = dryAligned.getWritePointer(ch);
            auto& dBuf = dryDelayBuffer[static_cast<size_t>(ch)];
            const size_t mask = dBuf.size();

            for (int i = 0; i < numSamples; ++i)
            {
                size_t writePos = (dryDelayIndex + static_cast<size_t>(i)) % mask;
                size_t readPos = (dryDelayIndex + static_cast<size_t>(i) + mask - static_cast<size_t>(latency)) % mask;
                dryOut[i] = dBuf[readPos];
                dBuf[writePos] = inData[i];
            }
        }
        dryDelayIndex = (dryDelayIndex + static_cast<size_t>(numSamples)) % dryDelayBuffer[0].size();
    }

    // Apply master drive
    if (std::abs(masterDriveDb) > 0.01f)
        buffer.applyGain(masterDriveLin);

    // Multiband Processing
    mbEngine.process(buffer);

    // Apply master trim & dry/wet mix
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        float* wet = buffer.getWritePointer(ch);
        const float* dry = (masterMix < 1.0f && ch < totalNumInputChannels) ? dryAligned.getReadPointer(ch) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            float outWet = wet[i] * masterTrimLin;
            if (dry != nullptr)
                wet[i] = dry[i] * (1.0f - masterMix) + outWet * masterMix;
            else
                wet[i] = outWet;
        }
    }

    // Push output samples into Oscilloscope FIFO
    const float* outData = buffer.getReadPointer(0);
    int start1, size1, start2, size2;
    scopeFifo.prepareToWrite(numSamples, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy(outData, outData + size1, scopeBuffer.data() + start1);
    if (size2 > 0)
        std::copy(outData + size1, outData + size1 + size2, scopeBuffer.data() + start2);

    scopeFifo.finishedWrite(size1 + size2);
}

void FriedTofuMBAudioProcessor::getScopeSamples(float* destBuffer, int numSamplesToRead)
{
    int start1, size1, start2, size2;
    scopeFifo.prepareToRead(numSamplesToRead, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy(scopeBuffer.data() + start1, scopeBuffer.data() + start1 + size1, destBuffer);
    if (size2 > 0)
        std::copy(scopeBuffer.data() + start2, scopeBuffer.data() + start2 + size2, destBuffer + size1);

    scopeFifo.finishedRead(size1 + size2);
}

int FriedTofuMBAudioProcessor::getBandDiodeA(int band) const
{
    if (band >= 0 && band < 3 && bandParams[band].diodeA)
        return static_cast<int>(bandParams[band].diodeA->load());
    return 0;
}

int FriedTofuMBAudioProcessor::getBandDiodeB(int band) const
{
    if (band >= 0 && band < 3 && bandParams[band].diodeB)
        return static_cast<int>(bandParams[band].diodeB->load());
    return 0;
}

float FriedTofuMBAudioProcessor::getBandNetworkZ(int band) const
{
    if (band >= 0 && band < 3 && bandParams[band].networkZ)
        return bandParams[band].networkZ->load();
    return 2200.0f;
}

float FriedTofuMBAudioProcessor::getBandDriveDb(int band) const
{
    if (band >= 0 && band < 3 && bandParams[band].drive)
        return bandParams[band].drive->load();
    return 0.0f;
}

juce::AudioProcessorEditor* FriedTofuMBAudioProcessor::createEditor()
{
    return new FriedTofuMBAudioProcessorEditor(*this);
}

bool FriedTofuMBAudioProcessor::hasEditor() const
{
    return true;
}

void FriedTofuMBAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FriedTofuMBAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FriedTofuMBAudioProcessor();
}
