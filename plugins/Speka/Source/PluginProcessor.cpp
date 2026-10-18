#include "PluginProcessor.h"
#include "PluginEditor.h"

SpekaAudioProcessor::SpekaAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , apvts (*this, nullptr, "Parameters", createParameterLayout())
{
    modeParam          = apvts.getRawParameterValue ("mode");
    carrierFreqParam   = apvts.getRawParameterValue ("carrierFreq");
    speakerTypeParam   = apvts.getRawParameterValue ("speakerType");
    driveParam         = apvts.getRawParameterValue ("drive");
    caseResonanceParam = apvts.getRawParameterValue ("caseResonance");
    rattleParam        = apvts.getRawParameterValue ("rattle");
    jitterParam        = apvts.getRawParameterValue ("jitter");
    sagParam           = apvts.getRawParameterValue ("sag");
    whineParam         = apvts.getRawParameterValue ("whine");
    autoGateParam      = apvts.getRawParameterValue ("autoGate");
    gateThreshParam    = apvts.getRawParameterValue ("gateThresh");
    mixParam           = apvts.getRawParameterValue ("mix");
    trimParam          = apvts.getRawParameterValue ("trim");

    crtBuffer.fill (0.0f);
}

SpekaAudioProcessor::~SpekaAudioProcessor()
{
}

const juce::String SpekaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpekaAudioProcessor::acceptsMidi() const { return false; }
bool SpekaAudioProcessor::producesMidi() const { return false; }
bool SpekaAudioProcessor::isMidiEffect() const { return false; }
double SpekaAudioProcessor::getTailLengthSeconds() const { return 0.05; }

const std::vector<SpekaAudioProcessor::Preset>& SpekaAudioProcessor::getPresets()
{
    static const std::vector<Preset> presets = {
        { "IBM 5150 RealSound (Default)", 0, 18900.0f, 0,  0.0f, 0.50f, 0.25f, 0.15f, 0.40f, 0.05f, true, -72.0f, 0.0f },
        { "1989 Xenon 2 / Speedball",     0, 11025.0f, 0,  2.0f, 0.65f, 0.40f, 0.20f, 0.50f, 0.08f, true, -72.0f, 0.0f },
        { "Commander Keen PC Beeper",     3, 18900.0f, 0,  4.0f, 0.35f, 0.15f, 0.05f, 0.20f, 0.02f, true, -72.0f, 0.0f },
        { "Budget Clone Piezo Screamer",  1, 22050.0f, 1, -3.0f, 0.20f, 0.10f, 0.10f, 0.30f, 0.04f, true, -72.0f, 0.0f },
        { "Sierra On-Line 1-Bit Voice",   2, 14000.0f, 0,  3.0f, 0.70f, 0.35f, 0.25f, 0.60f, 0.05f, true, -72.0f, 0.0f },
        { "Overdriven XT Motherboard",    0, 16000.0f, 0, 10.0f, 0.80f, 0.70f, 0.35f, 0.75f, 0.12f, true, -72.0f, 0.0f },
        { "Ultra-Clean 1-Bit Hi-Fi",      1, 24000.0f, 0, -1.0f, 0.20f, 0.00f, 0.00f, 0.05f, 0.01f, true, -72.0f, 0.0f },
        { "Arcade Steel Chassis Bezel",   0, 18900.0f, 0,  1.5f, 0.95f, 0.50f, 0.10f, 0.35f, 0.10f, true, -72.0f, 0.0f }
    };
    return presets;
}

int SpekaAudioProcessor::getNumPrograms()
{
    return static_cast<int> (getPresets().size());
}

int SpekaAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void SpekaAudioProcessor::setCurrentProgram (int index)
{
    loadPreset (index);
}

const juce::String SpekaAudioProcessor::getProgramName (int index)
{
    const auto& presets = getPresets();
    if (index >= 0 && index < static_cast<int> (presets.size()))
        return presets[static_cast<size_t> (index)].name;
    return {};
}

void SpekaAudioProcessor::changeProgramName (int, const juce::String&) {}

