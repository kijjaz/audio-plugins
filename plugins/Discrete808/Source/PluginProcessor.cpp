#include "PluginProcessor.h"
#include "DSP/SharedResources.h"
#include "PluginEditor.h"

Discrete808AudioProcessor::Discrete808AudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
  // Collect all voices for easier iteration
  voices = {&kick, &snare,   &lowTom,  &midTom,    &highTom, &rimshot,
            &clap, &cowbell, &maracas, &closedHat, &openHat, &cymbal};
}

Discrete808AudioProcessor::~Discrete808AudioProcessor() {}

const juce::String Discrete808AudioProcessor::getName() const {
  return JucePlugin_Name;
}
bool Discrete808AudioProcessor::acceptsMidi() const { return true; }
bool Discrete808AudioProcessor::producesMidi() const { return false; }
bool Discrete808AudioProcessor::isMidiEffect() const { return false; }
double Discrete808AudioProcessor::getTailLengthSeconds() const { return 0.5; }
int Discrete808AudioProcessor::getNumPrograms() { return 1; }
int Discrete808AudioProcessor::getCurrentProgram() { return 0; }
void Discrete808AudioProcessor::setCurrentProgram(int index) {}
const juce::String Discrete808AudioProcessor::getProgramName(int index) {
  return {};
}
void Discrete808AudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName) {
}

void Discrete808AudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  for (auto *v : voices) {
    v->prepare(sampleRate);
  }
  globalAccent = 0.0f;
}

void Discrete808AudioProcessor::releaseResources() {}

bool Discrete808AudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
      layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
    return false;
  return true;
}

