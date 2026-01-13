#pragma once
#include <cmath>

class TapeHysteresis {
public:
  void setParameters(float drive, float hysteresis, float beta) {
    this->G = drive;
    this->h = hysteresis;
    this->beta = beta;
  }

  float processSample(float x) {
    float deltaX = x - lastX;
    // y[n] = tanh(x[n] * G - h * tanh(delta_x[n] * beta))
    float y = std::tanh(x * G - h * std::tanh(deltaX * beta));
    lastX = x;
    distortionAmount = std::abs(y - x);
    return y;
  }

  float getDistortionAmount() const { return distortionAmount; }

private:
  float lastX = 0.0f;
  float distortionAmount = 0.0f;
  float G = 1.0f; // Drive
  float h = 0.1f; // Hysteresis coercivity
  float beta =
      100.0f; // Smoothing factor (higher = faster tracking of derivative)
};
