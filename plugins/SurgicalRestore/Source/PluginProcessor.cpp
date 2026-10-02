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

    // Stage 1: De-Click & M/S
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "click_sensitivity", "Click Sensitivity", juce::NormalisableRange<float> (0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        "side_boost", "Side Click Sens", juce::NormalisableRange<float> (1.0f, 3.5f, 0.1f), 2.2f));
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

    // Stage 0 / Hardware Modeling: Sub-Sonic Rumble Filter & Tape Azimuth
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "rumble_filter", "Rumble Filter (25Hz 18dB/oct)", true));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "azimuth_align", "Auto Azimuth Align (Tape)", false));

    // Master / Routing
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "delta_listen", "Audition Delta", false));
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        "bypass", "Bypass", false));

    return { params.begin(), params.end() };
}

int SurgicalRestoreAudioProcessor::getNumPrograms()
{
    return 4;
}

int SurgicalRestoreAudioProcessor::getCurrentProgram()
{
    return currentProgramIndex;
}

const juce::String SurgicalRestoreAudioProcessor::getProgramName (int index)
{
    switch (index)
    {
        case 0: return "Vinyl LP (33/45 RPM Microgroove)";
        case 1: return "Shellac 78 RPM (Archival)";
        case 2: return "Magnetic Tape / Cassette";
        case 3: return "Transparent Vocal Solo";
        default: return "Default";
    }
}

void SurgicalRestoreAudioProcessor::changeProgramName (int, const juce::String&)
{
}

void SurgicalRestoreAudioProcessor::setCurrentProgram (int index)
{
    currentProgramIndex = index;
    switch (index)
    {
        case 0: // Vinyl LP
            apvts.getParameter ("click_sensitivity")->setValueNotifyingHost (apvts.getParameterRange ("click_sensitivity").convertTo0to1 (6.2f));
            apvts.getParameter ("side_boost")->setValueNotifyingHost (apvts.getParameterRange ("side_boost").convertTo0to1 (2.5f));
            apvts.getParameter ("crackle_amount")->setValueNotifyingHost (apvts.getParameterRange ("crackle_amount").convertTo0to1 (40.0f));
            apvts.getParameter ("hiss_reduction")->setValueNotifyingHost (apvts.getParameterRange ("hiss_reduction").convertTo0to1 (6.0f));
            apvts.getParameter ("harmonic_shield")->setValueNotifyingHost (apvts.getParameterRange ("harmonic_shield").convertTo0to1 (0.80f));
            apvts.getParameter ("rumble_filter")->setValueNotifyingHost (1.0f);
            break;

        case 1: // Shellac 78 RPM Archival
            apvts.getParameter ("click_sensitivity")->setValueNotifyingHost (apvts.getParameterRange ("click_sensitivity").convertTo0to1 (8.5f));
            apvts.getParameter ("side_boost")->setValueNotifyingHost (apvts.getParameterRange ("side_boost").convertTo0to1 (3.2f));
            apvts.getParameter ("crackle_amount")->setValueNotifyingHost (apvts.getParameterRange ("crackle_amount").convertTo0to1 (85.0f));
            apvts.getParameter ("hiss_reduction")->setValueNotifyingHost (apvts.getParameterRange ("hiss_reduction").convertTo0to1 (18.0f));
            apvts.getParameter ("harmonic_shield")->setValueNotifyingHost (apvts.getParameterRange ("harmonic_shield").convertTo0to1 (0.60f));
            apvts.getParameter ("rumble_filter")->setValueNotifyingHost (1.0f);
            break;

        case 2: // Magnetic Tape / Cassette
            apvts.getParameter ("click_sensitivity")->setValueNotifyingHost (apvts.getParameterRange ("click_sensitivity").convertTo0to1 (3.0f));
            apvts.getParameter ("side_boost")->setValueNotifyingHost (apvts.getParameterRange ("side_boost").convertTo0to1 (1.0f));
            apvts.getParameter ("crackle_amount")->setValueNotifyingHost (apvts.getParameterRange ("crackle_amount").convertTo0to1 (15.0f));
            apvts.getParameter ("hiss_reduction")->setValueNotifyingHost (apvts.getParameterRange ("hiss_reduction").convertTo0to1 (14.0f));
            apvts.getParameter ("harmonic_shield")->setValueNotifyingHost (apvts.getParameterRange ("harmonic_shield").convertTo0to1 (0.85f));
            apvts.getParameter ("rumble_filter")->setValueNotifyingHost (0.0f);
            apvts.getParameter ("azimuth_align")->setValueNotifyingHost (1.0f);
            break;

        case 3: // Transparent Vocal Solo
            apvts.getParameter ("click_sensitivity")->setValueNotifyingHost (apvts.getParameterRange ("click_sensitivity").convertTo0to1 (4.5f));
            apvts.getParameter ("side_boost")->setValueNotifyingHost (apvts.getParameterRange ("side_boost").convertTo0to1 (2.0f));
            apvts.getParameter ("crackle_amount")->setValueNotifyingHost (apvts.getParameterRange ("crackle_amount").convertTo0to1 (25.0f));
            apvts.getParameter ("hiss_reduction")->setValueNotifyingHost (apvts.getParameterRange ("hiss_reduction").convertTo0to1 (8.0f));
            apvts.getParameter ("harmonic_shield")->setValueNotifyingHost (apvts.getParameterRange ("harmonic_shield").convertTo0to1 (0.95f));
            apvts.getParameter ("rumble_filter")->setValueNotifyingHost (1.0f);
            apvts.getParameter ("azimuth_align")->setValueNotifyingHost (0.0f);
            break;
    }
}

void SurgicalRestoreAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    azimuthEngine.prepare (sampleRate);

    for (int ch = 0; ch < 2; ++ch)
    {
        rumbleFilter[ch].prepare (sampleRate, 25.0f);
        lpcEngine[ch].prepare (1024, 16);
        decrackleEngine[ch].prepare (sampleRate);
        spectralDenoiser[ch].prepare (sampleRate);
    }

    // Size circular delay buffer for exact latency alignment
    delayBuffer.setSize (2, latencySamples + samplesPerBlock + 1024);
    delayBuffer.clear();
    delayWritePos = 0;

    // Report STFT latency to host DAW for PDC (Plugin Delay Compensation)
    setLatencySamples (latencySamples);
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
    float sideBoost = apvts.getRawParameterValue ("side_boost")->load();
    int maxWidth = (int)apvts.getRawParameterValue ("click_width")->load();
    float crackleAmt = apvts.getRawParameterValue ("crackle_amount")->load();
    float hissDb = apvts.getRawParameterValue ("hiss_reduction")->load();
    float harmShield = apvts.getRawParameterValue ("harmonic_shield")->load();
    bool enableRumble = apvts.getRawParameterValue ("rumble_filter")->load() > 0.5f;
    bool enableAzimuth = apvts.getRawParameterValue ("azimuth_align")->load() > 0.5f;

    int numSamples = buffer.getNumSamples();
    int delayBufSize = delayBuffer.getNumSamples();

    // 1. Store input in circular delay buffer for time-aligned Delta calculation
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto* inData = buffer.getReadPointer (ch);
        if (delayWritePos + numSamples <= delayBufSize)
        {
            delayBuffer.copyFrom (ch, delayWritePos, inData, numSamples);
        }
        else
        {
            int part1 = delayBufSize - delayWritePos;
            int part2 = numSamples - part1;
            delayBuffer.copyFrom (ch, delayWritePos, inData, part1);
            delayBuffer.copyFrom (ch, 0, inData + part1, part2);
        }
    }

    // Compute read position aligned with plugin latency (latencySamples)
    int delayReadPos = (delayWritePos - latencySamples + delayBufSize) % delayBufSize;
    delayWritePos = (delayWritePos + numSamples) % delayBufSize;

    // Sub-Sonic Rumble Filter (18 dB/oct HPF at 25 Hz)
    if (enableRumble)
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* chData = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                chData[i] = rumbleFilter[ch].processSample (chData[i]);
            }
        }
    }

    // Tape Azimuth & Phase Alignment (removes L/R skew before M/S or denoising)
    if (enableAzimuth && totalNumInputChannels == 2)
    {
        azimuthEngine.processBlock (buffer.getWritePointer (0), buffer.getWritePointer (1), numSamples, true);
        detectedAzimuthSkew.store (azimuthEngine.getDetectedSkewSamples());
    }

    // Restoration Processing (Mid/Side for Stereo, Direct for Mono)
    if (totalNumInputChannels == 2)
    {
        auto* left = buffer.getWritePointer (0);
        auto* right = buffer.getWritePointer (1);

        // 2. Transform to Mid/Side
        for (int i = 0; i < numSamples; ++i)
        {
            float m = (left[i] + right[i]) * 0.70710678f;
            float s = (left[i] - right[i]) * 0.70710678f;
            left[i] = m;
            right[i] = s;
        }

        // 3. Stage 1 (De-Click) & Stage 2 (De-Crackle) in M/S domain
        // ch 0 = Mid, ch 1 = Side
        for (int ch = 0; ch < 2; ++ch)
        {
            bool doDeClick = (clickSens > 0.05f);
            bool doDeCrackle = (crackleAmt > 0.05f);

            if (doDeClick || doDeCrackle)
            {
                auto* channelData = buffer.getWritePointer (ch);
                lpcEngine[ch].calculateCoefficients (channelData, numSamples);

                // Channel-specific sensitivity: Side channel benefits from empirical 3.2x boost
                float chSens = (ch == 1) ? std::min (10.0f, clickSens * sideBoost) : clickSens;
                float baseThresh = (ch == 1 ? 0.012f : 0.025f);
                float thresh = baseThresh * (11.0f - chSens);

                for (int i = 0; i < numSamples; ++i)
                {
                    float in = channelData[i];
                    float res = lpcEngine[ch].processSample (in);

                    if (doDeClick && std::abs (res) > thresh && i > 4 && i < numSamples - maxWidth)
                    {
                        sr_dsp::ARInpainter::inpaint (channelData, numSamples, i - 1, i + 3);
                    }

                    // De-crackle pass (ch 1 Side crackle handled aggressively)
                    if (doDeCrackle)
                    {
                        float cAmt = (ch == 1) ? crackleAmt * 1.25f : crackleAmt;
                        channelData[i] = decrackleEngine[ch].process (channelData[i], res, cAmt / 20.0f, 12.0f);
                    }
                }
            }
        }

        // 4. Transform back to Left/Right
        for (int i = 0; i < numSamples; ++i)
        {
            float m = left[i];
            float s = right[i];
            left[i]  = (m + s) * 0.70710678f;
            right[i] = (m - s) * 0.70710678f;
        }

        // 5. Stage 3 (Neural De-Hiss with Harmonic Shield)
        for (int ch = 0; ch < 2; ++ch)
        {
            spectralDenoiser[ch].processBlock (buffer.getWritePointer (ch), numSamples, hissDb, harmShield);
        }
    }
    else if (totalNumInputChannels == 1)
    {
        // Direct processing for Mono Tracks
        bool doDeClick = (clickSens > 0.05f);
        bool doDeCrackle = (crackleAmt > 0.05f);

        auto* channelData = buffer.getWritePointer (0);

        if (doDeClick || doDeCrackle)
        {
            lpcEngine[0].calculateCoefficients (channelData, numSamples);
            float thresh = 0.025f * (11.0f - clickSens);

            for (int i = 0; i < numSamples; ++i)
            {
                float in = channelData[i];
                float res = lpcEngine[0].processSample (in);

                if (doDeClick && std::abs (res) > thresh && i > 4 && i < numSamples - maxWidth)
                {
                    sr_dsp::ARInpainter::inpaint (channelData, numSamples, i - 1, i + 3);
                }

                if (doDeCrackle)
                {
                    channelData[i] = decrackleEngine[0].process (channelData[i], res, crackleAmt / 20.0f, 12.0f);
                }
            }
        }

        spectralDenoiser[0].processBlock (channelData, numSamples, hissDb, harmShield);
    }

    // Delta Mode: Output = TimeAlignedOriginal[n - latency] - Cleaned[n]
    if (deltaListen)
    {
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            auto* clean = buffer.getWritePointer (ch);
            for (int i = 0; i < numSamples; ++i)
            {
                int rPos = (delayReadPos + i) % delayBufSize;
                float origSample = delayBuffer.getSample (ch, rPos);
                clean[i] = origSample - clean[i];
            }
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
