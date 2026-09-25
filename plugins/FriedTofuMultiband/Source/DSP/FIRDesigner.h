#pragma once
#include <vector>
#include <cmath>
#include <numbers>

namespace FriedTofuDSP
{

/**
 * Designs linear-phase FIR filter kernels using windowed-sinc methods.
 */
class FIRDesigner
{
public:
    /**
     * Designs a linear-phase lowpass FIR filter of odd order N (delay = (N-1)/2).
     * Uses a Blackman-Harris or Kaiser window for high stopband attenuation (>70 dB).
     *
     * @param numTaps Must be an odd number (e.g. 255 or 511).
     * @param cutoffHz Cutoff frequency (-6 dB crossover point).
     * @param sampleRate Sampling rate.
     * @return Normalized FIR coefficient kernel.
     */
    static std::vector<float> designLowPass(int numTaps, double cutoffHz, double sampleRate)
    {
        if (numTaps % 2 == 0)
            numTaps += 1; // Guarantee odd length for type-I symmetric FIR (integer group delay)

        std::vector<float> coeffs(static_cast<size_t>(numTaps), 0.0f);
        const double fc = cutoffHz / sampleRate;
        const int M = numTaps - 1;
        const int mid = M / 2;

        double sum = 0.0;
        const double pi = 3.14159265358979323846;

        // 4-term Blackman-Harris window coefficients
        const double a0 = 0.35875;
        const double a1 = 0.48829;
        const double a2 = 0.14128;
        const double a3 = 0.01168;

        for (int n = 0; n < numTaps; ++n)
        {
            double sinc;
            if (n == mid)
            {
                sinc = 2.0 * fc;
            }
            else
            {
                const double x = pi * (n - mid);
                sinc = std::sin(2.0 * pi * fc * (n - mid)) / x;
            }

            // Window
            const double w = a0
                           - a1 * std::cos(2.0 * pi * n / M)
                           + a2 * std::cos(4.0 * pi * n / M)
                           - a3 * std::cos(6.0 * pi * n / M);

            double h = sinc * w;
            coeffs[static_cast<size_t>(n)] = static_cast<float>(h);
            sum += h;
        }

        // Normalize for unity gain at DC (0 Hz)
        if (std::abs(sum) > 1e-12)
        {
            const float norm = static_cast<float>(1.0 / sum);
            for (auto& c : coeffs)
                c *= norm;
        }

        return coeffs;
    }
};

} // namespace FriedTofuDSP
