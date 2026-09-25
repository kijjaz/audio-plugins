#include "PluginProcessor.h"
#include "PluginEditor.h"

VacuumTapeSimAudioProcessor::VacuumTapeSimAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    initPresets();
}

VacuumTapeSimAudioProcessor::~VacuumTapeSimAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout VacuumTapeSimAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 0.3f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("sag", "Sag", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("ips", "Tape Speed (IPS)", juce::NormalisableRange<float>(7.5f, 30.0f, 0.1f), 15.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("wow_flutter", "Wow & Flutter", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("bias", "Bias", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("asymmetry", "Asymmetry", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("eq_mode", "EQ Curve", juce::StringArray{"NAB", "CCIR"}, 0));
    
    // Parameters for gain staging and mixing
    params.push_back(std::make_unique<juce::AudioParameterBool>("auto_gain", "Auto Gain", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("output", "Output", juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("mix", "Dry / Wet", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 100.0f));

    return { params.begin(), params.end() };
}

void VacuumTapeSimAudioProcessor::initPresets()
{
    // Preset definition:
    // name, drive, sag, ips, wow_flutter, bias, asymmetry, eq_mode (0:NAB, 1:CCIR), auto_gain, output, mix
    presets = {
        { "Default Clean 15 IPS",    1.0f, 0.40f, 15.0f, 0.00f,  0.0f, 0.00f, 0, true,   0.0f, 100.0f },
        { "Mastering Warmth (30 IPS)", 0.8f, 0.20f, 30.0f, 0.00f,  0.2f, 0.00f, 1, true,   0.0f, 100.0f },
        { "Vintage 7.5 IPS Dark",    1.5f, 0.50f,  7.5f, 0.12f, -0.3f, 0.05f, 0, true,   0.0f, 100.0f },
        { "Drum Bus Fat Sag",        2.8f, 0.75f, 15.0f, 0.02f,  0.1f, 0.10f, 0, true,  -1.0f,  85.0f },
        { "Hot Tube Overdrive",      5.5f, 0.85f, 15.0f, 0.05f,  0.0f, 0.25f, 0, true,  -2.5f, 100.0f },
        { "Wobbly Cassette Lo-Fi",   1.8f, 0.60f,  7.5f, 0.55f, -0.6f, 0.15f, 0, true,   0.0f, 100.0f },
        { "Analog Tape Flanger",     1.2f, 0.30f, 15.0f, 0.85f,  0.0f, 0.00f, 1, false,  0.0f,  50.0f },
        { "Subtle Parallel Glow",    3.2f, 0.65f, 30.0f, 0.03f,  0.3f, 0.08f, 1, true,   0.0f,  40.0f }
    };
}

void VacuumTapeSimAudioProcessor::loadPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(presets.size())) return;
    currentProgram = index;
    const auto& p = presets[static_cast<size_t>(index)];

    if (auto* param = apvts.getParameter("drive")) param->setValueNotifyingHost(param->convertTo0to1(p.drive));
    if (auto* param = apvts.getParameter("sag")) param->setValueNotifyingHost(param->convertTo0to1(p.sag));
    if (auto* param = apvts.getParameter("ips")) param->setValueNotifyingHost(param->convertTo0to1(p.ips));
    if (auto* param = apvts.getParameter("wow_flutter")) param->setValueNotifyingHost(param->convertTo0to1(p.wow_flutter));
    if (auto* param = apvts.getParameter("bias")) param->setValueNotifyingHost(param->convertTo0to1(p.bias));
    if (auto* param = apvts.getParameter("asymmetry")) param->setValueNotifyingHost(param->convertTo0to1(p.asymmetry));
    if (auto* param = apvts.getParameter("eq_mode")) param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(p.eq_mode)));
    if (auto* param = apvts.getParameter("auto_gain")) param->setValueNotifyingHost(p.auto_gain ? 1.0f : 0.0f);
    if (auto* param = apvts.getParameter("output")) param->setValueNotifyingHost(param->convertTo0to1(p.output));
    if (auto* param = apvts.getParameter("mix")) param->setValueNotifyingHost(param->convertTo0to1(p.mix));
}

int VacuumTapeSimAudioProcessor::getNumPrograms() { return static_cast<int>(presets.size()); }
int VacuumTapeSimAudioProcessor::getCurrentProgram() { return currentProgram; }
void VacuumTapeSimAudioProcessor::setCurrentProgram (int index) { loadPreset(index); }
const juce::String VacuumTapeSimAudioProcessor::getProgramName (int index)
{
    if (index >= 0 && index < static_cast<int>(presets.size()))
        return presets[static_cast<size_t>(index)].name;
    return {};
}
void VacuumTapeSimAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/) {}

const juce::String VacuumTapeSimAudioProcessor::getName() const { return JucePlugin_Name; }
bool VacuumTapeSimAudioProcessor::acceptsMidi() const { return false; }
bool VacuumTapeSimAudioProcessor::producesMidi() const { return false; }
bool VacuumTapeSimAudioProcessor::isMidiEffect() const { return false; }
double VacuumTapeSimAudioProcessor::getTailLengthSeconds() const { return 0.0; }

void VacuumTapeSimAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int i = 0; i < 2; ++i)
    {
        tapeDSP[i].prepare(sampleRate, samplesPerBlock);
    }
}

void VacuumTapeSimAudioProcessor::releaseResources()
{
}

bool VacuumTapeSimAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void VacuumTapeSimAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Fetch parameter values
    float drive = apvts.getRawParameterValue("drive")->load();
    float sag = apvts.getRawParameterValue("sag")->load();
    float ips = apvts.getRawParameterValue("ips")->load();
    float wow = apvts.getRawParameterValue("wow_flutter")->load();
    float bias = apvts.getRawParameterValue("bias")->load();
    float asym = apvts.getRawParameterValue("asymmetry")->load();
    int eq = static_cast<int>(apvts.getRawParameterValue("eq_mode")->load());
    bool autoGain = apvts.getRawParameterValue("auto_gain")->load() > 0.5f;
    float outputDb = apvts.getRawParameterValue("output")->load();
    float mixPercent = apvts.getRawParameterValue("mix")->load();

    // Auto-makeup calculation
    // Since Langevin saturates as tanh-like curve, auto gain compensates for drive boost:
    // When Drive is 1.0 -> factor 1.0
    // When Drive is 10.0 -> factor 1.0 / sqrt(10.0) ≈ 0.316 (-10dB)
    float autoGainFactor = 1.0f;
    if (autoGain)
    {
        autoGainFactor = 1.0f / std::sqrt(std::max(0.1f, drive));
    }

    float outputLinear = juce::Decibels::decibelsToGain(outputDb) * autoGainFactor;
    float wetRatio = mixPercent / 100.0f;
    float dryRatio = 1.0f - wetRatio;

    // Update DSP
    for (int i = 0; i < totalNumInputChannels; ++i)
    {
        tapeDSP[i].updateParameters(drive, sag, ips, bias, asym, wow, eq);
    }

    // Process audio
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float dry = channelData[sample];
            float wet = tapeDSP[channel].processSample(dry);
            channelData[sample] = (dry * dryRatio + wet * wetRatio) * outputLinear;
        }
    }
}

juce::AudioProcessorEditor* VacuumTapeSimAudioProcessor::createEditor()
{
    return new VacuumTapeSimAudioProcessorEditor (*this);
}

bool VacuumTapeSimAudioProcessor::hasEditor() const
{
    return true;
}

void VacuumTapeSimAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void VacuumTapeSimAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VacuumTapeSimAudioProcessor();
}
