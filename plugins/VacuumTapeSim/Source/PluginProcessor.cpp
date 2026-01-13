#include "PluginProcessor.h"
#include "PluginEditor.h"

VacuumTapeAudioProcessor::VacuumTapeAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

VacuumTapeAudioProcessor::~VacuumTapeAudioProcessor() {}

void VacuumTapeAudioProcessor::prepareToPlay(double sampleRate,
                                             int samplesPerBlock) {
  juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 2};

  // Initialize oversamplers
  oversampler2x = std::make_unique<juce::dsp::Oversampling<float>>(
      2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true,
      true);
  oversampler4x = std::make_unique<juce::dsp::Oversampling<float>>(
      2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true,
      true);

  oversampler2x->initProcessing(samplesPerBlock);
  oversampler4x->initProcessing(samplesPerBlock);

  // Prepare DSP at highest possible rate initially, processBlock will handle
  // rate changes if needed But actually we should prepare with current state
  float upsampleFactor = 4.0f; // Default
  auto upspec = spec;
  upspec.sampleRate *= upsampleFactor;

  for (int i = 0; i < 2; ++i) {
    vacuum[i].prepare(upspec);
    loss[i].prepare(upspec);
  }
  transport.prepare(upspec);
}

void VacuumTapeAudioProcessor::releaseResources() {}

void VacuumTapeAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                            juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;

  // Get parameter values
  auto drive = apvts.getRawParameterValue("TAPE_DRIVE")->load();
  auto hysteresisVal = apvts.getRawParameterValue("TAPE_HYST")->load();
  auto speedChoice = (int)apvts.getRawParameterValue("TAPE_SPEED")->load();
  float speedIps = 30.0f;
  if (speedChoice == 0)
    speedIps = 9.5f;
  else if (speedChoice == 1)
    speedIps = 15.0f;

  auto wow = apvts.getRawParameterValue("TAPE_WOW")->load();
  auto flutter = apvts.getRawParameterValue("TAPE_FLUTTER")->load();

  auto threshold = apvts.getRawParameterValue("COMP_THRESH")->load();
  auto drain = apvts.getRawParameterValue("COMP_DRAIN")->load();
  auto recovery = apvts.getRawParameterValue("COMP_RECOV")->load();
  auto time = apvts.getRawParameterValue("COMP_TIME")->load();

  auto outputVol = apvts.getRawParameterValue("OUTPUT_VOL")->load();
  auto oversamplingChoice = (int)apvts.getRawParameterValue("OVERSAMPLING")
                                ->load(); // 0: 1x, 1: 2x, 2: 4x

  int newUpsampling =
      (oversamplingChoice == 0) ? 1 : (oversamplingChoice == 1 ? 2 : 4);

  if (newUpsampling != currentOversampling) {
    currentOversampling = newUpsampling;
    double rate = getSampleRate() * currentOversampling;
    juce::dsp::ProcessSpec spec{rate, (juce::uint32)buffer.getNumSamples(), 2};
    for (int i = 0; i < 2; ++i) {
      vacuum[i].prepare(spec);
      loss[i].prepare(spec);
    }
    transport.prepare(spec);
  }

  // Update DSP params
  for (int i = 0; i < 2; ++i) {
    vacuum[i].setParameters(threshold, drain, recovery, time);
    hysteresis[i].setParameters(drive, hysteresisVal, 100.0f); // Fast tracking
    loss[i].setSpeed(speedIps);
  }
  transport.setParameters(wow * 0.05f,
                          flutter * 0.05f); // Gentler scaling (max ~5ms)

  juce::dsp::AudioBlock<float> block(buffer);

  if (currentOversampling == 1) {
    processUpsampledBlock(block);
  } else if (currentOversampling == 2) {
    auto upsampledBlock = oversampler2x->processSamplesUp(block);
    processUpsampledBlock(upsampledBlock);
    oversampler2x->processSamplesDown(block);
  } else {
    auto upsampledBlock = oversampler4x->processSamplesUp(block);
    processUpsampledBlock(upsampledBlock);
    oversampler4x->processSamplesDown(block);
  }

  // Apply output volume
  buffer.applyGain(juce::Decibels::decibelsToGain(outputVol));
}

