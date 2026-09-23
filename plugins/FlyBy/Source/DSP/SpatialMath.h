#pragma once
#include <cmath>

namespace FlyBy::DSP {

constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float SPEED_OF_SOUND = 343.0f; // m/s in air at 20C
constexpr float HEAD_RADIUS = 0.0875f;   // standard 8.75 cm human head radius

struct Vector3 {
    float x = 0.0f; // Left (-) to Right (+)
    float y = 1.0f; // Front (+) to Back (-)
    float z = 0.0f; // Down (-) to Up (+)

    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const {
        float l = length();
        if (l < 1e-6f) return Vector3(0.0f, 1.0f, 0.0f);
        return Vector3(x / l, y / l, z / l);
    }

    Vector3 operator+(const Vector3& o) const { return Vector3(x + o.x, y + o.y, z + o.z); }
    Vector3 operator-(const Vector3& o) const { return Vector3(x - o.x, y - o.y, z - o.z); }
    Vector3 operator*(float s) const { return Vector3(x * s, y * s, z * s); }
};

struct SphericalCoord {
    float azimuthDeg = 0.0f;   // -180 to +180 deg (0 = front, +90 = right, -90 = left)
    float elevationDeg = 0.0f; // -90 to +90 deg (0 = horizon, +90 = zenith/overhead, -90 = nadir/below)
    float distanceMeters = 1.0f;

    SphericalCoord() = default;
    SphericalCoord(float az, float el, float dist)
        : azimuthDeg(az), elevationDeg(el), distanceMeters(dist) {}

    static SphericalCoord fromCartesian(const Vector3& pos) {
        float dist = pos.length();
        if (dist < 1e-4f) dist = 1e-4f;

        // elevation: angle from horizontal plane (XY) to Z
        float elRad = std::asin(std::fmax(-1.0f, std::fmin(1.0f, pos.z / dist)));
        float elDeg = elRad * (180.0f / PI);

        // azimuth: angle in XY plane, 0 = front (y+), +90 = right (x+), -90 = left (x-)
        float azRad = std::atan2(pos.x, pos.y);
        float azDeg = azRad * (180.0f / PI);

        return SphericalCoord(azDeg, elDeg, dist);
    }

    Vector3 toCartesian() const {
        float azRad = azimuthDeg * (PI / 180.0f);
        float elRad = elevationDeg * (PI / 180.0f);
        float cosEl = std::cos(elRad);

        float x = distanceMeters * cosEl * std::sin(azRad);
        float y = distanceMeters * cosEl * std::cos(azRad);
        float z = distanceMeters * std::sin(elRad);

        return Vector3(x, y, z);
    }
};

} // namespace FlyBy::DSP
