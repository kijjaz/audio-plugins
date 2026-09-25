#pragma once

struct Grain {
  bool active = false;
  float position = 0.0f; // Current pos in grain duration (samples)
  float duration = 0.0f; // Total duration (samples)
  float readPtr = 0.0f;  // Current read pointer in buffer
  float speed = 1.0f;    // Playback speed
  float pan = 0.5f;      // 0.0=L, 1.0=R
};

class SpaceDust {
public:
  SpaceDust() {
    // 2 seconds max delay at 96k safe
    bufferSize = 96000 * 2;
    delayBuffer.setSize(2, bufferSize);
    delayBuffer.clear();

    grains.resize(256); // Max 256 grains
  }

  void prepare(double sampleRate) {
    fs = sampleRate;
    bufferSize = (int)(fs * 2.0); // 2 seconds
    delayBuffer.setSize(2, bufferSize);
    delayBuffer.clear();
  }

  void process(juce::AudioBuffer<float> &buffer, float amount, float density) {
    if (amount < 0.001f)
      return;

    int numSamples = buffer.getNumSamples();
    auto *inL = buffer.getWritePointer(0);
    auto *inR =
        buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    auto *bufL = delayBuffer.getWritePointer(0);
    auto *bufR = delayBuffer.getWritePointer(1);

    for (int s = 0; s < numSamples; ++s) {
      float dryL = inL[s];
      float dryR = inR ? inR[s] : dryL;

      // 1. Write to Circular Buffer
      bufL[writePtr] = dryL;
      bufR[writePtr] = dryR;

      // 2. Spawn Grains
      // Probabilistic spawn
      // density 0..1 maps to probability
      // If density is 1.0, maybe 100 grains/sec?
      // 100 grains/sec = 1 per fs/100 samples
      // prob = 100/fs
      float prob = (density * 100.0f) / fs;
      if (random.nextFloat() < prob) {
        spawnGrain();
      }

      // 3. Render Grains
      float wetL = 0.0f;
      float wetR = 0.0f;

      for (auto &g : grains) {
        if (!g.active)
          continue;

        // Read from buffer
        float rIdx = g.readPtr;
        // Linear Interpolation
        int i1 = (int)rIdx;
        int i2 = (i1 + 1) % bufferSize;
        float frac = rIdx - i1;

        // Wrap index
        if (i1 < 0)
          i1 += bufferSize;
        if (i2 < 0)
          i2 += bufferSize;

        // Stereo Read (Mono buffer? No stereo)
        float l1 = delayBuffer.getSample(0, i1);
        float l2 = delayBuffer.getSample(0, i2);
        float sampL = l1 + frac * (l2 - l1);

        float r1 = delayBuffer.getSample(1, i1);
        float r2 = delayBuffer.getSample(1, i2);
        float sampR = r1 + frac * (r2 - r1);

        // Windowing (Parabolic)
        float t = g.position / g.duration;
        float win = 1.0f - std::pow(2.0f * t - 1.0f, 2.0f);

        wetL += sampL * win * (1.0f - g.pan);
        wetR += sampR * win * g.pan;

        // Advance Grain
        g.position += 1.0f;
        g.readPtr += g.speed;
        while (g.readPtr >= bufferSize)
          g.readPtr -= bufferSize;

        if (g.position >= g.duration) {
          g.active = false;
        }
      }

      // 4. Mix
      inL[s] += wetL * amount;
      if (inR)
        inR[s] += wetR * amount;

      // Advance Write Ptr
      writePtr++;
      if (writePtr >= bufferSize)
        writePtr = 0;
    }
  }

private:
  void spawnGrain() {
    // Find free grain
    for (auto &g : grains) {
      if (!g.active) {
        g.active = true;
        g.position = 0.0f;
        // Duration: 100ms to 500ms?
        // Random 2000 samples to 10000 samples
        g.duration = fs * (0.05f + random.nextFloat() * 0.2f); // 50-250ms

        // Read Ptr: random delay up to 2s
        float delaySamples = fs * (0.1f + random.nextFloat() * 1.5f);
        g.readPtr = (float)writePtr - delaySamples;
        if (g.readPtr < 0)
          g.readPtr += bufferSize;

        g.pan = random.nextFloat();
        g.speed = 0.95f + random.nextFloat() * 0.1f; // +/- pitch
        return;
      }
    }
  }

  juce::AudioBuffer<float> delayBuffer;
  std::vector<Grain> grains;
  int writePtr = 0;
  int bufferSize = 0;
  double fs = 44100.0;
  juce::Random random;
};
