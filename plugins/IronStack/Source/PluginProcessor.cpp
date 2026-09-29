#include "PluginProcessor.h"
#include "PluginEditor.h"

IronStackAudioProcessor::IronStackAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

IronStackAudioProcessor::~IronStackAudioProcessor() {}

bool IronStackAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const {
  const auto &mainInput = layouts.getMainInputChannelSet();
  const auto &mainOutput = layouts.getMainOutputChannelSet();

  // Valid input channel counts: Mono (1) or Stereo (2)
  if (mainInput != juce::AudioChannelSet::mono() &&
      mainInput != juce::AudioChannelSet::stereo())
    return false;

  // Valid output channel counts: Mono (1) or Stereo (2)
  if (mainOutput != juce::AudioChannelSet::mono() &&
      mainOutput != juce::AudioChannelSet::stereo())
    return false;

  // Supports:
  // 1. Mono In -> Mono Out
  // 2. Mono In -> Stereo Out
  // 3. Stereo In -> Stereo Out
  if (mainInput == juce::AudioChannelSet::stereo() &&
      mainOutput == juce::AudioChannelSet::mono())
    return false; // Stereo in -> Mono out is typically disallowed unless downmixed

  return true;
}

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
  layout.add(std::make_unique<juce::AudioParameterFloat>("presence", "Presence",
                                                         0.0f, 10.0f, 5.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("tight", "Tight",
                                                         0.0f, 10.0f, 2.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("sag", "Sag",
                                                         0.0f, 10.0f, 5.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>("spread", "Stereo Spread",
                                                         0.0f, 100.0f, 100.0f)); // % spread
  layout.add(std::make_unique<juce::AudioParameterBool>("bright", "Bright Switch", false));
  layout.add(std::make_unique<juce::AudioParameterFloat>("volume", "Volume",
                                                         0.0f, 1.0f, 0.5f));

  return layout;
}

void IronStackAudioProcessor::prepareToPlay(double sampleRate,
                                            int samplesPerBlock) {
  juce::dsp::ProcessSpec spec;
  spec.sampleRate = sampleRate;
  spec.maximumBlockSize = samplesPerBlock;
  spec.numChannels = 1;

  // Prepare Left / Mono Chain
  inputTubeL.prepare(spec);
  toneStackL.prepare(spec);

  // Prepare Right Chain (for Stereo In -> Stereo Out)
  inputTubeR.prepare(spec);
  toneStackR.prepare(spec);

  // Stereo Cabinets
  cabinetL.prepare(spec, Cabinet::Channel::Left);
  cabinetR.prepare(spec, Cabinet::Channel::Right);
}

void IronStackAudioProcessor::releaseResources() {}

void IronStackAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                          juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;

  int numSamples = buffer.getNumSamples();
  int numInChannels = getTotalNumInputChannels();
  int numOutChannels = getTotalNumOutputChannels();

  if (numSamples == 0 || numOutChannels == 0)
    return;

  // 1. Update Parameters
  int modelIdx = static_cast<int>(*apvts.getRawParameterValue("ampModel"));
  int cabIdx = static_cast<int>(*apvts.getRawParameterValue("cabModel"));
  float driveDb = *apvts.getRawParameterValue("drive");
  float bass = *apvts.getRawParameterValue("bass");
  float mid = *apvts.getRawParameterValue("mid");
  float treble = *apvts.getRawParameterValue("treble");
  float presence = *apvts.getRawParameterValue("presence");
  float tight = *apvts.getRawParameterValue("tight");
  float sag = *apvts.getRawParameterValue("sag");
  float spread = *apvts.getRawParameterValue("spread");
  bool bright = *apvts.getRawParameterValue("bright") > 0.5f;
  float vol = *apvts.getRawParameterValue("volume");

  auto selectedModel = static_cast<ToneStack::Model>(juce::jlimit(0, 9, modelIdx));
  toneStackL.setModel(selectedModel);
  toneStackR.setModel(selectedModel);
  toneStackL.setBright(bright);
  toneStackR.setBright(bright);

  // Set preamp tube staging voicing: Bass head vs Guitar lead
  bool isBassHead = (selectedModel == ToneStack::Model::FenderBassman ||
                     selectedModel == ToneStack::Model::FenderBassmanAA864 ||
                     selectedModel == ToneStack::Model::AmpegB15N ||
                     selectedModel == ToneStack::Model::AmpegB100R ||
                     selectedModel == ToneStack::Model::MarshallSuperBass);
  auto ampType = isBassHead ? TubeStage::AmpType::BassHead : TubeStage::AmpType::LeadGuitar;
  inputTubeL.setAmpType(ampType);
  inputTubeR.setAmpType(ampType);

  auto selectedCab = static_cast<Cabinet::Model>(juce::jlimit(0, 11, cabIdx));
  cabinetL.setModel(selectedCab);
  cabinetR.setModel(selectedCab);
  cabinetL.setPresence(presence);
  cabinetR.setPresence(presence);
  cabinetL.setStereoSpread(spread);
  cabinetR.setStereoSpread(spread);

  inputTubeL.setDrive(driveDb);
  inputTubeR.setDrive(driveDb);
  inputTubeL.setTight(tight);
  inputTubeR.setTight(tight);
  inputTubeL.setSag(sag);
  inputTubeR.setSag(sag);

  toneStackL.setKnobs(bass, mid, treble);
  toneStackR.setKnobs(bass, mid, treble);

  // Calibrated Reference Level Compensation:
  // Designed so that standard studio reference (-18 dBFS RMS input) delivers -18 dBFS RMS output
  // when Volume is at 12 o'clock (0.50 default setting) with default tone stack and drive.
  float headComp = toneStackL.getLevelCompensation();
  constexpr float kRefCompensation = 0.0811f;

  // Audio taper curve for master volume:
  // vol = 0.50 (12 o'clock default) -> 1.0 (0 dB unity gain)
  // vol = 0.00 -> 0.0 (-inf dB)
  // vol = 1.00 -> 3.98 (+12 dB boost)
  float masterGainLinear = 0.0f;
  if (vol > 0.001f) {
    float volDb = (vol <= 0.5f) 
      ? (-48.0f * (1.0f - vol / 0.5f))           // 0.0 to 0.5 maps to -48 dB .. 0 dB
      : (12.0f * ((vol - 0.5f) / 0.5f));          // 0.5 to 1.0 maps to 0 dB .. +12 dB
    masterGainLinear = std::pow(10.0f, volDb / 20.0f);
  }
  float totalGain = headComp * kRefCompensation * masterGainLinear;

  // 2. Process Audio Buses
  // Case A: Stereo In -> Stereo Out
  if (numInChannels >= 2 && numOutChannels >= 2) {
    auto *left = buffer.getWritePointer(0);
    auto *right = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i) {
      // Left channel preamp + tone stack + cabinet
      float xL = inputTubeL.processSample(left[i]);
      xL = toneStackL.processSample(xL);
      left[i] = cabinetL.processSample(xL) * totalGain;

      // Right channel preamp + tone stack + cabinet (independent stereo imaging)
      float xR = inputTubeR.processSample(right[i]);
      xR = toneStackR.processSample(xR);
      right[i] = cabinetR.processSample(xR) * totalGain;
    }
  }
  // Case B: Mono In -> Stereo Out
  else if (numInChannels == 1 && numOutChannels >= 2) {
    auto *left = buffer.getWritePointer(0);
    auto *right = buffer.getWritePointer(1);

    for (int i = 0; i < numSamples; ++i) {
      float mono = left[i];

      // Preamp & Tone Stack on Mono input
      float x = inputTubeL.processSample(mono);
      x = toneStackL.processSample(x);

      // Stereo split into dual cabinet simulation
      left[i] = cabinetL.processSample(x) * totalGain;
      right[i] = cabinetR.processSample(x) * totalGain;
    }
  }
  // Case C: Mono In -> Mono Out
  else {
    auto *channelData = buffer.getWritePointer(0);
    for (int i = 0; i < numSamples; ++i) {
      float x = channelData[i];
      x = inputTubeL.processSample(x);
      x = toneStackL.processSample(x);
      channelData[i] = cabinetL.processSample(x) * totalGain;
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
