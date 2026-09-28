#include "PluginProcessor.h"
#include "PluginEditor.h"

IronStackAudioProcessor::IronStackAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

IronStackAudioProcessor::~IronStackAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
IronStackAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  juce::StringArray models = {
      "Fender '59 Bassman (5F6-A)",
      "Fender '65 Bassman (AA864)",
      "Ampeg B-15N Portaflex",
      "Ampeg B-100R Rocket Bass",
      "Marshall Super Bass 100",
      "Fender Twin Reverb (AB763)",
      "Marshall JCM800 / 1959 Plexi",
      "Vox AC30 Top Boost",
      "Mesa Dual Rectifier",
      "Soldano SLO-100"
  };
  layout.add(std::make_unique<juce::AudioParameterChoice>(
      "ampModel", "Amp Circuit Model", models, 0));

  juce::StringArray cabs = {
      "4x10 Bassman Neo (Tone3000)",
      "2x12 Twin C12N (Tone3000)",
      "2x15 '70 Bassman CTS (Tone3000)",
      "8x10 Ampeg SVT Fridge",
      "1x18 Acoustic 360 Horn",
      "2x10 Eminence Legend (Vented)",
      "4x10 Eminence Legend (Vented)",
      "2x12 Hartke Pro 2200 (Tone3000)",
      "4x12 Marshall 1960A (Tone3000)",
      "4x12 Mesa Recto V30 (Tone3000)",
      "2x12 '66 Bassman C12NA (Tone3000)",
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

void IronStackAudioProcessor::prepareToPlay(double sampleRate,
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

void IronStackAudioProcessor::releaseResources() {}

void IronStackAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
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

  auto selectedModel = static_cast<ToneStack::Model>(juce::jlimit(0, 9, modelIdx));
  toneStack.setModel(selectedModel);

  // Set preamp tube staging voicing: Bass head vs Guitar lead
  bool isBassHead = (selectedModel == ToneStack::Model::FenderBassman ||
                     selectedModel == ToneStack::Model::FenderBassmanAA864 ||
                     selectedModel == ToneStack::Model::AmpegB15N ||
                     selectedModel == ToneStack::Model::AmpegB100R ||
                     selectedModel == ToneStack::Model::MarshallSuperBass);
  inputTube.setAmpType(isBassHead ? TubeStage::AmpType::BassHead : TubeStage::AmpType::LeadGuitar);

  auto selectedCab = static_cast<Cabinet::Model>(juce::jlimit(0, 11, cabIdx));
  cabinetL.setModel(selectedCab);
  cabinetR.setModel(selectedCab);

  inputTube.setDrive(driveDb);
  toneStack.setKnobs(bass, mid, treble);

  // Level compensation factor so every amp head setting delivers uniform volume at the same gain setting
  float headComp = toneStack.getLevelCompensation();

  // 2. Process Mono Chain (In-place on Ch 0)
  for (int i = 0; i < numSamples; ++i) {
    float x = channelData[i];

    // Preamp
    x = inputTube.processSample(x);

    // Tone Stack with balanced inter-model loudness normalization
    x = toneStack.processSample(x) * headComp;

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
      // Standard studio reference (-18 dBFS RMS input) delivers -18 dBFS RMS output
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

juce::AudioProcessorEditor *IronStackAudioProcessor::createEditor() {
  return new IronStackEditor(*this);
}

void IronStackAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void IronStackAudioProcessor::setStateInformation(const void *data,
                                                 int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new IronStackAudioProcessor();
}
