#pragma once

namespace FriedTofuDSP
{

struct DiodeParams
{
    const char* name;
    double Is;  // Saturation current (A)
    double N;   // Emission coefficient
    double Rs;  // Series resistance (Ohms)
};

// Parameters directly from LTspice standard.dio library + popular circuit models
static constexpr DiodeParams AvailableDiodes[] = {
    // 0: Si 1N4148 (Crisp, fast silicon clipping)
    { "Si 1N4148 (Crisp)", 2.52e-9, 1.752, 0.568 },
    // 1: Ge 1N34A (Silky, soft germanium compression)
    { "Ge 1N34A (Silky)",  200e-12, 2.19,  0.084 },
    // 2: Schottky BAT41 (Flash Fried, low forward threshold ~0.3V)
    { "Schottky BAT41",    2.0e-8,  1.05,  1.200 },
    // 3: Red LED (Deep Fried, high headroom ~1.8V, punchy crunch)
    { "Red LED (Deep)",    9.3e-19, 1.84,  3.500 }
};

static constexpr int NumDiodeTypes = sizeof(AvailableDiodes) / sizeof(AvailableDiodes[0]);

} // namespace FriedTofuDSP
