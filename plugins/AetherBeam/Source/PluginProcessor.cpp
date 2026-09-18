#include <fstream>
#include <sstream>
#include <set>
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

    // Audience & Furnishing Occupancy Absorption (Sabine / Eyring physics)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"occupancy", 1}, "Occupancy / People",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Spatial Microphone Polar Patterns
    juce::StringArray micChoices;
    micChoices.add("Binaural HRTF");
    micChoices.add("ORTF Cardioid Pair");
    micChoices.add("Blumlein Fig-8 Pair");
    micChoices.add("Omni Stereo Pair");

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{"micPattern", 1}, "Microphone Pattern", micChoices, 0));

    // Mid/Side Stereo Width
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"stereoWidth", 1}, "Stereo Width",
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

    // ISO 9613-1 Microclimate & Surface Roughness
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"airTemp", 1}, "Air Temp (C)",
        juce::NormalisableRange<float>(0.0f, 40.0f, 0.5f), 20.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"airHumidity", 1}, "Humidity (%)",
        juce::NormalisableRange<float>(10.0f, 95.0f, 1.0f), 50.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"surfaceScattering", 1}, "Surface Roughness",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.25f));

    // Dry / Wet Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{"mix", 1}, "Dry / Wet Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.40f));

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
    const AetherAcoustics::SpaceData* spacePtr = nullptr;
    if (spaceIdx == 999 && hasCustomSpace)
    {
        spacePtr = &customSpace;
    }
    else
    {
        if (spaceIdx < 0 || spaceIdx >= static_cast<int>(spaces.size())) return;
        spacePtr = &spaces[static_cast<size_t>(spaceIdx)];
    }
    const auto& space = *spacePtr;

    currentSpaceIndex.store(spaceIdx);
    currentPositionIndex.store(posIdx);
    currentRt60 = space.rt60;
    currentVolume = space.volume;
    currentArea = space.area;

    std::vector<AetherAcoustics::RaySegment> newRays;
    AetherAcoustics::Vec3 newSrc, newLis;

    if (posIdx >= 0 && posIdx < static_cast<int>(space.positions.size()))
    {
        const auto& pos = space.positions[static_cast<size_t>(posIdx)];
        newSrc = pos.source;
        newLis = pos.listener;
        newRays = AetherAcoustics::computeRealtimeRays(newSrc, newLis, space.minBound, space.maxBound, space.id.c_str(), 4, 96);
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
        newRays = AetherAcoustics::computeRealtimeRays(newSrc, newLis, space.minBound, space.maxBound, space.id.c_str(), 4, 96);
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
    float occupancy = apvts.getRawParameterValue("occupancy")->load();
    float airTemp = apvts.getRawParameterValue("airTemp")->load();
    float airHumidity = apvts.getRawParameterValue("airHumidity")->load();
    float surfaceScat = apvts.getRawParameterValue("surfaceScattering")->load();
    int micPatIdx = static_cast<int>(apvts.getRawParameterValue("micPattern")->load());
    float stereoWidth = apvts.getRawParameterValue("stereoWidth")->load();

    MicPolarPattern micPattern = static_cast<MicPolarPattern>(juce::jlimit(0, 3, micPatIdx));
    currentMicPattern.store(micPattern, std::memory_order_relaxed);
    currentStereoWidth.store(stereoWidth, std::memory_order_relaxed);
    currentOccupancy.store(occupancy, std::memory_order_relaxed);
    currentAirTemp.store(airTemp, std::memory_order_relaxed);
    currentAirHumidity.store(airHumidity, std::memory_order_relaxed);
    currentSurfaceScattering.store(surfaceScat, std::memory_order_relaxed);

    fdn.prepare(sampleRate, currentRt60 * decayScale, currentVolume, currentArea);
    fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult, occupancy, airTemp, airHumidity);

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
    float occupancy = apvts.getRawParameterValue("occupancy")->load();
    float airTemp = apvts.getRawParameterValue("airTemp")->load();
    float airHumidity = apvts.getRawParameterValue("airHumidity")->load();
    float surfaceScat = apvts.getRawParameterValue("surfaceScattering")->load();
    int micPatIdx = static_cast<int>(apvts.getRawParameterValue("micPattern")->load());
    float stereoWidth = apvts.getRawParameterValue("stereoWidth")->load();

    MicPolarPattern micPattern = static_cast<MicPolarPattern>(juce::jlimit(0, 3, micPatIdx));
    currentMicPattern.store(micPattern, std::memory_order_relaxed);
    currentStereoWidth.store(stereoWidth, std::memory_order_relaxed);
    currentOccupancy.store(occupancy, std::memory_order_relaxed);

    int spaceIndex = static_cast<int>(apvts.getRawParameterValue("space")->load());
    int positionIndex = static_cast<int>(apvts.getRawParameterValue("position")->load());

    int currentS = currentSpaceIndex.load();
    int currentP = currentPositionIndex.load();

    if (spaceIndex != currentS || positionIndex != currentP)
    {
        switchSpaceAndPosition(spaceIndex, positionIndex);
        fdn.prepare(getSampleRate(), currentRt60 * decayScale, currentVolume, currentArea);
        fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult, occupancy, airTemp, airHumidity);
    }
    else
    {
        // Continuously update FDN RT60, 3-Band Material Damping, and Occupancy absorption per block
        fdn.updateAcousticParameters(currentRt60 * decayScale, dampFreq, hfMult, bassMult, occupancy, airTemp, airHumidity);
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
                auto newRays = AetherAcoustics::computeRealtimeRays({ sX, sY, sZ }, { lX, lY, lZ }, space.minBound, space.maxBound, space.id.c_str(), 2, 24);
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

    for (int n = 0; n < numSamples; ++n)
    {
        float inL = leftChannel[n];
        float inR = rightChannel[n];
        float inMono = (inL + inR) * 0.5f;

        float earlyL = 0.0f, earlyR = 0.0f, lateInject = 0.0f;
        waveguideArray.processSample(inMono, driveSPL, beta, micPattern, stereoWidth, airTemp, airHumidity, surfaceScat, earlyL, earlyR, lateInject);

        float lateL = 0.0f, lateR = 0.0f;
        fdn.processSample(lateInject, 1.0f, lateL, lateR);

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

bool AetherBeamAudioProcessor::loadCustomObjMesh(const juce::File& objFile)
{
    if (!objFile.existsAsFile()) return false;

    std::ifstream file(objFile.getFullPathName().toStdString());
    if (!file.is_open()) return false;

    std::vector<AetherAcoustics::Vec3> vertices;
    std::set<std::pair<int, int>> uniqueEdges;

    std::string line;
    float minX = 1e9f, minY = 1e9f, minZ = 1e9f;
    float maxX = -1e9f, maxY = -1e9f, maxZ = -1e9f;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back({ x, y, z });
            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
        }
        else if (token == "f")
        {
            std::vector<int> faceIndices;
            std::string vertRef;
            while (iss >> vertRef)
            {
                size_t slashPos = vertRef.find('/');
                std::string idxStr = (slashPos == std::string::npos) ? vertRef : vertRef.substr(0, slashPos);
                try {
                    int vIdx = std::stoi(idxStr) - 1;
                    faceIndices.push_back(vIdx);
                } catch (...) {}
            }

            size_t n = faceIndices.size();
            for (size_t i = 0; i < n; ++i)
            {
                int v0 = faceIndices[i];
                int v1 = faceIndices[(i + 1) % n];
                if (v0 >= 0 && v0 < static_cast<int>(vertices.size()) &&
                    v1 >= 0 && v1 < static_cast<int>(vertices.size()))
                {
                    uniqueEdges.insert({ std::min(v0, v1), std::max(v0, v1) });
                }
            }
        }
    }

    if (vertices.empty()) return false;

    customSpace.id = "custom_imported_mesh";
    customSpace.title = objFile.getFileNameWithoutExtension().toStdString() + " (User 3D Mesh)";
    customSpace.category = "User 3D Meshes";
    customSpace.minBound = { minX, minY, minZ };
    customSpace.maxBound = { maxX, maxY, maxZ };

    float dx = maxX - minX;
    float dy = maxY - minY;
    float dz = maxZ - minZ;

    customSpace.volume = std::max(50.0f, dx * dy * dz * 0.70f);
    customSpace.area = 2.0f * (dx * dy + dy * dz + dx * dz);
    customSpace.rt60 = std::clamp(0.161f * (customSpace.volume / (0.12f * customSpace.area)), 0.5f, 15.0f);

    std::vector<std::pair<int, int>> allEdges(uniqueEdges.begin(), uniqueEdges.end());
    customSpace.wireframe.clear();

    constexpr int maxWireEdges = 300;
    if (allEdges.size() <= static_cast<size_t>(maxWireEdges))
    {
        for (const auto& e : allEdges)
            customSpace.wireframe.push_back({ vertices[e.first], vertices[e.second] });
    }
    else
    {
        float step = static_cast<float>(allEdges.size()) / static_cast<float>(maxWireEdges);
        for (int i = 0; i < maxWireEdges; ++i)
        {
            size_t idx = static_cast<size_t>(static_cast<float>(i) * step);
            if (idx < allEdges.size())
                customSpace.wireframe.push_back({ vertices[allEdges[idx].first], vertices[allEdges[idx].second] });
        }
    }

    // Generate 4 standard acoustic positions inside custom bounds
    customSpace.positions.clear();
    {
        AetherAcoustics::PositionData p1;
        p1.id = 0;
        p1.name = "Front Stage to Center Room";
        p1.source = { minX + dx * 0.5f, minY + dy * 0.2f, minZ + dz * 0.2f };
        p1.listener = { minX + dx * 0.5f, minY + dy * 0.6f, minZ + dz * 0.25f };
        customSpace.positions.push_back(p1);
    }
    {
        AetherAcoustics::PositionData p2;
        p2.id = 1;
        p2.name = "Diagonal Cross Reflection";
        p2.source = { minX + dx * 0.2f, minY + dy * 0.15f, minZ + dz * 0.2f };
        p2.listener = { minX + dx * 0.8f, minY + dy * 0.75f, minZ + dz * 0.25f };
        customSpace.positions.push_back(p2);
    }
    {
        AetherAcoustics::PositionData p3;
        p3.id = 2;
        p3.name = "Elevated Gallery / Balcony Perspective";
        p3.source = { minX + dx * 0.5f, minY + dy * 0.25f, minZ + dz * 0.2f };
        p3.listener = { minX + dx * 0.5f, minY + dy * 0.85f, minZ + dz * 0.7f };
        customSpace.positions.push_back(p3);
    }
    {
        AetherAcoustics::PositionData p4;
        p4.id = 3;
        p4.name = "Wall Bounce Focus";
        p4.source = { minX + dx * 0.85f, minY + dy * 0.3f, minZ + dz * 0.2f };
        p4.listener = { minX + dx * 0.15f, minY + dy * 0.5f, minZ + dz * 0.2f };
        customSpace.positions.push_back(p4);
    }

    hasCustomSpace = true;
    switchSpaceAndPosition(999, 0); // 999 triggers customSpace
    return true;
}

