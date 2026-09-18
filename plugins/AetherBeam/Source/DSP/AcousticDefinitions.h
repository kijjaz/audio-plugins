#pragma once
#include <cmath>
#include <vector>

namespace AetherAcoustics
{
    constexpr float SPEED_OF_SOUND = 343.2f;      // m/s in air at 20°C
    constexpr float AIR_DENSITY = 1.204f;         // kg/m^3
    constexpr float BETA_AIR = 1.20f;             // Nonlinearity parameter (1 + B/2A)
    constexpr float P0_REF = 2.0e-5f;             // 20 uPa (0 dB SPL reference)
    constexpr float ACOUSTIC_STIFFNESS = 1.204f * 343.2f * 343.2f * 343.2f; // rho0 * c0^3 ~ 48,642
    constexpr float HEAD_RADIUS = 0.0875f;        // 8.75 cm standard adult head radius

    // ISO 9613-1 International Standard Atmospheric Acoustic Attenuation
    struct AtmosphericProperties
    {
        float temperatureC = 20.0f;           // 0 to 40 deg C
        float relativeHumidityPct = 50.0f;    // 10 to 95 %
        float atmosphericPressureKPa = 101.325f;

        inline float computeAbsorptionAlpha(float freqHz) const
        {
            float T = temperatureC + 273.15f;
            constexpr float T01 = 273.16f;
            constexpr float T0 = 293.15f;
            float Pa = atmosphericPressureKPa;
            constexpr float Pr = 101.325f;

            float C = -6.8346f * std::pow(T01 / T, 1.261f) + 4.6151f;
            float pSatPr = std::pow(10.0f, C);
            float h = relativeHumidityPct * pSatPr;

            float frO = (Pa / Pr) * (24.0f + 4.04e4f * h * (0.02f + h) / (0.391f + h));
            float frN = (Pa / Pr) * std::sqrt(T0 / T) * (9.0f + 280.0f * h * std::exp(-4.170f * (std::pow(T0 / T, 0.333333f) - 1.0f)));

            float term1 = 1.84e-11f * (Pr / Pa) * std::sqrt(T / T0);
            float termO = std::pow(T / T0, -2.5f) * (0.01275f * std::exp(-2239.1f / T)) / (frO + (freqHz * freqHz) / frO);
            float termN = std::pow(T / T0, -2.5f) * (0.1068f * std::exp(-3352.0f / T)) / (frN + (freqHz * freqHz) / frN);

            return 8.686f * (freqHz * freqHz) * (term1 + termO + termN); // dB/m
        }

        inline float computePathAirDampingCoeff(float distanceMeters, float sampleRate) const
        {
            float alpha10k = computeAbsorptionAlpha(10000.0f);
            float attenuationDb = alpha10k * distanceMeters;
            float linearGain = std::pow(10.0f, -attenuationDb / 20.0f);
            float fc = std::clamp(18000.0f * linearGain, 1200.0f, 20000.0f);
            float w = 2.0f * 3.14159265f * fc / sampleRate;
            return std::clamp(std::exp(-w), 0.05f, 0.96f);
        }
    };

