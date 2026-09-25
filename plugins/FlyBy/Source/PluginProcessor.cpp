#include "PluginProcessor.h"
#include "PluginEditor.h"

FlyByAudioProcessor::FlyByAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

FlyByAudioProcessor::~FlyByAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout FlyByAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Flight Mode
    juce::StringArray modes = { "Manual 3D", "Swoop Dive", "Helical Spiral", "Zenith Flyover", "Hornet Flutter" };
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "flightMode", 1 }, "Flight Mode", modes, 0));

    // Flight Dynamics
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "speed", 1 }, "Flight Speed",
        juce::NormalisableRange<float>(0.05f, 4.0f, 0.01f, 0.5f), 0.25f,
        juce::AudioParameterFloatAttributes().withLabel("Hz")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "turbulence", 1 }, "Turbulence",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.15f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "doppler", 1 }, "Doppler Amount",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel("x")));

    // Altitude Bounds
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "altitudeMax", 1 }, "Ceiling (+Z)",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.1f), 2.5f,
        juce::AudioParameterFloatAttributes().withLabel("m")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "altitudeMin", 1 }, "Floor (-Z)",
        juce::NormalisableRange<float>(-2.5f, 0.0f, 0.1f), -0.8f,
        juce::AudioParameterFloatAttributes().withLabel("m")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "proximityRadius", 1 }, "Proximity",
        juce::NormalisableRange<float>(0.3f, 5.0f, 0.05f), 1.2f,
        juce::AudioParameterFloatAttributes().withLabel("m")));

    // Manual Spatial Coordinates
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "elevation", 1 }, "Elevation",
        juce::NormalisableRange<float>(-50.0f, 90.0f, 0.5f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("deg")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "azimuth", 1 }, "Azimuth",
        juce::NormalisableRange<float>(-180.0f, 180.0f, 0.5f), 0.0f,
        juce::AudioParameterFloatAttributes().withLabel("deg")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "distance", 1 }, "Distance",
        juce::NormalisableRange<float>(0.3f, 8.0f, 0.05f), 1.5f,
        juce::AudioParameterFloatAttributes().withLabel("m")));

    // Anthropometric Tuning
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "pinnaScale", 1 }, "Pinna Tuning",
        juce::NormalisableRange<float>(0.75f, 1.25f, 0.01f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel("x")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "elevationStrength", 1 }, "Elevation Depth",
        juce::NormalisableRange<float>(0.0f, 1.5f, 0.01f), 1.0f,
        juce::AudioParameterFloatAttributes().withLabel("x")));

    // Innovation 1: Transient-Preserving Elevation
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "crispness", 1 }, "Transient Crispness",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 70.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Innovation 4: Floor & Ceiling Boundary Proximity Reflectance
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "grounding", 1 }, "Room Grounding",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), 40.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    // Master / Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "dryWet", 1 }, "Dry / Wet",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f), 100.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    return { params.begin(), params.end() };
}

const juce::String FlyByAudioProcessor::getName() const { return "FlyBy"; }
bool FlyByAudioProcessor::acceptsMidi() const { return false; }
bool FlyByAudioProcessor::producesMidi() const { return false; }
bool FlyByAudioProcessor::isMidiEffect() const { return false; }
double FlyByAudioProcessor::getTailLengthSeconds() const { return 0.2; }
int FlyByAudioProcessor::getNumPrograms() { return 1; }
int FlyByAudioProcessor::getCurrentProgram() { return 0; }
void FlyByAudioProcessor::setCurrentProgram(int) {}
const juce::String FlyByAudioProcessor::getProgramName(int) { return {}; }
void FlyByAudioProcessor::changeProgramName(int, const juce::String&) {}

void FlyByAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/) {
    spatialModel.prepare(static_cast<float>(sampleRate));
    flightPathGen.prepare(static_cast<float>(sampleRate));
}

void FlyByAudioProcessor::releaseResources() {
    spatialModel.reset();
    flightPathGen.reset();
}

bool FlyByAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}

void FlyByAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/) {
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0) return;

    // Read parameter values
    int modeIdx = static_cast<int>(apvts.getRawParameterValue("flightMode")->load());
    auto mode = static_cast<FlyBy::DSP::FlightMode>(modeIdx);

    FlyBy::DSP::FlightPathGenerator::FlightConfig flightCfg;
    flightCfg.mode = mode;
    flightCfg.speedHz = apvts.getRawParameterValue("speed")->load();
    flightCfg.turbulence = apvts.getRawParameterValue("turbulence")->load();
    flightCfg.altitudeMax = apvts.getRawParameterValue("altitudeMax")->load();
    flightCfg.altitudeMin = apvts.getRawParameterValue("altitudeMin")->load();
    flightCfg.proximityRadius = apvts.getRawParameterValue("proximityRadius")->load();
    flightCfg.manualElevation = apvts.getRawParameterValue("elevation")->load();
    flightCfg.manualAzimuth = apvts.getRawParameterValue("azimuth")->load();
    flightCfg.manualDistance = apvts.getRawParameterValue("distance")->load();

    // Advance 3D trajectory position
    FlyBy::DSP::Vector3 pos = flightPathGen.advance(numSamples, flightCfg);

    // Update live atomic state for UI visualization
    currentX.store(pos.x, std::memory_order_relaxed);
    currentY.store(pos.y, std::memory_order_relaxed);
    currentZ.store(pos.z, std::memory_order_relaxed);
    
    FlyBy::DSP::SphericalCoord sp = FlyBy::DSP::SphericalCoord::fromCartesian(pos);
    currentAz.store(sp.azimuthDeg, std::memory_order_relaxed);
    currentEl.store(sp.elevationDeg, std::memory_order_relaxed);
    currentDist.store(sp.distanceMeters, std::memory_order_relaxed);

    // Update DSP spatial parameters
    FlyBy::DSP::AnthropometricModel::SpatialParams spatialParams;
    spatialParams.position = pos;
    spatialParams.pinnaScale = apvts.getRawParameterValue("pinnaScale")->load();
    spatialParams.elevationStrength = apvts.getRawParameterValue("elevationStrength")->load();
    spatialParams.dopplerAmount = apvts.getRawParameterValue("doppler")->load();
    spatialParams.crispness = apvts.getRawParameterValue("crispness")->load() * 0.01f;
    spatialParams.grounding = apvts.getRawParameterValue("grounding")->load() * 0.01f;
    spatialModel.update(spatialParams);

    // Audio Processing
    float dryWet = apvts.getRawParameterValue("dryWet")->load() * 0.01f;
    float dryGain = 1.0f - dryWet;
    float wetGain = dryWet;

    const float* inL = buffer.getReadPointer(0);
    const float* inR = (totalNumInputChannels > 1) ? buffer.getReadPointer(1) : inL;
    float* outL = buffer.getWritePointer(0);
    float* outR = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample) {
        // Downmix input to mono sound source in 3D space
        float monoIn = 0.5f * (inL[sample] + inR[sample]);
        
        float binauralL = 0.0f;
        float binauralR = 0.0f;
        spatialModel.processSample(monoIn, binauralL, binauralR);

        outL[sample] = inL[sample] * dryGain + binauralL * wetGain;
        outR[sample] = inR[sample] * dryGain + binauralR * wetGain;
    }
}

FlyByAudioProcessor::LiveFlightState FlyByAudioProcessor::getLiveFlightState() const {
    LiveFlightState s;
    s.posX = currentX.load(std::memory_order_relaxed);
    s.posY = currentY.load(std::memory_order_relaxed);
    s.posZ = currentZ.load(std::memory_order_relaxed);
    s.azimuthDeg = currentAz.load(std::memory_order_relaxed);
    s.elevationDeg = currentEl.load(std::memory_order_relaxed);
    s.distanceMeters = currentDist.load(std::memory_order_relaxed);
    return s;
}

bool FlyByAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* FlyByAudioProcessor::createEditor() {
    return new FlyByAudioProcessorEditor(*this);
}

void FlyByAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FlyByAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new FlyByAudioProcessor();
}
