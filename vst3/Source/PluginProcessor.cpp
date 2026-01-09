#include "PluginProcessor.h"
#include "PluginEditor.h"

CableLiftAudioProcessor::CableLiftAudioProcessor()
    : AudioProcessor(
          BusesProperties()
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout()) {}

CableLiftAudioProcessor::~CableLiftAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
CableLiftAudioProcessor::createParameterLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "liftLevel", "Elevation Level", 1.0f, 4.0f, 1.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "sonicPurity", "Sonic Purity", 0.0f, 1.0f, 0.5f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "staticReduction", "Static Reduction", 0.0f, 1.0f, 0.0f));
  layout.add(std::make_unique<juce::AudioParameterFloat>(
      "focusDetail", "Focus Detail", 0.0f, 1.0f, 0.5f));

  return layout;
}

void CableLiftAudioProcessor::prepareToPlay(double sampleRate,
                                            int samplesPerBlock) {
  humPhase = 0.0;
  lp1L = 0.0f;
  lp1R = 0.0f;
  writeIdxL = 0;
  writeIdxR = 0;
  jitterWalk = 0.0f;

  // Resize buffers
  delayBufferL.resize(48000, 0.0f);
  delayBufferR.resize(48000, 0.0f);
  std::fill(delayBufferL.begin(), delayBufferL.end(), 0.0f);
  std::fill(delayBufferR.begin(), delayBufferR.end(), 0.0f);

  // Reset filters
  bqL = {0, 0, 0, 0};
  bqR = {0, 0, 0, 0};
}

void CableLiftAudioProcessor::releaseResources() {}

void CableLiftAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                           juce::MidiBuffer &midiMessages) {
  juce::ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  // --- DSP Constants ---
  double sampleRate = getSampleRate();
  if (sampleRate <= 0)
    sampleRate = 44100.0;

  // Parameters
  float liftLevel = apvts.getRawParameterValue("liftLevel")->load();
  float sonicPurity = apvts.getRawParameterValue("sonicPurity")->load();
  float staticReduction = apvts.getRawParameterValue("staticReduction")->load();
  float focusDetail = apvts.getRawParameterValue("focusDetail")->load();

  // Badness: 1.0 (Level 1) -> 0.0 (Level 4)
  float badness = std::max(0.0f, (4.0f - liftLevel) / 3.0f);

  float humAmt = (badness * 0.1f) * (1.0f - staticReduction);
  float noiseAmt = (badness * 0.05f) * (1.0f - staticReduction);
  float jitterDepth = badness * 25.0f; // Max samples delay
  float capacitance = badness * 0.9f;
  float filterAlpha = 1.0f - (capacitance * 0.8f);

  float drive = 1.0f + (focusDetail * 0.5f) + (badness * 0.2f);
  float saturationMix = focusDetail * (1.0f - badness * 0.5f);
  bool applyHifiFilter = (1.0f - badness) > 0.1f;

  // Calc Biquad Coeffs (18kHz LPF, Q=0.8)
  double w0 = juce::MathConstants<double>::twoPi * 18000.0 / sampleRate;
  double cosw0 = std::cos(w0);
  double alphaQ = std::sin(w0) / (2.0 * 0.8);
  double a0 = 1.0 + alphaQ;
  double b0_c = ((1.0 - cosw0) / 2.0) / a0;
  double b1_c = (1.0 - cosw0) / a0;
  double b2_c = ((1.0 - cosw0) / 2.0) / a0;
  double a1_c = (-2.0 * cosw0) / a0;
  double a2_c = (1.0 - alphaQ) / a0;

  double humInc = juce::MathConstants<double>::twoPi * 50.0 / sampleRate;

  // Buffers
  auto *channelDataL = buffer.getWritePointer(0);
  auto *channelDataR =
      (totalNumInputChannels > 1) ? buffer.getWritePointer(1) : nullptr;

  // Circular Buffer logic
  int bufLen = 48000;

  for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
    // Update Hum Phase
    humPhase += humInc;
    if (humPhase > juce::MathConstants<double>::twoPi)
      humPhase -= juce::MathConstants<double>::twoPi;
    float hum = (float)std::sin(humPhase) * humAmt;
    float white = (random.nextFloat() * 2.0f - 1.0f) * noiseAmt;
    float interference = hum + white;

    // Jitter Walk
    jitterWalk += (random.nextFloat() - 0.5f) * 0.1f;
    if (jitterWalk > 1.0f)
      jitterWalk = 1.0f;
    if (jitterWalk < -1.0f)
      jitterWalk = -1.0f;
    float currentDelay = 100.0f + (jitterWalk * jitterDepth);

    // --- LEFT ---
    {
      float inSample = channelDataL[sample];

      // 1. Interference
      inSample += interference;

      // 2. Capacitance
      lp1L += filterAlpha * (inSample - lp1L);
      inSample = lp1L;

      // 3. Jitter (Variable Delay)
      delayBufferL[writeIdxL] = inSample;

      float readPos = (float)writeIdxL - currentDelay;
      while (readPos < 0)
        readPos += bufLen;
      while (readPos >= bufLen)
        readPos -= bufLen;

      int idxInt = (int)readPos;
      float frac = readPos - (float)idxInt;
      int idxNext = (idxInt + 1) % bufLen;
      float s1 = delayBufferL[idxInt];
      float s2 = delayBufferL[idxNext];
      inSample = s1 + frac * (s2 - s1);

      // 4. Distortion
      float dist = inSample * drive;
      dist = std::tanh(dist) + 0.05f * (dist * dist);
      if (dist > 1.0f)
        dist = 1.0f;
      if (dist < -1.0f)
        dist = -1.0f;
      inSample = inSample * (1.0f - saturationMix) + dist * saturationMix;

      // 5. Hifi Filter
      if (applyHifiFilter) {
        float y = (float)(b0_c * inSample + b1_c * bqL.x1 + b2_c * bqL.x2 -
                          a1_c * bqL.y1 - a2_c * bqL.y2);
        bqL.x2 = bqL.x1;
        bqL.x1 = inSample;
        bqL.y2 = bqL.y1;
        bqL.y1 = y;
        inSample = y;
      }

      channelDataL[sample] = inSample;
      writeIdxL = (writeIdxL + 1) % bufLen;
    }

    // --- RIGHT ---
    if (channelDataR) {
      float inSample = channelDataR[sample];
      inSample += interference;
      lp1R += filterAlpha * (inSample - lp1R);
      inSample = lp1R;

      delayBufferR[writeIdxR] = inSample;
      float readPos = (float)writeIdxR - currentDelay;
      while (readPos < 0)
        readPos += bufLen;
      while (readPos >= bufLen)
        readPos -= bufLen;
      int idxInt = (int)readPos;
      float frac = readPos - (float)idxInt;
      int idxNext = (idxInt + 1) % bufLen;
      inSample = delayBufferR[idxInt] +
                 frac * (delayBufferR[idxNext] - delayBufferR[idxInt]);

      float dist = inSample * drive;
      dist = std::tanh(dist) + 0.05f * (dist * dist);
      if (dist > 1.0f)
        dist = 1.0f;
      if (dist < -1.0f)
        dist = -1.0f;
      inSample = inSample * (1.0f - saturationMix) + dist * saturationMix;

      if (applyHifiFilter) {
        float y = (float)(b0_c * inSample + b1_c * bqR.x1 + b2_c * bqR.x2 -
                          a1_c * bqR.y1 - a2_c * bqR.y2);
        bqR.x2 = bqR.x1;
        bqR.x1 = inSample;
        bqR.y2 = bqR.y1;
        bqR.y1 = y;
        inSample = y;
      }

      channelDataR[sample] = inSample;
      writeIdxR = (writeIdxR + 1) % bufLen;
    }
  }
}

juce::AudioProcessorEditor *CableLiftAudioProcessor::createEditor() {
  return new CableLiftAudioProcessorEditor(*this);
}

void CableLiftAudioProcessor::getStateInformation(juce::MemoryBlock &destData) {
  auto state = apvts.copyState();
  std::unique_ptr<juce::XmlElement> xml(state.createXml());
  copyXmlToBinary(*xml, destData);
}

void CableLiftAudioProcessor::setStateInformation(const void *data,
                                                  int sizeInBytes) {
  std::unique_ptr<juce::XmlElement> xmlState(
      getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
    if (xmlState->hasTagName(apvts.state.getType()))
      apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new CableLiftAudioProcessor();
}