void SpekaAudioProcessor::loadPreset (int index)
{
    const auto& presets = getPresets();
    if (index < 0 || index >= static_cast<int> (presets.size()))
        return;

    currentProgram = index;
    const auto& p = presets[static_cast<size_t> (index)];

    if (auto* param = apvts.getParameter ("mode"))          param->setValueNotifyingHost (param->convertTo0to1 (p.mode));
    if (auto* param = apvts.getParameter ("carrierFreq"))   param->setValueNotifyingHost (param->convertTo0to1 (p.carrierFreq));
    if (auto* param = apvts.getParameter ("speakerType"))   param->setValueNotifyingHost (param->convertTo0to1 (p.speaker));
    if (auto* param = apvts.getParameter ("drive"))         param->setValueNotifyingHost (param->convertTo0to1 (p.driveDb));
    if (auto* param = apvts.getParameter ("caseResonance")) param->setValueNotifyingHost (param->convertTo0to1 (p.caseRes));
    if (auto* param = apvts.getParameter ("rattle"))        param->setValueNotifyingHost (param->convertTo0to1 (p.rattle));
    if (auto* param = apvts.getParameter ("jitter"))        param->setValueNotifyingHost (param->convertTo0to1 (p.jitter));
    if (auto* param = apvts.getParameter ("sag"))           param->setValueNotifyingHost (param->convertTo0to1 (p.sag));
    if (auto* param = apvts.getParameter ("whine"))         param->setValueNotifyingHost (param->convertTo0to1 (p.whine));
    if (auto* param = apvts.getParameter ("autoGate"))      param->setValueNotifyingHost (p.autoGate ? 1.0f : 0.0f);
    if (auto* param = apvts.getParameter ("gateThresh"))    param->setValueNotifyingHost (param->convertTo0to1 (p.gateThreshDb));
    if (auto* param = apvts.getParameter ("trim"))          param->setValueNotifyingHost (param->convertTo0to1 (p.trimDb));
}

void SpekaAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    for (auto& eng : engines)
    {
        eng.prepare (sampleRate);
    }

    crtBuffer.fill (0.0f);
    crtBufferIndex.store (0, std::memory_order_relaxed);
}

void SpekaAudioProcessor::releaseResources()
{
    for (auto& eng : engines)
    {
        eng.reset();
    }
}

bool SpekaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& mainIn  = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();

    if (mainIn != mainOut)
        return false;

    return mainIn == juce::AudioChannelSet::mono()
        || mainIn == juce::AudioChannelSet::stereo();
}

void SpekaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Read parameter values
    int modeIdx          = static_cast<int> (modeParam->load (std::memory_order_relaxed));
    float carrierHz      = carrierFreqParam->load (std::memory_order_relaxed);
    int speakerIdx       = static_cast<int> (speakerTypeParam->load (std::memory_order_relaxed));
    float driveDb        = driveParam->load (std::memory_order_relaxed);
    float caseRes        = caseResonanceParam->load (std::memory_order_relaxed);
    float rattleAmt      = rattleParam->load (std::memory_order_relaxed);
    float jitterAmt      = jitterParam->load (std::memory_order_relaxed);
    float sagAmt         = sagParam->load (std::memory_order_relaxed);
    float whineAmt       = whineParam->load (std::memory_order_relaxed);
    bool autoGateOn      = (autoGateParam->load (std::memory_order_relaxed) > 0.5f);
    float gateThreshDb   = gateThreshParam->load (std::memory_order_relaxed);
    float mixVal         = mixParam->load (std::memory_order_relaxed);
    float trimVal        = trimParam->load (std::memory_order_relaxed);

    // Update telemetry for GUI
    float ticks = std::max (2.0f, std::floor (1193182.0f / carrierHz));
    timerTicks.store (ticks, std::memory_order_relaxed);
    effectiveBits.store (std::log2 (ticks), std::memory_order_relaxed);

    auto modMode = static_cast<Speka::ModulationMode> (std::clamp (modeIdx, 0, 3));
    auto spkType = (speakerIdx == 1) ? Speka::SpeakerType::PiezoBuzzer12mm
                                     : Speka::SpeakerType::DynamicPaper2Inch;

    // Apply parameters to DSP engines
    for (auto& eng : engines)
    {
        eng.setModulationMode (modMode);
        eng.setCarrierFrequency (carrierHz);
        eng.setSpeakerType (spkType);
        eng.setDriveDb (driveDb);
        eng.setChassisResonance (caseRes);
        eng.setChassisRattle (rattleAmt);
        eng.setJitterAmount (jitterAmt);
        eng.setPowerSag (sagAmt);
        eng.setCarrierBleed (whineAmt);
        eng.setAutoGate (autoGateOn);
        eng.setGateThresholdDb (gateThreshDb);
        eng.setWetDryMix (mixVal);
        eng.setOutputTrimDb (trimVal);
    }

    const int numSamples = buffer.getNumSamples();
    float maxBlockExcursion = 0.0f;
    bool anySignal = false;

    int writeIdx = crtBufferIndex.load (std::memory_order_relaxed);

    if (totalNumInputChannels == 1)
    {
        auto* chData = buffer.getWritePointer (0);
        for (int i = 0; i < numSamples; ++i)
        {
            float s = chData[i];
            if (std::abs (s) > 0.0005f) anySignal = true;

            float out = engines[0].processSample (s);
            chData[i] = out;

            crtBuffer[writeIdx] = out;
            writeIdx = (writeIdx + 1) % kCrtBufferSize;
        }

        if (totalNumOutputChannels > 1)
            buffer.copyFrom (1, 0, buffer, 0, 0, numSamples);
    }
    else
    {
        auto* left  = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);

        for (int i = 0; i < numSamples; ++i)
        {
            float sL = left[i];
            float sR = right[i];
            if (std::abs (sL) > 0.0005f || std::abs (sR) > 0.0005f) anySignal = true;

            float outL = engines[0].processSample (sL);
            float outR = engines[1].processSample (sR);

            left[i]  = outL;
            right[i] = outR;

            crtBuffer[writeIdx] = (outL + outR) * 0.5f;
            writeIdx = (writeIdx + 1) % kCrtBufferSize;
        }
    }

    crtBufferIndex.store (writeIdx, std::memory_order_relaxed);
    gateActive.store (anySignal, std::memory_order_relaxed);

    // Approximate excursion based on output peak level for smooth visual speaker pulse
    maxBlockExcursion = buffer.getMagnitude (0, numSamples);
    float prevExc = currentExcursion.load (std::memory_order_relaxed);
    float targetExc = std::clamp (maxBlockExcursion * 1.5f, 0.0f, 1.0f);
    currentExcursion.store (prevExc + 0.25f * (targetExc - prevExc), std::memory_order_relaxed);
}

