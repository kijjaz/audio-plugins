#include "WDFDiodeClipper.h"

namespace FriedTofuDSP
{

WDFDiodeClipper::WDFDiodeClipper()
{
    reset();
}

void WDFDiodeClipper::reset()
{
    vPrev = 0.0;
}

void WDFDiodeClipper::setNetworkZ(double resistanceOhms)
{
    R = std::max(10.0, resistanceOhms);
}

void WDFDiodeClipper::setDiodeA(int diodeIndex)
{
    if (diodeIndex >= 0 && diodeIndex < NumDiodeTypes)
    {
        isA = AvailableDiodes[diodeIndex].Is;
        nA  = AvailableDiodes[diodeIndex].N;
    }
}

void WDFDiodeClipper::setDiodeB(int diodeIndex)
{
    if (diodeIndex >= 0 && diodeIndex < NumDiodeTypes)
    {
        isB = AvailableDiodes[diodeIndex].Is;
        nB  = AvailableDiodes[diodeIndex].N;
    }
}

void WDFDiodeClipper::setDiodeParamsA(double is, double n)
{
    isA = is;
    nA  = n;
}

void WDFDiodeClipper::setDiodeParamsB(double is, double n)
{
    isB = is;
    nB  = n;
}

float WDFDiodeClipper::processSample(float a)
{
    double V = vPrev;
    const double aDouble = static_cast<double>(a);
    
    // Damped Newton-Raphson solve (converges within 3-8 iterations with warm start)
    for (int it = 0; it < 8; ++it)
    {
        // Limit exponential argument to prevent numerical overflow (+-1.8V clamping limit)
        const double vExpA = std::min(V, 2.5) / (nA * Vt);
        const double vExpB = std::min(-V, 2.5) / (nB * Vt);
        
        const double eA = std::exp(vExpA);
        const double eB = std::exp(vExpB);
        
        const double g  = isA * (eA - 1.0) - isB * (eB - 1.0);
        const double gp = (isA / (nA * Vt)) * eA + (isB / (nB * Vt)) * eB;
        
        const double F  = V + R * g - aDouble;
        const double Fp = 1.0 + R * gp;
        
        const double dV = F / Fp;
        V -= dV;
        
        if (std::abs(dV) < 1e-9)
            break;
    }
    
    // Fallback sanitation against NaN / Inf
    if (!std::isfinite(V))
        V = 0.0;
        
    vPrev = V;
    return static_cast<float>(V);
}

double WDFDiodeClipper::solveStatic(double a, double R_ohms, double is_A, double n_A, double is_B, double n_B, double warm)
{
    double V = warm;
    for (int it = 0; it < 50; ++it)
    {
        const double vExpA = std::min(V, 2.5) / (n_A * Vt);
        const double vExpB = std::min(-V, 2.5) / (n_B * Vt);
        
        const double eA = std::exp(vExpA);
        const double eB = std::exp(vExpB);
        
        const double g  = is_A * (eA - 1.0) - is_B * (eB - 1.0);
        const double gp = (is_A / (n_A * Vt)) * eA + (is_B / (n_B * Vt)) * eB;
        
        const double F  = V + R_ohms * g - a;
        const double Fp = 1.0 + R_ohms * gp;
        
        const double dV = F / Fp;
        V -= dV;
        
        if (std::abs(dV) < 1e-10)
            break;
    }
    return std::isfinite(V) ? V : 0.0;
}

} // namespace FriedTofuDSP
