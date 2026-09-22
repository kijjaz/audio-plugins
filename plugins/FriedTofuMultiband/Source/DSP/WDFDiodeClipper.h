#pragma once
#include "DiodeModels.h"
#include <cmath>
#include <algorithm>

namespace FriedTofuDSP
{

class WDFDiodeClipper
{
public:
    WDFDiodeClipper();
    ~WDFDiodeClipper() = default;

    void reset();
    
    // Set network resistance (Ohms)
    void setNetworkZ(double resistanceOhms);
    
    // Set diode models for positive and negative paths
    void setDiodeA(int diodeIndex);
    void setDiodeB(int diodeIndex);
    
    // Direct diode parameter setting
    void setDiodeParamsA(double is, double n);
    void setDiodeParamsB(double is, double n);

    // Process a single sample
    // a: incident wave / driving voltage into WDF root
    // Returns: node voltage across the diode pair V
    float processSample(float a);

    // Static solve function for transfer curve visualization without mutating state
    static double solveStatic(double a, double R, double isA, double nA, double isB, double nB, double warm = 0.0);

private:
    static constexpr double Vt = 0.025864; // Thermal voltage at room temperature (25 C)
    
    double R { 2200.0 }; // Default 2.2 kOhm
    
    double isA { 2.52e-9 };
    double nA  { 1.752 };
    
    double isB { 2.52e-9 };
    double nB  { 1.752 };

    double vPrev { 0.0 }; // Warm-start memory
};

} // namespace FriedTofuDSP