void SpekaAudioProcessor::getCrtWaveform (std::array<float, kCrtBufferSize>& dest) const
{
    int currentIdx = crtBufferIndex.load (std::memory_order_relaxed);
    for (int i = 0; i < kCrtBufferSize; ++i)
    {
        int readIdx = (currentIdx + i) % kCrtBufferSize;
        dest[i] = crtBuffer[readIdx];
    }
}

juce::AudioProcessorEditor* SpekaAudioProcessor::createEditor()
{
    return new SpekaAudioProcessorEditor (*this);
}

bool SpekaAudioProcessor::hasEditor() const
{
    return true;
}

void SpekaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SpekaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout SpekaAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // 1. Mode (RealSound PWM, Sigma-Delta, DeltaMod, Direct 1-Bit)
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "mode", 1 }, "Modulation Mode",
        juce::StringArray { "RealSound PWM", "Sigma-Delta PDM", "Delta Modulation", "Direct 1-Bit" }, 0));

    // 2. Carrier Frequency (Hz)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "carrierFreq", 1 }, "Carrier Freq",
        juce::NormalisableRange<float> (6000.0f, 28000.0f, 1.0f, 0.65f), 18900.0f,
        juce::AudioParameterFloatAttributes().withLabel ("Hz")));

    // 3. Speaker Transducer Type
    params.push_back (std::make_unique<juce::AudioParameterChoice> (
        juce::ParameterID { "speakerType", 1 }, "Transducer",
        juce::StringArray { "2.25\" Paper Cone (IBM 5150)", "12mm Ceramic Piezo" }, 0));

    // 4. Input Drive (dB)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "drive", 1 }, "Drive",
        juce::NormalisableRange<float> (-12.0f, 24.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    // 5. Chassis Resonance (0.0 to 1.0)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "caseResonance", 1 }, "Chassis Cavity",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.60f));

    // 6. Chassis Rattle (0.0 to 1.0)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "rattle", 1 }, "Panel Rattle",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.35f));

    // 7. Jitter / Timing Drift (0.0 to 1.0)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "jitter", 1 }, "CPU Jitter",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.15f));

    // 8. Power Supply Sag (0.0 to 1.0)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "sag", 1 }, "Rail Droop",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.50f));

    // 9. Carrier Bleed / Whistle (0.0 to 1.0)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "whine", 1 }, "Carrier Bleed",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.05f));

    // 10. Auto-Gate Enable
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "autoGate", 1 }, "Auto-Gate (Port 0x61)", true));

    // 11. Gate Threshold (dBFS)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "gateThresh", 1 }, "Gate Threshold",
        juce::NormalisableRange<float> (-90.0f, -40.0f, 0.5f), -72.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dBFS")));

    // 12. Wet/Dry Mix
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 }, "Dry/Wet",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel ("%")));

    // 13. Output Trim (dB)
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "trim", 1 }, "Output Trim",
        juce::NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    return { params.begin(), params.end() };
}

// JUCE Plugin Creation Macro
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpekaAudioProcessor();
}
