#include "PluginProcessor.h"
#include "PluginEditor.h"

AetherBeamAudioProcessor::AetherBeamAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input",  juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    switchSpaceAndPosition(0, 0);
}

juce::AudioProcessorValueTreeState::ParameterLayout AetherBeamAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray spaceNames;
    for (const auto& s : AetherAcoustics::AcousticDatabase::getSpaces())
    {
        spaceNames.add(s.title);
    }

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"space", 1}, "Acoustic Space", spaceNames, 0));

    // 4 positions + Custom drag position per space
    juce::StringArray positionChoices;
    for (int i = 0; i < 4; ++i)
        positionChoices.add("Preset " + juce::String(i + 1));
    positionChoices.add("Custom Interactive Position");

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"position", 1}, "Source/Listener Position", positionChoices, 0));

    // Normalized 0.0 to 1.0 coordinates inside current space bounding box
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sourceX", 1}, "Source X",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"sourceY", 1}, "Source Y",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.2f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"listenerX", 1}, "Listener X",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"listenerY", 1}, "Listener Y",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.7f));

    // Air Nonlinearity & Shock Wave Steepening
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"airNonlinearity", 1}, "Air Non-Linearity (Beta)",
        juce::NormalisableRange<float>(0.0f, 2.5f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"driveSPL", 1}, "Drive SPL (dB)",
        juce::NormalisableRange<float>(80.0f, 150.0f, 0.5f), 122.0f));

    // Reverb Time (RT60 Scale)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"decayScale", 1}, "RT60 Scale",
        juce::NormalisableRange<float>(0.2f, 2.5f, 0.01f), 1.0f));

    // Material Damping EQ:
    // 1. High Cutoff Frequency (carpet / curtains / audience absorption)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"dampFreq", 1}, "HF Damping",
        juce::NormalisableRange<float>(800.0f, 18000.0f, 10.0f, 0.35f), 6500.0f));

    // 2. HF Decay Multiplier (rate of treble decay relative to mid-band)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"hfDecayMult", 1}, "HF Mult",
        juce::NormalisableRange<float>(0.1f, 1.0f, 0.01f), 0.55f));

    // 3. Bass Decay Multiplier (wood resonance vs solid granite)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"bassDecayMult", 1}, "Bass Mult",
        juce::NormalisableRange<float>(0.2f, 2.0f, 0.01f), 1.0f));

    // Dry / Wet Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"mix", 1}, "Dry / Wet Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));

    // Engine Performance / Quality Mode (CPU optimization)
    juce::StringArray qualityChoices;
    qualityChoices.add("Eco Mode (Low CPU)");
    qualityChoices.add("Balanced Studio");
    qualityChoices.add("Ultra Physical (Full)");
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"qualityMode", 1}, "Engine Quality", qualityChoices, 1));

    return { params.begin(), params.end() };
}

AetherAcoustics::Vec3 AetherBeamAudioProcessor::getCurrentSourcePos() const
{
    std::lock_guard<std::mutex> lock(rayMutex);
    return currentSourcePos;
}

AetherAcoustics::Vec3 AetherBeamAudioProcessor::getCurrentListenerPos() const
{
    std::lock_guard<std::mutex> lock(rayMutex);
    return currentListenerPos;
}

std::vector<AetherAcoustics::RaySegment> AetherBeamAudioProcessor::getCurrentRays() const
{
    std::lock_guard<std::mutex> lock(rayMutex);
    return currentRays;
}

void AetherBeamAudioProcessor::switchSpaceAndPosition(int spaceIdx, int posIdx)
{
    const auto& spaces = AetherAcoustics::AcousticDatabase::getSpaces();
    if (spaceIdx < 0 || spaceIdx >= static_cast<int>(spaces.size())) return;

    currentSpaceIndex.store(spaceIdx);
    currentPositionIndex.store(posIdx);

    const auto& space = spaces[static_cast<size_t>(spaceIdx)];
    currentRt60 = space.rt60;
    currentVolume = space.volume;
    currentArea = space.area;
    currentDimX = std::abs(space.maxBound.x - space.minBound.x);
    currentDimY = std::abs(space.maxBound.y - space.minBound.y);
    currentDimZ = std::abs(space.maxBound.z - space.minBound.z);

    std::vector<AetherAcoustics::RaySegment> newRays;
    AetherAcoustics::Vec3 newSrc, newLis;

    if (posIdx >= 0 && posIdx < static_cast<int>(space.positions.size()))
    {
        const auto& pos = space.positions[static_cast<size_t>(posIdx)];
        newSrc = pos.source;
        newLis = pos.listener;
        newRays = AetherAcoustics::computeRealtimeRays(newSrc, newLis, space.minBound, space.maxBound, space.id, 4, 96);
    }
    else
    {
        float sXN = targetSourceXNorm.load();
        float sYN = targetSourceYNorm.load();
        float lXN = targetListenerXNorm.load();
        float lYN = targetListenerYNorm.load();

        float sX = space.minBound.x + juce::jlimit(0.02f, 0.98f, sXN) * (space.maxBound.x - space.minBound.x);
        float sY = space.minBound.y + juce::jlimit(0.02f, 0.98f, sYN) * (space.maxBound.y - space.minBound.y);
        float sZ = space.minBound.z + 1.2f;

        float lX = space.minBound.x + juce::jlimit(0.02f, 0.98f, lXN) * (space.maxBound.x - space.minBound.x);
        float lY = space.minBound.y + juce::jlimit(0.02f, 0.98f, lYN) * (space.maxBound.y - space.minBound.y);
        float lZ = space.minBound.z + 1.7f;

        newSrc = { sX, sY, sZ };
        newLis = { lX, lY, lZ };
        newRays = AetherAcoustics::computeRealtimeRays(newSrc, newLis, space.minBound, space.maxBound, space.id, 4, 96);
    }

    {
        std::lock_guard<std::mutex> lock(rayMutex);
        currentSourcePos = newSrc;
        currentListenerPos = newLis;
        currentRays = newRays;
    }

    updateAcousticPaths(newRays);
}