void Discrete808AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumOutputChannels = getTotalNumOutputChannels();
  buffer.clear();

  // 1. Update Global Parameters from APVTS
  // In a real implementation, we'd use listeners/cached values for efficiency.
  // This is the simplified v2 update loop.

  // Update Shared Resources (Noise seed/sync)
  SharedResources::getInstance().updateNoise();

  // Map APVTS to Voice Parameters
  // We'll use a prefix-based naming convention: e.g., "KICK_LEVEL", "KICK_TUNE"
  auto updateVoiceParams = [&](Voice *v, const juce::String &prefix,
                               const std::vector<int> &paramIndices) {
    for (int i = 0; i < paramIndices.size(); ++i) {
      juce::String paramID =
          prefix + "_" + juce::String(i); // This needs to match layout
      // For now, let's just do a manual mapping for the main ones
    }
  };

  // Manual Parameter Push (Optimized version would use Atomic floats)
  kick.setParameter(KickVoice::kTune, *apvts.getRawParameterValue("KICK_TUNE"));
  kick.setParameter(KickVoice::kDecay,
                    *apvts.getRawParameterValue("KICK_DECAY"));
  kick.setParameter(KickVoice::kTone, *apvts.getRawParameterValue("KICK_TONE"));
  kick.setParameter(KickVoice::kLevel,
                    *apvts.getRawParameterValue("KICK_LEVEL"));

  snare.setParameter(SnareVoice::kTune,
                     *apvts.getRawParameterValue("SNARE_TUNE"));
  snare.setParameter(SnareVoice::kSnappy,
                     *apvts.getRawParameterValue("SNARE_SNAPPY"));
  snare.setParameter(SnareVoice::kTone,
                     *apvts.getRawParameterValue("SNARE_TONE"));
  snare.setParameter(SnareVoice::kLevel,
                     *apvts.getRawParameterValue("SNARE_LEVEL"));

  lowTom.setParameter(TomVoice::kTune,
                      *apvts.getRawParameterValue("LOWTOM_TUNE"));
  lowTom.setParameter(TomVoice::kDecay,
                      *apvts.getRawParameterValue("LOWTOM_DECAY"));
  lowTom.setParameter(TomVoice::kLevel,
                      *apvts.getRawParameterValue("LOWTOM_LEVEL"));

  midTom.setParameter(TomVoice::kTune,
                      *apvts.getRawParameterValue("MIDTOM_TUNE"));
  midTom.setParameter(TomVoice::kDecay,
                      *apvts.getRawParameterValue("MIDTOM_DECAY"));
  midTom.setParameter(TomVoice::kLevel,
                      *apvts.getRawParameterValue("MIDTOM_LEVEL"));

  highTom.setParameter(TomVoice::kTune,
                       *apvts.getRawParameterValue("HIGHTOM_TUNE"));
  highTom.setParameter(TomVoice::kDecay,
                       *apvts.getRawParameterValue("HIGHTOM_DECAY"));
  highTom.setParameter(TomVoice::kLevel,
                       *apvts.getRawParameterValue("HIGHTOM_LEVEL"));

  rimshot.setParameter(RimshotVoice::kTune,
                       *apvts.getRawParameterValue("RIMSHOT_TUNE"));
  rimshot.setParameter(RimshotVoice::kLevel,
                       *apvts.getRawParameterValue("RIMSHOT_LEVEL"));

  clap.setParameter(ClapVoice::kTone, *apvts.getRawParameterValue("CLAP_TONE"));
  clap.setParameter(ClapVoice::kDecay,
                    *apvts.getRawParameterValue("CLAP_DECAY"));
  clap.setParameter(ClapVoice::kLevel,
                    *apvts.getRawParameterValue("CLAP_LEVEL"));

  cowbell.setParameter(CowbellVoice::kTune,
                       *apvts.getRawParameterValue("COWBELL_TUNE"));
  cowbell.setParameter(CowbellVoice::kDecay,
                       *apvts.getRawParameterValue("COWBELL_DECAY"));
  cowbell.setParameter(CowbellVoice::kLevel,
                       *apvts.getRawParameterValue("COWBELL_LEVEL"));

  maracas.setParameter(MaracasVoice::kLevel,
                       *apvts.getRawParameterValue("MARACAS_LEVEL"));

  closedHat.setParameter(HiHatVoice::kDecay,
                         *apvts.getRawParameterValue("CLOSEDHAT_DECAY"));
  closedHat.setParameter(HiHatVoice::kTone,
                         *apvts.getRawParameterValue("CLOSEDHAT_TONE"));
  closedHat.setParameter(HiHatVoice::kLevel,
                         *apvts.getRawParameterValue("CLOSEDHAT_LEVEL"));

  openHat.setParameter(HiHatVoice::kDecay,
                       *apvts.getRawParameterValue("OPENHAT_DECAY"));
  openHat.setParameter(HiHatVoice::kTone,
                       *apvts.getRawParameterValue("OPENHAT_TONE"));
  openHat.setParameter(HiHatVoice::kLevel,
                       *apvts.getRawParameterValue("OPENHAT_LEVEL"));

  cymbal.setParameter(CymbalVoice::kTune,
                      *apvts.getRawParameterValue("CYMBAL_TUNE"));
  cymbal.setParameter(CymbalVoice::kDecay,
                      *apvts.getRawParameterValue("CYMBAL_DECAY"));
  cymbal.setParameter(CymbalVoice::kLevel,
                      *apvts.getRawParameterValue("CYMBAL_LEVEL"));

  // Global Accent & Master Level
  globalAccent = *apvts.getRawParameterValue("MASTER_ACCENT");
  float masterGain = *apvts.getRawParameterValue("MASTER_LEVEL");
  SharedResources::getInstance().setAccentLevel(globalAccent);

  auto updateVelSens = [&](Voice &v, const juce::String &prefix) {
    v.setParameter(4, *apvts.getRawParameterValue(prefix + "_VEL_TONE"));
    v.setParameter(5, *apvts.getRawParameterValue(prefix + "_VEL_DECAY"));
  };

  updateVelSens(kick, "KICK");
  updateVelSens(snare, "SNARE");
  updateVelSens(lowTom, "LOWTOM");
  updateVelSens(midTom, "MIDTOM");
  updateVelSens(highTom, "HIGHTOM");
  updateVelSens(rimshot, "RIMSHOT");
  updateVelSens(clap, "CLAP");
  updateVelSens(cowbell, "COWBELL");
  updateVelSens(maracas, "MARACAS");
  updateVelSens(closedHat, "CLOSEDHAT");
  updateVelSens(openHat, "OPENHAT");
  updateVelSens(cymbal, "CYMBAL");

  // 2. Handle MIDI (GM Standard)
  for (const auto metadata : midiMessages) {
    auto msg = metadata.getMessage();
    if (msg.isNoteOn()) {
      float vel = msg.getFloatVelocity();
      int note = msg.getNoteNumber();

      switch (note) {
      case 36:
        kick.trigger(vel);
        break;
      case 37:
        rimshot.trigger(vel);
        break;
      case 38:
      case 40:
        snare.trigger(vel);
        break;
      case 39:
        clap.trigger(vel);
        break;
      case 41:
      case 43:
        lowTom.trigger(vel);
        break;
      case 45:
      case 47:
        midTom.trigger(vel);
        break;
      case 48:
      case 50:
        highTom.trigger(vel);
        break;
      case 42:
      case 44:
        closedHat.trigger(vel);
        break;
      case 46:
        openHat.trigger(vel);
        break;
      case 49:
      case 51:
      case 52:
        cymbal.trigger(vel);
        break;
      case 56:
        cowbell.trigger(vel);
        break;
      case 70:
        maracas.trigger(vel);
        break;
      }
    }
  }

  // 3. Render Audio
  int numSamples = buffer.getNumSamples();
  for (int s = 0; s < numSamples; ++s) {
    // Update noise sample for this time step
    SharedResources::getInstance().updateNoise();

    float sampleOutput = 0.0f;
    for (auto *v : voices) {
      sampleOutput += (float)v->renderSample();
    }

    // Write to stereo buffer
    for (int ch = 0; ch < totalNumOutputChannels; ++ch) {
      buffer.setSample(ch, s, sampleOutput * masterGain);
    }
  }
}

