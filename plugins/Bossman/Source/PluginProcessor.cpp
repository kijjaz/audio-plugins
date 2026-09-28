#include "PluginProcessor.h"
#include "PluginEditor.h"

Fender59AudioProcessor::Fender59AudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

Fender59AudioProcessor::~Fender59AudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
Fender59AudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  juce::StringArray models = {
      "Fender '59 Bassman",
      "Fender Twin Reverb",
      "Marshall JCM800",
      "Vox AC30 Top Boost",
      "Mesa Dual Rectifier",
      "Soldano SLO-100"
  };
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      "ampModel", "Amp Circuit Model", models, 0));

  juce::StringArray cabs = {
      "4x10 Jensen P10R (Fender Open)",
      "2x12 Jensen C12N (Twin Open)",
      "4x12 Greenback (Plexi Closed)",
      "4x12 Vintage 30 (Mesa Closed)",
      "2x12 Alnico Blue (Vox Chime)",
      "Bypass (Direct Out)"
  };
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      "cabModel", "Cabinet Simulation", cabs, 0));

  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "drive", "Drive", 0.0f, 60.0f, 20.0f)); // dB gain
  layout.add(std::make_unique<juce::AudioParameterFloat>("bass", "Bass", 0.0f,
                                                         10.0f, 5.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("mid", "Middle", 0.0f,
                                                         10.0f, 5.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("treble", "Treble",
                                                         0.0f, 10.0f, 5.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("volume", "Volume",
                                                         0.0f, 1.0f, 0.5f));

  return layout;
}

void Fender59AudioProcessor::prepareToPlay(double sampleRate,
                                           int samplesPerBlock) {
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = 1; // Mono processing for Preamp/ToneStack

  inputTube.prepare(spec);
  toneStack.prepare(spec);

  // Stereo Cabinets
  spec.numChannels = 1;
  cabinetL.prepare(spec, Cabinet::Channel::Left);
  cabinetR.prepare(spec, Cabinet::Channel::Right);
}

void Fender59AudioProcessor::releaseResources() {}

void Fender59AudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                          juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;

  // Mono Input
  auto *channelData = buffer.getWritePointer(0);
  int numSamples = buffer.getNumSamples();

  // 1. Update Parameters
  int modelIdx = static_cast<int>(*apvts.getRawParameterValue("ampModel"));
  int cabIdx = static_cast<int>(*apvts.getRawParameterValue("cabModel"));
  float driveDb = *apvts.getRawParameterValue("drive");
  float bass = *apvts.getRawParameterValue("bass");
  float mid = *apvts.getRawParameterValue("mid");
  float treble = *apvts.getRawParameterValue("treble");
  float vol = *apvts.getRawParameterValue("volume");

  toneStack.setModel(static_cast<ToneStack::Model>(juce::jlimit(0, 5, modelIdx)));
  auto selectedCab = static_cast<Cabinet::Model>(juce::jlimit(0, 5, cabIdx));
  cabinetL.setModel(selectedCab);
  cabinetR.setModel(selectedCab);

  inputTube.setDrive(driveDb);
  toneStack.setKnobs(bass, mid, treble);

  // 2. Process Mono Chain (In-place on Ch 0)
  for (int i = 0; i < numSamples; ++i) {
    float x = channelData[i];

    // Preamp
    x = inputTube.processSample(x);

    // Tone Stack
    x = toneStack.processSample(x);

    channelData[i] = x;
  }

  // 3. Stereo Split & Cabinet Sim
  // Copy Mono logic to Right channel if it exists
  if (getTotalNumOutputChannels() > 1) {
    auto *left = buffer.getWritePointer(0);
    auto *right = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i) {
      float mono = left[i];

      // Calibrated Output Compensation:
      // Calibrated so that standard studio reference (-18 dBFS RMS input) delivers -18 dBFS RMS output
      // when Volume is at 12 o'clock (0.50 middle setting) with default tone stack and drive.
      constexpr float kRefCompensation = 1.2415f;
      left[i] = cabinetL.processSample(mono) * kRefCompensation * vol;
      right[i] = cabinetR.processSample(mono) * kRefCompensation * vol;
    }
  } else {
    // Mono output fallback
    constexpr float kRefCompensation = 1.2415f;
    for (int i = 0; i < numSamples; ++i) {
      channelData[i] = cabinetL.processSample(channelData[i]) * kRefCompensation * vol;
    }
  }
}

juce::AudioProcessorEditor *Fender59AudioProcessor::createEditor() {
  return new juce::GenericAudioProcessorEditor(*this);
}

void Fender59AudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void Fender59AudioProcessor::setStateInformation(const void *data,
                                                 int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new Fender59AudioProcessor();
}
