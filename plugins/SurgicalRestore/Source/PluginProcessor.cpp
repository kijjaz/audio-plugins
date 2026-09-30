#include "PluginProcessor.h"
#include "PluginEditor.h"

SurgicalRestoreAudioProcessor::SurgicalRestoreAudioProcessor()
    : AudioProcessor (BusesProperties().withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
    , apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SurgicalRestoreAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Stage 1: De-Click
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "click_sensitivity", "Click Sensitivity", juce::NormalisableRange<float> (1.0f, 10.0f, 0.1f), 5.0f));
    params.push_back (std::make_unique<juce::AudioParameterInt> (
        "click_width", "Max Click Width", 4, 64, 24));

    // Stage 2: De-Crackle
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "crackle_amount", "De-Crackle Amount", juce::NormalisableRange<float> (0.0f, 100.0f, 1.0f), 50.0f));

    // Stage 3: De-Hiss
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "hiss_reduction", "Hiss Reduction (dB)", juce::NormalisableRange<float> (0.0f, 24.0f, 0.5f), 10.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "harmonic_shield", "Harmonic Shield", juce::NormalisableRange<float> (0.0f, 1.0f, 0.05f), 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "hf_tilt", "Air Tilt (dB)", juce::NormalisableRange<float> (0.0f, 8.0f, 0.5f), 3.0f));

    // Master / Routing
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "delta_listen", "Audition Delta", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

void SurgicalRestoreAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
    {
        lpcEngine[ch].prepare (1024, 16);
        decrackleEngine[ch].prepare (sampleRate);
        spectralDenoiser[ch].prepare (sampleRate);
    }

    delayBuffer.setSize (2, samplesPerBlock + lookaheadSamples);
    delayBuffer.clear();
    delayWritePos = 0;

    // Report lookahead latency to host for sample-accurate time-alignment
    setLatencySamples (lookaheadSamples);
}

void SurgicalRestoreAudioProcessor::releaseResources()
{
}

bool SurgicalRestoreAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
        return false;

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void SurgicalRestoreAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    bool isBypassed = apvts.getRawParameterValue ("bypass")->load() > 0.5f;
    if (isBypassed) return;

    bool deltaListen = apvts.getRawParameterValue ("delta_listen")->load() > 0.5f;
    float clickSens = apvts.getRawParameterValue ("click_sensitivity")->load();
    int maxWidth = (int)apvts.getRawParameterValue ("click_width")->load();
    float crackleAmt = apvts.getRawParameterValue ("crackle_amount")->load();
    float hissDb = apvts.getRawParameterValue ("hiss_reduction")->load();
    float harmShield = apvts.getRawParameterValue ("harmonic_shield")->load();

    int numSamples = buffer.getNumSamples();
    juce::AudioBuffer<float> originalCopy;
    if (deltaListen)
    {
        originalCopy.makeCopyOf (buffer);
    }

    // Mid/Side Processing
    if (totalNumInputChannels == 2)
    {
        auto* left = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);

        // 1. Transform to Mid/Side
        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.70710678f;
            float s = (left[i] - right[i]) * 0.70710678f;
            left[i] = m;
            right[i] = s;
        }

        // 2. Stage 1 (De-Click) & Stage 2 (De-Crackle) in M/S domain
        for (int ch = 0; ch < 2; ++ch)
        {
            auto* channelData = buffer.getWritePointer (ch);
            lpcEngine[ch].calculateCoefficients (channelData, numSamples);

            for (int i = 0; i < numSamples; ++i)
            {
                float in = channelData[i];
                float res = lpcEngine[ch].processSample (in);

                // Threshold detection
                float thresh = (ch == 1 ? 0.015f : 0.025f) * (11.0f - clickSens);
                if (std::abs (res) > thresh && i > 4 && i < numSamples - maxWidth)
                {
                    sr_dsp::ARInpainter::inpaint (channelData, numSamples, i - 1, i + 3);
                }

                // De-crackle pass
                channelData[i] = decrackleEngine[ch].process (channelData[i], res, crackleAmt / 20.0f, 12.0f);
            }
        }

        // 3. Transform back to Left/Right
        for (int i = 0; i < numSamples; ++i)
        {
            float m = left[i];
            float s = right[i];
            left[i]  = (m + s) * 0.70710678f;
            right[i] = (m - s) * 0.70710678f;
        }

        // 4. Stage 3 (Neural De-Hiss with Harmonic Shield)
        for (int ch = 0; ch < 2; ++ch)
        {
            spectralDenoiser[ch].processBlock (buffer.getWritePointer (ch), numSamples, hissDb, harmShield);
        }
    }

    // Delta Mode: Output = Original - Cleaned
    if (deltaListen)
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* orig = originalCopy.getReadPointer (ch);
            auto* clean = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
                clean[i] = orig[i] - clean[i];
        }
    }
}

juce::AudioProcessorEditor* SurgicalRestoreAudioProcessor::createEditor()
{
    return new SurgicalRestoreAudioProcessorEditor (*this);
}

void SurgicalRestoreAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SurgicalRestoreAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SurgicalRestoreAudioProcessor();
}