void VacuumTapeAudioProcessor::processUpsampledBlock(
    juce::dsp::AudioBlock<float> &block) {

  float maxGR = 0.0f;
  float maxSat = 0.0f;

  for (int channel = 0; channel < (int)block.getNumChannels(); ++channel) {
    auto *channelData = block.getChannelPointer(channel);

    for (int sample = 0; sample < (int)block.getNumSamples(); ++sample) {
      float s = channelData[sample];

      // 1. Vacuum Compression
      s = vacuum[channel].processSample(s);
      maxGR = std::max(maxGR, 1.0f - vacuum[channel].getCurrentVoltage());

      // 2. Tape Hysteresis (Saturation)
      s = hysteresis[channel].processSample(s);
      maxSat = std::max(maxSat, hysteresis[channel].getDistortionAmount());

      // 3. Tape Loss (Frequency response)
      s = loss[channel].processSample(s);

      // 4. Transport (Wow/Flutter)
      s = transport.processSample(channel, s);

      channelData[sample] = s;
    }
  }

  // Update atomic meters (with a bit of smoothing/decay if needed, but simple
  // peak for now)
  gainReductionLevel.store(maxGR);
  saturationLevel.store(maxSat);
}

juce::AudioProcessorValueTreeState::ParameterLayout
VacuumTapeAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  auto floatToString = [](float value, int) { return juce::String(value, 2); };
  auto stringToFloat = [](const juce::String &text) {
    return text.getFloatValue();
  };

  // COMPRESSOR GROUP
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("COMP_THRESH", 1), "Threshold",
      juce::NormalisableRange<float>(0.01f, 1.0f), 0.5f, "size",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("COMP_DRAIN", 1), "Drain",
      juce::NormalisableRange<float>(0.1f, 50.0f), 10.0f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("COMP_RECOV", 1), "Recovery",
      juce::NormalisableRange<float>(0.1f, 50.0f), 5.0f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("COMP_TIME", 1), "Comp Time",
      juce::NormalisableRange<float>(0.01f, 1.0f), 0.5f, "size",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));

  // TAPE GROUP
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("TAPE_DRIVE", 1), "Drive",
      juce::NormalisableRange<float>(1.0f, 10.0f), 1.0f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("TAPE_HYST", 1), "Hysteresis",
      juce::NormalisableRange<float>(0.0f, 0.5f), 0.1f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID("TAPE_SPEED", 1), "Tape Speed",
      juce::StringArray{"9.5 ips", "15 ips", "30 ips"}, 2));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("TAPE_WOW", 1), "Wow",
      juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("TAPE_FLUTTER", 1), "Flutter",
      juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f, "",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));

  // FINAL STAGE
  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      juce::ParameterID("OUTPUT_VOL", 1), "Output Volume",
      juce::NormalisableRange<float>(-24.0f, 24.0f), 0.0f, "dB",
      juce::AudioProcessorParameter::genericParameter, floatToString,
      stringToFloat));
  params.push_back(std::make_unique<juce::AudioParameterChoice>(
      juce::ParameterID("OVERSAMPLING", 1), "Oversampling",
      juce::StringArray{"1x", "2x", "4x"}, 2));

  return {params.begin(), params.end()};
}

juce::AudioProcessorEditor *VacuumTapeAudioProcessor::createEditor() {
  return new VacuumTapeAudioProcessorEditor(*this);
}

void VacuumTapeAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void VacuumTapeAudioProcessor::setStateInformation(const void *data,
                                                   int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Boilerplate for JUCE
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new VacuumTapeAudioProcessor();
}
