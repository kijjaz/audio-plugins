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

    return { params.begin(), params.end() };
}

const juce::String VacuumTapeSimAudioProcessor::getName() const { return JucePlugin_Name; }
bool VacuumTapeSimAudioProcessor::acceptsMidi() const { return false; }
bool VacuumTapeSimAudioProcessor::producesMidi() const { return false; }
bool VacuumTapeSimAudioProcessor::isMidiEffect() const { return false; }
double VacuumTapeSimAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int VacuumTapeSimAudioProcessor::getNumPrograms() { return 1; }
int VacuumTapeSimAudioProcessor::getCurrentProgram() { return 0; }
void VacuumTapeSimAudioProcessor::setCurrentProgram (int index) {}
const juce::String VacuumTapeSimAudioProcessor::getProgramName (int index) { return {}; }
void VacuumTapeSimAudioProcessor::changeProgramName (int index, const juce::String& newName) {}

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
            channelData[sample] = tapeDSP[channel].processSample(channelData[sample]);
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
