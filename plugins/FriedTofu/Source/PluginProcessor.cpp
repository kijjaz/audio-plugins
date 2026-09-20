#include "PluginProcessor.h"
#include "PluginEditor.h"

FriedTofuAudioProcessor::FriedTofuAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    driveParam      = apvts.getRawParameterValue("drive");
    networkZParam   = apvts.getRawParameterValue("networkZ");
    diodeAParam     = apvts.getRawParameterValue("diodeA");
    diodeBParam     = apvts.getRawParameterValue("diodeB");
    toneParam       = apvts.getRawParameterValue("tone");
    mixParam        = apvts.getRawParameterValue("mix");
    trimParam       = apvts.getRawParameterValue("trim");
    oversampleParam = apvts.getRawParameterValue("oversample");
}

FriedTofuAudioProcessor::~FriedTofuAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout FriedTofuAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Drive: -12.0 dB to +40.0 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("drive", 1), "Drive",
        juce::NormalisableRange<float>(-12.0f, 40.0f, 0.1f), 12.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Network Z: log-range from 200 Ohm to 22,000 Ohm (slider 2.30 to 4.34 in log10)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("networkZ", 1), "Network Z",
        juce::NormalisableRange<float>(200.0f, 22000.0f, 1.0f, 0.35f), 2200.0f,
        juce::AudioParameterFloatAttributes().withLabel("Ohms")));

    // Diode models
    juce::StringArray diodeChoices;
    for (int i = 0; i < FriedTofuDSP::NumDiodeTypes; ++i)
        diodeChoices.add(FriedTofuDSP::AvailableDiodes[i].name);

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("diodeA", 1), "Diode A (+)", diodeChoices, 0));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("diodeB", 1), "Diode B (-)", diodeChoices, 0));

    // Tone LPF: 700 Hz to 20000 Hz
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("tone", 1), "Tone",
        juce::NormalisableRange<float>(700.0f, 20000.0f, 1.0f, 0.3f), 14000.0f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    // Mix: 0% to 100%
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("mix", 1), "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Trim / Output Gain: -24 dB to +12 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("trim", 1), "Trim",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")));

    // Oversampling factor: 1x (Aliased), 2x, 4x (Crisp Clean)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("oversample", 1), "Oversampling",
        juce::StringArray { "1x (Raw)", "2x", "4x (Crisp Clean)" }, 2));

    return { params.begin(), params.end() };
}

const juce::String FriedTofuAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FriedTofuAudioProcessor::acceptsMidi() const { return false; }
bool FriedTofuAudioProcessor::producesMidi() const { return false; }
bool FriedTofuAudioProcessor::isMidiEffect() const { return false; }
double FriedTofuAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int FriedTofuAudioProcessor::getNumPrograms() { return 1; }
int FriedTofuAudioProcessor::getCurrentProgram() { return 0; }
void FriedTofuAudioProcessor::setCurrentProgram(int) {}
const juce::String FriedTofuAudioProcessor::getProgramName(int) { return {}; }
void FriedTofuAudioProcessor::changeProgramName(int, const juce::String&) {}

void FriedTofuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Reset WDF clippers
    for (auto& clipper : diodeClippers)
        clipper.reset();

    // Configure 2x and 4x Polyphase IIR Oversamplers
    oversampler2x = std::make_unique<juce::dsp::Oversampling<float>>(
        2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false);
    oversampler2x->initProcessing(static_cast<size_t>(samplesPerBlock));

    oversampler4x = std::make_unique<juce::dsp::Oversampling<float>>(
        2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false);
    oversampler4x->initProcessing(static_cast<size_t>(samplesPerBlock));

    // Initialize Tone filters
    for (auto& filter : toneFilters)
        filter.reset();
}

void FriedTofuAudioProcessor::releaseResources()
{
    if (oversampler2x) oversampler2x->reset();
    if (oversampler4x) oversampler4x->reset();
}

