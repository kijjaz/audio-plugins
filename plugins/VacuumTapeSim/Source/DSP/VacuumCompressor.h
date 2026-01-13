#include <algorithm>
#include <cmath>
#include <juce_dsp/juce_dsp.h>

class VacuumCompressor {
public:
  void prepare(const juce::dsp::ProcessSpec &spec) {
    sampleRate = spec.sampleRate;
    voltage = 1.0f;
  }

  void setParameters(float threshold, float drain, float recovery, float time) {
    this->threshold = threshold;
    this->drain = drain;
    this->recovery = recovery;
    this->time = time;
  }

  float processSample(float input) {
    float absInput = std::abs(input);

    // Energy-based drain (scaled by time/capacitor size)
    if (absInput > threshold) {
      float excess = absInput - threshold;
      voltage -=
          excess * drain * (1.0f / (float)sampleRate) * (1.0f / (time + 0.1f));
    } else {
      // Smooth recovery toward 1.0 (RC-style, scaled by time)
      voltage += (1.0f - voltage) * recovery * (1.0f / (float)sampleRate) *
                 (1.0f / (time + 0.1f));
    }

    voltage = std::clamp(voltage, 0.01f, 1.0f);
    return input * voltage;
  }

  float getCurrentVoltage() const { return voltage; }

private:
  double sampleRate = 44100.0;
  float voltage = 1.0f;
  float threshold = 0.5f;
  float drain = 10.0f;
  float recovery = 5.0f;
  float time = 0.5f;
};
