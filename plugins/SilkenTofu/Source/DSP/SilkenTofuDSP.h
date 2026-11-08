#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <algorithm>
#include <array>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SilkenTofuDSP
{

struct Parameters
{
    // 1. Core Teletronix LA-2A Controls
    float peakReduction = 45.0f;     // 0.0 .. 100.0
    float gainDb        = 0.0f;      // -12.0 .. +24.0 dB (or Auto-Level linked)
    bool  limitMode     = false;     // false = Compress (~3:1), true = Limit (~inf:1)
    bool  autoLevel     = true;      // Automatically compensates nominal GR so volume stays matched

    // 2. "T4-Bean" Semiconductor & Electro-Optical Physics Lab
    int   crystalType   = 0;         // 0 = Vintage CdS, 1 = CdSe Vactrol (LA-3A speed), 2 = Silicon Fast
    float trapMemory    = 100.0f;    // -50.0% (Optical Bloom) .. 0% (Fast) .. 100% (1965 Stock) .. 200% (Deep Lock)
    float topologyMorph = 0.0f;      // 0% = Vintage Feedback, 100% = Feed-Forward, 125% = Negative-Ratio Pump
    float punchHoldoffMs= 2.0f;      // 0.0 .. 30.0 ms inductor choke on EL panel transient drive
    float biasGlow      = 15.0f;     // 0.0 .. 100.0% (#4: T4B Pre-Warmth / Ambient Bias Glow)
    float cellAge       = 15.0f;     // 0.0 .. 100.0% (1965 Factory New -> 50-Yr Worn Phosphor)

    // 3. Oversampled Analog Tube (12AX7A + 12BH7A) & Transformer (UTC HA-100X / A-24) Stage
    float tubeIronDriveDb = 3.0f;    // -6.0 .. +18.0 dB (internally gain-compensated saturation drive)
    bool  fatCathodeCap   = false;   // Bypasses R11 (1.5k) on V1 12AX7A -> +6dB internal drive & rich H2
    bool  load600Ohm      = false;   // Vintage 600-Ohm console termination on V2 12BH7A WCF & A-24
    float bPlusSag        = 45.0f;   // 0.0 .. 100.0% (R29/C7A 275V DC supply rail droop under load)
    float millerAir       = 0.0f;    // -100% (Dark Velvet RC roll-off) .. 0% .. +100% (Luminous HF Air boost under GR)

    // 4. Sidechain & External Key Ducking (#3)
    float r37Emphasis     = 50.0f;   // 0% (Flat) .. 50% (1965 Stock) .. 100% (HF De-Ess / Bass Immunity)
    float scHighPassHz    = 40.0f;   // 20.0 .. 350.0 Hz sidechain high-pass filter
    bool  extSidechain    = false;   // #3: True External Key Input + Optical Ducking Mode
    bool  scListen        = false;   // Audition filtered sidechain driving the EL panel

    // 5. Global Mix & Unity Trim (AGENTS.md Rule 2: 0.0 dB on all factory presets)
    float mix             = 100.0f;  // 0.0 .. 100.0% Dry/Wet parallel compression
    float trimDb          = 0.0f;    // -12.0 .. +12.0 dB Master Output Trim (0.0 dB default)
    bool  bypass          = false;   // Phase-locked oversampled soft bypass
};

struct Telemetry
{
    float gainReductionDb = 0.0f;    // Instantaneous T4B attenuation (negative dB)
    float elLuminescence  = 0.0f;    // 0.0 .. 1.5 photon flux L(t)
    float shallowTrapN1   = 0.0f;    // Shallow carrier density N1(t)
    float deepTrapN2      = 0.0f;    // Deep lattice trap density N2(t)
    float bPlusVolts      = 275.0f;  // Instantaneous B+ rail voltage (275V rest -> ~225V sag)
    float inputVuDb       = -60.0f;  // Ballistic Input level in dBFS
    float outputVuDb      = -60.0f;  // Ballistic Output level in dBFS
};

/**
 * Sample-accurate 1-pole exponential parameter smoother (12 ms time constant)
 * Eliminates block-boundary stepping and zipper noise during fast knob turns or DAW automation.
 */
struct SmoothedParam
{
    double current = 0.0;
    double target  = 0.0;

    void reset(double val) noexcept
    {
        current = target = val;
    }

    inline double step(double alpha) noexcept
    {
        current += alpha * (target - current);
        return current;
    }
};

/**
 * Precomputed block/oversampled-rate filter coefficients for the non-linear analog stage.
 * Avoids redundant std::exp() calls inside the 8x oversampled inner loop.
 */
struct AnalogStageCoeffs
{
    double omegaXfmr        = 0.0;
    double leakXfmr         = 0.0;
    double alphaMiller      = 0.0;
    double alphaGridCharge  = 0.0;
    double alphaGridRelease = 0.0;
    double alphaBDrop       = 0.0;
    double alphaBRec        = 0.0;
    double smoothAlphaOs    = 0.0;
    double alphaGainOs      = 0.0;

    void update(double osSampleRate) noexcept
    {
        const double dtOs = 1.0 / std::max(22050.0, osSampleRate);
        omegaXfmr        = 2.0 * M_PI * 65.0 * dtOs;
        leakXfmr         = std::exp(-2.0 * M_PI * 12.0 * dtOs);
        alphaMiller      = 1.0 - std::exp(-2.0 * M_PI * 4800.0 * dtOs);
        alphaGridCharge  = 1.0 - std::exp(-dtOs / 0.0015);
        alphaGridRelease = 1.0 - std::exp(-dtOs / 0.045);
        alphaBDrop       = 1.0 - std::exp(-dtOs / 0.008);
        alphaBRec        = 1.0 - std::exp(-dtOs / 0.120);
        smoothAlphaOs    = 1.0 - std::exp(-dtOs / 0.012);
        alphaGainOs      = 1.0 - std::exp(-dtOs / 0.00018); // C1-smooth oversampled T4B gain filter
    }
};

/**
 * Pure C++17 Zero-Allocation White-Box Engine for the Teletronix LA-2A & T4B Cell.
 * Exact sample-rate invariance (44.1 kHz to 384 kHz) via continuous-time dt = 1/fs scaling,
 * with per-sample parameter smoothing across all continuous controls.
 */
class ChannelEngine
{
public:
    void prepare(double sampleRate, float channelToleranceOffset = 0.0f)
    {
        fs_ = std::max(22050.0, sampleRate);
        dt_ = 1.0 / fs_;
        tol_ = channelToleranceOffset; // Subtle stereo canister tolerance
        paramSmoothAlpha_ = 1.0 - std::exp(-dt_ / 0.012); // 12 ms sample-accurate knob smoothing
        vuBallisticAlpha_ = 1.0 - std::exp(-dt_ / 0.120); // Smooth VU meter RMS integration
        reset();
    }

    void reset()
    {
        n1_ = 0.0;
        n2_ = 0.0;
        rLdr_ = 2000000.0;
        lastGainLin_ = 1.0;
        gOsSmooth_ = 1.0;
        lastTubeHarmonicEnv_ = 0.0;
        acEnv_ = 0.0;
        punchEnv_ = 0.0;
        scHpX1_ = 0.0;
        scHpY1_ = 0.0;
        r37Lp_ = 0.0;
        elCapPrev_ = 0.0;

        phiIn_ = 0.0;
        phiInPrev_ = 0.0;
        c2GridBias_ = 0.0;
        bPlusRail_ = 1.0;
        millerLp_ = 0.0;
        dcBlockX1_ = 0.0;
        dcBlockY1_ = 0.0;

        inVuSq_  = 1e-10;
        outVuSq_ = 1e-10;
        smoothInitialized_ = false;
    }

    /**
     * Updates target values for sample-accurate parameter smoothers at the start of a block.
     */
    void setTargets(const Parameters& p) noexcept
    {
        const double makeupTarget = static_cast<double>(computeMakeupGainLinear(p));
        const double driveTarget  = std::pow(10.0, static_cast<double>(p.tubeIronDriveDb) / 20.0);
        const double wetTarget    = std::clamp(static_cast<double>(p.mix) * 0.01, 0.0, 1.0);
        const double trimTarget   = std::pow(10.0, static_cast<double>(p.trimDb) / 20.0);
        const double bypTarget    = p.bypass ? 1.0 : 0.0;

        if (!smoothInitialized_)
        {
            smPeakReduction_.reset(p.peakReduction);
            smTrapMemory_.reset(p.trapMemory);
            smTopologyMorph_.reset(p.topologyMorph);
            smPunchHoldoff_.reset(p.punchHoldoffMs);
            smBiasGlow_.reset(p.biasGlow);
            smCellAge_.reset(p.cellAge);
            smR37Emphasis_.reset(p.r37Emphasis);
            smScHighPassHz_.reset(p.scHighPassHz);

            smDriveLin_.reset(driveTarget);
            smBPlusSag_.reset(p.bPlusSag);
            smMillerAir_.reset(p.millerAir);
            smMakeupLin_.reset(makeupTarget);
            smWetMix_.reset(wetTarget);
            smTrimLin_.reset(trimTarget);
            smBypass_.reset(bypTarget);
            smoothInitialized_ = true;
        }
        else
        {
            smPeakReduction_.target = p.peakReduction;
            smTrapMemory_.target    = p.trapMemory;
            smTopologyMorph_.target = p.topologyMorph;
            smPunchHoldoff_.target  = p.punchHoldoffMs;
            smBiasGlow_.target      = p.biasGlow;
            smCellAge_.target       = p.cellAge;
            smR37Emphasis_.target   = p.r37Emphasis;
            smScHighPassHz_.target  = p.scHighPassHz;

            smDriveLin_.target      = driveTarget;
            smBPlusSag_.target      = p.bPlusSag;
            smMillerAir_.target     = p.millerAir;
            smMakeupLin_.target     = makeupTarget;
            smWetMix_.target        = wetTarget;
            smTrimLin_.target       = trimTarget;
            smBypass_.target        = bypTarget;
        }
    }

    /**
     * Step 1: Advances the T4B Electro-Optical Cell & Sidechain at base sample rate (fs_).
     * Returns instantaneous voltage divider linear gain g[n] and updates telemetry.
     */
    float processT4BSidechain(float mainIn, float keyIn, const Parameters& p, float& outScMonitor)
    {
        if (!smoothInitialized_)
            setTargets(p);

        // Track ballistic input VU level
        inVuSq_ += vuBallisticAlpha_ * (static_cast<double>(mainIn) * static_cast<double>(mainIn) - inVuSq_);

        // Sample-accurate smoothing of all T4B & sidechain parameters
        const double prVal      = smPeakReduction_.step(paramSmoothAlpha_);
        const double trapVal    = smTrapMemory_.step(paramSmoothAlpha_);
        const double topoVal    = smTopologyMorph_.step(paramSmoothAlpha_);
        const double punchValMs = smPunchHoldoff_.step(paramSmoothAlpha_);
        const double biasVal    = smBiasGlow_.step(paramSmoothAlpha_);
        const double ageVal     = smCellAge_.step(paramSmoothAlpha_);
        const double r37Val     = smR37Emphasis_.step(paramSmoothAlpha_);
        const double scHpValHz  = smScHighPassHz_.step(paramSmoothAlpha_);

        const double age = std::clamp(ageVal * 0.01, 0.0, 1.0);
        const double mem = std::clamp(trapVal * 0.01, -0.5, 2.0);
        const double biasGlowNorm = std::clamp(biasVal * 0.01, 0.0, 1.0);

        // 1. Resting Dark Resistance & Pre-Warmth Bias Glow (#4)
        const double nBias = 0.045 * biasGlowNorm;
        const double rDark = 2000000.0 * (1.0 + 0.50 * age) * (1.0 + 0.03 * tol_);
        const double r7 = 68000.0;

        // Resting conductance normalization (including bias glow) so 0 Peak Reduction = 0.0 dB insertion loss
        const double condRest = 1.0 + 38.0 * nBias + 26.0 * nBias * std::max(0.0, mem);
        const double muExp = p.limitMode ? 1.48 : 1.15;
        const double rLdrRest = rDark / std::pow(condRest, muExp);
        const double rShuntRest = (rLdrRest * 1000000.0) / (rLdrRest + 1000000.0);
        const double gRest = rShuntRest / (r7 + rShuntRest);

        // Current T4B voltage divider gain g[n]
        const double rShunt = (rLdr_ * 1000000.0) / (rLdr_ + 1000000.0);
        double gNow = (rShunt / (r7 + rShunt)) / gRest;
        gNow = std::clamp(gNow, 0.04, 1.4);
        lastGainLin_ = gNow;

        // 2. Topology Morph (0% Vintage Feedback <-> 100% Feed-Forward <-> 125% Negative-Ratio)
        // Includes closed-loop 12AX7A tube harmonic feedback when feedback topology (eta < 1.0) is active!
        const double eta = std::clamp(topoVal * 0.01, 0.0, 1.25);
        const double fbWeight = std::max(0.0, 1.0 - eta);
        const double tubeFbBoost = 1.0 + 0.12 * fbWeight * lastTubeHarmonicEnv_;
        const double rawTap = p.extSidechain
            ? ((1.0 - 0.5 * eta) * (static_cast<double>(keyIn) * gNow * tubeFbBoost) + 0.5 * eta * static_cast<double>(keyIn))
            : ((1.0 - eta) * (static_cast<double>(mainIn) * gNow * tubeFbBoost) + eta * static_cast<double>(mainIn));

        // 3. Sidechain High-Pass Filter (20 Hz .. 350 Hz)
        const double fcHp = std::clamp(scHpValHz, 20.0, 350.0);
        const double rHp = std::exp(-2.0 * M_PI * fcHp * dt_);
        const double hpOut = rawTap - scHpX1_ + rHp * scHpY1_;
        scHpX1_ = rawTap;
        scHpY1_ = hpOut;

        // 4. R37 Pre-Emphasis Tilt Network (150pF C4 + 1M R37 shelf above 1.2 kHz)
        const double alphaR37 = 1.0 - std::exp(-2.0 * M_PI * 1250.0 * dt_);
        r37Lp_ += alphaR37 * (hpOut - r37Lp_);
        const double hfShelf = hpOut - r37Lp_;
        const double r37Boost = (r37Val * 0.01) * 2.2;
        const double scFiltered = hpOut + r37Boost * hfShelf;

        // Capacitive EL panel displacement boost (I = C dV/dt subtly brightens on sibilance)
        const double dV = (scFiltered - elCapPrev_) * std::min(fs_ / 44100.0, 4.0);
        elCapPrev_ = scFiltered;
        const double scDriven = scFiltered + 0.16 * dV;

        outScMonitor = static_cast<float>(scDriven);

        // 5. 6AQ5A Beam Power Pentode + NE-2 Neon Lamp Soft Clamping (~165V peak)
        // Coupled to the shared 275V B+ supply rail so heavy B+ sag gently softens the 6AQ5 screen swing!
        const double prNorm = std::clamp(prVal * 0.01, 0.0, 1.0);
        const double prExp = prNorm * (0.28 + 0.72 * prNorm * (1.0 + 2.8 * prNorm * prNorm));
        const double bPlusScreenMod = 0.88 + 0.12 * bPlusRail_;
        const double scGain = prExp * (p.limitMode ? 520.0 : 410.0) * bPlusScreenMod;
        double vSc = 165.0 * std::tanh((std::abs(scDriven) * scGain) / 165.0);

        // 6. Transient Punch Holdoff (Inductor choke across EL Panel drive: 0..30 ms)
        const double alphaAc = 1.0 - std::exp(-dt_ / 0.018);
        if (vSc > acEnv_)
            acEnv_ += (1.0 - std::exp(-dt_ / 0.0002)) * (vSc - acEnv_);
        else
            acEnv_ += alphaAc * (vSc - acEnv_);

        if (punchValMs > 0.1)
        {
            const double tauHold = punchValMs * 0.001;
            const double alphaHold = 1.0 - std::exp(-dt_ / std::max(tauHold, 0.0002));
            if (acEnv_ > punchEnv_)
                punchEnv_ += alphaHold * (acEnv_ - punchEnv_);
            else
                punchEnv_ += (1.0 - std::exp(-dt_ / 0.005)) * (acEnv_ - punchEnv_);
            vSc = punchEnv_;
        }
        else
        {
            vSc = acEnv_;
            punchEnv_ = acEnv_;
        }

        // 7. Destriau Law for AC ZnS:Cu Electroluminescent Panel: L(V) = |V|^gamma * exp(-V0 / |V|)
        const double gammaEl = p.limitMode ? 2.45 : 1.90;
        const double v0El = (p.limitMode ? 15.0 : 19.0) * (1.0 + 0.40 * age);
        double L = 0.0;
        if (vSc > 1e-3)
        {
            const double norm = std::pow(68.0, gammaEl) * std::exp(-v0El / 68.0);
            L = (std::pow(vSc, gammaEl) * std::exp(-v0El / vSc)) / norm;
        }
        L += nBias; // Ambient pre-warmth photons (#4)

        // 8. Two-Trap Semiconductor Physics (CdS / CdSe / Silicon) — Scaled by dt_ = 1/fs_
        double tauA0 = 0.014, betaA = 12.0, tauRFast = 0.060, tauRDeep0 = 0.90, kTrapBase = 1.8;
        if (p.crystalType == 1) // CdSe Vactrol (Punchy Mid-Speed LA-3A style)
        {
            tauA0 = 0.0045; betaA = 18.0; tauRFast = 0.030; tauRDeep0 = 0.35; kTrapBase = 0.95;
        }
        else if (p.crystalType == 2) // Silicon Photodiode Mod (Sub-ms FET-like snap)
        {
            tauA0 = 0.0008; betaA = 35.0; tauRFast = 0.015; tauRDeep0 = 0.12; kTrapBase = 0.25;
        }

        const double kTrap = kTrapBase * std::abs(mem);
        const double tauAttack = (tauA0 * (1.0 - 0.04 * tol_)) / (1.0 + betaA * L);

        if (L > n1_)
        {
            const double alphaA = 1.0 - std::exp(-dt_ / std::max(tauAttack, 1e-5));
            n1_ += alphaA * (L - n1_);
        }
        else
        {
            const double alphaR1 = 1.0 - std::exp(-dt_ / tauRFast);
            const double deepBleed = (mem >= 0.0) ? (0.55 * n2_) : 0.0;
            const double targetN1 = std::max(nBias, L + deepBleed);
            n1_ += alphaR1 * (targetN1 - n1_);
        }

        if (n1_ > n2_)
        {
            const double alphaFill = 1.0 - std::exp(-dt_ * std::max(kTrap, 0.1));
            n2_ += alphaFill * (n1_ - n2_);
        }
        else
        {
            const double tauDeep = tauRDeep0 * (1.0 + 6.5 * std::max(0.0, mem) * std::min(n2_, 1.5));
            const double alphaEmpty = 1.0 - std::exp(-dt_ / std::max(tauDeep, 0.05));
            n2_ += alphaEmpty * (n1_ - n2_);
        }

        // 9. Photoconductivity Law R_LDR(t) (Supports Negative Trap Memory "Optical Bloom"!)
        double condFactor = 1.0 + 38.0 * n1_ + (mem >= 0.0 ? (26.0 * mem * n2_) : (18.0 * mem * n2_));
        condFactor = std::max(0.65, condFactor);
        rLdr_ = rDark / std::pow(condFactor, muExp);

        lastLum_ = static_cast<float>(L);
        return static_cast<float>(gNow);
    }

    /**
     * Step 2: Processes the Non-Linear Analog Signal Path & Phase-Locked Dry/Wet Blend
     * inside the oversampled domain (1x, 2x, 4x, or 8x) with C1-continuous gain envelope
     * filtering, closed-loop tube feedback tracking, and click-free oversampled bypass.
     */
    float processAnalogAndMixOsSample(float dryOs, float t4bGainLin, const Parameters& p, const AnalogStageCoeffs& c)
    {
        const double driveLin   = smDriveLin_.step(c.smoothAlphaOs);
        const double sagAmount  = std::clamp(smBPlusSag_.step(c.smoothAlphaOs) * 0.01, 0.0, 1.0);
        const double millerVal  = smMillerAir_.step(c.smoothAlphaOs);
        const double makeupLin  = smMakeupLin_.step(c.smoothAlphaOs);
        const double wetMix     = smWetMix_.step(c.smoothAlphaOs);
        const double dryMix     = 1.0 - wetMix;
        const double bypCross   = smBypass_.step(c.smoothAlphaOs);

        // C1-continuous oversampled T4B envelope interpolation (eliminates linear slope corners)
        gOsSmooth_ += c.alphaGainOs * (static_cast<double>(t4bGainLin) - gOsSmooth_);
        const double gSmooth = gOsSmooth_;

        const double v1Gain   = p.fatCathodeCap ? 1.45 : 1.0;
        const double asymBias = (p.fatCathodeCap ? -1.05 : -1.25) + 0.02 * tol_;

        // Small-signal unity calibration of the Koren triode + WCF stage (AGENTS.md Rule 2)
        constexpr double eps = 1e-4;
        const double u0 = 2.1 + asymBias;
        const double ip0 = std::pow(std::log1p(std::exp(1.6 * u0)), 1.35);
        const double uEps = 2.1 + (asymBias + eps * v1Gain);
        const double ipEps = std::pow(std::log1p(std::exp(1.6 * uEps)), 1.35);
        const double smallSigGain = std::abs(ipEps - ip0) / eps;
        const double invComp = 1.0 / (std::max(smallSigGain, 1e-4) * std::pow(driveLin, 0.78));

        // --- STAGE 1: UTC HA-100X Input Transformer (Permalloy 1/f Flux Hysteresis) ---
        const double vin = static_cast<double>(dryOs) * driveLin;
        phiIn_ = c.leakXfmr * phiIn_ + c.omegaXfmr * vin;
        const double dphi = phiIn_ - phiInPrev_;
        phiInPrev_ = phiIn_;
        const double signDphi = (dphi >= 0.0) ? 1.0 : -1.0;
        const double fluxSat = std::tanh(1.35 * (phiIn_ - 0.15 * signDphi * (1.0 - std::exp(-std::abs(phiIn_))))) - 1.35 * phiIn_;
        const double vXfmrIn = vin + 0.22 * fluxSat;

        // --- STAGE 2: T4B Voltage Divider Attenuation + Dynamic Miller Grid Impedance Air ---
        const double vDiv = vXfmrIn * gSmooth;
        millerLp_ += c.alphaMiller * (vDiv - millerLp_);
        const double hfMiller = vDiv - millerLp_;
        const double grDepth = std::clamp(1.0 - gSmooth, 0.0, 0.85);
        const double millerTilt = (millerVal * 0.01) * grDepth * 1.35;
        const double vGridIn = vDiv + millerTilt * hfMiller;

        // --- STAGE 3: V1 (12AX7A) Grounded-Cathode Triode + C2 Grid Conduction Sag ---
        const double vGk = vGridIn * v1Gain + asymBias - c2GridBias_;

        double vGkEff = vGk;
        if (vGk > -0.25)
        {
            const double gridExcess = vGk + 0.25;
            c2GridBias_ += c.alphaGridCharge * (0.35 * gridExcess - c2GridBias_);
            vGkEff = -0.25 + 0.65 * std::tanh(gridExcess / 0.65);
        }
        else
        {
            c2GridBias_ *= (1.0 - c.alphaGridRelease);
        }

        // --- STAGE 4: Shared B+ Power Supply Rail Sag (R29 = 4.7k / C7A Filter Cap) ---
        const double scLoad = grDepth * 0.14 * sagAmount;
        const double instantDemand = scLoad + 0.08 * sagAmount * std::min(std::abs(vGkEff - asymBias), 2.5);
        const double targetBPlus = std::max(0.76, 1.0 - instantDemand);
        if (targetBPlus < bPlusRail_)
            bPlusRail_ += c.alphaBDrop * (targetBPlus - bPlusRail_);
        else
            bPlusRail_ += c.alphaBRec * (targetBPlus - bPlusRail_);

        // Koren Triode Plate Equation (Non-inverting secondary via UTC A-24 output transformer)
        const double uTriode = 2.1 * bPlusRail_ + vGkEff;
        const double uRest   = 2.1 * bPlusRail_ + asymBias - c2GridBias_;
        const double ip      = std::pow(std::log1p(std::exp(1.6 * uTriode)), 1.35);
        const double ipRest  = std::pow(std::log1p(std::exp(1.6 * uRest)), 1.35);
        const double vPlate1 = (ip - ipRest);

        // --- STAGE 5: V2 (12BH7A) White Cathode Follower + UTC A-24 Output Transformer ---
        const double wcfPos = (p.load600Ohm ? 1.65 : 1.95) * bPlusRail_;
        const double wcfNeg = (p.load600Ohm ? 1.05 : 1.55) * bPlusRail_;
        const double vWcf   = (vPlate1 >= 0.0)
            ? (wcfPos * std::tanh(vPlate1 / wcfPos))
            : (wcfNeg * std::tanh(vPlate1 / wcfNeg));

        const double vLinearCal = vWcf * invComp;
        // Track non-linear harmonic difference for closed-loop T4B feedback coupling
        const double harmDiff = std::abs(vLinearCal - vDiv);
        lastTubeHarmonicEnv_ += c.smoothAlphaOs * (std::min(harmDiff, 1.5) - lastTubeHarmonicEnv_);

        const double wetOs = vLinearCal * makeupLin;
        const double activeOut = dryMix * static_cast<double>(dryOs) + wetMix * wetOs;
        return static_cast<float>((1.0 - bypCross) * activeOut + bypCross * static_cast<double>(dryOs));
    }

    /**
     * Backward-compatible wrapper for single-sample wet stage evaluation.
     */
    float processAnalogStageSample(float xIn, float t4bGainLin, const Parameters& p, double osSampleRate)
    {
        setTargets(p);
        gOsSmooth_ = static_cast<double>(t4bGainLin);
        AnalogStageCoeffs c;
        c.update(osSampleRate);
        Parameters wetOnly = p;
        wetOnly.mix = 100.0f;
        wetOnly.gainDb = 0.0f;
        wetOnly.autoLevel = false;
        wetOnly.bypass = false;
        smMakeupLin_.reset(1.0);
        smWetMix_.reset(1.0);
        smBypass_.reset(0.0);
        return processAnalogAndMixOsSample(xIn, t4bGainLin, wetOnly, c);
    }

    /**
     * Post-mix Output Capacitor C3 / UTC A-24 DC Blocker (5 Hz) + Smoothed Master Trim at base sample rate.
     * Running this after Dry/Wet blending guarantees zero low-frequency phase rotation between Dry and Wet.
     */
    float processPostMixDcBlockAndTrim(float xMixed)
    {
        const double trimLin = smTrimLin_.step(paramSmoothAlpha_);
        const double bypCross = smBypass_.current;
        const double effectiveTrim = (1.0 - bypCross) * trimLin + bypCross * 1.0;
        const double x = static_cast<double>(xMixed);
        const double dcR = std::exp(-2.0 * M_PI * 5.0 * dt_);
        const double dcY = (x - dcBlockX1_ + dcR * dcBlockY1_) * effectiveTrim;
        dcBlockX1_ = x;
        dcBlockY1_ = dcY;

        outVuSq_ += vuBallisticAlpha_ * (dcY * dcY - outVuSq_);
        return static_cast<float>(dcY);
    }

    float processPostMixDcBlock(float xMixed)
    {
        const double x = static_cast<double>(xMixed);
        const double dcR = std::exp(-2.0 * M_PI * 5.0 * dt_);
        const double dcY = x - dcBlockX1_ + dcR * dcBlockY1_;
        dcBlockX1_ = x;
        dcBlockY1_ = dcY;
        outVuSq_ += vuBallisticAlpha_ * (dcY * dcY - outVuSq_);
        return static_cast<float>(dcY);
    }

    /**
     * Computes makeup gain (including optional Auto-Level Link calibrated to nominal speech/music).
     */
    static float computeMakeupGainLinear(const Parameters& p)
    {
        double totalDb = static_cast<double>(p.gainDb);
        if (p.autoLevel)
        {
            const double prNorm = std::clamp(static_cast<double>(p.peakReduction) * 0.01, 0.0, 1.0);
            const double autoDb = (std::pow(prNorm, 1.45) * 9.5 + std::pow(prNorm, 2.8) * 8.5) * (p.limitMode ? 1.22 : 1.0);
            totalDb += autoDb;
        }
        return static_cast<float>(std::pow(10.0, totalDb / 20.0));
    }

    void setEffectiveGainReductionLin(double gLin) noexcept
    {
        lastGainLin_ = std::clamp(gLin, 0.04, 1.4);
    }

    Telemetry getTelemetry() const
    {
        Telemetry t;
        t.gainReductionDb = static_cast<float>(20.0 * std::log10(std::max(lastGainLin_, 1e-4)));
        t.elLuminescence  = lastLum_;
        t.shallowTrapN1   = static_cast<float>(n1_);
        t.deepTrapN2      = static_cast<float>(n2_);
        t.bPlusVolts      = static_cast<float>(bPlusRail_ * 275.0);
        t.inputVuDb       = static_cast<float>(10.0 * std::log10(std::max(inVuSq_, 1e-10)));
        t.outputVuDb      = static_cast<float>(10.0 * std::log10(std::max(outVuSq_, 1e-10)));
        return t;
    }

private:
    double fs_ = 48000.0;
    double dt_ = 1.0 / 48000.0;
    double tol_ = 0.0;
    double paramSmoothAlpha_ = 0.002;
    double vuBallisticAlpha_ = 0.0002;
    bool   smoothInitialized_ = false;

    // Sample-accurate parameter smoothers
    SmoothedParam smPeakReduction_, smTrapMemory_, smTopologyMorph_, smPunchHoldoff_;
    SmoothedParam smBiasGlow_, smCellAge_, smR37Emphasis_, smScHighPassHz_;
    SmoothedParam smDriveLin_, smBPlusSag_, smMillerAir_, smMakeupLin_, smWetMix_, smTrimLin_, smBypass_;

    // T4B & Sidechain states
    double n1_ = 0.0;
    double n2_ = 0.0;
    double rLdr_ = 2000000.0;
    double lastGainLin_ = 1.0;
    double gOsSmooth_ = 1.0;
    double lastTubeHarmonicEnv_ = 0.0;
    float  lastLum_ = 0.0f;
    double acEnv_ = 0.0;
    double punchEnv_ = 0.0;
    double scHpX1_ = 0.0;
    double scHpY1_ = 0.0;
    double r37Lp_ = 0.0;
    double elCapPrev_ = 0.0;

    // Analog Tube & Transformer states
    double phiIn_ = 0.0;
    double phiInPrev_ = 0.0;
    double c2GridBias_ = 0.0;
    double bPlusRail_ = 1.0;
    double millerLp_ = 0.0;
    double dcBlockX1_ = 0.0;
    double dcBlockY1_ = 0.0;

    // Ballistic VU states
    double inVuSq_  = 1e-10;
    double outVuSq_ = 1e-10;
};

struct FactoryPreset
{
    const char* name;
    const char* category;
    Parameters params;
    bool isMidSide = false;
    float stereoLink = 80.0f;
    Parameters sideParams {};
};

inline const std::array<FactoryPreset, 9>& getFactoryPresets()
{
    static const std::array<FactoryPreset, 9> presets = {{
        {
            "1. 1965 Pasadena Vocal Silk",
            "Vintage LA-2A",
            { 42.0f, 1.5f, false, true,   0, 100.0f, 0.0f, 2.5f, 20.0f, 15.0f,   2.5f, false, false, 40.0f, 15.0f,   55.0f, 65.0f, false, false,   100.0f, 0.0f, false },
            false, 80.0f, {}
        },
        {
            "2. Warm Motown Bass Iron (600 Ohm)",
            "Bass & Low End",
            { 44.0f, 4.7f, true,  true,   0, 135.0f, 15.0f, 6.0f, 35.0f, 30.0f,  6.5f, true,  true,  65.0f, -25.0f,  30.0f, 90.0f, false, false,   100.0f, 0.0f, false },
            false, 100.0f, {}
        },
        {
            "3. Punchy CdSe Drum Bus (LA-3A Mod)",
            "Drums & Transients",
            { 45.0f, 1.4f, false, true,   1, 45.0f,  55.0f, 12.0f, 15.0f, 10.0f, 5.0f, false, true,  55.0f, 25.0f,   45.0f, 110.0f, false, false,  100.0f, 0.0f, false },
            false, 75.0f, {}
        },
        {
            "4. Silicon Snap Limiter (FET-Opto Hybrid)",
            "Modern Limiting",
            { 38.0f, 3.7f, true,  true,   2, 20.0f,  75.0f, 4.0f,  25.0f, 0.0f,  4.0f, true,  false, 35.0f, 40.0f,   60.0f, 80.0f, false, false,   100.0f, 0.0f, false },
            false, 90.0f, {}
        },
        {
            "5. Reverse Optical Bloom (Negative Trap)",
            "Creative Sound Design",
            { 46.0f, 2.8f, false, true,   0, -45.0f, 95.0f, 8.0f, 10.0f, 20.0f,  7.5f, true,  true,  75.0f, 50.0f,   50.0f, 55.0f, false, false,   100.0f, 0.0f, false },
            false, 60.0f, {}
        },
        {
            "6. Molten White-Cathode Tube Overdrive",
            "Harmonic Saturation",
            { 34.0f, 5.2f, false, true,   1, 80.0f,  25.0f, 5.0f,  30.0f, 45.0f, 13.5f, true, true,  85.0f, -35.0f,  40.0f, 45.0f, false, false,   100.0f, 0.0f, false },
            false, 50.0f, {}
        },
        {
            "7. External Optical Key Ducker",
            "Sidechain & Mixing",
            { 42.0f, 6.1f, true,  true,   0, 90.0f,  60.0f, 0.5f,  40.0f, 10.0f, 2.0f, false, false, 30.0f, 10.0f,   50.0f, 40.0f, true,  false,   100.0f, 0.0f, false },
            false, 100.0f, {}
        },
        {
            "8. Master Bus Luminous Glue",
            "Mastering",
            { 32.0f, 1.0f, false, true,   0, 115.0f, 10.0f, 10.0f, 50.0f, 10.0f, 1.5f, false, false, 25.0f, 35.0f,   65.0f, 95.0f, false, false,   100.0f, 0.0f, false },
            false, 85.0f, {}
        },
        {
            "9. Mid-Side Mastering Sculptor (M/S)",
            "Mid-Side Mastering",
            // MID Channel: Tight warm CdS vocal/bass control with 95 Hz SC Warm-Cut & rich Fat Ck H2
            { 44.0f, 2.25f, false, true,  0, 110.0f, 15.0f, 6.0f,  30.0f, 15.0f, 3.5f, true,  false, 35.0f, 10.0f,   50.0f, 95.0f, false, false,   100.0f, 0.0f, false },
            true, 25.0f,
            // SIDE Channel: Fast CdSe Vactrol with +55% Miller Air, 160 Hz SC HPF, & gentle stereo width bloom
            { 30.0f, 2.2f, false, true,   1, -20.0f, 0.0f,  10.0f, 40.0f, 10.0f, 4.5f, false, true,  25.0f, 55.0f,   65.0f, 160.0f, false, false,  100.0f, 0.0f, false }
        }
    }};
    return presets;
}

} // namespace SilkenTofuDSP