// Called from UI mouse drag thread - 100% lock-free, zero heap-allocation
void AetherBeamAudioProcessor::setCustomCoordinates(float srcXNorm, float srcYNorm, float lisXNorm, float lisYNorm)
{
    targetSourceXNorm.store(srcXNorm);
    targetSourceYNorm.store(srcYNorm);
    targetListenerXNorm.store(lisXNorm);
    targetListenerYNorm.store(lisYNorm);
    coordinatesDirty.store(true, std::memory_order_release);
}

void AetherBeamAudioProcessor::updateAcousticPaths(const std::vector<AetherAcoustics::RaySegment>& rays)
{
    std::array<AetherAcoustics::BeamDescriptor, NonlinearWaveguideArray::MAX_BEAM_PATHS> descriptors{};
    int count = std::min(static_cast<int>(rays.size()), NonlinearWaveguideArray::MAX_BEAM_PATHS);

    for (int i = 0; i < count; ++i)
    {
        const auto& ray = rays[static_cast<size_t>(i)];
        auto& desc = descriptors[static_cast<size_t>(i)];
        desc.distanceMeters = ray.distanceMeters;
        desc.delaySec = ray.distanceMeters / AetherAcoustics::SPEED_OF_SOUND;
        desc.gain = ray.gain;
        desc.order = ray.order;
        desc.absorptionFactor = std::pow(0.96f, static_cast<float>(ray.order));

        if (ray.points.size() >= 2)
        {
            size_t n = ray.points.size();
            float dx = ray.points[n - 1].x - ray.points[n - 2].x;
            float dy = ray.points[n - 1].y - ray.points[n - 2].y;
            float dz = ray.points[n - 1].z - ray.points[n - 2].z;
            float len = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (len > 1e-4f)
            {
                desc.dirX = dx / len;
                desc.dirY = dy / len;
                desc.dirZ = dz / len;
            }
        }
    }

    waveguideArray.updateBeams(descriptors.data(), count);
}

void AetherBeamAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    waveguideArray.prepare(sampleRate, 150.0f);
    
    float decayScale = apvts.getRawParameterValue("decayScale")->load();
    float dampFreq = apvts.getRawParameterValue("dampFreq")->load();
    float hfMult = apvts.getRawParameterValue("hfDecayMult")->load();
    float bassMult = apvts.getRawParameterValue("bassDecayMult")->load();

    fdn.prepare(sampleRate, currentRt60 * decayScale, currentVolume, currentArea,
                currentDimX, currentDimY, currentDimZ);
    fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult);

    tailDetector.prepare(sampleRate, -96.0f, 1.5f, 500.0f);
    updateAcousticPaths(currentRays);
}

void AetherBeamAudioProcessor::releaseResources()
{
    waveguideArray.reset();
    fdn.reset();
    tailDetector.reset();
}

