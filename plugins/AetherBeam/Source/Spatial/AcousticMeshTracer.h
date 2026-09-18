#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <array>
#include "../DSP/AcousticDefinitions.h"

class AcousticMeshTracer
{
public:
    struct Vec3
    {
        float x = 0.0f, y = 0.0f, z = 0.0f;
        Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
        Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
        Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
        float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
        Vec3 cross(const Vec3& o) const { return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x}; }
        float norm() const { return std::sqrt(x * x + y * y + z * z); }
        Vec3 normalized() const { float n = norm(); return n > 1e-6f ? (*this * (1.0f / n)) : Vec3{0, 1, 0}; }
    };

    struct Triangle
    {
        Vec3 v0, v1, v2;
        Vec3 normal;
        float absorption = 0.035f;
    };

    bool loadFromOBJ(const std::string& filepath, float defaultAbsorption = 0.035f)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) return false;

        std::vector<Vec3> tempVertices;
        triangles.clear();

        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v")
            {
                Vec3 v;
                iss >> v.x >> v.y >> v.z;
                tempVertices.push_back(v);
            }
            else if (prefix == "f")
            {
                std::vector<int> faceIndices;
                std::string token;
                while (iss >> token)
                {
                    size_t slashPos = token.find('/');
                    std::string idxStr = (slashPos == std::string::npos) ? token : token.substr(0, slashPos);
                    faceIndices.push_back(std::stoi(idxStr) - 1);
                }

                if (faceIndices.size() >= 3)
                {
                    for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
                    {
                        Triangle tri;
                        tri.v0 = tempVertices[faceIndices[0]];
                        tri.v1 = tempVertices[faceIndices[i]];
                        tri.v2 = tempVertices[faceIndices[i + 1]];
                        Vec3 e1 = tri.v1 - tri.v0;
                        Vec3 e2 = tri.v2 - tri.v0;
                        tri.normal = e1.cross(e2).normalized();
                        tri.absorption = defaultAbsorption;
                        triangles.push_back(tri);
                    }
                }
            }
        }
        return !triangles.empty();
    }

    std::vector<AetherAcoustics::BeamDescriptor> tracePaths(Vec3 source, Vec3 listener, int maxOrder = 1)
    {
        std::vector<AetherAcoustics::BeamDescriptor> paths;

        // 1. Direct path
        Vec3 toListener = listener - source;
        float directDist = toListener.norm();
        if (directDist > 0.01f)
        {
            Vec3 dir = toListener.normalized();
            AetherAcoustics::BeamDescriptor direct;
            direct.distanceMeters = directDist;
            direct.delaySec = directDist / AetherAcoustics::SPEED_OF_SOUND;
            direct.gain = 1.0f / std::max(directDist, 0.5f);
            direct.dirX = dir.x;
            direct.dirY = dir.y;
            direct.dirZ = dir.z;
            direct.absorptionFactor = 1.0f;
            direct.order = 0;
            paths.push_back(direct);
        }

        // 2. Specular reflection paths across boundary triangles
        if (maxOrder >= 1)
        {
            for (const auto& tri : triangles)
            {
                float distSrc = (source - tri.v0).dot(tri.normal);
                float distLis = (listener - tri.v0).dot(tri.normal);
                if (distSrc <= 0.05f || distLis <= 0.05f) continue;

                // Virtual mirror source
                Vec3 sPrime = source - tri.normal * (2.0f * distSrc);
                Vec3 rayToLis = listener - sPrime;
                float totalDist = rayToLis.norm();
                if (totalDist < 0.01f) continue;

                Vec3 unitRay = rayToLis.normalized();
                float denom = unitRay.dot(tri.normal);
                if (std::abs(denom) < 1e-5f) continue;

                float tHit = (tri.v0 - sPrime).dot(tri.normal) / denom;
                Vec3 hitPt = sPrime + unitRay * tHit;

                // Point-in-triangle check
                Vec3 c0 = (tri.v1 - tri.v0).cross(hitPt - tri.v0);
                Vec3 c1 = (tri.v2 - tri.v1).cross(hitPt - tri.v1);
                Vec3 c2 = (tri.v0 - tri.v2).cross(hitPt - tri.v2);

                if (c0.dot(tri.normal) >= -0.05f && c1.dot(tri.normal) >= -0.05f && c2.dot(tri.normal) >= -0.05f)
                {
                    Vec3 arrivalDir = (hitPt - listener).normalized(); // Direction toward bounce point
                    AetherAcoustics::BeamDescriptor refl;
                    refl.distanceMeters = totalDist;
                    refl.delaySec = totalDist / AetherAcoustics::SPEED_OF_SOUND;
                    refl.gain = 1.0f / std::max(totalDist, 0.5f);
                    refl.dirX = arrivalDir.x;
                    refl.dirY = arrivalDir.y;
                    refl.dirZ = arrivalDir.z;
                    refl.absorptionFactor = std::sqrt(std::max(0.01f, 1.0f - tri.absorption));
                    refl.order = 1;
                    paths.push_back(refl);
                }
            }
        }
        return paths;
    }

    size_t getTriangleCount() const { return triangles.size(); }

private:
    std::vector<Triangle> triangles;
};