juce::AudioProcessorValueTreeState::ParameterLayout
Discrete808AudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  // Master
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MASTER_ACCENT", "Accent", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MASTER_LEVEL", "Output Level", 0.0f, 1.0f, 0.7f));

  // Kick
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "KICK_TUNE", "Kick Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "KICK_DECAY", "Kick Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "KICK_TONE", "Kick Tone", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "KICK_LEVEL", "Kick Level", 0.0f, 1.0f, 0.8f));

  // Snare
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "SNARE_TUNE", "Snare Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "SNARE_SNAPPY", "Snare Snappy", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "SNARE_TONE", "Snare Tone", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "SNARE_LEVEL", "Snare Level", 0.0f, 1.0f, 0.7f));

  // Toms
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "LOWTOM_TUNE", "LT Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "LOWTOM_DECAY", "LT Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "LOWTOM_LEVEL", "LT Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MIDTOM_TUNE", "MT Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MIDTOM_DECAY", "MT Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MIDTOM_LEVEL", "MT Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "HIGHTOM_TUNE", "HT Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "HIGHTOM_DECAY", "HT Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "HIGHTOM_LEVEL", "HT Level", 0.0f, 1.0f, 0.7f));

  // Percussion
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "RIMSHOT_TUNE", "Rim Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "RIMSHOT_LEVEL", "Rim Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLAP_TONE", "Clap Tone", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLAP_DECAY", "Clap Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLAP_LEVEL", "Clap Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "COWBELL_TUNE", "Cowbell Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "COWBELL_DECAY", "Cowbell Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "COWBELL_LEVEL", "Cowbell Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "MARACAS_LEVEL", "Maracas Level", 0.0f, 1.0f, 0.7f));

  // Hats/Cymbal
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLOSEDHAT_DECAY", "CH Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLOSEDHAT_TONE", "CH Tone", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CLOSEDHAT_LEVEL", "CH Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "OPENHAT_DECAY", "OH Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "OPENHAT_TONE", "OH Tone", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "OPENHAT_LEVEL", "OH Level", 0.0f, 1.0f, 0.7f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CYMBAL_TUNE", "Cym Tune", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CYMBAL_DECAY", "Cym Decay", 0.0f, 1.0f, 0.5f));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "CYMBAL_LEVEL", "Cym Level", 0.0f, 1.0f, 0.7f));

  auto addVelSens = [&](const juce::String &prefix) {
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        prefix + "_VEL_TONE", prefix + " Vel->Tone", 0.0f, 1.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        prefix + "_VEL_DECAY", prefix + " Vel->Decay", 0.0f, 1.0f, 0.2f));
  };

  addVelSens("KICK");
  addVelSens("SNARE");
  addVelSens("LOWTOM");
  addVelSens("MIDTOM");
  addVelSens("HIGHTOM");
  addVelSens("RIMSHOT");
  addVelSens("CLAP");
  addVelSens("COWBELL");
  addVelSens("MARACAS");
  addVelSens("CLOSEDHAT");
  addVelSens("OPENHAT");
  addVelSens("CYMBAL");

  return {params.begin(), params.end()};
}

bool Discrete808AudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor *Discrete808AudioProcessor::createEditor() {
  return new Discrete808AudioProcessorEditor(*this);
}

void Discrete808AudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void Discrete808AudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState != nullptr)
    apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new Discrete808AudioProcessor();
}