bool FriedTofuAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void FriedTofuAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (numSamples == 0)
        return;

    // Read parameters
    const float driveDb    = driveParam->load();
    const float driveLin   = juce::Decibels::decibelsToGain(driveDb);
    const float networkZ   = networkZParam->load();
    const int diodeA       = static_cast<int>(diodeAParam->load());
    const int diodeB       = static_cast<int>(diodeBParam->load());
    const float toneFreq   = toneParam->load();
    const float mix        = mixParam->load() / 100.0f;
    const float trimDb     = trimParam->load();
    const float trimLin    = juce::Decibels::decibelsToGain(trimDb);
    const int oversampleMode = static_cast<int>(oversampleParam->load()); // 0=1x, 1=2x, 2=4x

    // Update diode clippers settings
    for (int ch = 0; ch < 2; ++ch)
    {
        diodeClippers[ch].setNetworkZ(networkZ);
        diodeClippers[ch].setDiodeA(diodeA);
        diodeClippers[ch].setDiodeB(diodeB);
    }

    // Update tone filter coefficients (lowpass 2nd order Butterworth)
    auto toneCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, toneFreq, 0.7071f);
    for (auto& filter : toneFilters)
        filter.coefficients = toneCoeffs;

    // Keep dry buffer copy for Mix
    juce::AudioBuffer<float> dryBuffer;
    if (mix < 1.0f)
    {
        dryBuffer.makeCopyOf(buffer);
    }

    // Apply Drive gain
    buffer.applyGain(driveLin);

    // Audio block wrapper
    juce::dsp::AudioBlock<float> block(buffer);

    auto processNonLinearity = [this](juce::dsp::AudioBlock<float>& targetBlock)
    {
        const size_t numChannels = targetBlock.getNumChannels();
        const size_t numLen      = targetBlock.getNumSamples();

        for (size_t ch = 0; ch < numChannels; ++ch)
        {
            auto* channelData = targetBlock.getChannelPointer(ch);
            auto& clipper     = diodeClippers[ch % 2];

            for (size_t i = 0; i < numLen; ++i)
            {
                channelData[i] = clipper.processSample(channelData[i]);
            }
        }
    };

    if (oversampleMode == 2 && oversampler4x)
    {
        auto upsampledBlock = oversampler4x->processSamplesUp(block);
        processNonLinearity(upsampledBlock);
        oversampler4x->processSamplesDown(block);
    }
    else if (oversampleMode == 1 && oversampler2x)
    {
        auto upsampledBlock = oversampler2x->processSamplesUp(block);
        processNonLinearity(upsampledBlock);
        oversampler2x->processSamplesDown(block);
    }
    else
    {
        // 1x Raw
        processNonLinearity(block);
    }

    // Post Tone Filter & Mix & Trim
    const int numChannels = std::min(totalNumInputChannels, 2);
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        const auto* dry = (mix < 1.0f) ? dryBuffer.getReadPointer(ch) : nullptr;

        for (int i = 0; i < numSamples; ++i)
        {
            // Tone LPF filter on wet signal
            float filteredWet = toneFilters[ch].processSample(wet[i]);
            
            // Apply Trim
            filteredWet *= trimLin;

            // Dry/Wet blend
            if (dry != nullptr)
                wet[i] = dry[i] * (1.0f - mix) + filteredWet * mix;
            else
                wet[i] = filteredWet;
        }
    }

    // Push output samples into Oscilloscope FIFO (Channel 0 / left)
    const float* outData = buffer.getReadPointer(0);
    int start1, size1, start2, size2;
    scopeFifo.prepareToWrite(numSamples, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy(outData, outData + size1, scopeBuffer.data() + start1);
    if (size2 > 0)
        std::copy(outData + size1, outData + size1 + size2, scopeBuffer.data() + start2);

    scopeFifo.finishedWrite(size1 + size2);
}

void FriedTofuAudioProcessor::getScopeSamples(float* destBuffer, int numSamplesToRead)
{
    int start1, size1, start2, size2;
    scopeFifo.prepareToRead(numSamplesToRead, start1, size1, start2, size2);

    if (size1 > 0)
        std::copy(scopeBuffer.data() + start1, scopeBuffer.data() + start1 + size1, destBuffer);
    if (size2 > 0)
        std::copy(scopeBuffer.data() + start2, scopeBuffer.data() + start2 + size2, destBuffer + size1);

    scopeFifo.finishedRead(size1 + size2);
}

int FriedTofuAudioProcessor::getDiodeIndexA() const
{
    return diodeAParam ? static_cast<int>(diodeAParam->load()) : 0;
}

int FriedTofuAudioProcessor::getDiodeIndexB() const
{
    return diodeBParam ? static_cast<int>(diodeBParam->load()) : 0;
}

float FriedTofuAudioProcessor::getNetworkZOhms() const
{
    return networkZParam ? networkZParam->load() : 2200.0f;
}

float FriedTofuAudioProcessor::getDriveLinear() const
{
    return driveParam ? juce::Decibels::decibelsToGain(driveParam->load()) : 1.0f;
}

juce::AudioProcessorEditor* FriedTofuAudioProcessor::createEditor()
{
    return new FriedTofuAudioProcessorEditor(*this);
}

bool FriedTofuAudioProcessor::hasEditor() const
{
    return true;
}

void FriedTofuAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FriedTofuAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FriedTofuAudioProcessor();
}