bool AetherBeamAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AetherBeamAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();

    auto* leftChannel = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);

    // Read parameters
    float beta = apvts.getRawParameterValue("airNonlinearity")->load();
    float driveSPL = apvts.getRawParameterValue("driveSPL")->load();
    float mix = apvts.getRawParameterValue("mix")->load();
    float decayScale = apvts.getRawParameterValue("decayScale")->load();
    float dampFreq = apvts.getRawParameterValue("dampFreq")->load();
    float hfMult = apvts.getRawParameterValue("hfDecayMult")->load();
    float bassMult = apvts.getRawParameterValue("bassDecayMult")->load();

    int spaceIndex = static_cast<int>(apvts.getRawParameterValue("space")->load());
    int positionIndex = static_cast<int>(apvts.getRawParameterValue("position")->load());
    int qualityMode = static_cast<int>(apvts.getRawParameterValue("qualityMode")->load());

    int currentS = currentSpaceIndex.load();
    int currentP = currentPositionIndex.load();

    if (spaceIndex != currentS || positionIndex != currentP)
    {
        switchSpaceAndPosition(spaceIndex, positionIndex);
        fdn.reconfigureGeometry(currentRt60 * decayScale, currentVolume, currentArea,
                                currentDimX, currentDimY, currentDimZ);
        fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult);
    }
    else
    {
        // Continuously update FDN RT60 and 3-Band Material Damping parameters smoothly per block
        fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult);
    }

    if (coordinatesDirty.load(std::memory_order_relaxed))
    {
        // Rate-limit ray re-computation: update at most once every ~25ms (~40 Hz) during mouse dragging
        static int blocksSinceLastRayCalc = 0;
        if (++blocksSinceLastRayCalc >= 12) // ~12 blocks of 128 samples at 96kHz ~ 16ms
        {
            blocksSinceLastRayCalc = 0;
            coordinatesDirty.store(false, std::memory_order_release);

            const auto& spaces = AetherAcoustics::AcousticDatabase::getSpaces();
            if (currentS >= 0 && currentS < static_cast<int>(spaces.size()))
            {
                const auto& space = spaces[static_cast<size_t>(currentS)];
                float sXN = targetSourceXNorm.load();
                float sYN = targetSourceYNorm.load();
                float lXN = targetListenerXNorm.load();
                float lYN = targetListenerYNorm.load();

                float sX = space.minBound.x + juce::jlimit(0.02f, 0.98f, sXN) * (space.maxBound.x - space.minBound.x);
                float sY = space.minBound.y + juce::jlimit(0.02f, 0.98f, sYN) * (space.maxBound.y - space.minBound.y);
                float sZ = space.minBound.z + 1.2f;

                float lX = space.minBound.x + juce::jlimit(0.02f, 0.98f, lXN) * (space.maxBound.x - space.minBound.x);
                float lY = space.minBound.y + juce::jlimit(0.02f, 0.98f, lYN) * (space.maxBound.y - space.minBound.y);
                float lZ = space.minBound.z + 1.7f;

                // During active movement, compute 2nd order (24 rays) for effortless real-time glide
                auto newRays = AetherAcoustics::computeRealtimeRays({ sX, sY, sZ }, { lX, lY, lZ }, space.minBound, space.maxBound, space.id, 2, 24);
                {
                    std::unique_lock<std::mutex> lock(rayMutex, std::try_to_lock);
                    if (lock.owns_lock())
                    {
                        currentSourcePos = { sX, sY, sZ };
                        currentListenerPos = { lX, lY, lZ };
                        currentRays = newRays;
                    }
                }
                updateAcousticPaths(newRays);
            }
        }
    }

    // Check if input block has signal
    float blockInputMag = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        blockInputMag = std::max(blockInputMag, buffer.getMagnitude(ch, 0, numSamples));

    // Smart Sleep Optimization:
    // If input is silent (< -96 dBFS) and tail has completely finished decaying, skip DSP entirely (0.0% CPU)
    if (blockInputMag < 1.58e-5f && tailDetector.isSleeping())
    {
        if (mix < 1.0f)
        {
            // Just pass through dry signal (which is silent anyway)
            return;
        }
        buffer.clear();
        return;
    }

    for (int n = 0; n < numSamples; ++n)
    {
        float inL = leftChannel[n];
        float inR = rightChannel[n];
        float inMono = (inL + inR) * 0.5f;

        float earlyL = 0.0f, earlyR = 0.0f, lateInject = 0.0f;
        waveguideArray.processSample(inMono, driveSPL, beta, earlyL, earlyR, lateInject, qualityMode);

        float lateL = 0.0f, lateR = 0.0f;
        fdn.processSample(lateInject, 1.0f, lateL, lateR, qualityMode);

        float wetL = earlyL + lateL * 1.5f;
        float wetR = earlyR + lateR * 1.5f;

        // Apply studio tail detector
        float tailGate = tailDetector.process(wetL, wetR);
        wetL *= tailGate;
        wetR *= tailGate;

        // Dry/Wet Mix
        leftChannel[n] = (1.0f - mix) * inL + mix * wetL;
        rightChannel[n] = (1.0f - mix) * inR + mix * wetR;
    }
}

juce::AudioProcessorEditor* AetherBeamAudioProcessor::createEditor()
{
    return new AetherBeamAudioProcessorEditor(*this);
}

void AetherBeamAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AetherBeamAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AetherBeamAudioProcessor();
}
