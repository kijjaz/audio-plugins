#pragma once
#include <vector>
#include <cmath>
#include <algorithm>

namespace FlyBy::DSP {

/**
 * FractionalDelayLine with 4-point Cubic Hermite interpolation.
 * Ensures click-free modulation for smooth ITD and dynamic Doppler pitch shifting.
 */
class FractionalDelayLine {
public:
    FractionalDelayLine() = default;

    void setMaxDelaySamples(int maxSamples) {
        bufferSize = std::max(64, maxSamples + 16);
        buffer.assign(bufferSize, 0.0f);
        writeIndex = 0;
    }

    void reset() {
        std::fill(buffer.begin(), buffer.end(), 0.0f);
        writeIndex = 0;
    }

    void write(float sample) {
        buffer[writeIndex] = sample;
        writeIndex = (writeIndex + 1) % bufferSize;
    }

    // Read with cubic Hermite interpolation
    float read(float delaySamples) const {
        if (delaySamples < 0.0f) delaySamples = 0.0f;
        if (delaySamples > static_cast<float>(bufferSize - 4)) {
            delaySamples = static_cast<float>(bufferSize - 4);
        }

        float readPos = static_cast<float>(writeIndex) - delaySamples;
        while (readPos < 0.0f) readPos += bufferSize;
        while (readPos >= bufferSize) readPos -= bufferSize;

        int i1 = static_cast<int>(readPos);
        float frac = readPos - static_cast<float>(i1);

        int i0 = (i1 - 1 + bufferSize) % bufferSize;
        int i2 = (i1 + 1) % bufferSize;
        int i3 = (i1 + 2) % bufferSize;

        float y0 = buffer[i0];
        float y1 = buffer[i1];
        float y2 = buffer[i2];
        float y3 = buffer[i3];

        // 4-point Cubic Hermite polynomial
        float c0 = y1;
        float c1 = 0.5f * (y2 - y0);
        float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
        float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

        return ((c3 * frac + c2) * frac + c1) * frac + c0;
    }

private:
    std::vector<float> buffer;
    int bufferSize = 1024;
    int writeIndex = 0;
};

} // namespace FlyBy::DSP