    struct Vec3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
        Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
        Vec3 operator*(float s) const { return { x * s, y * s, z * s }; }
        float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
        Vec3 cross(const Vec3& o) const { return { y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x }; }
        float norm() const { return std::sqrt(x * x + y * y + z * z); }
        Vec3 normalized() const { float n = norm(); return n > 1e-6f ? (*this * (1.0f / n)) : Vec3{ 0, 1, 0 }; }
    };

    struct WireSegment
    {
        Vec3 a, b;
    };

    struct RaySegment
    {
        std::vector<Vec3> points;
        float distanceMeters = 0.0f;
        int order = 0;
        float gain = 1.0f;
    };

    struct BeamDescriptor
    {
        float distanceMeters = 1.0f;
        float delaySec = 0.003f;
        float gain = 1.0f;
        float dirX = 0.0f;
        float dirY = 1.0f;
        float dirZ = 0.0f;
        float absorptionFactor = 1.0f;
        int order = 0;
    };

    struct PositionData
    {
        int id = 0;
        const char* name = "";
        Vec3 source;
        Vec3 listener;
        float directDistance = 0.0f;
        std::vector<RaySegment> rays;
    };

    struct SpaceData
    {
        std::string id = "";
        std::string title = "";
        std::string category = "";
        float rt60 = 2.0f;
        float volume = 1000.0f;
        float area = 500.0f;
        Vec3 minBound;
        Vec3 maxBound;
        std::vector<WireSegment> wireframe;
        std::vector<PositionData> positions;
    };

    // Strict 3D Architectural Enclosure Validator
    inline bool isPointInsideArchitecture(const char* spaceId, const Vec3& p, const Vec3& bMin, const Vec3& bMax)
    {
        constexpr float eps = 0.5f;
        if (p.x < bMin.x - eps || p.x > bMax.x + eps ||
            p.y < bMin.y - eps || p.y > bMax.y + eps ||
            p.z < bMin.z - eps || p.z > bMax.z + eps)
            return false;

        if (spaceId == nullptr) return true;
        std::string sId(spaceId);

        // 1. Sydney Opera House (Pitched roof: 25m at center X=0, sloping to 13m at side walls X=+/-12m)
        if (sId.find("sydney") != std::string::npos)
        {
            float maxZ = 25.0f - std::abs(p.x);
            return p.z <= maxZ + eps;
        }

        // 2. Rome Pantheon (Cylindrical drum R=21.65m up to Z=21.65m, hemisphere above)
        if (sId.find("pantheon") != std::string::npos)
        {
            float r = std::sqrt(p.x * p.x + p.y * p.y);
            if (r > 21.65f + eps) return false;
            if (p.z <= 21.65f + eps) return true;
            float domeR2 = r * r + (p.z - 21.65f) * (p.z - 21.65f);
            return domeR2 <= (21.65f * 21.65f + 1.0f);
        }

        // 3. Hagia Sophia (Central dome R=15.5m up to 55m, semi-domes up to 36m, side aisles up to 18m)
        if (sId.find("hagia") != std::string::npos)
        {
            float r = std::sqrt(p.x * p.x + p.y * p.y);
            if (r < 15.5f)
            {
                float normR = r / 15.5f;
                float domeCeil = 25.0f + 30.0f * std::sqrt(std::max(0.0f, 1.0f - normR * normR));
                return p.z <= domeCeil + eps;
            }
            float rN = std::sqrt(p.x * p.x + (p.y - 18.0f) * (p.y - 18.0f));
            float rS = std::sqrt(p.x * p.x + (p.y + 18.0f) * (p.y + 18.0f));
            if (rN < 14.0f || rS < 14.0f)
            {
                return p.z <= 36.0f + eps;
            }
            return p.z <= 18.0f + eps;
        }

        // 4. Sibenik Cathedral (Vaulted nave width 9m apex 20m, crossing dome 29.8m, aisles 11.5m)
        if (sId.find("sibenik") != std::string::npos)
        {
            if (std::abs(p.x) < 4.5f && p.y >= 24.0f && p.y <= 32.0f)
                return p.z <= 29.8f + eps;
            if (std::abs(p.x) < 4.5f)
            {
                float normX = std::abs(p.x) / 4.5f;
                float naveCeil = 14.0f + 6.0f * std::sqrt(std::max(0.0f, 1.0f - normX * normX));
                return p.z <= naveCeil + eps;
            }
            return p.z <= 11.5f + eps;
        }

        // 5. Hamilton Mausoleum (High circular dome R=8.5m)
        if (sId.find("hamilton") != std::string::npos)
        {
            float r = std::sqrt(p.x * p.x + p.y * p.y);
            if (r > 8.5f + eps) return false;
            float normR = r / 8.5f;
            float ceilZ = 22.0f + 14.0f * std::sqrt(std::max(0.0f, 1.0f - normR * normR));
            return p.z <= ceilZ + eps;
        }

        // 6. Epidaurus Amphitheatre (Open-air ancient theatre, direct & ground paths only)
        if (sId.find("epidaurus") != std::string::npos)
        {
            return p.z <= 23.5f + eps;
        }

        return true;
    }

    // 3rd-4th Order Architectural Physical Ray Tracer
    inline std::vector<RaySegment> computeRealtimeRays(const Vec3& src, const Vec3& lis, 
                                                      const Vec3& bMin, const Vec3& bMax,
                                                      const char* spaceId = nullptr,
                                                      int maxOrder = 4,
                                                      size_t maxRaysToReturn = 64)
    {
        std::vector<RaySegment> allRays;
        Vec3 L = { bMax.x - bMin.x, bMax.y - bMin.y, bMax.z - bMin.z };
        if (L.x <= 0.1f || L.y <= 0.1f || L.z <= 0.1f)
            return allRays;

        Vec3 s_rel = src - bMin;
        Vec3 l_rel = lis - bMin;

        auto get1DImage = [](float s, float dim, int n) -> float {
            return (n % 2 == 0) ? (n * dim + s) : ((n + 1) * dim - s);
        };

        auto foldCoord = [](float v, float dim) -> float {
            float period = 2.0f * dim;
            float rem = std::fmod(v, period);
            if (rem < 0.0f) rem += period;
            if (rem > dim) rem = period - rem;
            return rem;
        };

        // Sweep image lattice in 3D: Order = |nx| + |ny| + |nz|
        for (int nx = -maxOrder; nx <= maxOrder; ++nx)
        {
            for (int ny = -maxOrder; ny <= maxOrder; ++ny)
            {
                for (int nz = -maxOrder; nz <= maxOrder; ++nz)
                {
                    int order = std::abs(nx) + std::abs(ny) + std::abs(nz);
                    if (order > maxOrder) continue;

                    // If open air amphitheatre, discard ceiling reflections (nz > 0)
                    if (nz > 0 && spaceId != nullptr && std::string(spaceId).find("epidaurus") != std::string::npos)
                        continue;

                    Vec3 imgPos = {
                        get1DImage(s_rel.x, L.x, nx),
                        get1DImage(s_rel.y, L.y, ny),
                        get1DImage(s_rel.z, L.z, nz)
                    };

                    Vec3 rayVec = l_rel - imgPos;
                    float dist = rayVec.norm();
                    if (dist < 0.05f) continue;

                    RaySegment ray;
                    ray.distanceMeters = dist;
                    ray.order = order;

                    // Physical 1/d spherical geometric attenuation + reflection loss
                    float reflectionLoss = std::pow(0.88f, static_cast<float>(order));
                    ray.gain = reflectionLoss / std::max(dist, 0.4f);

                    // Reconstruct exact bounce points along ray trajectory
                    if (order == 0)
                    {
                        ray.points = { src, lis };
                    }
                    else
                    {
                        std::vector<float> ts;
                        Vec3 delta = rayVec;
                        float dims[3] = { L.x, L.y, L.z };
                        float imgArr[3] = { imgPos.x, imgPos.y, imgPos.z };
                        float lisArr[3] = { l_rel.x, l_rel.y, l_rel.z };

                        for (int d = 0; d < 3; ++d)
                        {
                            if (std::abs(delta.x) < 1e-5f && d == 0) continue;
                            if (std::abs(delta.y) < 1e-5f && d == 1) continue;
                            if (std::abs(delta.z) < 1e-5f && d == 2) continue;

                            float dVal = (d == 0 ? delta.x : (d == 1 ? delta.y : delta.z));
                            float v0 = std::min(imgArr[d], lisArr[d]);
                            float v1 = std::max(imgArr[d], lisArr[d]);
                            int kMin = static_cast<int>(std::floor(v0 / dims[d]));
                            int kMax = static_cast<int>(std::ceil(v1 / dims[d]));

                            for (int k = kMin; k <= kMax; ++k)
                            {
                                float plane = k * dims[d];
                                float t = (plane - imgArr[d]) / dVal;
                                if (t > 1e-4f && t < 0.9999f)
                                {
                                    ts.push_back(t);
                                }
                            }
                        }

                        std::sort(ts.begin(), ts.end());
                        ts.erase(std::unique(ts.begin(), ts.end(), [](float a, float b) {
                            return std::abs(a - b) < 1e-4f;
                        }), ts.end());

                        ray.points.push_back(src);
                        bool rayPhysicallyContained = true;

                        for (float t : ts)
                        {
                            Vec3 wp = imgPos + delta * t;
                            Vec3 folded = {
                                foldCoord(wp.x, L.x) + bMin.x,
                                foldCoord(wp.y, L.y) + bMin.y,
                                foldCoord(wp.z, L.z) + bMin.z
                            };

                            // STRICT ARCHITECTURAL CONTAINMENT CHECK:
                            // If this reflection point lies outside the genuine 3D building geometry, discard ray
                            if (!isPointInsideArchitecture(spaceId, folded, bMin, bMax))
                            {
                                rayPhysicallyContained = false;
                                break;
                            }

                            ray.points.push_back(folded);
                        }

                        if (!rayPhysicallyContained) continue;
                        ray.points.push_back(lis);
                    }

                    allRays.push_back(std::move(ray));
                }
            }
        }

        // Sort rays by time of arrival (distance) so closest, most physically energetic rays arrive first
        std::sort(allRays.begin(), allRays.end(), [](const RaySegment& a, const RaySegment& b) {
            return a.distanceMeters < b.distanceMeters;
        });

        if (allRays.size() > maxRaysToReturn)
            allRays.resize(maxRaysToReturn);

        return allRays;
    }
}
