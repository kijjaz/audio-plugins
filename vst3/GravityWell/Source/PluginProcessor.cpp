#include "PluginProcessor.h"
#include "GravityData.h"
#include "PluginEditor.h"

GravityWellAudioProcessor::GravityWellAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput(
          "Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {
  // Add 16 voices
  for (int i = 0; i < 16; ++i)
    synth.addVoice(new GravityVoice());

  // Add a sound
  synth.addSound(new GravitySound());
}

GravityWellAudioProcessor::~GravityWellAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
GravityWellAudioProcessor::createParameterLayout() {
  std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "speed", "Simulation Speed", 0.0f, 5.0f, 1.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "gravity", "Gravity Amount", 0.0f, 1.0f, 1.0f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "dust_amt", "Space Dust Amount", 0.0f, 1.0f, 0.2f));

  params.push_back(std::make_unique<juce::AudioParameterFloat>(
      "dust_dens", "Space Dust Density", 0.0f, 1.0f, 0.5f));

  return {params.begin(), params.end()};
}

void GravityWellAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock) {
  readHead = 0.0;
  synth.setCurrentPlaybackSampleRate(sampleRate);
  spaceDust.prepare(sampleRate);
}

void GravityWellAudioProcessor::releaseResources() {}

void GravityWellAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // Clear buffer
  for (auto i = 0; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // Params
  float speed = apvts.getRawParameterValue("speed")->load();
  float gravityAmt = apvts.getRawParameterValue("gravity")->load();
  double sampleRate = getSampleRate();
  if (sampleRate <= 0)
    sampleRate = 44100;

  // MIDI handled by synth.renderNextBlock later

  // DSP Loop
  float gravity = apvts.getRawParameterValue("gravity")->load();

  float dustAmt = 0.2f; // Default if not in tree
  if (auto *p = apvts.getRawParameterValue("dust_amt"))
    dustAmt = p->load();

  float dustDens = 0.5f;
  if (auto *p = apvts.getRawParameterValue("dust_dens"))
    dustDens = p->load();

  // 2. Physics Update (Once per block for control signals)
  double stepInc = (100.0 * speed) / getSampleRate();
  double blockStep = stepInc * buffer.getNumSamples();

  // Calculate "Current" Physics state for this block (Control Rate)
  // We interpolate at current readHead
  int idx = (int)readHead;
  int nextIdx = (idx + 1) % kGravityRows;
  float frac = (float)(readHead - idx);

  auto getVal = [&](int col) {
    return kGravityData[idx * kGravityCols + col] * (1.0f - frac) +
           kGravityData[nextIdx * kGravityCols + col] * frac;
  };

  // Update Read Head for next block
  readHead += blockStep;
  if (readHead >= kGravityRows)
    readHead -= kGravityRows;

  // Prepare Voice Params
  GravityVoice::VoiceParams vp;
  vp.gravityAmt = gravity;
  vp.d12 = getVal(6);
  float d23 = getVal(7);
  float v2 = getVal(10);
  float v3 = getVal(11);

  float pd12 = std::max(0.0f, 1.0f - vp.d12);
  vp.fmIdxPlanet = std::pow(pd12, 3.0f) * 2000.0f * gravity;
  vp.fmIdxMoon = std::pow(std::max(0.0f, 1.0f - d23), 2.0f) * 1000.0f * gravity;
  vp.detune2 = (v2 - 0.5f) * 10.0f * gravity;
  vp.flare = std::pow(v3, 4.0f) * 5.0f;

  auto getPan = [](float x, float &L, float &R) {
    float pan = std::max(-1.0f, std::min(x, 1.0f));
    L = 0.5f * (1.0f - pan);
    R = 0.5f * (1.0f + pan);
  };

  getPan(getVal(0) * 0.5f, vp.sunL, vp.sunR);
  getPan(getVal(2), vp.plL, vp.plR);
  getPan(getVal(4), vp.mnL, vp.mnR);

  // Update Voices
  for (int i = 0; i < synth.getNumVoices(); ++i) {
    if (auto *v = dynamic_cast<GravityVoice *>(synth.getVoice(i))) {
      v->updateParams(vp);
    }
  }

  // 3. Render Synth
  synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

  // 4. Space Dust
  spaceDust.process(buffer, dustAmt, dustDens);

  // 5. Scope Buffering (for UI)
  const float *readL = buffer.getReadPointer(0);
  const float *readR =
      buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : readL;
  int num = buffer.getNumSamples();
  int wp = scopeWritePtr.load();

  for (int i = 0; i < num; ++i) {
    scopeFifo[wp] = readL[i];
    scopeFifo[wp + 1] = readR[i];
    wp += 2;
    if (wp >= kScopeSize * 2)
      wp = 0;
  }
  scopeWritePtr.store(wp);
}

juce::AudioProcessorEditor *GravityWellAudioProcessor::createEditor() {
  return new GravityWellAudioProcessorEditor(*this);
}

void GravityWellAudioProcessor::getStateInformation(
    juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void GravityWellAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new GravityWellAudioProcessor();
}
