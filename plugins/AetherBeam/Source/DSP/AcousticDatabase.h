#pragma once
#include <vector>
#include <string>
#include <array>
#include "AcousticDefinitions.h"

namespace AetherAcoustics
{
    class AcousticDatabase
    {
    public:
        static const std::vector<SpaceData>& getSpaces()
        {
            static const std::vector<SpaceData> spaces = initDatabase();
            return spaces;
        }

        static int getNumSpaces() { return 12; }
        static int getNumPositionsPerSpace() { return 4; }

    private:
        static std::vector<SpaceData> initDatabase()
        {
            std::vector<SpaceData> list;
            list.reserve(12);

            // Space 0: capitol_chamber_4
            {
                SpaceData sd;
                sd.id = "capitol_chamber_4";
                sd.title = "Capitol Studios Chamber 4 (Los Angeles)";
                sd.category = "Studio Echo Chambers";
                sd.rt60 = 4.200000f;
                sd.volume = 57.000000f;
                sd.area = 92.000000f;
                sd.minBound = { -2.100000f, 0.000000f, 0.000000f };
                sd.maxBound = { 2.100000f, 6.800000f, 3.400000f };
                sd.wireframe.reserve(14);
                sd.wireframe.push_back({ { -1.750000f, 0.000000f, 0.000000f }, { 1.750000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 1.750000f, 0.000000f, 0.000000f }, { 2.100000f, 6.800000f, 0.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 6.800000f, 0.000000f }, { -2.100000f, 6.800000f, 0.000000f } });
                sd.wireframe.push_back({ { -2.100000f, 6.800000f, 0.000000f }, { -1.750000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -1.750000f, 0.000000f, 3.400000f }, { 1.750000f, 0.000000f, 3.400000f } });
                sd.wireframe.push_back({ { 1.750000f, 0.000000f, 3.400000f }, { 2.100000f, 6.800000f, 3.400000f } });
                sd.wireframe.push_back({ { 2.100000f, 6.800000f, 3.400000f }, { -2.100000f, 6.800000f, 3.400000f } });
                sd.wireframe.push_back({ { -2.100000f, 6.800000f, 3.400000f }, { -1.750000f, 0.000000f, 3.400000f } });
                sd.wireframe.push_back({ { -1.750000f, 0.000000f, 0.000000f }, { -1.750000f, 0.000000f, 3.400000f } });
                sd.wireframe.push_back({ { 1.750000f, 0.000000f, 0.000000f }, { 1.750000f, 0.000000f, 3.400000f } });
                sd.wireframe.push_back({ { 2.100000f, 6.800000f, 0.000000f }, { 2.100000f, 6.800000f, 3.400000f } });
                sd.wireframe.push_back({ { -2.100000f, 6.800000f, 0.000000f }, { -2.100000f, 6.800000f, 3.400000f } });
                sd.wireframe.push_back({ { -1.000000f, 2.200000f, 2.400000f }, { 1.000000f, 2.500000f, 2.600000f } });
                sd.wireframe.push_back({ { -1.000000f, 4.400000f, 2.500000f }, { 1.000000f, 4.600000f, 2.300000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Corner Speaker to Far Corner Mic";
                    pd.source = { 1.200000f, 0.800000f, 0.600000f };
                    pd.listener = { -1.500000f, 5.800000f, 1.600000f };
                    pd.directDistance = 5.770000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.770000f;
                        rs.order = 0;
                        rs.gain = 0.173000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.090000f;
                        rs.order = 1;
                        rs.gain = 0.163000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ 0.460000f, 2.160000f, 0.000000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.250000f;
                        rs.order = 1;
                        rs.gain = 0.159000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ 1.650000f, 1.270000f, 0.710000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.250000f;
                        rs.order = 1;
                        rs.gain = 0.159000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ -1.950000f, 4.950000f, 1.440000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.980000f;
                        rs.order = 1;
                        rs.gain = 0.142000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ -0.340000f, 3.480000f, 3.110000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.200000f;
                        rs.order = 1;
                        rs.gain = 0.138000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ 0.870000f, 0.000000f, 0.720000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.570000f;
                        rs.order = 1;
                        rs.gain = 0.131000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ -1.110000f, 6.800000f, 1.460000f });
                        rs.points.push_back({ -1.500000f, 5.800000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Front Wall Slap to Center Chamber";
                    pd.source = { 0.000000f, 1.200000f, 0.800000f };
                    pd.listener = { 0.000000f, 3.800000f, 1.400000f };
                    pd.directDistance = 2.670000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 2.670000f;
                        rs.order = 0;
                        rs.gain = 0.375000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.410000f;
                        rs.order = 1;
                        rs.gain = 0.291000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 2.150000f, -0.000000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.390000f;
                        rs.order = 1;
                        rs.gain = 0.226000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ -1.730000f, 2.280000f, 1.080000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.390000f;
                        rs.order = 1;
                        rs.gain = 0.226000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 1.730000f, 2.280000f, 1.080000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.650000f;
                        rs.order = 1;
                        rs.gain = 0.214000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 2.460000f, 3.020000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.040000f;
                        rs.order = 1;
                        rs.gain = 0.197000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.940000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 8.620000f;
                        rs.order = 1;
                        rs.gain = 0.115000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.200000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 6.800000f, 1.190000f });
                        rs.points.push_back({ 0.000000f, 3.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Angled Wall Bounce to Stereo Right";
                    pd.source = { -1.000000f, 1.000000f, 0.600000f };
                    pd.listener = { 1.600000f, 5.200000f, 1.500000f };
                    pd.directDistance = 5.020000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.020000f;
                        rs.order = 0;
                        rs.gain = 0.199000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.370000f;
                        rs.order = 1;
                        rs.gain = 0.185000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ -0.260000f, 2.200000f, 0.000000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.390000f;
                        rs.order = 1;
                        rs.gain = 0.184000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ 1.920000f, 4.630000f, 1.390000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.840000f;
                        rs.order = 1;
                        rs.gain = 0.170000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ -1.670000f, 1.540000f, 0.740000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.420000f;
                        rs.order = 1;
                        rs.gain = 0.155000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ 0.470000f, 3.200000f, 3.080000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.780000f;
                        rs.order = 1;
                        rs.gain = 0.147000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ -0.580000f, -0.000000f, 0.750000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.890000f;
                        rs.order = 1;
                        rs.gain = 0.126000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.000000f, 1.000000f, 0.600000f });
                        rs.points.push_back({ 1.040000f, 6.800000f, 1.310000f });
                        rs.points.push_back({ 1.600000f, 5.200000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Rear Diffuse Field to Sweetspot";
                    pd.source = { 0.500000f, 2.000000f, 0.700000f };
                    pd.listener = { -0.800000f, 4.500000f, 1.200000f };
                    pd.directDistance = 2.860000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 2.860000f;
                        rs.order = 0;
                        rs.gain = 0.349000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.292000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ 0.020000f, 2.920000f, 0.000000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.292000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ 0.020000f, 2.920000f, 0.000000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.240000f;
                        rs.order = 1;
                        rs.gain = 0.234000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ -1.840000f, 3.540000f, 1.030000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.610000f;
                        rs.order = 1;
                        rs.gain = 0.216000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ 1.760000f, 2.640000f, 0.850000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.120000f;
                        rs.order = 1;
                        rs.gain = 0.194000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ -0.190000f, 3.150000f, 3.080000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.650000f;
                        rs.order = 1;
                        rs.gain = 0.150000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ 0.100000f, 0.000000f, 0.850000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.240000f;
                        rs.order = 1;
                        rs.gain = 0.137000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.500000f, 2.000000f, 0.700000f });
                        rs.points.push_back({ -0.380000f, 6.800000f, 1.040000f });
                        rs.points.push_back({ -0.800000f, 4.500000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 1: abbey_road_chamber_2
            {
                SpaceData sd;
                sd.id = "abbey_road_chamber_2";
                sd.title = "Abbey Road Studio Chamber 2 (London)";
                sd.category = "Studio Echo Chambers";
                sd.rt60 = 2.800000f;
                sd.volume = 66.000000f;
                sd.area = 104.000000f;
                sd.minBound = { -1.850000f, 0.000000f, 0.000000f };
                sd.maxBound = { 1.850000f, 6.400000f, 2.800000f };
                sd.wireframe.reserve(112);
                sd.wireframe.push_back({ { -1.850000f, 0.000000f, 0.000000f }, { 1.850000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 1.850000f, 0.000000f, 0.000000f }, { 1.850000f, 6.400000f, 0.000000f } });
                sd.wireframe.push_back({ { 1.850000f, 6.400000f, 0.000000f }, { -1.850000f, 6.400000f, 0.000000f } });
                sd.wireframe.push_back({ { -1.850000f, 6.400000f, 0.000000f }, { -1.850000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -1.850000f, 0.000000f, 2.800000f }, { 1.850000f, 0.000000f, 2.800000f } });
                sd.wireframe.push_back({ { 1.850000f, 0.000000f, 2.800000f }, { 1.850000f, 6.400000f, 2.800000f } });
                sd.wireframe.push_back({ { 1.850000f, 6.400000f, 2.800000f }, { -1.850000f, 6.400000f, 2.800000f } });
                sd.wireframe.push_back({ { -1.850000f, 6.400000f, 2.800000f }, { -1.850000f, 0.000000f, 2.800000f } });
                sd.wireframe.push_back({ { -1.850000f, 0.000000f, 0.000000f }, { -1.850000f, 0.000000f, 2.800000f } });
                sd.wireframe.push_back({ { 1.850000f, 0.000000f, 0.000000f }, { 1.850000f, 0.000000f, 2.800000f } });
                sd.wireframe.push_back({ { 1.850000f, 6.400000f, 0.000000f }, { 1.850000f, 6.400000f, 2.800000f } });
                sd.wireframe.push_back({ { -1.850000f, 6.400000f, 0.000000f }, { -1.850000f, 6.400000f, 2.800000f } });
                sd.wireframe.push_back({ { -0.550000f, 2.000000f, 0.000000f }, { -0.623223f, 2.176777f, 0.000000f } });
                sd.wireframe.push_back({ { -0.550000f, 2.000000f, 2.400000f }, { -0.623223f, 2.176777f, 2.400000f } });
                sd.wireframe.push_back({ { -0.550000f, 2.000000f, 0.000000f }, { -0.550000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.623223f, 2.176777f, 0.000000f }, { -0.800000f, 2.250000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.623223f, 2.176777f, 2.400000f }, { -0.800000f, 2.250000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 2.250000f, 0.000000f }, { -0.976777f, 2.176777f, 0.000000f } });
                sd.wireframe.push_back({ { -0.800000f, 2.250000f, 2.400000f }, { -0.976777f, 2.176777f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 2.250000f, 0.000000f }, { -0.800000f, 2.250000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.976777f, 2.176777f, 0.000000f }, { -1.050000f, 2.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.976777f, 2.176777f, 2.400000f }, { -1.050000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { -1.050000f, 2.000000f, 0.000000f }, { -0.976777f, 1.823223f, 0.000000f } });
                sd.wireframe.push_back({ { -1.050000f, 2.000000f, 2.400000f }, { -0.976777f, 1.823223f, 2.400000f } });
                sd.wireframe.push_back({ { -1.050000f, 2.000000f, 0.000000f }, { -1.050000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.976777f, 1.823223f, 0.000000f }, { -0.800000f, 1.750000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.976777f, 1.823223f, 2.400000f }, { -0.800000f, 1.750000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 1.750000f, 0.000000f }, { -0.623223f, 1.823223f, 0.000000f } });
                sd.wireframe.push_back({ { -0.800000f, 1.750000f, 2.400000f }, { -0.623223f, 1.823223f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 1.750000f, 0.000000f }, { -0.800000f, 1.750000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.623223f, 1.823223f, 0.000000f }, { -0.550000f, 2.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.623223f, 1.823223f, 2.400000f }, { -0.550000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { 1.050000f, 2.000000f, 0.000000f }, { 0.976777f, 2.176777f, 0.000000f } });
                sd.wireframe.push_back({ { 1.050000f, 2.000000f, 2.400000f }, { 0.976777f, 2.176777f, 2.400000f } });
                sd.wireframe.push_back({ { 1.050000f, 2.000000f, 0.000000f }, { 1.050000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.976777f, 2.176777f, 0.000000f }, { 0.800000f, 2.250000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.976777f, 2.176777f, 2.400000f }, { 0.800000f, 2.250000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 2.250000f, 0.000000f }, { 0.623223f, 2.176777f, 0.000000f } });
                sd.wireframe.push_back({ { 0.800000f, 2.250000f, 2.400000f }, { 0.623223f, 2.176777f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 2.250000f, 0.000000f }, { 0.800000f, 2.250000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.623223f, 2.176777f, 0.000000f }, { 0.550000f, 2.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.623223f, 2.176777f, 2.400000f }, { 0.550000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.550000f, 2.000000f, 0.000000f }, { 0.623223f, 1.823223f, 0.000000f } });
                sd.wireframe.push_back({ { 0.550000f, 2.000000f, 2.400000f }, { 0.623223f, 1.823223f, 2.400000f } });
                sd.wireframe.push_back({ { 0.550000f, 2.000000f, 0.000000f }, { 0.550000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.623223f, 1.823223f, 0.000000f }, { 0.800000f, 1.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.623223f, 1.823223f, 2.400000f }, { 0.800000f, 1.750000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 1.750000f, 0.000000f }, { 0.976777f, 1.823223f, 0.000000f } });
                sd.wireframe.push_back({ { 0.800000f, 1.750000f, 2.400000f }, { 0.976777f, 1.823223f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 1.750000f, 0.000000f }, { 0.800000f, 1.750000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.976777f, 1.823223f, 0.000000f }, { 1.050000f, 2.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.976777f, 1.823223f, 2.400000f }, { 1.050000f, 2.000000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.550000f, 4.200000f, 0.000000f }, { -0.623223f, 4.376777f, 0.000000f } });
                sd.wireframe.push_back({ { -0.550000f, 4.200000f, 2.400000f }, { -0.623223f, 4.376777f, 2.400000f } });
                sd.wireframe.push_back({ { -0.550000f, 4.200000f, 0.000000f }, { -0.550000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.623223f, 4.376777f, 0.000000f }, { -0.800000f, 4.450000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.623223f, 4.376777f, 2.400000f }, { -0.800000f, 4.450000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 4.450000f, 0.000000f }, { -0.976777f, 4.376777f, 0.000000f } });
                sd.wireframe.push_back({ { -0.800000f, 4.450000f, 2.400000f }, { -0.976777f, 4.376777f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 4.450000f, 0.000000f }, { -0.800000f, 4.450000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.976777f, 4.376777f, 0.000000f }, { -1.050000f, 4.200000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.976777f, 4.376777f, 2.400000f }, { -1.050000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { -1.050000f, 4.200000f, 0.000000f }, { -0.976777f, 4.023223f, 0.000000f } });
                sd.wireframe.push_back({ { -1.050000f, 4.200000f, 2.400000f }, { -0.976777f, 4.023223f, 2.400000f } });
                sd.wireframe.push_back({ { -1.050000f, 4.200000f, 0.000000f }, { -1.050000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.976777f, 4.023223f, 0.000000f }, { -0.800000f, 3.950000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.976777f, 4.023223f, 2.400000f }, { -0.800000f, 3.950000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 3.950000f, 0.000000f }, { -0.623223f, 4.023223f, 0.000000f } });
                sd.wireframe.push_back({ { -0.800000f, 3.950000f, 2.400000f }, { -0.623223f, 4.023223f, 2.400000f } });
                sd.wireframe.push_back({ { -0.800000f, 3.950000f, 0.000000f }, { -0.800000f, 3.950000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.623223f, 4.023223f, 0.000000f }, { -0.550000f, 4.200000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.623223f, 4.023223f, 2.400000f }, { -0.550000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { 1.050000f, 4.200000f, 0.000000f }, { 0.976777f, 4.376777f, 0.000000f } });
                sd.wireframe.push_back({ { 1.050000f, 4.200000f, 2.400000f }, { 0.976777f, 4.376777f, 2.400000f } });
                sd.wireframe.push_back({ { 1.050000f, 4.200000f, 0.000000f }, { 1.050000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.976777f, 4.376777f, 0.000000f }, { 0.800000f, 4.450000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.976777f, 4.376777f, 2.400000f }, { 0.800000f, 4.450000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 4.450000f, 0.000000f }, { 0.623223f, 4.376777f, 0.000000f } });
                sd.wireframe.push_back({ { 0.800000f, 4.450000f, 2.400000f }, { 0.623223f, 4.376777f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 4.450000f, 0.000000f }, { 0.800000f, 4.450000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.623223f, 4.376777f, 0.000000f }, { 0.550000f, 4.200000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.623223f, 4.376777f, 2.400000f }, { 0.550000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.550000f, 4.200000f, 0.000000f }, { 0.623223f, 4.023223f, 0.000000f } });
                sd.wireframe.push_back({ { 0.550000f, 4.200000f, 2.400000f }, { 0.623223f, 4.023223f, 2.400000f } });
                sd.wireframe.push_back({ { 0.550000f, 4.200000f, 0.000000f }, { 0.550000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.623223f, 4.023223f, 0.000000f }, { 0.800000f, 3.950000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.623223f, 4.023223f, 2.400000f }, { 0.800000f, 3.950000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 3.950000f, 0.000000f }, { 0.976777f, 4.023223f, 0.000000f } });
                sd.wireframe.push_back({ { 0.800000f, 3.950000f, 2.400000f }, { 0.976777f, 4.023223f, 2.400000f } });
                sd.wireframe.push_back({ { 0.800000f, 3.950000f, 0.000000f }, { 0.800000f, 3.950000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.976777f, 4.023223f, 0.000000f }, { 1.050000f, 4.200000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.976777f, 4.023223f, 2.400000f }, { 1.050000f, 4.200000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.250000f, 3.100000f, 0.000000f }, { 0.176777f, 3.276777f, 0.000000f } });
                sd.wireframe.push_back({ { 0.250000f, 3.100000f, 2.400000f }, { 0.176777f, 3.276777f, 2.400000f } });
                sd.wireframe.push_back({ { 0.250000f, 3.100000f, 0.000000f }, { 0.250000f, 3.100000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.176777f, 3.276777f, 0.000000f }, { 0.000000f, 3.350000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.176777f, 3.276777f, 2.400000f }, { 0.000000f, 3.350000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.000000f, 3.350000f, 0.000000f }, { -0.176777f, 3.276777f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 3.350000f, 2.400000f }, { -0.176777f, 3.276777f, 2.400000f } });
                sd.wireframe.push_back({ { 0.000000f, 3.350000f, 0.000000f }, { 0.000000f, 3.350000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.176777f, 3.276777f, 0.000000f }, { -0.250000f, 3.100000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.176777f, 3.276777f, 2.400000f }, { -0.250000f, 3.100000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.250000f, 3.100000f, 0.000000f }, { -0.176777f, 2.923223f, 0.000000f } });
                sd.wireframe.push_back({ { -0.250000f, 3.100000f, 2.400000f }, { -0.176777f, 2.923223f, 2.400000f } });
                sd.wireframe.push_back({ { -0.250000f, 3.100000f, 0.000000f }, { -0.250000f, 3.100000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.176777f, 2.923223f, 0.000000f }, { -0.000000f, 2.850000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.176777f, 2.923223f, 2.400000f }, { -0.000000f, 2.850000f, 2.400000f } });
                sd.wireframe.push_back({ { -0.000000f, 2.850000f, 0.000000f }, { 0.176777f, 2.923223f, 0.000000f } });
                sd.wireframe.push_back({ { -0.000000f, 2.850000f, 2.400000f }, { 0.176777f, 2.923223f, 2.400000f } });
                sd.wireframe.push_back({ { -0.000000f, 2.850000f, 0.000000f }, { -0.000000f, 2.850000f, 2.400000f } });
                sd.wireframe.push_back({ { 0.176777f, 2.923223f, 0.000000f }, { 0.250000f, 3.100000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.176777f, 2.923223f, 2.400000f }, { 0.250000f, 3.100000f, 2.400000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Speaker Behind Ceramic Pipes to Mics";
                    pd.source = { -1.200000f, 0.800000f, 0.600000f };
                    pd.listener = { 1.200000f, 5.400000f, 1.400000f };
                    pd.directDistance = 5.250000f;
                    pd.rays.reserve(1);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.700000f;
                        rs.order = 1;
                        rs.gain = 0.148000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.200000f, 0.800000f, 0.600000f });
                        rs.points.push_back({ -0.890000f, 0.000000f, 0.700000f });
                        rs.points.push_back({ 1.200000f, 5.400000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Direct Column Diffuser Slap";
                    pd.source = { 0.000000f, 1.000000f, 0.700000f };
                    pd.listener = { 0.000000f, 3.500000f, 1.400000f };
                    pd.directDistance = 2.600000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 2.600000f;
                        rs.order = 0;
                        rs.gain = 0.385000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.260000f;
                        rs.order = 1;
                        rs.gain = 0.304000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 1.830000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.293000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 2.630000f, 2.200000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.293000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 2.630000f, 2.200000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.293000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 2.630000f, 2.200000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 3.400000f;
                        rs.order = 1;
                        rs.gain = 0.293000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 2.630000f, 2.200000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.300000f;
                        rs.order = 1;
                        rs.gain = 0.230000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ 0.000000f, 2.500000f, 2.800000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 4.520000f;
                        rs.order = 1;
                        rs.gain = 0.219000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 1.000000f, 0.700000f });
                        rs.points.push_back({ -1.850000f, 2.250000f, 1.050000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Tiled Corner Flutter Reflection";
                    pd.source = { -1.400000f, 0.500000f, 0.600000f };
                    pd.listener = { 1.400000f, 5.800000f, 1.500000f };
                    pd.directDistance = 6.060000f;
                    pd.rays.reserve(2);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.530000f;
                        rs.order = 1;
                        rs.gain = 0.152000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.400000f, 0.500000f, 0.600000f });
                        rs.points.push_back({ 1.850000f, 5.160000f, 1.390000f });
                        rs.points.push_back({ 1.400000f, 5.800000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.950000f;
                        rs.order = 1;
                        rs.gain = 0.143000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -1.400000f, 0.500000f, 0.600000f });
                        rs.points.push_back({ -1.180000f, 0.000000f, 0.670000f });
                        rs.points.push_back({ 1.400000f, 5.800000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Mid-Chamber Pipe Scatter";
                    pd.source = { 0.800000f, 1.500000f, 0.600000f };
                    pd.listener = { -0.800000f, 4.800000f, 1.400000f };
                    pd.directDistance = 3.750000f;
                    pd.rays.reserve(2);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.020000f;
                        rs.order = 1;
                        rs.gain = 0.198000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.800000f, 1.500000f, 0.600000f });
                        rs.points.push_back({ -1.850000f, 3.860000f, 1.170000f });
                        rs.points.push_back({ -0.800000f, 4.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.020000f;
                        rs.order = 1;
                        rs.gain = 0.198000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.800000f, 1.500000f, 0.600000f });
                        rs.points.push_back({ 1.850000f, 2.440000f, 0.830000f });
                        rs.points.push_back({ -0.800000f, 4.800000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 2: sponza_palace
            {
                SpaceData sd;
                sd.id = "sponza_palace";
                sd.title = "Sponza Palace Atrium (Dubrovnik)";
                sd.category = "Renaissance Chambers";
                sd.rt60 = 2.450000f;
                sd.volume = 8400.000000f;
                sd.area = 2800.000000f;
                sd.minBound = { -7.800000f, 0.000000f, 0.000000f };
                sd.maxBound = { 7.800000f, 34.800000f, 16.500000f };
                sd.wireframe.reserve(246);
                sd.wireframe.push_back({ { -7.800000f, 0.000000f, 0.000000f }, { 7.800000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 7.800000f, 0.000000f, 0.000000f }, { 7.800000f, 34.800000f, 0.000000f } });
                sd.wireframe.push_back({ { 7.800000f, 34.800000f, 0.000000f }, { -7.800000f, 34.800000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.800000f, 34.800000f, 0.000000f }, { -7.800000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.800000f, 0.000000f, 5.200000f }, { 7.800000f, 0.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 7.800000f, 0.000000f, 5.200000f }, { 7.800000f, 34.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 7.800000f, 34.800000f, 5.200000f }, { -7.800000f, 34.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -7.800000f, 34.800000f, 5.200000f }, { -7.800000f, 0.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -7.800000f, 0.000000f, 10.400000f }, { 7.800000f, 0.000000f, 10.400000f } });
                sd.wireframe.push_back({ { 7.800000f, 0.000000f, 10.400000f }, { 7.800000f, 34.800000f, 10.400000f } });
                sd.wireframe.push_back({ { 7.800000f, 34.800000f, 10.400000f }, { -7.800000f, 34.800000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 34.800000f, 10.400000f }, { -7.800000f, 0.000000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 0.000000f, 16.500000f }, { 7.800000f, 0.000000f, 16.500000f } });
                sd.wireframe.push_back({ { 7.800000f, 0.000000f, 16.500000f }, { 7.800000f, 34.800000f, 16.500000f } });
                sd.wireframe.push_back({ { 7.800000f, 34.800000f, 16.500000f }, { -7.800000f, 34.800000f, 16.500000f } });
                sd.wireframe.push_back({ { -7.800000f, 34.800000f, 16.500000f }, { -7.800000f, 0.000000f, 16.500000f } });
                sd.wireframe.push_back({ { -7.800000f, 0.000000f, 0.000000f }, { -7.800000f, 0.000000f, 16.500000f } });
                sd.wireframe.push_back({ { 7.800000f, 0.000000f, 0.000000f }, { 7.800000f, 0.000000f, 16.500000f } });
                sd.wireframe.push_back({ { 7.800000f, 34.800000f, 0.000000f }, { 7.800000f, 34.800000f, 16.500000f } });
                sd.wireframe.push_back({ { -7.800000f, 34.800000f, 0.000000f }, { -7.800000f, 34.800000f, 16.500000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 0.000000f }, { 3.400000f, 6.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 0.000000f }, { 3.400000f, 28.800000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 0.000000f }, { -3.400000f, 28.800000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 0.000000f }, { -3.400000f, 6.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 5.200000f }, { 3.400000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 5.200000f }, { 3.400000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 5.200000f }, { -3.400000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 5.200000f }, { -3.400000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 10.400000f }, { 3.400000f, 6.000000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 10.400000f }, { 3.400000f, 28.800000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 10.400000f }, { -3.400000f, 28.800000f, 10.400000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 10.400000f }, { -3.400000f, 6.000000f, 10.400000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 0.000000f }, { -3.400000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 0.000000f }, { 3.400000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 5.200000f }, { -3.400000f, 6.000000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 5.200000f }, { 3.400000f, 6.000000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 6.000000f, 5.200000f }, { -3.400000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 5.200000f }, { 7.800000f, 6.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 9.800000f, 0.000000f }, { -3.400000f, 9.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 9.800000f, 0.000000f }, { 3.400000f, 9.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 9.800000f, 5.200000f }, { -3.400000f, 9.800000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 9.800000f, 5.200000f }, { 3.400000f, 9.800000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 9.800000f, 5.200000f }, { -3.400000f, 9.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 9.800000f, 5.200000f }, { 7.800000f, 9.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 13.600000f, 0.000000f }, { -3.400000f, 13.600000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 13.600000f, 0.000000f }, { 3.400000f, 13.600000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 13.600000f, 5.200000f }, { -3.400000f, 13.600000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 13.600000f, 5.200000f }, { 3.400000f, 13.600000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 13.600000f, 5.200000f }, { -3.400000f, 13.600000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 13.600000f, 5.200000f }, { 7.800000f, 13.600000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 17.400000f, 0.000000f }, { -3.400000f, 17.400000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 17.400000f, 0.000000f }, { 3.400000f, 17.400000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 17.400000f, 5.200000f }, { -3.400000f, 17.400000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 17.400000f, 5.200000f }, { 3.400000f, 17.400000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 17.400000f, 5.200000f }, { -3.400000f, 17.400000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 17.400000f, 5.200000f }, { 7.800000f, 17.400000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 21.200000f, 0.000000f }, { -3.400000f, 21.200000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 21.200000f, 0.000000f }, { 3.400000f, 21.200000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 21.200000f, 5.200000f }, { -3.400000f, 21.200000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 21.200000f, 5.200000f }, { 3.400000f, 21.200000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 21.200000f, 5.200000f }, { -3.400000f, 21.200000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 21.200000f, 5.200000f }, { 7.800000f, 21.200000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 25.000000f, 0.000000f }, { -3.400000f, 25.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 25.000000f, 0.000000f }, { 3.400000f, 25.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 25.000000f, 5.200000f }, { -3.400000f, 25.000000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 25.000000f, 5.200000f }, { 3.400000f, 25.000000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 25.000000f, 5.200000f }, { -3.400000f, 25.000000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 25.000000f, 5.200000f }, { 7.800000f, 25.000000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 0.000000f }, { -3.400000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 0.000000f }, { 3.400000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 5.200000f }, { -3.400000f, 28.800000f, 10.400000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 5.200000f }, { 3.400000f, 28.800000f, 10.400000f } });
                sd.wireframe.push_back({ { -7.800000f, 28.800000f, 5.200000f }, { -3.400000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 5.200000f }, { 7.800000f, 28.800000f, 5.200000f } });
                sd.wireframe.push_back({ { -3.400000f, 9.800000f, 3.100000f }, { -3.400000f, 9.611841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 9.800000f, 3.100000f }, { 3.400000f, 9.611841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 9.800000f, 8.300000f }, { -3.400000f, 9.611841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 9.800000f, 8.300000f }, { 3.400000f, 9.611841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 9.611841f, 3.924379f }, { -3.400000f, 9.084631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 9.611841f, 3.924379f }, { 3.400000f, 9.084631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 9.611841f, 9.124379f }, { -3.400000f, 9.084631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 9.611841f, 9.124379f }, { 3.400000f, 9.084631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 9.084631f, 4.585480f }, { -3.400000f, 8.322790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 9.084631f, 4.585480f }, { 3.400000f, 8.322790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 9.084631f, 9.785480f }, { -3.400000f, 8.322790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 9.084631f, 9.785480f }, { 3.400000f, 8.322790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 8.322790f, 4.952363f }, { -3.400000f, 7.477210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 8.322790f, 4.952363f }, { 3.400000f, 7.477210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 8.322790f, 10.152363f }, { -3.400000f, 7.477210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 8.322790f, 10.152363f }, { 3.400000f, 7.477210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 7.477210f, 4.952363f }, { -3.400000f, 6.715369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 7.477210f, 4.952363f }, { 3.400000f, 6.715369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 7.477210f, 10.152363f }, { -3.400000f, 6.715369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 7.477210f, 10.152363f }, { 3.400000f, 6.715369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 6.715369f, 4.585480f }, { -3.400000f, 6.188159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 6.715369f, 4.585480f }, { 3.400000f, 6.188159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 6.715369f, 9.785480f }, { -3.400000f, 6.188159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 6.715369f, 9.785480f }, { 3.400000f, 6.188159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 6.188159f, 3.924379f }, { -3.400000f, 6.000000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.188159f, 3.924379f }, { 3.400000f, 6.000000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.188159f, 9.124379f }, { -3.400000f, 6.000000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.188159f, 9.124379f }, { 3.400000f, 6.000000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 13.600000f, 3.100000f }, { -3.400000f, 13.411841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 13.600000f, 3.100000f }, { 3.400000f, 13.411841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 13.600000f, 8.300000f }, { -3.400000f, 13.411841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 13.600000f, 8.300000f }, { 3.400000f, 13.411841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 13.411841f, 3.924379f }, { -3.400000f, 12.884631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 13.411841f, 3.924379f }, { 3.400000f, 12.884631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 13.411841f, 9.124379f }, { -3.400000f, 12.884631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 13.411841f, 9.124379f }, { 3.400000f, 12.884631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 12.884631f, 4.585480f }, { -3.400000f, 12.122790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 12.884631f, 4.585480f }, { 3.400000f, 12.122790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 12.884631f, 9.785480f }, { -3.400000f, 12.122790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 12.884631f, 9.785480f }, { 3.400000f, 12.122790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 12.122790f, 4.952363f }, { -3.400000f, 11.277210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 12.122790f, 4.952363f }, { 3.400000f, 11.277210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 12.122790f, 10.152363f }, { -3.400000f, 11.277210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 12.122790f, 10.152363f }, { 3.400000f, 11.277210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 11.277210f, 4.952363f }, { -3.400000f, 10.515369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 11.277210f, 4.952363f }, { 3.400000f, 10.515369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 11.277210f, 10.152363f }, { -3.400000f, 10.515369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 11.277210f, 10.152363f }, { 3.400000f, 10.515369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 10.515369f, 4.585480f }, { -3.400000f, 9.988159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 10.515369f, 4.585480f }, { 3.400000f, 9.988159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 10.515369f, 9.785480f }, { -3.400000f, 9.988159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 10.515369f, 9.785480f }, { 3.400000f, 9.988159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 9.988159f, 3.924379f }, { -3.400000f, 9.800000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 9.988159f, 3.924379f }, { 3.400000f, 9.800000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 9.988159f, 9.124379f }, { -3.400000f, 9.800000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 9.988159f, 9.124379f }, { 3.400000f, 9.800000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 17.400000f, 3.100000f }, { -3.400000f, 17.211841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 17.400000f, 3.100000f }, { 3.400000f, 17.211841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 17.400000f, 8.300000f }, { -3.400000f, 17.211841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 17.400000f, 8.300000f }, { 3.400000f, 17.211841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 17.211841f, 3.924379f }, { -3.400000f, 16.684631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 17.211841f, 3.924379f }, { 3.400000f, 16.684631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 17.211841f, 9.124379f }, { -3.400000f, 16.684631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 17.211841f, 9.124379f }, { 3.400000f, 16.684631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 16.684631f, 4.585480f }, { -3.400000f, 15.922790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 16.684631f, 4.585480f }, { 3.400000f, 15.922790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 16.684631f, 9.785480f }, { -3.400000f, 15.922790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 16.684631f, 9.785480f }, { 3.400000f, 15.922790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 15.922790f, 4.952363f }, { -3.400000f, 15.077210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 15.922790f, 4.952363f }, { 3.400000f, 15.077210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 15.922790f, 10.152363f }, { -3.400000f, 15.077210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 15.922790f, 10.152363f }, { 3.400000f, 15.077210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 15.077210f, 4.952363f }, { -3.400000f, 14.315369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 15.077210f, 4.952363f }, { 3.400000f, 14.315369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 15.077210f, 10.152363f }, { -3.400000f, 14.315369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 15.077210f, 10.152363f }, { 3.400000f, 14.315369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 14.315369f, 4.585480f }, { -3.400000f, 13.788159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 14.315369f, 4.585480f }, { 3.400000f, 13.788159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 14.315369f, 9.785480f }, { -3.400000f, 13.788159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 14.315369f, 9.785480f }, { 3.400000f, 13.788159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 13.788159f, 3.924379f }, { -3.400000f, 13.600000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 13.788159f, 3.924379f }, { 3.400000f, 13.600000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 13.788159f, 9.124379f }, { -3.400000f, 13.600000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 13.788159f, 9.124379f }, { 3.400000f, 13.600000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 21.200000f, 3.100000f }, { -3.400000f, 21.011841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 21.200000f, 3.100000f }, { 3.400000f, 21.011841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 21.200000f, 8.300000f }, { -3.400000f, 21.011841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 21.200000f, 8.300000f }, { 3.400000f, 21.011841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 21.011841f, 3.924379f }, { -3.400000f, 20.484631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 21.011841f, 3.924379f }, { 3.400000f, 20.484631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 21.011841f, 9.124379f }, { -3.400000f, 20.484631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 21.011841f, 9.124379f }, { 3.400000f, 20.484631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 20.484631f, 4.585480f }, { -3.400000f, 19.722790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 20.484631f, 4.585480f }, { 3.400000f, 19.722790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 20.484631f, 9.785480f }, { -3.400000f, 19.722790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 20.484631f, 9.785480f }, { 3.400000f, 19.722790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 19.722790f, 4.952363f }, { -3.400000f, 18.877210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 19.722790f, 4.952363f }, { 3.400000f, 18.877210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 19.722790f, 10.152363f }, { -3.400000f, 18.877210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 19.722790f, 10.152363f }, { 3.400000f, 18.877210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 18.877210f, 4.952363f }, { -3.400000f, 18.115369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 18.877210f, 4.952363f }, { 3.400000f, 18.115369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 18.877210f, 10.152363f }, { -3.400000f, 18.115369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 18.877210f, 10.152363f }, { 3.400000f, 18.115369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 18.115369f, 4.585480f }, { -3.400000f, 17.588159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 18.115369f, 4.585480f }, { 3.400000f, 17.588159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 18.115369f, 9.785480f }, { -3.400000f, 17.588159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 18.115369f, 9.785480f }, { 3.400000f, 17.588159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 17.588159f, 3.924379f }, { -3.400000f, 17.400000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 17.588159f, 3.924379f }, { 3.400000f, 17.400000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 17.588159f, 9.124379f }, { -3.400000f, 17.400000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 17.588159f, 9.124379f }, { 3.400000f, 17.400000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 25.000000f, 3.100000f }, { -3.400000f, 24.811841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 25.000000f, 3.100000f }, { 3.400000f, 24.811841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 25.000000f, 8.300000f }, { -3.400000f, 24.811841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 25.000000f, 8.300000f }, { 3.400000f, 24.811841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 24.811841f, 3.924379f }, { -3.400000f, 24.284631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 24.811841f, 3.924379f }, { 3.400000f, 24.284631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 24.811841f, 9.124379f }, { -3.400000f, 24.284631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 24.811841f, 9.124379f }, { 3.400000f, 24.284631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 24.284631f, 4.585480f }, { -3.400000f, 23.522790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 24.284631f, 4.585480f }, { 3.400000f, 23.522790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 24.284631f, 9.785480f }, { -3.400000f, 23.522790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 24.284631f, 9.785480f }, { 3.400000f, 23.522790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 23.522790f, 4.952363f }, { -3.400000f, 22.677210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 23.522790f, 4.952363f }, { 3.400000f, 22.677210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 23.522790f, 10.152363f }, { -3.400000f, 22.677210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 23.522790f, 10.152363f }, { 3.400000f, 22.677210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 22.677210f, 4.952363f }, { -3.400000f, 21.915369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 22.677210f, 4.952363f }, { 3.400000f, 21.915369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 22.677210f, 10.152363f }, { -3.400000f, 21.915369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 22.677210f, 10.152363f }, { 3.400000f, 21.915369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 21.915369f, 4.585480f }, { -3.400000f, 21.388159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 21.915369f, 4.585480f }, { 3.400000f, 21.388159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 21.915369f, 9.785480f }, { -3.400000f, 21.388159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 21.915369f, 9.785480f }, { 3.400000f, 21.388159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 21.388159f, 3.924379f }, { -3.400000f, 21.200000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 21.388159f, 3.924379f }, { 3.400000f, 21.200000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 21.388159f, 9.124379f }, { -3.400000f, 21.200000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 21.388159f, 9.124379f }, { 3.400000f, 21.200000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 3.100000f }, { -3.400000f, 28.611841f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 3.100000f }, { 3.400000f, 28.611841f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 8.300000f }, { -3.400000f, 28.611841f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 8.300000f }, { 3.400000f, 28.611841f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 28.611841f, 3.924379f }, { -3.400000f, 28.084631f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 28.611841f, 3.924379f }, { 3.400000f, 28.084631f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 28.611841f, 9.124379f }, { -3.400000f, 28.084631f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 28.611841f, 9.124379f }, { 3.400000f, 28.084631f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 28.084631f, 4.585480f }, { -3.400000f, 27.322790f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 28.084631f, 4.585480f }, { 3.400000f, 27.322790f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 28.084631f, 9.785480f }, { -3.400000f, 27.322790f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 28.084631f, 9.785480f }, { 3.400000f, 27.322790f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 27.322790f, 4.952363f }, { -3.400000f, 26.477210f, 4.952363f } });
                sd.wireframe.push_back({ { 3.400000f, 27.322790f, 4.952363f }, { 3.400000f, 26.477210f, 4.952363f } });
                sd.wireframe.push_back({ { -3.400000f, 27.322790f, 10.152363f }, { -3.400000f, 26.477210f, 10.152363f } });
                sd.wireframe.push_back({ { 3.400000f, 27.322790f, 10.152363f }, { 3.400000f, 26.477210f, 10.152363f } });
                sd.wireframe.push_back({ { -3.400000f, 26.477210f, 4.952363f }, { -3.400000f, 25.715369f, 4.585480f } });
                sd.wireframe.push_back({ { 3.400000f, 26.477210f, 4.952363f }, { 3.400000f, 25.715369f, 4.585480f } });
                sd.wireframe.push_back({ { -3.400000f, 26.477210f, 10.152363f }, { -3.400000f, 25.715369f, 9.785480f } });
                sd.wireframe.push_back({ { 3.400000f, 26.477210f, 10.152363f }, { 3.400000f, 25.715369f, 9.785480f } });
                sd.wireframe.push_back({ { -3.400000f, 25.715369f, 4.585480f }, { -3.400000f, 25.188159f, 3.924379f } });
                sd.wireframe.push_back({ { 3.400000f, 25.715369f, 4.585480f }, { 3.400000f, 25.188159f, 3.924379f } });
                sd.wireframe.push_back({ { -3.400000f, 25.715369f, 9.785480f }, { -3.400000f, 25.188159f, 9.124379f } });
                sd.wireframe.push_back({ { 3.400000f, 25.715369f, 9.785480f }, { 3.400000f, 25.188159f, 9.124379f } });
                sd.wireframe.push_back({ { -3.400000f, 25.188159f, 3.924379f }, { -3.400000f, 25.000000f, 3.100000f } });
                sd.wireframe.push_back({ { 3.400000f, 25.188159f, 3.924379f }, { 3.400000f, 25.000000f, 3.100000f } });
                sd.wireframe.push_back({ { -3.400000f, 25.188159f, 9.124379f }, { -3.400000f, 25.000000f, 8.300000f } });
                sd.wireframe.push_back({ { 3.400000f, 25.188159f, 9.124379f }, { 3.400000f, 25.000000f, 8.300000f } });
                sd.wireframe.push_back({ { -3.400000f, 6.000000f, 16.500000f }, { 3.400000f, 6.000000f, 16.500000f } });
                sd.wireframe.push_back({ { 3.400000f, 6.000000f, 16.500000f }, { 3.400000f, 28.800000f, 16.500000f } });
                sd.wireframe.push_back({ { 3.400000f, 28.800000f, 16.500000f }, { -3.400000f, 28.800000f, 16.500000f } });
                sd.wireframe.push_back({ { -3.400000f, 28.800000f, 16.500000f }, { -3.400000f, 6.000000f, 16.500000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Courtyard Center to Ground Arcade";
                    pd.source = { 0.000000f, 17.000000f, 1.200000f };
                    pd.listener = { -4.500000f, 12.000000f, 1.200000f };
                    pd.directDistance = 6.730000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.730000f;
                        rs.order = 0;
                        rs.gain = 0.149000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.740000f;
                        rs.order = 1;
                        rs.gain = 0.146000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -2.250000f, 14.500000f, 0.990000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.230000f;
                        rs.order = 1;
                        rs.gain = 0.136000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -4.780000f, 11.960000f, 1.200000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 9.540000f;
                        rs.order = 1;
                        rs.gain = 0.103000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -4.940000f, 16.560000f, 1.180000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.300000f;
                        rs.order = 1;
                        rs.gain = 0.080000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -5.710000f, 13.600000f, 5.250000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.700000f;
                        rs.order = 1;
                        rs.gain = 0.078000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -7.950000f, 14.820000f, 1.200000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.700000f;
                        rs.order = 1;
                        rs.gain = 0.078000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ -7.950000f, 14.820000f, 1.200000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.860000f;
                        rs.order = 1;
                        rs.gain = 0.077000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 17.000000f, 1.200000f });
                        rs.points.push_back({ 2.370000f, 13.480000f, 4.230000f });
                        rs.points.push_back({ -4.500000f, 12.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Front Portal to Deep Courtyard Recess";
                    pd.source = { 0.000000f, 5.000000f, 1.500000f };
                    pd.listener = { 0.000000f, 28.000000f, 1.500000f };
                    pd.directDistance = 23.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.000000f;
                        rs.order = 0;
                        rs.gain = 0.043000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.020000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 16.500000f, 0.990000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.200000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 16.500000f, -0.010000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.200000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 16.500000f, -0.010000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.500000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ -2.420000f, 16.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.500000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ -2.420000f, 16.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.500000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ -2.420000f, 16.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.500000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.500000f });
                        rs.points.push_back({ -2.420000f, 16.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Ground Arcade to 1st-Tier Balcony";
                    pd.source = { 4.200000f, 10.000000f, 1.200000f };
                    pd.listener = { -4.200000f, 22.000000f, 5.800000f };
                    pd.directDistance = 15.350000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.350000f;
                        rs.order = 0;
                        rs.gain = 0.065000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.510000f;
                        rs.order = 1;
                        rs.gain = 0.063000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ 2.660000f, 13.870000f, 2.190000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.510000f;
                        rs.order = 1;
                        rs.gain = 0.063000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ 2.590000f, 13.980000f, 2.230000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 17.390000f;
                        rs.order = 1;
                        rs.gain = 0.057000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ -2.480000f, 22.860000f, 6.360000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.460000f;
                        rs.order = 1;
                        rs.gain = 0.053000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ -2.440000f, 12.360000f, 6.220000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.460000f;
                        rs.order = 1;
                        rs.gain = 0.053000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ -2.450000f, 12.360000f, 6.220000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 21.260000f;
                        rs.order = 1;
                        rs.gain = 0.046000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ -3.050000f, 18.000000f, 11.010000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 21.280000f;
                        rs.order = 1;
                        rs.gain = 0.046000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.200000f, 10.000000f, 1.200000f });
                        rs.points.push_back({ 0.300000f, 8.740000f, 7.140000f });
                        rs.points.push_back({ -4.200000f, 22.000000f, 5.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Diagonal Column Corner Cross Echo";
                    pd.source = { -4.800000f, 7.000000f, 1.200000f };
                    pd.listener = { 4.800000f, 27.000000f, 1.400000f };
                    pd.directDistance = 22.190000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.190000f;
                        rs.order = 0;
                        rs.gain = 0.045000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.190000f;
                        rs.order = 1;
                        rs.gain = 0.044000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -1.570000f, 13.740000f, 0.990000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.840000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.980000f, 6.790000f, 1.480000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.840000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.980000f, 6.790000f, 1.470000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.930000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.610000f, 6.590000f, 1.200000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.930000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.610000f, 6.590000f, 1.200000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.930000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.610000f, 6.590000f, 1.200000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.930000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -4.800000f, 7.000000f, 1.200000f });
                        rs.points.push_back({ -4.610000f, 6.590000f, 1.200000f });
                        rs.points.push_back({ 4.800000f, 27.000000f, 1.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 3: sibenik_cathedral
            {
                SpaceData sd;
                sd.id = "sibenik_cathedral";
                sd.title = "St. James Cathedral (UNESCO Sibenik)";
                sd.category = "Sacred Cathedrals";
                sd.rt60 = 4.800000f;
                sd.volume = 14200.000000f;
                sd.area = 4100.000000f;
                sd.minBound = { -8.500000f, 0.000000f, 0.000000f };
                sd.maxBound = { 8.500000f, 40.500000f, 29.800000f };
                sd.wireframe.reserve(133);
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { 8.500000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 8.500000f, 40.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 40.500000f, 0.000000f }, { -8.500000f, 40.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 40.500000f, 0.000000f }, { -8.500000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 12.000000f }, { -8.500000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 12.000000f }, { -4.200000f, 0.000000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 40.500000f, 12.000000f }, { -4.200000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 12.000000f }, { 8.500000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 12.000000f }, { 4.200000f, 0.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 40.500000f, 12.000000f }, { 4.200000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { -8.500000f, 0.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 8.500000f, 0.000000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 40.500000f, 0.000000f }, { -8.500000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 40.500000f, 0.000000f }, { 8.500000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 0.000000f, 0.000000f }, { -4.200000f, 0.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 0.000000f, 0.000000f }, { 4.200000f, 0.000000f, 19.500000f } });
                sd.wireframe.push_back({ { -4.200000f, 0.000000f, 19.500000f }, { 4.200000f, 0.000000f, 19.500000f } });
                sd.wireframe.push_back({ { -4.200000f, 0.000000f, 19.500000f }, { -4.200000f, 28.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 0.000000f, 19.500000f }, { 4.200000f, 28.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 0.000000f, 19.500000f }, { 3.880294f, 0.000000f, 20.625089f } });
                sd.wireframe.push_back({ { 3.880294f, 0.000000f, 20.625089f }, { 2.969848f, 0.000000f, 21.578894f } });
                sd.wireframe.push_back({ { 2.969848f, 0.000000f, 21.578894f }, { 1.607270f, 0.000000f, 22.216206f } });
                sd.wireframe.push_back({ { 1.607270f, 0.000000f, 22.216206f }, { 0.000000f, 0.000000f, 22.440000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 22.440000f }, { -1.607270f, 0.000000f, 22.216206f } });
                sd.wireframe.push_back({ { -1.607270f, 0.000000f, 22.216206f }, { -2.969848f, 0.000000f, 21.578894f } });
                sd.wireframe.push_back({ { -2.969848f, 0.000000f, 21.578894f }, { -3.880294f, 0.000000f, 20.625089f } });
                sd.wireframe.push_back({ { -3.880294f, 0.000000f, 20.625089f }, { -4.200000f, 0.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 7.000000f, 19.500000f }, { 3.880294f, 7.000000f, 20.625089f } });
                sd.wireframe.push_back({ { 3.880294f, 7.000000f, 20.625089f }, { 2.969848f, 7.000000f, 21.578894f } });
                sd.wireframe.push_back({ { 2.969848f, 7.000000f, 21.578894f }, { 1.607270f, 7.000000f, 22.216206f } });
                sd.wireframe.push_back({ { 1.607270f, 7.000000f, 22.216206f }, { 0.000000f, 7.000000f, 22.440000f } });
                sd.wireframe.push_back({ { 0.000000f, 7.000000f, 22.440000f }, { -1.607270f, 7.000000f, 22.216206f } });
                sd.wireframe.push_back({ { -1.607270f, 7.000000f, 22.216206f }, { -2.969848f, 7.000000f, 21.578894f } });
                sd.wireframe.push_back({ { -2.969848f, 7.000000f, 21.578894f }, { -3.880294f, 7.000000f, 20.625089f } });
                sd.wireframe.push_back({ { -3.880294f, 7.000000f, 20.625089f }, { -4.200000f, 7.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 14.000000f, 19.500000f }, { 3.880294f, 14.000000f, 20.625089f } });
                sd.wireframe.push_back({ { 3.880294f, 14.000000f, 20.625089f }, { 2.969848f, 14.000000f, 21.578894f } });
                sd.wireframe.push_back({ { 2.969848f, 14.000000f, 21.578894f }, { 1.607270f, 14.000000f, 22.216206f } });
                sd.wireframe.push_back({ { 1.607270f, 14.000000f, 22.216206f }, { 0.000000f, 14.000000f, 22.440000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.000000f, 22.440000f }, { -1.607270f, 14.000000f, 22.216206f } });
                sd.wireframe.push_back({ { -1.607270f, 14.000000f, 22.216206f }, { -2.969848f, 14.000000f, 21.578894f } });
                sd.wireframe.push_back({ { -2.969848f, 14.000000f, 21.578894f }, { -3.880294f, 14.000000f, 20.625089f } });
                sd.wireframe.push_back({ { -3.880294f, 14.000000f, 20.625089f }, { -4.200000f, 14.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 21.000000f, 19.500000f }, { 3.880294f, 21.000000f, 20.625089f } });
                sd.wireframe.push_back({ { 3.880294f, 21.000000f, 20.625089f }, { 2.969848f, 21.000000f, 21.578894f } });
                sd.wireframe.push_back({ { 2.969848f, 21.000000f, 21.578894f }, { 1.607270f, 21.000000f, 22.216206f } });
                sd.wireframe.push_back({ { 1.607270f, 21.000000f, 22.216206f }, { 0.000000f, 21.000000f, 22.440000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.000000f, 22.440000f }, { -1.607270f, 21.000000f, 22.216206f } });
                sd.wireframe.push_back({ { -1.607270f, 21.000000f, 22.216206f }, { -2.969848f, 21.000000f, 21.578894f } });
                sd.wireframe.push_back({ { -2.969848f, 21.000000f, 21.578894f }, { -3.880294f, 21.000000f, 20.625089f } });
                sd.wireframe.push_back({ { -3.880294f, 21.000000f, 20.625089f }, { -4.200000f, 21.000000f, 19.500000f } });
                sd.wireframe.push_back({ { 4.200000f, 28.000000f, 19.500000f }, { 3.880294f, 28.000000f, 20.625089f } });
                sd.wireframe.push_back({ { 3.880294f, 28.000000f, 20.625089f }, { 2.969848f, 28.000000f, 21.578894f } });
                sd.wireframe.push_back({ { 2.969848f, 28.000000f, 21.578894f }, { 1.607270f, 28.000000f, 22.216206f } });
                sd.wireframe.push_back({ { 1.607270f, 28.000000f, 22.216206f }, { 0.000000f, 28.000000f, 22.440000f } });
                sd.wireframe.push_back({ { 0.000000f, 28.000000f, 22.440000f }, { -1.607270f, 28.000000f, 22.216206f } });
                sd.wireframe.push_back({ { -1.607270f, 28.000000f, 22.216206f }, { -2.969848f, 28.000000f, 21.578894f } });
                sd.wireframe.push_back({ { -2.969848f, 28.000000f, 21.578894f }, { -3.880294f, 28.000000f, 20.625089f } });
                sd.wireframe.push_back({ { -3.880294f, 28.000000f, 20.625089f }, { -4.200000f, 28.000000f, 19.500000f } });
                sd.wireframe.push_back({ { -4.200000f, 4.000000f, 0.000000f }, { -4.200000f, 4.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 4.000000f, 0.000000f }, { 4.200000f, 4.000000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 4.000000f, 12.000000f }, { -4.200000f, 4.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 4.000000f, 12.000000f }, { 8.500000f, 4.000000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 8.800000f, 0.000000f }, { -4.200000f, 8.800000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 8.800000f, 0.000000f }, { 4.200000f, 8.800000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 8.800000f, 12.000000f }, { -4.200000f, 8.800000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 8.800000f, 12.000000f }, { 8.500000f, 8.800000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 13.600000f, 0.000000f }, { -4.200000f, 13.600000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 13.600000f, 0.000000f }, { 4.200000f, 13.600000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 13.600000f, 12.000000f }, { -4.200000f, 13.600000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 13.600000f, 12.000000f }, { 8.500000f, 13.600000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 18.400000f, 0.000000f }, { -4.200000f, 18.400000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 18.400000f, 0.000000f }, { 4.200000f, 18.400000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 18.400000f, 12.000000f }, { -4.200000f, 18.400000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 18.400000f, 12.000000f }, { 8.500000f, 18.400000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 23.200000f, 0.000000f }, { -4.200000f, 23.200000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 23.200000f, 0.000000f }, { 4.200000f, 23.200000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 23.200000f, 12.000000f }, { -4.200000f, 23.200000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 23.200000f, 12.000000f }, { 8.500000f, 23.200000f, 12.000000f } });
                sd.wireframe.push_back({ { -4.200000f, 28.000000f, 0.000000f }, { -4.200000f, 28.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 28.000000f, 0.000000f }, { 4.200000f, 28.000000f, 12.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 28.000000f, 12.000000f }, { -4.200000f, 28.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 28.000000f, 12.000000f }, { 8.500000f, 28.000000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.000000f, 33.250000f, 19.500000f }, { 2.828427f, 36.078427f, 19.500000f } });
                sd.wireframe.push_back({ { 4.000000f, 33.250000f, 26.800000f }, { 2.828427f, 36.078427f, 26.800000f } });
                sd.wireframe.push_back({ { 4.000000f, 33.250000f, 19.500000f }, { 4.000000f, 33.250000f, 26.800000f } });
                sd.wireframe.push_back({ { 4.000000f, 33.250000f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 36.078427f, 19.500000f }, { 0.000000f, 37.250000f, 19.500000f } });
                sd.wireframe.push_back({ { 2.828427f, 36.078427f, 26.800000f }, { 0.000000f, 37.250000f, 26.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 36.078427f, 19.500000f }, { 2.828427f, 36.078427f, 26.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 36.078427f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { 0.000000f, 37.250000f, 19.500000f }, { -2.828427f, 36.078427f, 19.500000f } });
                sd.wireframe.push_back({ { 0.000000f, 37.250000f, 26.800000f }, { -2.828427f, 36.078427f, 26.800000f } });
                sd.wireframe.push_back({ { 0.000000f, 37.250000f, 19.500000f }, { 0.000000f, 37.250000f, 26.800000f } });
                sd.wireframe.push_back({ { 0.000000f, 37.250000f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 36.078427f, 19.500000f }, { -4.000000f, 33.250000f, 19.500000f } });
                sd.wireframe.push_back({ { -2.828427f, 36.078427f, 26.800000f }, { -4.000000f, 33.250000f, 26.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 36.078427f, 19.500000f }, { -2.828427f, 36.078427f, 26.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 36.078427f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { -4.000000f, 33.250000f, 19.500000f }, { -2.828427f, 30.421573f, 19.500000f } });
                sd.wireframe.push_back({ { -4.000000f, 33.250000f, 26.800000f }, { -2.828427f, 30.421573f, 26.800000f } });
                sd.wireframe.push_back({ { -4.000000f, 33.250000f, 19.500000f }, { -4.000000f, 33.250000f, 26.800000f } });
                sd.wireframe.push_back({ { -4.000000f, 33.250000f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 30.421573f, 19.500000f }, { -0.000000f, 29.250000f, 19.500000f } });
                sd.wireframe.push_back({ { -2.828427f, 30.421573f, 26.800000f }, { -0.000000f, 29.250000f, 26.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 30.421573f, 19.500000f }, { -2.828427f, 30.421573f, 26.800000f } });
                sd.wireframe.push_back({ { -2.828427f, 30.421573f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { -0.000000f, 29.250000f, 19.500000f }, { 2.828427f, 30.421573f, 19.500000f } });
                sd.wireframe.push_back({ { -0.000000f, 29.250000f, 26.800000f }, { 2.828427f, 30.421573f, 26.800000f } });
                sd.wireframe.push_back({ { -0.000000f, 29.250000f, 19.500000f }, { -0.000000f, 29.250000f, 26.800000f } });
                sd.wireframe.push_back({ { -0.000000f, 29.250000f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 30.421573f, 19.500000f }, { 4.000000f, 33.250000f, 19.500000f } });
                sd.wireframe.push_back({ { 2.828427f, 30.421573f, 26.800000f }, { 4.000000f, 33.250000f, 26.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 30.421573f, 19.500000f }, { 2.828427f, 30.421573f, 26.800000f } });
                sd.wireframe.push_back({ { 2.828427f, 30.421573f, 26.800000f }, { 0.000000f, 33.250000f, 29.800000f } });
                sd.wireframe.push_back({ { 0.000000f, 36.300000f, 0.000000f }, { 2.100000f, 36.581347f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 36.300000f, 12.000000f }, { 2.100000f, 36.581347f, 12.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 36.300000f, 0.000000f }, { 0.000000f, 36.300000f, 12.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 36.581347f, 0.000000f }, { 3.637307f, 37.350000f, 0.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 36.581347f, 12.000000f }, { 3.637307f, 37.350000f, 12.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 36.581347f, 0.000000f }, { 2.100000f, 36.581347f, 12.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 37.350000f, 0.000000f }, { 4.200000f, 38.400000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 37.350000f, 12.000000f }, { 4.200000f, 38.400000f, 12.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 37.350000f, 0.000000f }, { 3.637307f, 37.350000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 38.400000f, 0.000000f }, { 3.637307f, 39.450000f, 0.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 38.400000f, 12.000000f }, { 3.637307f, 39.450000f, 12.000000f } });
                sd.wireframe.push_back({ { 4.200000f, 38.400000f, 0.000000f }, { 4.200000f, 38.400000f, 12.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 39.450000f, 0.000000f }, { 2.100000f, 40.218653f, 0.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 39.450000f, 12.000000f }, { 2.100000f, 40.218653f, 12.000000f } });
                sd.wireframe.push_back({ { 3.637307f, 39.450000f, 0.000000f }, { 3.637307f, 39.450000f, 12.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 40.218653f, 0.000000f }, { 0.000000f, 40.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 40.218653f, 12.000000f }, { 0.000000f, 40.500000f, 12.000000f } });
                sd.wireframe.push_back({ { 2.100000f, 40.218653f, 0.000000f }, { 2.100000f, 40.218653f, 12.000000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "High Altar Sanctuary to Center Nave";
                    pd.source = { 0.000000f, 34.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 18.000000f, 1.200000f };
                    pd.directDistance = 16.010000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.010000f;
                        rs.order = 0;
                        rs.gain = 0.062000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.020000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.210000f, 24.060000f, 1.390000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.020000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.210000f, 24.060000f, 1.390000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.050000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.510000f, 25.170000f, 1.380000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.060000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.420000f, 31.740000f, 1.650000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.080000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.730000f, 25.420000f, 1.340000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.110000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.730000f, 30.580000f, 1.560000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.130000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 34.000000f, 1.800000f });
                        rs.points.push_back({ -0.960000f, 25.560000f, 1.310000f });
                        rs.points.push_back({ 0.000000f, 18.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Stone Pulpit to North Aisle Vaults";
                    pd.source = { -3.500000f, 26.000000f, 2.500000f };
                    pd.listener = { -5.500000f, 14.000000f, 1.200000f };
                    pd.directDistance = 12.230000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.630000f;
                        rs.order = 1;
                        rs.gain = 0.078000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -4.210000f, 25.440000f, 1.880000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.790000f;
                        rs.order = 1;
                        rs.gain = 0.077000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -4.230000f, 25.650000f, 2.960000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.820000f;
                        rs.order = 1;
                        rs.gain = 0.071000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -2.280000f, 18.190000f, -0.200000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.990000f;
                        rs.order = 1;
                        rs.gain = 0.070000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -2.130000f, 18.220000f, -0.300000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 14.170000f;
                        rs.order = 1;
                        rs.gain = 0.070000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -1.980000f, 18.240000f, -0.410000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.420000f;
                        rs.order = 1;
                        rs.gain = 0.060000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -6.510000f, 20.250000f, 6.950000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.110000f;
                        rs.order = 1;
                        rs.gain = 0.054000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -0.830000f, 26.500000f, -1.160000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.130000f;
                        rs.order = 1;
                        rs.gain = 0.054000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -3.500000f, 26.000000f, 2.500000f });
                        rs.points.push_back({ -0.910000f, 26.740000f, 5.390000f });
                        rs.points.push_back({ -5.500000f, 14.000000f, 1.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Octagonal Dome Crossing to West Portal";
                    pd.source = { 0.000000f, 28.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 5.000000f, 1.600000f };
                    pd.directDistance = 23.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.000000f;
                        rs.order = 0;
                        rs.gain = 0.043000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.410000f;
                        rs.order = 1;
                        rs.gain = 0.042000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ 0.360000f, 27.870000f, 1.420000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.760000f;
                        rs.order = 1;
                        rs.gain = 0.041000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ -2.930000f, 16.430000f, 1.190000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.020000f;
                        rs.order = 1;
                        rs.gain = 0.041000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ 0.830000f, 27.750000f, 2.660000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.280000f;
                        rs.order = 1;
                        rs.gain = 0.041000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ -1.250000f, 27.380000f, 0.600000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.400000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ -3.320000f, 10.470000f, 0.440000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.560000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ -0.980000f, 28.390000f, 1.350000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 25.370000f;
                        rs.order = 1;
                        rs.gain = 0.039000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 28.000000f, 1.800000f });
                        rs.points.push_back({ -2.100000f, 27.180000f, 3.760000f });
                        rs.points.push_back({ 0.000000f, 5.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Side Apse to Elevated Organ Gallery";
                    pd.source = { 4.500000f, 32.000000f, 1.500000f };
                    pd.listener = { 0.000000f, 7.000000f, 7.200000f };
                    pd.directDistance = 26.030000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.030000f;
                        rs.order = 0;
                        rs.gain = 0.038000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.420000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 3.350000f, 31.030000f, 2.120000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.520000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 1.760000f, 24.250000f, 1.350000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.580000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 1.630000f, 24.030000f, 1.280000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.640000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 1.510000f, 23.830000f, 1.200000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.700000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 2.720000f, 30.640000f, 2.430000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.700000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 1.390000f, 23.650000f, 1.120000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.840000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 4.500000f, 32.000000f, 1.500000f });
                        rs.points.push_back({ 2.870000f, 31.200000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 7.000000f, 7.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 4: musikverein_vienna
            {
                SpaceData sd;
                sd.id = "musikverein_vienna";
                sd.title = "Grosser Musikvereinssaal (Vienna)";
                sd.category = "Concert Halls";
                sd.rt60 = 2.050000f;
                sd.volume = 15000.000000f;
                sd.area = 4200.000000f;
                sd.minBound = { -9.900000f, 0.000000f, 0.000000f };
                sd.maxBound = { 9.900000f, 48.800000f, 17.700000f };
                sd.wireframe.reserve(41);
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 48.800000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 48.800000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 0.000000f }, { -9.900000f, 48.800000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 0.000000f }, { -9.900000f, 48.800000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.900000f, 0.000000f, 1.200000f }, { 8.900000f, 0.000000f, 1.200000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 0.000000f }, { 8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { -9.900000f, 48.800000f, 0.000000f }, { 8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { -8.900000f, 0.000000f, 1.200000f }, { 8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { 8.900000f, 0.000000f, 1.200000f }, { 8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { -9.900000f, 48.800000f, 0.000000f }, { -8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { -8.900000f, 0.000000f, 1.200000f }, { -8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { 8.900000f, 12.000000f, 1.200000f }, { -8.900000f, 12.000000f, 1.200000f } });
                sd.wireframe.push_back({ { -9.900000f, 12.000000f, 6.000000f }, { -7.700000f, 12.000000f, 6.000000f } });
                sd.wireframe.push_back({ { -9.900000f, 12.000000f, 6.000000f }, { -7.700000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { -7.700000f, 12.000000f, 6.000000f }, { -7.700000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { -9.900000f, 12.000000f, 6.000000f }, { -9.900000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { -7.700000f, 48.800000f, 6.000000f }, { -9.900000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { 7.700000f, 12.000000f, 6.000000f }, { 9.900000f, 12.000000f, 6.000000f } });
                sd.wireframe.push_back({ { 7.700000f, 12.000000f, 6.000000f }, { 9.900000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { 9.900000f, 12.000000f, 6.000000f }, { 9.900000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { 7.700000f, 12.000000f, 6.000000f }, { 7.700000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 6.000000f }, { 7.700000f, 48.800000f, 6.000000f } });
                sd.wireframe.push_back({ { -9.900000f, 44.800000f, 7.500000f }, { 9.900000f, 44.800000f, 7.500000f } });
                sd.wireframe.push_back({ { -9.900000f, 44.800000f, 7.500000f }, { 9.900000f, 48.800000f, 8.500000f } });
                sd.wireframe.push_back({ { 9.900000f, 44.800000f, 7.500000f }, { 9.900000f, 48.800000f, 8.500000f } });
                sd.wireframe.push_back({ { -9.900000f, 44.800000f, 7.500000f }, { -9.900000f, 48.800000f, 8.500000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 8.500000f }, { -9.900000f, 48.800000f, 8.500000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 0.000000f }, { -9.900000f, 0.000000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 48.800000f, 0.000000f }, { -9.900000f, 0.000000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 0.000000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 0.000000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 17.700000f }, { 9.900000f, 0.000000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 0.000000f, 0.000000f }, { 9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 0.000000f }, { 9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 17.700000f }, { 9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 0.000000f, 17.700000f }, { 9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 0.000000f }, { -9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 48.800000f, 0.000000f }, { -9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { -9.900000f, 0.000000f, 17.700000f }, { -9.900000f, 48.800000f, 17.700000f } });
                sd.wireframe.push_back({ { 9.900000f, 48.800000f, 17.700000f }, { -9.900000f, 48.800000f, 17.700000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Conductor Podium to Front Row";
                    pd.source = { 0.000000f, 4.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 10.000000f, 1.500000f };
                    pd.directDistance = 6.010000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.010000f;
                        rs.order = 0;
                        rs.gain = 0.166000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.070000f;
                        rs.order = 1;
                        rs.gain = 0.161000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.200000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 14.000000f;
                        rs.order = 1;
                        rs.gain = 0.070000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.710000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 20.690000f;
                        rs.order = 1;
                        rs.gain = 0.047000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 9.900000f, 7.000000f, 1.650000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 20.690000f;
                        rs.order = 1;
                        rs.gain = 0.047000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ -9.900000f, 7.000000f, 1.650000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 32.660000f;
                        rs.order = 1;
                        rs.gain = 0.030000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 6.970000f, 17.700000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 83.600000f;
                        rs.order = 1;
                        rs.gain = 0.012000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 4.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 48.800000f, 1.640000f });
                        rs.points.push_back({ 0.000000f, 10.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Mid-Parquet Golden Hall Sweetspot";
                    pd.source = { 0.000000f, 6.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 24.000000f, 1.600000f };
                    pd.directDistance = 18.000000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.000000f;
                        rs.order = 0;
                        rs.gain = 0.056000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.040000f;
                        rs.order = 1;
                        rs.gain = 0.053000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 12.070000f, 1.200000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.760000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 9.900000f, 15.000000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 26.760000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ -9.900000f, 15.000000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.000000f;
                        rs.order = 1;
                        rs.gain = 0.033000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.760000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.720000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 14.940000f, 17.700000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 67.600000f;
                        rs.order = 1;
                        rs.gain = 0.014000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 48.800000f, 1.670000f });
                        rs.points.push_back({ 0.000000f, 24.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Left Balcony Lateral Reflection";
                    pd.source = { 2.000000f, 5.000000f, 1.800000f };
                    pd.listener = { -8.000000f, 28.000000f, 6.500000f };
                    pd.directDistance = 25.520000f;
                    pd.rays.reserve(6);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 25.520000f;
                        rs.order = 0;
                        rs.gain = 0.039000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 25.760000f;
                        rs.order = 1;
                        rs.gain = 0.038000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ 0.980000f, 7.340000f, 1.200000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.800000f;
                        rs.order = 1;
                        rs.gain = 0.028000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ 0.480000f, 0.000000f, 2.510000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.880000f;
                        rs.order = 1;
                        rs.gain = 0.028000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ 9.900000f, 12.040000f, 3.240000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.920000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ -3.870000f, 18.490000f, 17.700000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 65.540000f;
                        rs.order = 1;
                        rs.gain = 0.015000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 2.000000f, 5.000000f, 1.800000f });
                        rs.points.push_back({ -4.780000f, 48.800000f, 4.990000f });
                        rs.points.push_back({ -8.000000f, 28.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Rear Organ Gallery Perspective";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 44.000000f, 7.000000f };
                    pd.directDistance = 37.360000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.360000f;
                        rs.order = 0;
                        rs.gain = 0.027000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.550000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 10.470000f, 1.200000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 42.290000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 9.900000f, 25.500000f, 4.400000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 42.290000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -9.900000f, 25.500000f, 4.400000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 45.570000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 29.120000f, 17.700000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 46.890000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 48.800000f, 6.460000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.260000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 2.510000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 7.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 5: concertgebouw_amsterdam
            {
                SpaceData sd;
                sd.id = "concertgebouw_amsterdam";
                sd.title = "Concertgebouw Grote Zaal (Amsterdam)";
                sd.category = "Concert Halls";
                sd.rt60 = 2.200000f;
                sd.volume = 18780.000000f;
                sd.area = 4900.000000f;
                sd.minBound = { -14.000000f, 0.000000f, 0.000000f };
                sd.maxBound = { 14.000000f, 44.000000f, 17.000000f };
                sd.wireframe.reserve(55);
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 44.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 44.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 0.000000f }, { -14.000000f, 44.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 0.000000f }, { -14.000000f, 44.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 4.500000f }, { 12.000000f, 0.000000f, 4.500000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 4.500000f }, { 12.000000f, 6.000000f, 1.500000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 4.500000f }, { 12.000000f, 6.000000f, 1.500000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 4.500000f }, { -12.000000f, 6.000000f, 1.500000f } });
                sd.wireframe.push_back({ { 12.000000f, 6.000000f, 1.500000f }, { -12.000000f, 6.000000f, 1.500000f } });
                sd.wireframe.push_back({ { -12.500000f, 6.000000f, 1.400000f }, { 12.500000f, 6.000000f, 1.400000f } });
                sd.wireframe.push_back({ { -12.500000f, 6.000000f, 1.400000f }, { 12.500000f, 15.000000f, 1.400000f } });
                sd.wireframe.push_back({ { 12.500000f, 6.000000f, 1.400000f }, { 12.500000f, 15.000000f, 1.400000f } });
                sd.wireframe.push_back({ { -12.500000f, 6.000000f, 1.400000f }, { -12.500000f, 15.000000f, 1.400000f } });
                sd.wireframe.push_back({ { 12.500000f, 15.000000f, 1.400000f }, { -12.500000f, 15.000000f, 1.400000f } });
                sd.wireframe.push_back({ { -14.000000f, 15.000000f, 6.500000f }, { -10.500000f, 15.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 15.000000f, 6.500000f }, { -10.500000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -10.500000f, 15.000000f, 6.500000f }, { -10.500000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 15.000000f, 6.500000f }, { -14.000000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -10.500000f, 44.000000f, 6.500000f }, { -14.000000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { 10.500000f, 15.000000f, 6.500000f }, { 14.000000f, 15.000000f, 6.500000f } });
                sd.wireframe.push_back({ { 10.500000f, 15.000000f, 6.500000f }, { 14.000000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 15.000000f, 6.500000f }, { 14.000000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { 10.500000f, 15.000000f, 6.500000f }, { 10.500000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 6.500000f }, { 10.500000f, 44.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 39.000000f, 7.500000f }, { 14.000000f, 39.000000f, 7.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 39.000000f, 7.500000f }, { 14.000000f, 44.000000f, 9.300000f } });
                sd.wireframe.push_back({ { 14.000000f, 39.000000f, 7.500000f }, { 14.000000f, 44.000000f, 9.300000f } });
                sd.wireframe.push_back({ { -14.000000f, 39.000000f, 7.500000f }, { -14.000000f, 44.000000f, 9.300000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 9.300000f }, { -14.000000f, 44.000000f, 9.300000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 0.000000f }, { -14.000000f, 0.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 44.000000f, 0.000000f }, { -14.000000f, 0.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 0.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 0.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 14.500000f }, { 14.000000f, 0.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 0.000000f }, { 14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 0.000000f }, { 14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 14.500000f }, { 14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 0.000000f }, { -14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 44.000000f, 0.000000f }, { -14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 14.500000f }, { -14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 14.500000f }, { -14.000000f, 44.000000f, 14.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 14.500000f }, { -12.000000f, 2.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 44.000000f, 14.500000f }, { -12.000000f, 2.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 0.000000f, 14.500000f }, { 12.000000f, 2.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 14.500000f }, { 12.000000f, 2.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 2.000000f, 17.000000f }, { 12.000000f, 2.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 0.000000f, 14.500000f }, { 12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 14.500000f }, { 12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 2.000000f, 17.000000f }, { 12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 2.000000f, 17.000000f }, { 12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 44.000000f, 14.500000f }, { -12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 44.000000f, 14.500000f }, { -12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 2.000000f, 17.000000f }, { -12.000000f, 42.000000f, 17.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 42.000000f, 17.000000f }, { -12.000000f, 42.000000f, 17.000000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Stage to Choir Amphitheatre Tier";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 1.500000f, 4.200000f };
                    pd.directDistance = 6.000000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.000000f;
                        rs.order = 0;
                        rs.gain = 0.167000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.100000f;
                        rs.order = 1;
                        rs.gain = 0.159000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 3.250000f, 2.880000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.360000f;
                        rs.order = 1;
                        rs.gain = 0.152000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 6.310000f, 1.400000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.540000f;
                        rs.order = 1;
                        rs.gain = 0.034000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 4.010000f, 17.000000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.640000f;
                        rs.order = 1;
                        rs.gain = 0.034000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 14.000000f, 4.250000f, 3.000000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.640000f;
                        rs.order = 1;
                        rs.gain = 0.034000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -14.000000f, 4.250000f, 3.000000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 79.540000f;
                        rs.order = 1;
                        rs.gain = 0.012000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 2.920000f });
                        rs.points.push_back({ 0.000000f, 1.500000f, 4.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Prime Parquet Row 12 Sweetspot";
                    pd.source = { 0.000000f, 6.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 20.000000f, 1.600000f };
                    pd.directDistance = 14.000000f;
                    pd.rays.reserve(5);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 14.000000f;
                        rs.order = 0;
                        rs.gain = 0.071000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.310000f;
                        rs.order = 1;
                        rs.gain = 0.031000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 14.000000f, 13.000000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.310000f;
                        rs.order = 1;
                        rs.gain = 0.031000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ -14.000000f, 13.000000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.650000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 12.950000f, 17.000000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 62.000000f;
                        rs.order = 1;
                        rs.gain = 0.016000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 1.680000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Right Horseshoe Balcony Overhang";
                    pd.source = { -2.000000f, 6.000000f, 1.800000f };
                    pd.listener = { 12.000000f, 26.000000f, 6.200000f };
                    pd.directDistance = 24.810000f;
                    pd.rays.reserve(5);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.810000f;
                        rs.order = 0;
                        rs.gain = 0.040000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ -2.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 12.000000f, 26.000000f, 6.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.960000f;
                        rs.order = 1;
                        rs.gain = 0.039000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -2.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ -0.920000f, 7.540000f, 1.400000f });
                        rs.points.push_back({ 12.000000f, 26.000000f, 6.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 27.260000f;
                        rs.order = 1;
                        rs.gain = 0.036000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -2.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 14.000000f, 23.780000f, 5.710000f });
                        rs.points.push_back({ 12.000000f, 26.000000f, 6.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 43.170000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -2.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ -14.000000f, 12.320000f, 3.190000f });
                        rs.points.push_back({ 12.000000f, 26.000000f, 6.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 57.890000f;
                        rs.order = 1;
                        rs.gain = 0.017000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -2.000000f, 6.000000f, 1.800000f });
                        rs.points.push_back({ 7.500000f, 44.000000f, 4.790000f });
                        rs.points.push_back({ 12.000000f, 26.000000f, 6.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Deep Rear Parterre Under Balcony";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 39.000000f, 1.800000f };
                    pd.directDistance = 32.000000f;
                    pd.rays.reserve(5);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 32.000000f;
                        rs.order = 0;
                        rs.gain = 0.031000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 39.000000f, 1.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 42.000000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 44.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 39.000000f, 1.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 42.520000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 14.000000f, 23.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 39.000000f, 1.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 42.520000f;
                        rs.order = 1;
                        rs.gain = 0.023000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -14.000000f, 23.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 39.000000f, 1.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 44.140000f;
                        rs.order = 1;
                        rs.gain = 0.022000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 23.000000f, 17.000000f });
                        rs.points.push_back({ 0.000000f, 39.000000f, 1.800000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 6: pantheon_rome
            {
                SpaceData sd;
                sd.id = "pantheon_rome";
                sd.title = "The Pantheon Rotunda & Dome (Rome)";
                sd.category = "World Landmarks";
                sd.rt60 = 6.200000f;
                sd.volume = 85000.000000f;
                sd.area = 11800.000000f;
                sd.minBound = { -21.650000f, -21.650000f, 0.000000f };
                sd.maxBound = { 21.650000f, 21.650000f, 43.300000f };
                sd.wireframe.reserve(576);
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 21.650000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 20.910000f, 5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 0.000000f }, { 20.910000f, 5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 18.750000f, 10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 0.000000f }, { 18.750000f, 10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 15.310000f, 15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 0.000000f }, { 15.310000f, 15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 10.820000f, 18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 0.000000f }, { 10.820000f, 18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 5.600000f, 20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 0.000000f }, { 5.600000f, 20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 0.000000f, 21.650000f, 0.000000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 0.000000f }, { 0.000000f, 21.650000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -5.600000f, 20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 0.000000f }, { -5.600000f, 20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -10.820000f, 18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 0.000000f }, { -10.820000f, 18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -15.310000f, 15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 0.000000f }, { -15.310000f, 15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -18.750000f, 10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 0.000000f }, { -18.750000f, 10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -20.910000f, 5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 0.000000f }, { -20.910000f, 5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -21.650000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 0.000000f }, { -21.650000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -20.910000f, -5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 0.000000f }, { -20.910000f, -5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -18.750000f, -10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 0.000000f }, { -18.750000f, -10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -15.310000f, -15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 0.000000f }, { -15.310000f, -15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -10.820000f, -18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 0.000000f }, { -10.820000f, -18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -5.600000f, -20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 0.000000f }, { -5.600000f, -20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { -0.000000f, -21.650000f, 0.000000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 0.000000f }, { -0.000000f, -21.650000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 5.600000f, -20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 0.000000f }, { 5.600000f, -20.910000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 10.820000f, -18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 0.000000f }, { 10.820000f, -18.750000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 15.310000f, -15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 0.000000f }, { 15.310000f, -15.310000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 18.750000f, -10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 0.000000f }, { 18.750000f, -10.820000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 0.000000f }, { 20.910000f, -5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 0.000000f }, { 20.910000f, -5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 0.000000f }, { 20.910000f, -5.600000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 0.000000f }, { 21.650000f, 0.000000f, 21.650000f } });
                sd.wireframe.push_back({ { 20.910000f, -5.600000f, 0.000000f }, { 21.650000f, 0.000000f, 21.650000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 0.000000f }, { 20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 0.000000f }, { 20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 21.650000f }, { 20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 0.000000f }, { 18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 0.000000f }, { 18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 21.650000f }, { 18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 0.000000f }, { 15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 0.000000f }, { 15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 21.650000f }, { 15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 0.000000f }, { 10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 0.000000f }, { 10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 21.650000f }, { 10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 0.000000f }, { 5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 0.000000f }, { 5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 21.650000f }, { 5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 0.000000f }, { 0.000000f, 21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 0.000000f }, { 0.000000f, 21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 21.650000f }, { 0.000000f, 21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 0.000000f }, { -5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 0.000000f }, { -5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 21.650000f }, { -5.600000f, 20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 0.000000f }, { -10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 0.000000f }, { -10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 21.650000f }, { -10.820000f, 18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 0.000000f }, { -15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 0.000000f }, { -15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 21.650000f }, { -15.310000f, 15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 0.000000f }, { -18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 0.000000f }, { -18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 21.650000f }, { -18.750000f, 10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 0.000000f }, { -20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 0.000000f }, { -20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 21.650000f }, { -20.910000f, 5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 0.000000f }, { -21.650000f, 0.000000f, 21.650000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 0.000000f }, { -21.650000f, 0.000000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 21.650000f }, { -21.650000f, 0.000000f, 21.650000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 0.000000f }, { -20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 0.000000f }, { -20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 21.650000f }, { -20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 0.000000f }, { -18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 0.000000f }, { -18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 21.650000f }, { -18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 0.000000f }, { -15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 0.000000f }, { -15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 21.650000f }, { -15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 0.000000f }, { -10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 0.000000f }, { -10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 21.650000f }, { -10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 0.000000f }, { -5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 0.000000f }, { -5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 21.650000f }, { -5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 0.000000f }, { -0.000000f, -21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 0.000000f }, { -0.000000f, -21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 21.650000f }, { -0.000000f, -21.650000f, 21.650000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 0.000000f }, { 5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 0.000000f }, { 5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 21.650000f }, { 5.600000f, -20.910000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 0.000000f }, { 10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 0.000000f }, { 10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 21.650000f }, { 10.820000f, -18.750000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 0.000000f }, { 15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 0.000000f }, { 15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 21.650000f }, { 15.310000f, -15.310000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 0.000000f }, { 18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 0.000000f }, { 18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 21.650000f }, { 18.750000f, -10.820000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 0.000000f }, { 20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 20.910000f, -5.600000f, 0.000000f }, { 20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 21.650000f }, { 20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 21.650000f }, { 20.910000f, -5.600000f, 21.650000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 21.650000f }, { 21.580000f, 0.000000f, 23.380000f } });
                sd.wireframe.push_back({ { 20.910000f, -5.600000f, 21.650000f }, { 21.580000f, 0.000000f, 23.380000f } });
                sd.wireframe.push_back({ { 21.650000f, 0.000000f, 21.650000f }, { 20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 21.650000f }, { 20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 21.580000f, 0.000000f, 23.380000f }, { 20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 20.910000f, 5.600000f, 21.650000f }, { 18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 21.650000f }, { 18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 20.850000f, 5.590000f, 23.380000f }, { 18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.750000f, 10.820000f, 21.650000f }, { 15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 21.650000f }, { 15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.690000f, 10.790000f, 23.380000f }, { 15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.310000f, 15.310000f, 21.650000f }, { 10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 21.650000f }, { 10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.260000f, 15.260000f, 23.380000f }, { 10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.820000f, 18.750000f, 21.650000f }, { 5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 21.650000f }, { 5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.790000f, 18.690000f, 23.380000f }, { 5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.600000f, 20.910000f, 21.650000f }, { 0.000000f, 21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 21.650000f }, { 0.000000f, 21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.590000f, 20.850000f, 23.380000f }, { 0.000000f, 21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.650000f, 21.650000f }, { -5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 21.650000f }, { -5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.580000f, 23.380000f }, { -5.590000f, 20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.600000f, 20.910000f, 21.650000f }, { -10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 21.650000f }, { -10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.590000f, 20.850000f, 23.380000f }, { -10.790000f, 18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.820000f, 18.750000f, 21.650000f }, { -15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 21.650000f }, { -15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.790000f, 18.690000f, 23.380000f }, { -15.260000f, 15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.310000f, 15.310000f, 21.650000f }, { -18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 21.650000f }, { -18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.260000f, 15.260000f, 23.380000f }, { -18.690000f, 10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.750000f, 10.820000f, 21.650000f }, { -20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 21.650000f }, { -20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.690000f, 10.790000f, 23.380000f }, { -20.850000f, 5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.910000f, 5.600000f, 21.650000f }, { -21.580000f, 0.000000f, 23.380000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 21.650000f }, { -21.580000f, 0.000000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.850000f, 5.590000f, 23.380000f }, { -21.580000f, 0.000000f, 23.380000f } });
                sd.wireframe.push_back({ { -21.650000f, 0.000000f, 21.650000f }, { -20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 21.650000f }, { -20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -21.580000f, 0.000000f, 23.380000f }, { -20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.910000f, -5.600000f, 21.650000f }, { -18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 21.650000f }, { -18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -20.850000f, -5.590000f, 23.380000f }, { -18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.750000f, -10.820000f, 21.650000f }, { -15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 21.650000f }, { -15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -18.690000f, -10.790000f, 23.380000f }, { -15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.310000f, -15.310000f, 21.650000f }, { -10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 21.650000f }, { -10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -15.260000f, -15.260000f, 23.380000f }, { -10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.820000f, -18.750000f, 21.650000f }, { -5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 21.650000f }, { -5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -10.790000f, -18.690000f, 23.380000f }, { -5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.600000f, -20.910000f, 21.650000f }, { -0.000000f, -21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 21.650000f }, { -0.000000f, -21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { -5.590000f, -20.850000f, 23.380000f }, { -0.000000f, -21.580000f, 23.380000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.650000f, 21.650000f }, { 5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 21.650000f }, { 5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.580000f, 23.380000f }, { 5.590000f, -20.850000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.600000f, -20.910000f, 21.650000f }, { 10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 21.650000f }, { 10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 5.590000f, -20.850000f, 23.380000f }, { 10.790000f, -18.690000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.820000f, -18.750000f, 21.650000f }, { 15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 21.650000f }, { 15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 10.790000f, -18.690000f, 23.380000f }, { 15.260000f, -15.260000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.310000f, -15.310000f, 21.650000f }, { 18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 21.650000f }, { 18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 15.260000f, -15.260000f, 23.380000f }, { 18.690000f, -10.790000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.750000f, -10.820000f, 21.650000f }, { 20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 20.910000f, -5.600000f, 21.650000f }, { 20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 21.580000f, 0.000000f, 23.380000f }, { 20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 18.690000f, -10.790000f, 23.380000f }, { 20.850000f, -5.590000f, 23.380000f } });
                sd.wireframe.push_back({ { 21.580000f, 0.000000f, 23.380000f }, { 20.440000f, 0.000000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.850000f, -5.590000f, 23.380000f }, { 20.440000f, 0.000000f, 28.790000f } });
                sd.wireframe.push_back({ { 21.580000f, 0.000000f, 23.380000f }, { 19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.850000f, 5.590000f, 23.380000f }, { 19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.440000f, 0.000000f, 28.790000f }, { 19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.850000f, 5.590000f, 23.380000f }, { 17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 18.690000f, 10.790000f, 23.380000f }, { 17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 19.740000f, 5.290000f, 28.790000f }, { 17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 18.690000f, 10.790000f, 23.380000f }, { 14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 15.260000f, 15.260000f, 23.380000f }, { 14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 17.700000f, 10.220000f, 28.790000f }, { 14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 15.260000f, 15.260000f, 23.380000f }, { 10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.790000f, 18.690000f, 23.380000f }, { 10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 14.450000f, 14.450000f, 28.790000f }, { 10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.790000f, 18.690000f, 23.380000f }, { 5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.590000f, 20.850000f, 23.380000f }, { 5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.220000f, 17.700000f, 28.790000f }, { 5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.590000f, 20.850000f, 23.380000f }, { 0.000000f, 20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.580000f, 23.380000f }, { 0.000000f, 20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.290000f, 19.740000f, 28.790000f }, { 0.000000f, 20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { 0.000000f, 21.580000f, 23.380000f }, { -5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.590000f, 20.850000f, 23.380000f }, { -5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 0.000000f, 20.440000f, 28.790000f }, { -5.290000f, 19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.590000f, 20.850000f, 23.380000f }, { -10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.790000f, 18.690000f, 23.380000f }, { -10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.290000f, 19.740000f, 28.790000f }, { -10.220000f, 17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.790000f, 18.690000f, 23.380000f }, { -14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -15.260000f, 15.260000f, 23.380000f }, { -14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.220000f, 17.700000f, 28.790000f }, { -14.450000f, 14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -15.260000f, 15.260000f, 23.380000f }, { -17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -18.690000f, 10.790000f, 23.380000f }, { -17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -14.450000f, 14.450000f, 28.790000f }, { -17.700000f, 10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -18.690000f, 10.790000f, 23.380000f }, { -19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -20.850000f, 5.590000f, 23.380000f }, { -19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -17.700000f, 10.220000f, 28.790000f }, { -19.740000f, 5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -20.850000f, 5.590000f, 23.380000f }, { -20.440000f, 0.000000f, 28.790000f } });
                sd.wireframe.push_back({ { -21.580000f, 0.000000f, 23.380000f }, { -20.440000f, 0.000000f, 28.790000f } });
                sd.wireframe.push_back({ { -19.740000f, 5.290000f, 28.790000f }, { -20.440000f, 0.000000f, 28.790000f } });
                sd.wireframe.push_back({ { -21.580000f, 0.000000f, 23.380000f }, { -19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -20.850000f, -5.590000f, 23.380000f }, { -19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -20.440000f, 0.000000f, 28.790000f }, { -19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { -20.850000f, -5.590000f, 23.380000f }, { -17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -18.690000f, -10.790000f, 23.380000f }, { -17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -19.740000f, -5.290000f, 28.790000f }, { -17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { -18.690000f, -10.790000f, 23.380000f }, { -14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -15.260000f, -15.260000f, 23.380000f }, { -14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -17.700000f, -10.220000f, 28.790000f }, { -14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { -15.260000f, -15.260000f, 23.380000f }, { -10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.790000f, -18.690000f, 23.380000f }, { -10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -14.450000f, -14.450000f, 28.790000f }, { -10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.790000f, -18.690000f, 23.380000f }, { -5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.590000f, -20.850000f, 23.380000f }, { -5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -10.220000f, -17.700000f, 28.790000f }, { -5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.590000f, -20.850000f, 23.380000f }, { -0.000000f, -20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.580000f, 23.380000f }, { -0.000000f, -20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { -5.290000f, -19.740000f, 28.790000f }, { -0.000000f, -20.440000f, 28.790000f } });
                sd.wireframe.push_back({ { -0.000000f, -21.580000f, 23.380000f }, { 5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.590000f, -20.850000f, 23.380000f }, { 5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { -0.000000f, -20.440000f, 28.790000f }, { 5.290000f, -19.740000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.590000f, -20.850000f, 23.380000f }, { 10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.790000f, -18.690000f, 23.380000f }, { 10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 5.290000f, -19.740000f, 28.790000f }, { 10.220000f, -17.700000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.790000f, -18.690000f, 23.380000f }, { 14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 15.260000f, -15.260000f, 23.380000f }, { 14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 10.220000f, -17.700000f, 28.790000f }, { 14.450000f, -14.450000f, 28.790000f } });
                sd.wireframe.push_back({ { 15.260000f, -15.260000f, 23.380000f }, { 17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 18.690000f, -10.790000f, 23.380000f }, { 17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 14.450000f, -14.450000f, 28.790000f }, { 17.700000f, -10.220000f, 28.790000f } });
                sd.wireframe.push_back({ { 18.690000f, -10.790000f, 23.380000f }, { 19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.850000f, -5.590000f, 23.380000f }, { 19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.440000f, 0.000000f, 28.790000f }, { 19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 17.700000f, -10.220000f, 28.790000f }, { 19.740000f, -5.290000f, 28.790000f } });
                sd.wireframe.push_back({ { 20.440000f, 0.000000f, 28.790000f }, { 17.960000f, 0.000000f, 33.740000f } });
                sd.wireframe.push_back({ { 19.740000f, -5.290000f, 28.790000f }, { 17.960000f, 0.000000f, 33.740000f } });
                sd.wireframe.push_back({ { 20.440000f, 0.000000f, 28.790000f }, { 17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 19.740000f, 5.290000f, 28.790000f }, { 17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.960000f, 0.000000f, 33.740000f }, { 17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 19.740000f, 5.290000f, 28.790000f }, { 15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.700000f, 10.220000f, 28.790000f }, { 15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.350000f, 4.650000f, 33.740000f }, { 15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.700000f, 10.220000f, 28.790000f }, { 12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 14.450000f, 14.450000f, 28.790000f }, { 12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 15.550000f, 8.980000f, 33.740000f }, { 12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 14.450000f, 14.450000f, 28.790000f }, { 8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 10.220000f, 17.700000f, 28.790000f }, { 8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 12.700000f, 12.700000f, 33.740000f }, { 8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 10.220000f, 17.700000f, 28.790000f }, { 4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 5.290000f, 19.740000f, 28.790000f }, { 4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 8.980000f, 15.550000f, 33.740000f }, { 4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 5.290000f, 19.740000f, 28.790000f }, { 0.000000f, 17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { 0.000000f, 20.440000f, 28.790000f }, { 0.000000f, 17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { 4.650000f, 17.350000f, 33.740000f }, { 0.000000f, 17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { 0.000000f, 20.440000f, 28.790000f }, { -4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -5.290000f, 19.740000f, 28.790000f }, { -4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 0.000000f, 17.960000f, 33.740000f }, { -4.650000f, 17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -5.290000f, 19.740000f, 28.790000f }, { -8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -10.220000f, 17.700000f, 28.790000f }, { -8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -4.650000f, 17.350000f, 33.740000f }, { -8.980000f, 15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -10.220000f, 17.700000f, 28.790000f }, { -12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -14.450000f, 14.450000f, 28.790000f }, { -12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -8.980000f, 15.550000f, 33.740000f }, { -12.700000f, 12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -14.450000f, 14.450000f, 28.790000f }, { -15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.700000f, 10.220000f, 28.790000f }, { -15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -12.700000f, 12.700000f, 33.740000f }, { -15.550000f, 8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.700000f, 10.220000f, 28.790000f }, { -17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -19.740000f, 5.290000f, 28.790000f }, { -17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -15.550000f, 8.980000f, 33.740000f }, { -17.350000f, 4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -19.740000f, 5.290000f, 28.790000f }, { -17.960000f, 0.000000f, 33.740000f } });
                sd.wireframe.push_back({ { -20.440000f, 0.000000f, 28.790000f }, { -17.960000f, 0.000000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.350000f, 4.650000f, 33.740000f }, { -17.960000f, 0.000000f, 33.740000f } });
                sd.wireframe.push_back({ { -20.440000f, 0.000000f, 28.790000f }, { -17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -19.740000f, -5.290000f, 28.790000f }, { -17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.960000f, 0.000000f, 33.740000f }, { -17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { -19.740000f, -5.290000f, 28.790000f }, { -15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.700000f, -10.220000f, 28.790000f }, { -15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.350000f, -4.650000f, 33.740000f }, { -15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { -17.700000f, -10.220000f, 28.790000f }, { -12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -14.450000f, -14.450000f, 28.790000f }, { -12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -15.550000f, -8.980000f, 33.740000f }, { -12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { -14.450000f, -14.450000f, 28.790000f }, { -8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -10.220000f, -17.700000f, 28.790000f }, { -8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -12.700000f, -12.700000f, 33.740000f }, { -8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { -10.220000f, -17.700000f, 28.790000f }, { -4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -5.290000f, -19.740000f, 28.790000f }, { -4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -8.980000f, -15.550000f, 33.740000f }, { -4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -5.290000f, -19.740000f, 28.790000f }, { -0.000000f, -17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { -0.000000f, -20.440000f, 28.790000f }, { -0.000000f, -17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { -4.650000f, -17.350000f, 33.740000f }, { -0.000000f, -17.960000f, 33.740000f } });
                sd.wireframe.push_back({ { -0.000000f, -20.440000f, 28.790000f }, { 4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 5.290000f, -19.740000f, 28.790000f }, { 4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { -0.000000f, -17.960000f, 33.740000f }, { 4.650000f, -17.350000f, 33.740000f } });
                sd.wireframe.push_back({ { 5.290000f, -19.740000f, 28.790000f }, { 8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 10.220000f, -17.700000f, 28.790000f }, { 8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 4.650000f, -17.350000f, 33.740000f }, { 8.980000f, -15.550000f, 33.740000f } });
                sd.wireframe.push_back({ { 10.220000f, -17.700000f, 28.790000f }, { 12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 14.450000f, -14.450000f, 28.790000f }, { 12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 8.980000f, -15.550000f, 33.740000f }, { 12.700000f, -12.700000f, 33.740000f } });
                sd.wireframe.push_back({ { 14.450000f, -14.450000f, 28.790000f }, { 15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.700000f, -10.220000f, 28.790000f }, { 15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 12.700000f, -12.700000f, 33.740000f }, { 15.550000f, -8.980000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.700000f, -10.220000f, 28.790000f }, { 17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 19.740000f, -5.290000f, 28.790000f }, { 17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.960000f, 0.000000f, 33.740000f }, { 17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 15.550000f, -8.980000f, 33.740000f }, { 17.350000f, -4.650000f, 33.740000f } });
                sd.wireframe.push_back({ { 17.960000f, 0.000000f, 33.740000f }, { 14.310000f, 0.000000f, 37.900000f } });
                sd.wireframe.push_back({ { 17.350000f, -4.650000f, 33.740000f }, { 14.310000f, 0.000000f, 37.900000f } });
                sd.wireframe.push_back({ { 17.960000f, 0.000000f, 33.740000f }, { 13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 17.350000f, 4.650000f, 33.740000f }, { 13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 14.310000f, 0.000000f, 37.900000f }, { 13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 17.350000f, 4.650000f, 33.740000f }, { 12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 15.550000f, 8.980000f, 33.740000f }, { 12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 13.820000f, 3.700000f, 37.900000f }, { 12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 15.550000f, 8.980000f, 33.740000f }, { 10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.700000f, 12.700000f, 33.740000f }, { 10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.390000f, 7.150000f, 37.900000f }, { 10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.700000f, 12.700000f, 33.740000f }, { 7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 8.980000f, 15.550000f, 33.740000f }, { 7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 10.120000f, 10.120000f, 37.900000f }, { 7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 8.980000f, 15.550000f, 33.740000f }, { 3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 4.650000f, 17.350000f, 33.740000f }, { 3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 7.150000f, 12.390000f, 37.900000f }, { 3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 4.650000f, 17.350000f, 33.740000f }, { 0.000000f, 14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { 0.000000f, 17.960000f, 33.740000f }, { 0.000000f, 14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { 3.700000f, 13.820000f, 37.900000f }, { 0.000000f, 14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { 0.000000f, 17.960000f, 33.740000f }, { -3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -4.650000f, 17.350000f, 33.740000f }, { -3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.310000f, 37.900000f }, { -3.700000f, 13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -4.650000f, 17.350000f, 33.740000f }, { -7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -8.980000f, 15.550000f, 33.740000f }, { -7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -3.700000f, 13.820000f, 37.900000f }, { -7.150000f, 12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -8.980000f, 15.550000f, 33.740000f }, { -10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.700000f, 12.700000f, 33.740000f }, { -10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -7.150000f, 12.390000f, 37.900000f }, { -10.120000f, 10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.700000f, 12.700000f, 33.740000f }, { -12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -15.550000f, 8.980000f, 33.740000f }, { -12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -10.120000f, 10.120000f, 37.900000f }, { -12.390000f, 7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -15.550000f, 8.980000f, 33.740000f }, { -13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.350000f, 4.650000f, 33.740000f }, { -13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.390000f, 7.150000f, 37.900000f }, { -13.820000f, 3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.350000f, 4.650000f, 33.740000f }, { -14.310000f, 0.000000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.960000f, 0.000000f, 33.740000f }, { -14.310000f, 0.000000f, 37.900000f } });
                sd.wireframe.push_back({ { -13.820000f, 3.700000f, 37.900000f }, { -14.310000f, 0.000000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.960000f, 0.000000f, 33.740000f }, { -13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.350000f, -4.650000f, 33.740000f }, { -13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -14.310000f, 0.000000f, 37.900000f }, { -13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { -17.350000f, -4.650000f, 33.740000f }, { -12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -15.550000f, -8.980000f, 33.740000f }, { -12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -13.820000f, -3.700000f, 37.900000f }, { -12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { -15.550000f, -8.980000f, 33.740000f }, { -10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.700000f, -12.700000f, 33.740000f }, { -10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.390000f, -7.150000f, 37.900000f }, { -10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { -12.700000f, -12.700000f, 33.740000f }, { -7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -8.980000f, -15.550000f, 33.740000f }, { -7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -10.120000f, -10.120000f, 37.900000f }, { -7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { -8.980000f, -15.550000f, 33.740000f }, { -3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -4.650000f, -17.350000f, 33.740000f }, { -3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -7.150000f, -12.390000f, 37.900000f }, { -3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -4.650000f, -17.350000f, 33.740000f }, { -0.000000f, -14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { -0.000000f, -17.960000f, 33.740000f }, { -0.000000f, -14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { -3.700000f, -13.820000f, 37.900000f }, { -0.000000f, -14.310000f, 37.900000f } });
                sd.wireframe.push_back({ { -0.000000f, -17.960000f, 33.740000f }, { 3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 4.650000f, -17.350000f, 33.740000f }, { 3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.310000f, 37.900000f }, { 3.700000f, -13.820000f, 37.900000f } });
                sd.wireframe.push_back({ { 4.650000f, -17.350000f, 33.740000f }, { 7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 8.980000f, -15.550000f, 33.740000f }, { 7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 3.700000f, -13.820000f, 37.900000f }, { 7.150000f, -12.390000f, 37.900000f } });
                sd.wireframe.push_back({ { 8.980000f, -15.550000f, 33.740000f }, { 10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.700000f, -12.700000f, 33.740000f }, { 10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 7.150000f, -12.390000f, 37.900000f }, { 10.120000f, -10.120000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.700000f, -12.700000f, 33.740000f }, { 12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 15.550000f, -8.980000f, 33.740000f }, { 12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 10.120000f, -10.120000f, 37.900000f }, { 12.390000f, -7.150000f, 37.900000f } });
                sd.wireframe.push_back({ { 15.550000f, -8.980000f, 33.740000f }, { 13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 17.350000f, -4.650000f, 33.740000f }, { 13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 14.310000f, 0.000000f, 37.900000f }, { 13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 12.390000f, -7.150000f, 37.900000f }, { 13.820000f, -3.700000f, 37.900000f } });
                sd.wireframe.push_back({ { 14.310000f, 0.000000f, 37.900000f }, { 9.720000f, 0.000000f, 40.990000f } });
                sd.wireframe.push_back({ { 13.820000f, -3.700000f, 37.900000f }, { 9.720000f, 0.000000f, 40.990000f } });
                sd.wireframe.push_back({ { 14.310000f, 0.000000f, 37.900000f }, { 9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 13.820000f, 3.700000f, 37.900000f }, { 9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 9.720000f, 0.000000f, 40.990000f }, { 9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 13.820000f, 3.700000f, 37.900000f }, { 8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 12.390000f, 7.150000f, 37.900000f }, { 8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 9.390000f, 2.520000f, 40.990000f }, { 8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 12.390000f, 7.150000f, 37.900000f }, { 6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 10.120000f, 10.120000f, 37.900000f }, { 6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 8.420000f, 4.860000f, 40.990000f }, { 6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 10.120000f, 10.120000f, 37.900000f }, { 4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 7.150000f, 12.390000f, 37.900000f }, { 4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 6.870000f, 6.870000f, 40.990000f }, { 4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 7.150000f, 12.390000f, 37.900000f }, { 2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 3.700000f, 13.820000f, 37.900000f }, { 2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 4.860000f, 8.420000f, 40.990000f }, { 2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 3.700000f, 13.820000f, 37.900000f }, { 0.000000f, 9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.310000f, 37.900000f }, { 0.000000f, 9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { 2.520000f, 9.390000f, 40.990000f }, { 0.000000f, 9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.310000f, 37.900000f }, { -2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -3.700000f, 13.820000f, 37.900000f }, { -2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 0.000000f, 9.720000f, 40.990000f }, { -2.520000f, 9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -3.700000f, 13.820000f, 37.900000f }, { -4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -7.150000f, 12.390000f, 37.900000f }, { -4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -2.520000f, 9.390000f, 40.990000f }, { -4.860000f, 8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -7.150000f, 12.390000f, 37.900000f }, { -6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -10.120000f, 10.120000f, 37.900000f }, { -6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -4.860000f, 8.420000f, 40.990000f }, { -6.870000f, 6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -10.120000f, 10.120000f, 37.900000f }, { -8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -12.390000f, 7.150000f, 37.900000f }, { -8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -6.870000f, 6.870000f, 40.990000f }, { -8.420000f, 4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -12.390000f, 7.150000f, 37.900000f }, { -9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -13.820000f, 3.700000f, 37.900000f }, { -9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -8.420000f, 4.860000f, 40.990000f }, { -9.390000f, 2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -13.820000f, 3.700000f, 37.900000f }, { -9.720000f, 0.000000f, 40.990000f } });
                sd.wireframe.push_back({ { -14.310000f, 0.000000f, 37.900000f }, { -9.720000f, 0.000000f, 40.990000f } });
                sd.wireframe.push_back({ { -9.390000f, 2.520000f, 40.990000f }, { -9.720000f, 0.000000f, 40.990000f } });
                sd.wireframe.push_back({ { -14.310000f, 0.000000f, 37.900000f }, { -9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -13.820000f, -3.700000f, 37.900000f }, { -9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -9.720000f, 0.000000f, 40.990000f }, { -9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { -13.820000f, -3.700000f, 37.900000f }, { -8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -12.390000f, -7.150000f, 37.900000f }, { -8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -9.390000f, -2.520000f, 40.990000f }, { -8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { -12.390000f, -7.150000f, 37.900000f }, { -6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -10.120000f, -10.120000f, 37.900000f }, { -6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -8.420000f, -4.860000f, 40.990000f }, { -6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { -10.120000f, -10.120000f, 37.900000f }, { -4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -7.150000f, -12.390000f, 37.900000f }, { -4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -6.870000f, -6.870000f, 40.990000f }, { -4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { -7.150000f, -12.390000f, 37.900000f }, { -2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -3.700000f, -13.820000f, 37.900000f }, { -2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -4.860000f, -8.420000f, 40.990000f }, { -2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -3.700000f, -13.820000f, 37.900000f }, { -0.000000f, -9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.310000f, 37.900000f }, { -0.000000f, -9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { -2.520000f, -9.390000f, 40.990000f }, { -0.000000f, -9.720000f, 40.990000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.310000f, 37.900000f }, { 2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 3.700000f, -13.820000f, 37.900000f }, { 2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { -0.000000f, -9.720000f, 40.990000f }, { 2.520000f, -9.390000f, 40.990000f } });
                sd.wireframe.push_back({ { 3.700000f, -13.820000f, 37.900000f }, { 4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 7.150000f, -12.390000f, 37.900000f }, { 4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 2.520000f, -9.390000f, 40.990000f }, { 4.860000f, -8.420000f, 40.990000f } });
                sd.wireframe.push_back({ { 7.150000f, -12.390000f, 37.900000f }, { 6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 10.120000f, -10.120000f, 37.900000f }, { 6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 4.860000f, -8.420000f, 40.990000f }, { 6.870000f, -6.870000f, 40.990000f } });
                sd.wireframe.push_back({ { 10.120000f, -10.120000f, 37.900000f }, { 8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 12.390000f, -7.150000f, 37.900000f }, { 8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 6.870000f, -6.870000f, 40.990000f }, { 8.420000f, -4.860000f, 40.990000f } });
                sd.wireframe.push_back({ { 12.390000f, -7.150000f, 37.900000f }, { 9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 13.820000f, -3.700000f, 37.900000f }, { 9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 9.720000f, 0.000000f, 40.990000f }, { 9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 8.420000f, -4.860000f, 40.990000f }, { 9.390000f, -2.520000f, 40.990000f } });
                sd.wireframe.push_back({ { 9.720000f, 0.000000f, 40.990000f }, { 4.500000f, 0.000000f, 42.830000f } });
                sd.wireframe.push_back({ { 9.390000f, -2.520000f, 40.990000f }, { 4.500000f, 0.000000f, 42.830000f } });
                sd.wireframe.push_back({ { 9.720000f, 0.000000f, 40.990000f }, { 4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 9.390000f, 2.520000f, 40.990000f }, { 4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.500000f, 0.000000f, 42.830000f }, { 4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 9.390000f, 2.520000f, 40.990000f }, { 3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 8.420000f, 4.860000f, 40.990000f }, { 3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.350000f, 1.160000f, 42.830000f }, { 3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 8.420000f, 4.860000f, 40.990000f }, { 3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 6.870000f, 6.870000f, 40.990000f }, { 3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 3.900000f, 2.250000f, 42.830000f }, { 3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 6.870000f, 6.870000f, 40.990000f }, { 2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.860000f, 8.420000f, 40.990000f }, { 2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 3.180000f, 3.180000f, 42.830000f }, { 2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.860000f, 8.420000f, 40.990000f }, { 1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.520000f, 9.390000f, 40.990000f }, { 1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.250000f, 3.900000f, 42.830000f }, { 1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.520000f, 9.390000f, 40.990000f }, { 0.000000f, 4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { 0.000000f, 9.720000f, 40.990000f }, { 0.000000f, 4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { 1.160000f, 4.350000f, 42.830000f }, { 0.000000f, 4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { 0.000000f, 9.720000f, 40.990000f }, { -1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.520000f, 9.390000f, 40.990000f }, { -1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 0.000000f, 4.500000f, 42.830000f }, { -1.160000f, 4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.520000f, 9.390000f, 40.990000f }, { -2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.860000f, 8.420000f, 40.990000f }, { -2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -1.160000f, 4.350000f, 42.830000f }, { -2.250000f, 3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.860000f, 8.420000f, 40.990000f }, { -3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -6.870000f, 6.870000f, 40.990000f }, { -3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.250000f, 3.900000f, 42.830000f }, { -3.180000f, 3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -6.870000f, 6.870000f, 40.990000f }, { -3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -8.420000f, 4.860000f, 40.990000f }, { -3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -3.180000f, 3.180000f, 42.830000f }, { -3.900000f, 2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -8.420000f, 4.860000f, 40.990000f }, { -4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.390000f, 2.520000f, 40.990000f }, { -4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -3.900000f, 2.250000f, 42.830000f }, { -4.350000f, 1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.390000f, 2.520000f, 40.990000f }, { -4.500000f, 0.000000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.720000f, 0.000000f, 40.990000f }, { -4.500000f, 0.000000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.350000f, 1.160000f, 42.830000f }, { -4.500000f, 0.000000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.720000f, 0.000000f, 40.990000f }, { -4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.390000f, -2.520000f, 40.990000f }, { -4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.500000f, 0.000000f, 42.830000f }, { -4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { -9.390000f, -2.520000f, 40.990000f }, { -3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -8.420000f, -4.860000f, 40.990000f }, { -3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.350000f, -1.160000f, 42.830000f }, { -3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { -8.420000f, -4.860000f, 40.990000f }, { -3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -6.870000f, -6.870000f, 40.990000f }, { -3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -3.900000f, -2.250000f, 42.830000f }, { -3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { -6.870000f, -6.870000f, 40.990000f }, { -2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.860000f, -8.420000f, 40.990000f }, { -2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -3.180000f, -3.180000f, 42.830000f }, { -2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { -4.860000f, -8.420000f, 40.990000f }, { -1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.520000f, -9.390000f, 40.990000f }, { -1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.250000f, -3.900000f, 42.830000f }, { -1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -2.520000f, -9.390000f, 40.990000f }, { -0.000000f, -4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { -0.000000f, -9.720000f, 40.990000f }, { -0.000000f, -4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { -1.160000f, -4.350000f, 42.830000f }, { -0.000000f, -4.500000f, 42.830000f } });
                sd.wireframe.push_back({ { -0.000000f, -9.720000f, 40.990000f }, { 1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.520000f, -9.390000f, 40.990000f }, { 1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { -0.000000f, -4.500000f, 42.830000f }, { 1.160000f, -4.350000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.520000f, -9.390000f, 40.990000f }, { 2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.860000f, -8.420000f, 40.990000f }, { 2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 1.160000f, -4.350000f, 42.830000f }, { 2.250000f, -3.900000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.860000f, -8.420000f, 40.990000f }, { 3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 6.870000f, -6.870000f, 40.990000f }, { 3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 2.250000f, -3.900000f, 42.830000f }, { 3.180000f, -3.180000f, 42.830000f } });
                sd.wireframe.push_back({ { 6.870000f, -6.870000f, 40.990000f }, { 3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 8.420000f, -4.860000f, 40.990000f }, { 3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 3.180000f, -3.180000f, 42.830000f }, { 3.900000f, -2.250000f, 42.830000f } });
                sd.wireframe.push_back({ { 8.420000f, -4.860000f, 40.990000f }, { 4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 9.390000f, -2.520000f, 40.990000f }, { 4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.500000f, 0.000000f, 42.830000f }, { 4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 3.900000f, -2.250000f, 42.830000f }, { 4.350000f, -1.160000f, 42.830000f } });
                sd.wireframe.push_back({ { 4.500000f, 0.000000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 4.350000f, 1.160000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 3.900000f, 2.250000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 3.180000f, 3.180000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 2.250000f, 3.900000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 1.160000f, 4.350000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 0.000000f, 4.500000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -1.160000f, 4.350000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -2.250000f, 3.900000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -3.180000f, 3.180000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -3.900000f, 2.250000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -4.350000f, 1.160000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -4.500000f, 0.000000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -4.350000f, -1.160000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -3.900000f, -2.250000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -3.180000f, -3.180000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -2.250000f, -3.900000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -1.160000f, -4.350000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { -0.000000f, -4.500000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 1.160000f, -4.350000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 2.250000f, -3.900000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 3.180000f, -3.180000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 3.900000f, -2.250000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.wireframe.push_back({ { 4.350000f, -1.160000f, 42.830000f }, { 0.000000f, 0.000000f, 43.300000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Central Floor to Oculus Axis Focus";
                    pd.source = { 0.000000f, 0.000000f, 1.500000f };
                    pd.listener = { 4.000000f, 4.000000f, 1.500000f };
                    pd.directDistance = 5.660000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.660000f;
                        rs.order = 0;
                        rs.gain = 0.177000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.400000f;
                        rs.order = 1;
                        rs.gain = 0.155000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 2.000000f, 2.000000f, 0.000000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.400000f;
                        rs.order = 1;
                        rs.gain = 0.155000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 2.000000f, 2.000000f, 0.000000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.330000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 16.770000f, 13.400000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.330000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 13.400000f, 16.770000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.770000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 19.360000f, 9.350000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.770000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 9.350000f, 19.360000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 38.600000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 21.030000f, 4.710000f, 1.500000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Imperial Altar Across Marble Rotunda";
                    pd.source = { 0.000000f, 16.000000f, 1.800000f };
                    pd.listener = { 0.000000f, -14.000000f, 1.500000f };
                    pd.directDistance = 30.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.000000f;
                        rs.order = 0;
                        rs.gain = 0.033000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.180000f;
                        rs.order = 1;
                        rs.gain = 0.033000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -0.360000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.180000f;
                        rs.order = 1;
                        rs.gain = 0.033000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -0.360000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 41.130000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ 1.260000f, 21.480000f, 1.760000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 41.130000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ -1.260000f, 21.480000f, 1.760000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 45.080000f;
                        rs.order = 1;
                        rs.gain = 0.022000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ -1.650000f, -21.430000f, 1.550000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 45.080000f;
                        rs.order = 1;
                        rs.gain = 0.022000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ 1.650000f, -21.430000f, 1.550000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.740000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 16.000000f, 1.800000f });
                        rs.points.push_back({ -18.790000f, -10.720000f, 1.610000f });
                        rs.points.push_back({ 0.000000f, -14.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Curved Whispering Wall Reflection";
                    pd.source = { 18.000000f, 0.000000f, 1.500000f };
                    pd.listener = { -18.000000f, 0.000000f, 1.500000f };
                    pd.directDistance = 36.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.000000f;
                        rs.order = 0;
                        rs.gain = 0.028000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.120000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 18.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -18.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Diagonal Across Opus Sectile Floor";
                    pd.source = { -12.000000f, -12.000000f, 1.500000f };
                    pd.listener = { 12.000000f, 12.000000f, 1.500000f };
                    pd.directDistance = 33.940000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.940000f;
                        rs.order = 0;
                        rs.gain = 0.029000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.070000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ -12.000000f, -12.000000f, 1.500000f });
                        rs.points.push_back({ -0.000000f, -0.000000f, 0.000000f });
                        rs.points.push_back({ 12.000000f, 12.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 7: berlin_philharmonie
            {
                SpaceData sd;
                sd.id = "berlin_philharmonie";
                sd.title = "Berlin Philharmonie (Vineyard Hall)";
                sd.category = "Concert Halls";
                sd.rt60 = 2.100000f;
                sd.volume = 21000.000000f;
                sd.area = 5600.000000f;
                sd.minBound = { -25.000000f, -14.000000f, 0.000000f };
                sd.maxBound = { 25.000000f, 55.000000f, 21.000000f };
                sd.wireframe.reserve(71);
                sd.wireframe.push_back({ { -8.000000f, 0.000000f, 0.800000f }, { 8.000000f, 0.000000f, 0.800000f } });
                sd.wireframe.push_back({ { -8.000000f, 0.000000f, 0.800000f }, { 8.000000f, 14.000000f, 0.800000f } });
                sd.wireframe.push_back({ { 8.000000f, 0.000000f, 0.800000f }, { 8.000000f, 14.000000f, 0.800000f } });
                sd.wireframe.push_back({ { -8.000000f, 0.000000f, 0.800000f }, { -8.000000f, 14.000000f, 0.800000f } });
                sd.wireframe.push_back({ { 8.000000f, 14.000000f, 0.800000f }, { -8.000000f, 14.000000f, 0.800000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 6.500000f }, { 10.000000f, -10.000000f, 6.500000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 6.500000f }, { 8.000000f, 0.000000f, 2.000000f } });
                sd.wireframe.push_back({ { 10.000000f, -10.000000f, 6.500000f }, { 8.000000f, 0.000000f, 2.000000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 6.500000f }, { -8.000000f, 0.000000f, 2.000000f } });
                sd.wireframe.push_back({ { 8.000000f, 0.000000f, 2.000000f }, { -8.000000f, 0.000000f, 2.000000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 6.500000f }, { -10.000000f, -10.000000f, 8.500000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 6.500000f }, { 10.000000f, -10.000000f, 8.500000f } });
                sd.wireframe.push_back({ { 10.000000f, -10.000000f, 6.500000f }, { 10.000000f, -10.000000f, 8.500000f } });
                sd.wireframe.push_back({ { -10.000000f, -10.000000f, 8.500000f }, { 10.000000f, -10.000000f, 8.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 14.000000f, 1.000000f }, { 14.000000f, 14.000000f, 1.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 14.000000f, 1.000000f }, { 16.000000f, 32.000000f, 4.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 1.000000f }, { 16.000000f, 32.000000f, 4.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 14.000000f, 1.000000f }, { -16.000000f, 32.000000f, 4.500000f } });
                sd.wireframe.push_back({ { 16.000000f, 32.000000f, 4.500000f }, { -16.000000f, 32.000000f, 4.500000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 4.000000f }, { -14.000000f, 14.000000f, 4.000000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 4.000000f }, { -24.000000f, 34.000000f, 9.000000f } });
                sd.wireframe.push_back({ { -14.000000f, 14.000000f, 4.000000f }, { -24.000000f, 34.000000f, 9.000000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 4.000000f }, { -22.000000f, 10.000000f, 9.000000f } });
                sd.wireframe.push_back({ { -24.000000f, 34.000000f, 9.000000f }, { -22.000000f, 10.000000f, 9.000000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 4.000000f }, { -10.000000f, 6.000000f, 5.500000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 4.000000f }, { -14.000000f, 14.000000f, 5.500000f } });
                sd.wireframe.push_back({ { -14.000000f, 14.000000f, 4.000000f }, { -14.000000f, 14.000000f, 5.500000f } });
                sd.wireframe.push_back({ { -10.000000f, 6.000000f, 5.500000f }, { -14.000000f, 14.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 4.000000f }, { 10.000000f, 6.000000f, 4.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 4.000000f }, { 22.000000f, 10.000000f, 9.000000f } });
                sd.wireframe.push_back({ { 10.000000f, 6.000000f, 4.000000f }, { 22.000000f, 10.000000f, 9.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 4.000000f }, { 24.000000f, 34.000000f, 9.000000f } });
                sd.wireframe.push_back({ { 22.000000f, 10.000000f, 9.000000f }, { 24.000000f, 34.000000f, 9.000000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 4.000000f }, { 14.000000f, 14.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 4.000000f }, { 10.000000f, 6.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 10.000000f, 6.000000f, 4.000000f }, { 10.000000f, 6.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 14.000000f, 14.000000f, 5.500000f }, { 10.000000f, 6.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 16.000000f, 32.000000f, 4.500000f }, { 20.000000f, 50.000000f, 11.500000f } });
                sd.wireframe.push_back({ { -16.000000f, 32.000000f, 4.500000f }, { 20.000000f, 50.000000f, 11.500000f } });
                sd.wireframe.push_back({ { -16.000000f, 32.000000f, 4.500000f }, { -20.000000f, 50.000000f, 11.500000f } });
                sd.wireframe.push_back({ { 20.000000f, 50.000000f, 11.500000f }, { -20.000000f, 50.000000f, 11.500000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 0.000000f }, { 25.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 0.000000f, 0.000000f }, { 25.000000f, 40.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 40.000000f, 0.000000f }, { 0.000000f, 55.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 55.000000f, 0.000000f }, { -25.000000f, 40.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 0.000000f }, { -25.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 40.000000f, 0.000000f }, { -25.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 0.000000f }, { 0.000000f, -14.000000f, 16.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 0.000000f, 0.000000f }, { 0.000000f, -14.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 0.000000f }, { 25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 0.000000f, 0.000000f }, { 25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 16.000000f }, { 25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 0.000000f, 0.000000f }, { 25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 40.000000f, 0.000000f }, { 25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 0.000000f, 16.000000f }, { 25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 40.000000f, 0.000000f }, { 0.000000f, 55.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 55.000000f, 0.000000f }, { 0.000000f, 55.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 40.000000f, 16.000000f }, { 0.000000f, 55.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 55.000000f, 0.000000f }, { -25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 40.000000f, 0.000000f }, { -25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 55.000000f, 16.000000f }, { -25.000000f, 40.000000f, 16.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 40.000000f, 0.000000f }, { -25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 0.000000f, 0.000000f }, { -25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 16.000000f }, { -25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 40.000000f, 16.000000f }, { -25.000000f, 0.000000f, 16.000000f } });
                sd.wireframe.push_back({ { 0.000000f, -14.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 0.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.wireframe.push_back({ { 25.000000f, 40.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 55.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 40.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.wireframe.push_back({ { -25.000000f, 0.000000f, 16.000000f }, { 0.000000f, 20.000000f, 21.000000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Center Stage to Front Arena Stalls";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 20.000000f, 2.200000f };
                    pd.directDistance = 13.010000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.010000f;
                        rs.order = 0;
                        rs.gain = 0.077000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.020000f;
                        rs.order = 1;
                        rs.gain = 0.074000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 19.810000f, 2.130000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.220000f;
                        rs.order = 1;
                        rs.gain = 0.074000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 12.420000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.860000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 1.440000f, 10.650000f, 19.510000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.860000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -1.440000f, 10.650000f, 19.510000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.660000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 25.000000f, 13.500000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.660000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -25.000000f, 13.500000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, 20.000000f, 2.200000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Stage to Left Vineyard Terrace";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { -16.000000f, 22.000000f, 6.500000f };
                    pd.directDistance = 22.430000f;
                    pd.rays.reserve(5);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.430000f;
                        rs.order = 0;
                        rs.gain = 0.045000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -16.000000f, 22.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.930000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -2.390000f, 9.240000f, 0.800000f });
                        rs.points.push_back({ -16.000000f, 22.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.880000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -16.130000f, 18.260000f, 5.750000f });
                        rs.points.push_back({ -16.000000f, 22.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.570000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -12.810000f, 16.440000f, 18.440000f });
                        rs.points.push_back({ -16.000000f, 22.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 61.700000f;
                        rs.order = 1;
                        rs.gain = 0.016000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -24.640000f, 40.220000f, 4.960000f });
                        rs.points.push_back({ -16.000000f, 22.000000f, 6.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Stage to Rear Choir Terrace Tier";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, -5.000000f, 4.500000f };
                    pd.directDistance = 12.300000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.300000f;
                        rs.order = 0;
                        rs.gain = 0.081000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.400000f;
                        rs.order = 1;
                        rs.gain = 0.078000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -4.230000f, 3.910000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 12.890000f;
                        rs.order = 1;
                        rs.gain = 0.076000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 4.450000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.840000f;
                        rs.order = 1;
                        rs.gain = 0.031000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 1.180000f, -2.000000f, 17.670000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.840000f;
                        rs.order = 1;
                        rs.gain = 0.031000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -1.180000f, -2.000000f, 17.670000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.490000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 25.000000f, 1.000000f, 3.150000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 51.490000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -25.000000f, 1.000000f, 3.150000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Stage to Upper Rear Vineyard Tier";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 40.000000f, 8.000000f };
                    pd.directDistance = 33.580000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.580000f;
                        rs.order = 0;
                        rs.gain = 0.030000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.720000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 38.520000f, 7.030000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.820000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 24.750000f, 3.090000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.000000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 11.020000f, 0.800000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 44.260000f;
                        rs.order = 1;
                        rs.gain = 0.022000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 1.140000f, 31.180000f, 19.310000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 44.260000f;
                        rs.order = 1;
                        rs.gain = 0.022000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -1.140000f, 31.180000f, 19.310000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 60.230000f;
                        rs.order = 1;
                        rs.gain = 0.016000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 25.000000f, 23.500000f, 4.900000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 60.230000f;
                        rs.order = 1;
                        rs.gain = 0.016000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -25.000000f, 23.500000f, 4.900000f });
                        rs.points.push_back({ 0.000000f, 40.000000f, 8.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 8: sydney_opera_house
            {
                SpaceData sd;
                sd.id = "sydney_opera_house";
                sd.title = "Sydney Opera House Concert Hall";
                sd.category = "Concert Halls";
                sd.rt60 = 2.000000f;
                sd.volume = 25000.000000f;
                sd.area = 6200.000000f;
                sd.minBound = { -12.000000f, 0.000000f, 0.000000f };
                sd.maxBound = { 12.000000f, 52.000000f, 25.000000f };
                sd.wireframe.reserve(44);
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 52.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 52.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { -12.000000f, 52.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 52.000000f, 0.000000f }, { -12.000000f, 52.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -10.500000f, 1.000000f, 1.100000f }, { 10.500000f, 1.000000f, 1.100000f } });
                sd.wireframe.push_back({ { -10.500000f, 1.000000f, 1.100000f }, { 10.500000f, 14.000000f, 1.100000f } });
                sd.wireframe.push_back({ { 10.500000f, 1.000000f, 1.100000f }, { 10.500000f, 14.000000f, 1.100000f } });
                sd.wireframe.push_back({ { -10.500000f, 1.000000f, 1.100000f }, { -10.500000f, 14.000000f, 1.100000f } });
                sd.wireframe.push_back({ { 10.500000f, 14.000000f, 1.100000f }, { -10.500000f, 14.000000f, 1.100000f } });
                sd.wireframe.push_back({ { -12.000000f, 14.000000f, 5.000000f }, { -9.200000f, 14.000000f, 5.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 14.000000f, 5.000000f }, { -9.200000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { -9.200000f, 14.000000f, 5.000000f }, { -9.200000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 14.000000f, 5.000000f }, { -12.000000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { -9.200000f, 48.000000f, 5.000000f }, { -12.000000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { 9.200000f, 14.000000f, 5.000000f }, { 12.000000f, 14.000000f, 5.000000f } });
                sd.wireframe.push_back({ { 9.200000f, 14.000000f, 5.000000f }, { 12.000000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 14.000000f, 5.000000f }, { 12.000000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { 9.200000f, 14.000000f, 5.000000f }, { 9.200000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 48.000000f, 5.000000f }, { 9.200000f, 48.000000f, 5.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { -12.000000f, 0.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { -12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 52.000000f, 0.000000f }, { -12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 13.000000f }, { -12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 0.000000f, 13.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 0.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 13.000000f }, { 12.000000f, 0.000000f, 13.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 0.000000f }, { 12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 52.000000f, 0.000000f }, { 12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 52.000000f, 0.000000f }, { 12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 52.000000f, 13.000000f }, { 12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 13.000000f }, { 12.000000f, 52.000000f, 13.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 13.000000f }, { 0.000000f, 0.000000f, 25.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 0.000000f, 13.000000f }, { 0.000000f, 0.000000f, 25.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 52.000000f, 13.000000f }, { 0.000000f, 0.000000f, 25.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 0.000000f, 13.000000f }, { 0.000000f, 52.000000f, 25.000000f } });
                sd.wireframe.push_back({ { -12.000000f, 52.000000f, 13.000000f }, { 0.000000f, 52.000000f, 25.000000f } });
                sd.wireframe.push_back({ { 12.000000f, 52.000000f, 13.000000f }, { 0.000000f, 52.000000f, 25.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 0.000000f, 25.000000f }, { 0.000000f, 52.000000f, 25.000000f } });
                sd.wireframe.push_back({ { -5.000000f, 4.000000f, 11.500000f }, { 5.000000f, 4.000000f, 11.500000f } });
                sd.wireframe.push_back({ { -5.000000f, 4.000000f, 11.500000f }, { 5.000000f, 20.000000f, 11.500000f } });
                sd.wireframe.push_back({ { 5.000000f, 4.000000f, 11.500000f }, { 5.000000f, 20.000000f, 11.500000f } });
                sd.wireframe.push_back({ { -5.000000f, 4.000000f, 11.500000f }, { -5.000000f, 20.000000f, 11.500000f } });
                sd.wireframe.push_back({ { 5.000000f, 20.000000f, 11.500000f }, { -5.000000f, 20.000000f, 11.500000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Stage to Front Stalls Petal Focus";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 22.000000f, 1.600000f };
                    pd.directDistance = 15.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.000000f;
                        rs.order = 0;
                        rs.gain = 0.067000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.680000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 14.420000f, 11.500000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.300000f;
                        rs.order = 1;
                        rs.gain = 0.035000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -12.000000f, 14.500000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.300000f;
                        rs.order = 1;
                        rs.gain = 0.035000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 12.000000f, 14.500000f, 1.700000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 29.000000f;
                        rs.order = 1;
                        rs.gain = 0.034000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.750000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.200000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 11.650000f, 14.470000f, 13.350000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 36.200000f;
                        rs.order = 1;
                        rs.gain = 0.027000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -11.650000f, 14.470000f, 13.350000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 75.000000f;
                        rs.order = 1;
                        rs.gain = 0.013000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 52.000000f, 1.680000f });
                        rs.points.push_back({ 0.000000f, 22.000000f, 1.600000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Stage to Mid-Hall Brushbox Parquet";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 32.000000f, 2.400000f };
                    pd.directDistance = 25.010000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 25.010000f;
                        rs.order = 0;
                        rs.gain = 0.040000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.280000f;
                        rs.order = 1;
                        rs.gain = 0.032000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 19.900000f, 11.500000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.660000f;
                        rs.order = 1;
                        rs.gain = 0.028000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -12.000000f, 19.500000f, 2.100000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.660000f;
                        rs.order = 1;
                        rs.gain = 0.028000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 12.000000f, 19.500000f, 2.100000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 39.000000f;
                        rs.order = 1;
                        rs.gain = 0.025000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.910000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 40.910000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 11.450000f, 19.660000f, 13.550000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 40.910000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -11.450000f, 19.660000f, 13.550000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 65.000000f;
                        rs.order = 1;
                        rs.gain = 0.015000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 52.000000f, 2.220000f });
                        rs.points.push_back({ 0.000000f, 32.000000f, 2.400000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Stage to Elevated Side Box Tier";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 9.500000f, 26.000000f, 7.500000f };
                    pd.directDistance = 21.990000f;
                    pd.rays.reserve(7);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 21.990000f;
                        rs.order = 0;
                        rs.gain = 0.045000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.400000f;
                        rs.order = 1;
                        rs.gain = 0.044000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.940000f, 8.870000f, 1.100000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.570000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 12.000000f, 22.720000f, 6.520000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 34.810000f;
                        rs.order = 1;
                        rs.gain = 0.028000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 2.020000f, 0.000000f, 3.010000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 38.930000f;
                        rs.order = 1;
                        rs.gain = 0.025000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -12.000000f, 13.810000f, 3.840000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 41.670000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -8.090000f, 15.780000f, 16.910000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 71.860000f;
                        rs.order = 1;
                        rs.gain = 0.014000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 6.020000f, 52.000000f, 5.410000f });
                        rs.points.push_back({ 9.500000f, 26.000000f, 7.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Stage to High Crown Vault Rear Tier";
                    pd.source = { 0.000000f, 7.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 46.000000f, 12.000000f };
                    pd.directDistance = 40.310000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 40.310000f;
                        rs.order = 0;
                        rs.gain = 0.025000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 40.690000f;
                        rs.order = 1;
                        rs.gain = 0.024000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 9.350000f, 1.100000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 46.920000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -12.000000f, 26.500000f, 6.900000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 46.920000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 12.000000f, 26.500000f, 6.900000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 47.200000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 8.330000f, 31.990000f, 16.670000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 47.200000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ -8.330000f, 31.990000f, 16.670000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 52.010000f;
                        rs.order = 1;
                        rs.gain = 0.019000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 52.000000f, 10.800000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 53.970000f;
                        rs.order = 1;
                        rs.gain = 0.018000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 7.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -0.000000f, 3.150000f });
                        rs.points.push_back({ 0.000000f, 46.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 9: hagia_sophia
            {
                SpaceData sd;
                sd.id = "hagia_sophia";
                sd.title = "Hagia Sophia Grand Dome (Istanbul)";
                sd.category = "World Landmarks";
                sd.rt60 = 11.000000f;
                sd.volume = 150000.000000f;
                sd.area = 18500.000000f;
                sd.minBound = { -34.000000f, -37.500000f, 0.000000f };
                sd.maxBound = { 34.000000f, 37.500000f, 55.000000f };
                sd.wireframe.reserve(337);
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 0.000000f }, { 34.000000f, -37.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 0.000000f }, { 34.000000f, 37.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 34.000000f, -37.500000f, 0.000000f }, { 34.000000f, 37.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 0.000000f }, { -34.000000f, 37.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 34.000000f, 37.500000f, 0.000000f }, { -34.000000f, 37.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 0.000000f }, { -15.500000f, -21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -21.500000f, 0.000000f }, { -15.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 0.000000f }, { -21.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -15.500000f, 0.000000f }, { -21.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 0.000000f }, { -21.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -15.500000f, 0.000000f }, { -21.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 0.000000f }, { -15.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -21.500000f, 0.000000f }, { -15.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 40.000000f }, { -15.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -21.500000f, 0.000000f }, { -15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -15.500000f, 0.000000f }, { -15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -21.500000f, 40.000000f }, { -15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -15.500000f, 0.000000f }, { -21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -15.500000f, 0.000000f }, { -21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, -21.500000f, 40.000000f }, { -21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, -15.500000f, 40.000000f }, { -21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 0.000000f }, { 21.500000f, -21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -21.500000f, 0.000000f }, { 21.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 0.000000f }, { 15.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -15.500000f, 0.000000f }, { 15.500000f, -15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 0.000000f }, { 15.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -15.500000f, 0.000000f }, { 15.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 0.000000f }, { 21.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -21.500000f, 0.000000f }, { 21.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 40.000000f }, { 21.500000f, -21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -21.500000f, 0.000000f }, { 21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -15.500000f, 0.000000f }, { 21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -21.500000f, 40.000000f }, { 21.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -15.500000f, 0.000000f }, { 15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -15.500000f, 0.000000f }, { 15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, -21.500000f, 40.000000f }, { 15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, -15.500000f, 40.000000f }, { 15.500000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 0.000000f }, { 21.500000f, 15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 15.500000f, 0.000000f }, { 21.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 0.000000f }, { 15.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 21.500000f, 0.000000f }, { 15.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 0.000000f }, { 15.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 21.500000f, 0.000000f }, { 15.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 0.000000f }, { 21.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 15.500000f, 0.000000f }, { 21.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 40.000000f }, { 21.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 15.500000f, 0.000000f }, { 21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 21.500000f, 0.000000f }, { 21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 15.500000f, 40.000000f }, { 21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 21.500000f, 0.000000f }, { 15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 21.500000f, 0.000000f }, { 15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 15.500000f, 40.000000f }, { 15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 21.500000f, 21.500000f, 40.000000f }, { 15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 0.000000f }, { -15.500000f, 15.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 15.500000f, 0.000000f }, { -15.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 0.000000f }, { -21.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 21.500000f, 0.000000f }, { -21.500000f, 21.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 0.000000f }, { -21.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 21.500000f, 0.000000f }, { -21.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 0.000000f }, { -15.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 15.500000f, 0.000000f }, { -15.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 40.000000f }, { -15.500000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 15.500000f, 0.000000f }, { -15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 21.500000f, 0.000000f }, { -15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 15.500000f, 40.000000f }, { -15.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 21.500000f, 0.000000f }, { -21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 21.500000f, 0.000000f }, { -21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -21.500000f, 15.500000f, 40.000000f }, { -21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 21.500000f, 40.000000f }, { -21.500000f, 21.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 0.000000f }, { -34.000000f, -37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { -34.000000f, 37.500000f, 0.000000f }, { -34.000000f, -37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 0.000000f }, { 34.000000f, -37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, -37.500000f, 0.000000f }, { 34.000000f, -37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 28.000000f }, { 34.000000f, -37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, -37.500000f, 0.000000f }, { 34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, 37.500000f, 0.000000f }, { 34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, -37.500000f, 28.000000f }, { 34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, 37.500000f, 0.000000f }, { -34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { -34.000000f, 37.500000f, 0.000000f }, { -34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { -34.000000f, -37.500000f, 28.000000f }, { -34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 34.000000f, 37.500000f, 28.000000f }, { -34.000000f, 37.500000f, 28.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 0.000000f, 40.000000f }, { 13.420000f, 7.750000f, 40.000000f } });
                sd.wireframe.push_back({ { 13.420000f, 7.750000f, 40.000000f }, { 7.750000f, 13.420000f, 40.000000f } });
                sd.wireframe.push_back({ { 7.750000f, 13.420000f, 40.000000f }, { 0.000000f, 15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 15.500000f, 40.000000f }, { -7.750000f, 13.420000f, 40.000000f } });
                sd.wireframe.push_back({ { -7.750000f, 13.420000f, 40.000000f }, { -13.420000f, 7.750000f, 40.000000f } });
                sd.wireframe.push_back({ { -13.420000f, 7.750000f, 40.000000f }, { -15.500000f, 0.000000f, 40.000000f } });
                sd.wireframe.push_back({ { -15.500000f, 0.000000f, 40.000000f }, { -13.420000f, -7.750000f, 40.000000f } });
                sd.wireframe.push_back({ { -13.420000f, -7.750000f, 40.000000f }, { -7.750000f, -13.420000f, 40.000000f } });
                sd.wireframe.push_back({ { -7.750000f, -13.420000f, 40.000000f }, { -0.000000f, -15.500000f, 40.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -15.500000f, 40.000000f }, { 7.750000f, -13.420000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 0.000000f, 40.000000f }, { 13.420000f, -7.750000f, 40.000000f } });
                sd.wireframe.push_back({ { 7.750000f, -13.420000f, 40.000000f }, { 13.420000f, -7.750000f, 40.000000f } });
                sd.wireframe.push_back({ { 15.500000f, 0.000000f, 40.000000f }, { 14.520000f, 0.000000f, 45.240000f } });
                sd.wireframe.push_back({ { 13.420000f, -7.750000f, 40.000000f }, { 14.520000f, 0.000000f, 45.240000f } });
                sd.wireframe.push_back({ { 15.500000f, 0.000000f, 40.000000f }, { 12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 13.420000f, 7.750000f, 40.000000f }, { 12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 14.520000f, 0.000000f, 45.240000f }, { 12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 13.420000f, 7.750000f, 40.000000f }, { 7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.750000f, 13.420000f, 40.000000f }, { 7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 12.580000f, 7.260000f, 45.240000f }, { 7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.750000f, 13.420000f, 40.000000f }, { 0.000000f, 14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { 0.000000f, 15.500000f, 40.000000f }, { 0.000000f, 14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.260000f, 12.580000f, 45.240000f }, { 0.000000f, 14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { 0.000000f, 15.500000f, 40.000000f }, { -7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.750000f, 13.420000f, 40.000000f }, { -7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.520000f, 45.240000f }, { -7.260000f, 12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.750000f, 13.420000f, 40.000000f }, { -12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -13.420000f, 7.750000f, 40.000000f }, { -12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.260000f, 12.580000f, 45.240000f }, { -12.580000f, 7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -13.420000f, 7.750000f, 40.000000f }, { -14.520000f, 0.000000f, 45.240000f } });
                sd.wireframe.push_back({ { -15.500000f, 0.000000f, 40.000000f }, { -14.520000f, 0.000000f, 45.240000f } });
                sd.wireframe.push_back({ { -12.580000f, 7.260000f, 45.240000f }, { -14.520000f, 0.000000f, 45.240000f } });
                sd.wireframe.push_back({ { -15.500000f, 0.000000f, 40.000000f }, { -12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -13.420000f, -7.750000f, 40.000000f }, { -12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -14.520000f, 0.000000f, 45.240000f }, { -12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { -13.420000f, -7.750000f, 40.000000f }, { -7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.750000f, -13.420000f, 40.000000f }, { -7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -12.580000f, -7.260000f, 45.240000f }, { -7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.750000f, -13.420000f, 40.000000f }, { -0.000000f, -14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { -0.000000f, -15.500000f, 40.000000f }, { -0.000000f, -14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { -7.260000f, -12.580000f, 45.240000f }, { -0.000000f, -14.520000f, 45.240000f } });
                sd.wireframe.push_back({ { -0.000000f, -15.500000f, 40.000000f }, { 7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.750000f, -13.420000f, 40.000000f }, { 7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.520000f, 45.240000f }, { 7.260000f, -12.580000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.750000f, -13.420000f, 40.000000f }, { 12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 13.420000f, -7.750000f, 40.000000f }, { 12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 14.520000f, 0.000000f, 45.240000f }, { 12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 7.260000f, -12.580000f, 45.240000f }, { 12.580000f, -7.260000f, 45.240000f } });
                sd.wireframe.push_back({ { 14.520000f, 0.000000f, 45.240000f }, { 11.710000f, 0.000000f, 49.820000f } });
                sd.wireframe.push_back({ { 12.580000f, -7.260000f, 45.240000f }, { 11.710000f, 0.000000f, 49.820000f } });
                sd.wireframe.push_back({ { 14.520000f, 0.000000f, 45.240000f }, { 10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 12.580000f, 7.260000f, 45.240000f }, { 10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 11.710000f, 0.000000f, 49.820000f }, { 10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 12.580000f, 7.260000f, 45.240000f }, { 5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 7.260000f, 12.580000f, 45.240000f }, { 5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 10.140000f, 5.860000f, 49.820000f }, { 5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 7.260000f, 12.580000f, 45.240000f }, { 0.000000f, 11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.520000f, 45.240000f }, { 0.000000f, 11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { 5.860000f, 10.140000f, 49.820000f }, { 0.000000f, 11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { 0.000000f, 14.520000f, 45.240000f }, { -5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -7.260000f, 12.580000f, 45.240000f }, { -5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 0.000000f, 11.710000f, 49.820000f }, { -5.860000f, 10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -7.260000f, 12.580000f, 45.240000f }, { -10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -12.580000f, 7.260000f, 45.240000f }, { -10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -5.860000f, 10.140000f, 49.820000f }, { -10.140000f, 5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -12.580000f, 7.260000f, 45.240000f }, { -11.710000f, 0.000000f, 49.820000f } });
                sd.wireframe.push_back({ { -14.520000f, 0.000000f, 45.240000f }, { -11.710000f, 0.000000f, 49.820000f } });
                sd.wireframe.push_back({ { -10.140000f, 5.860000f, 49.820000f }, { -11.710000f, 0.000000f, 49.820000f } });
                sd.wireframe.push_back({ { -14.520000f, 0.000000f, 45.240000f }, { -10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -12.580000f, -7.260000f, 45.240000f }, { -10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -11.710000f, 0.000000f, 49.820000f }, { -10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { -12.580000f, -7.260000f, 45.240000f }, { -5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -7.260000f, -12.580000f, 45.240000f }, { -5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -10.140000f, -5.860000f, 49.820000f }, { -5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -7.260000f, -12.580000f, 45.240000f }, { -0.000000f, -11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.520000f, 45.240000f }, { -0.000000f, -11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { -5.860000f, -10.140000f, 49.820000f }, { -0.000000f, -11.710000f, 49.820000f } });
                sd.wireframe.push_back({ { -0.000000f, -14.520000f, 45.240000f }, { 5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 7.260000f, -12.580000f, 45.240000f }, { 5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { -0.000000f, -11.710000f, 49.820000f }, { 5.860000f, -10.140000f, 49.820000f } });
                sd.wireframe.push_back({ { 7.260000f, -12.580000f, 45.240000f }, { 10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 12.580000f, -7.260000f, 45.240000f }, { 10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 11.710000f, 0.000000f, 49.820000f }, { 10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 5.860000f, -10.140000f, 49.820000f }, { 10.140000f, -5.860000f, 49.820000f } });
                sd.wireframe.push_back({ { 11.710000f, 0.000000f, 49.820000f }, { 7.430000f, 0.000000f, 53.170000f } });
                sd.wireframe.push_back({ { 10.140000f, -5.860000f, 49.820000f }, { 7.430000f, 0.000000f, 53.170000f } });
                sd.wireframe.push_back({ { 11.710000f, 0.000000f, 49.820000f }, { 6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 10.140000f, 5.860000f, 49.820000f }, { 6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 7.430000f, 0.000000f, 53.170000f }, { 6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 10.140000f, 5.860000f, 49.820000f }, { 3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 5.860000f, 10.140000f, 49.820000f }, { 3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 6.430000f, 3.710000f, 53.170000f }, { 3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 5.860000f, 10.140000f, 49.820000f }, { 0.000000f, 7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 0.000000f, 11.710000f, 49.820000f }, { 0.000000f, 7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 3.710000f, 6.430000f, 53.170000f }, { 0.000000f, 7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 0.000000f, 11.710000f, 49.820000f }, { -3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -5.860000f, 10.140000f, 49.820000f }, { -3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 0.000000f, 7.430000f, 53.170000f }, { -3.710000f, 6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -5.860000f, 10.140000f, 49.820000f }, { -6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -10.140000f, 5.860000f, 49.820000f }, { -6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -3.710000f, 6.430000f, 53.170000f }, { -6.430000f, 3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -10.140000f, 5.860000f, 49.820000f }, { -7.430000f, 0.000000f, 53.170000f } });
                sd.wireframe.push_back({ { -11.710000f, 0.000000f, 49.820000f }, { -7.430000f, 0.000000f, 53.170000f } });
                sd.wireframe.push_back({ { -6.430000f, 3.710000f, 53.170000f }, { -7.430000f, 0.000000f, 53.170000f } });
                sd.wireframe.push_back({ { -11.710000f, 0.000000f, 49.820000f }, { -6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -10.140000f, -5.860000f, 49.820000f }, { -6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -7.430000f, 0.000000f, 53.170000f }, { -6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { -10.140000f, -5.860000f, 49.820000f }, { -3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -5.860000f, -10.140000f, 49.820000f }, { -3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -6.430000f, -3.710000f, 53.170000f }, { -3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -5.860000f, -10.140000f, 49.820000f }, { -0.000000f, -7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -0.000000f, -11.710000f, 49.820000f }, { -0.000000f, -7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -3.710000f, -6.430000f, 53.170000f }, { -0.000000f, -7.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -0.000000f, -11.710000f, 49.820000f }, { 3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 5.860000f, -10.140000f, 49.820000f }, { 3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { -0.000000f, -7.430000f, 53.170000f }, { 3.710000f, -6.430000f, 53.170000f } });
                sd.wireframe.push_back({ { 5.860000f, -10.140000f, 49.820000f }, { 6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 10.140000f, -5.860000f, 49.820000f }, { 6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 7.430000f, 0.000000f, 53.170000f }, { 6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 3.710000f, -6.430000f, 53.170000f }, { 6.430000f, -3.710000f, 53.170000f } });
                sd.wireframe.push_back({ { 7.430000f, 0.000000f, 53.170000f }, { 3.290000f, 0.000000f, 54.660000f } });
                sd.wireframe.push_back({ { 6.430000f, -3.710000f, 53.170000f }, { 3.290000f, 0.000000f, 54.660000f } });
                sd.wireframe.push_back({ { 7.430000f, 0.000000f, 53.170000f }, { 2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 6.430000f, 3.710000f, 53.170000f }, { 2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.290000f, 0.000000f, 54.660000f }, { 2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 6.430000f, 3.710000f, 53.170000f }, { 1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.710000f, 6.430000f, 53.170000f }, { 1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 2.850000f, 1.650000f, 54.660000f }, { 1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.710000f, 6.430000f, 53.170000f }, { 0.000000f, 3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { 0.000000f, 7.430000f, 53.170000f }, { 0.000000f, 3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { 1.650000f, 2.850000f, 54.660000f }, { 0.000000f, 3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { 0.000000f, 7.430000f, 53.170000f }, { -1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -3.710000f, 6.430000f, 53.170000f }, { -1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 0.000000f, 3.290000f, 54.660000f }, { -1.650000f, 2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -3.710000f, 6.430000f, 53.170000f }, { -2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -6.430000f, 3.710000f, 53.170000f }, { -2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -1.650000f, 2.850000f, 54.660000f }, { -2.850000f, 1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -6.430000f, 3.710000f, 53.170000f }, { -3.290000f, 0.000000f, 54.660000f } });
                sd.wireframe.push_back({ { -7.430000f, 0.000000f, 53.170000f }, { -3.290000f, 0.000000f, 54.660000f } });
                sd.wireframe.push_back({ { -2.850000f, 1.650000f, 54.660000f }, { -3.290000f, 0.000000f, 54.660000f } });
                sd.wireframe.push_back({ { -7.430000f, 0.000000f, 53.170000f }, { -2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -6.430000f, -3.710000f, 53.170000f }, { -2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -3.290000f, 0.000000f, 54.660000f }, { -2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { -6.430000f, -3.710000f, 53.170000f }, { -1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -3.710000f, -6.430000f, 53.170000f }, { -1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -2.850000f, -1.650000f, 54.660000f }, { -1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -3.710000f, -6.430000f, 53.170000f }, { -0.000000f, -3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { -0.000000f, -7.430000f, 53.170000f }, { -0.000000f, -3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { -1.650000f, -2.850000f, 54.660000f }, { -0.000000f, -3.290000f, 54.660000f } });
                sd.wireframe.push_back({ { -0.000000f, -7.430000f, 53.170000f }, { 1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.710000f, -6.430000f, 53.170000f }, { 1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { -0.000000f, -3.290000f, 54.660000f }, { 1.650000f, -2.850000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.710000f, -6.430000f, 53.170000f }, { 2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 6.430000f, -3.710000f, 53.170000f }, { 2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.290000f, 0.000000f, 54.660000f }, { 2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 1.650000f, -2.850000f, 54.660000f }, { 2.850000f, -1.650000f, 54.660000f } });
                sd.wireframe.push_back({ { 3.290000f, 0.000000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 2.850000f, 1.650000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 1.650000f, 2.850000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 3.290000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -1.650000f, 2.850000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -2.850000f, 1.650000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -3.290000f, 0.000000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -2.850000f, -1.650000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -1.650000f, -2.850000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -3.290000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 1.650000f, -2.850000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 2.850000f, -1.650000f, 54.660000f }, { 0.000000f, 0.000000f, 55.000000f } });
                sd.wireframe.push_back({ { 13.950000f, 18.750000f, 28.000000f }, { 12.080000f, 25.730000f, 28.000000f } });
                sd.wireframe.push_back({ { 12.080000f, 25.730000f, 28.000000f }, { 6.970000f, 30.830000f, 28.000000f } });
                sd.wireframe.push_back({ { 6.970000f, 30.830000f, 28.000000f }, { 0.000000f, 32.700000f, 28.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 32.700000f, 28.000000f }, { -6.970000f, 30.830000f, 28.000000f } });
                sd.wireframe.push_back({ { -6.970000f, 30.830000f, 28.000000f }, { -12.080000f, 25.730000f, 28.000000f } });
                sd.wireframe.push_back({ { -12.080000f, 25.730000f, 28.000000f }, { -13.950000f, 18.750000f, 28.000000f } });
                sd.wireframe.push_back({ { 13.950000f, 18.750000f, 28.000000f }, { 12.220000f, 18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { 13.950000f, 18.750000f, 28.000000f }, { 10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.080000f, 25.730000f, 28.000000f }, { 10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.220000f, 18.750000f, 33.780000f }, { 10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.080000f, 25.730000f, 28.000000f }, { 6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.970000f, 30.830000f, 28.000000f }, { 6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 10.590000f, 24.860000f, 33.780000f }, { 6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.970000f, 30.830000f, 28.000000f }, { 0.000000f, 30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { 0.000000f, 32.700000f, 28.000000f }, { 0.000000f, 30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.110000f, 29.340000f, 33.780000f }, { 0.000000f, 30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { 0.000000f, 32.700000f, 28.000000f }, { -6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.970000f, 30.830000f, 28.000000f }, { -6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 0.000000f, 30.970000f, 33.780000f }, { -6.110000f, 29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.970000f, 30.830000f, 28.000000f }, { -10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.080000f, 25.730000f, 28.000000f }, { -10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.110000f, 29.340000f, 33.780000f }, { -10.590000f, 24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.080000f, 25.730000f, 28.000000f }, { -12.220000f, 18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { -13.950000f, 18.750000f, 28.000000f }, { -12.220000f, 18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { -10.590000f, 24.860000f, 33.780000f }, { -12.220000f, 18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.220000f, 18.750000f, 33.780000f }, { 7.470000f, 18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { 12.220000f, 18.750000f, 33.780000f }, { 6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 10.590000f, 24.860000f, 33.780000f }, { 6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 7.470000f, 18.750000f, 38.130000f }, { 6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 10.590000f, 24.860000f, 33.780000f }, { 3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.110000f, 29.340000f, 33.780000f }, { 3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.470000f, 22.490000f, 38.130000f }, { 3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.110000f, 29.340000f, 33.780000f }, { 0.000000f, 26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 0.000000f, 30.970000f, 33.780000f }, { 0.000000f, 26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 3.740000f, 25.220000f, 38.130000f }, { 0.000000f, 26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 0.000000f, 30.970000f, 33.780000f }, { -3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.110000f, 29.340000f, 33.780000f }, { -3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 0.000000f, 26.220000f, 38.130000f }, { -3.740000f, 25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.110000f, 29.340000f, 33.780000f }, { -6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -10.590000f, 24.860000f, 33.780000f }, { -6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -3.740000f, 25.220000f, 38.130000f }, { -6.470000f, 22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -10.590000f, 24.860000f, 33.780000f }, { -7.470000f, 18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { -12.220000f, 18.750000f, 33.780000f }, { -7.470000f, 18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.470000f, 22.490000f, 38.130000f }, { -7.470000f, 18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { -13.950000f, -18.750000f, 28.000000f }, { -12.080000f, -25.730000f, 28.000000f } });
                sd.wireframe.push_back({ { -12.080000f, -25.730000f, 28.000000f }, { -6.970000f, -30.830000f, 28.000000f } });
                sd.wireframe.push_back({ { -6.970000f, -30.830000f, 28.000000f }, { -0.000000f, -32.700000f, 28.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -32.700000f, 28.000000f }, { 6.970000f, -30.830000f, 28.000000f } });
                sd.wireframe.push_back({ { 6.970000f, -30.830000f, 28.000000f }, { 12.080000f, -25.730000f, 28.000000f } });
                sd.wireframe.push_back({ { 12.080000f, -25.730000f, 28.000000f }, { 13.950000f, -18.750000f, 28.000000f } });
                sd.wireframe.push_back({ { -13.950000f, -18.750000f, 28.000000f }, { -12.220000f, -18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { -13.950000f, -18.750000f, 28.000000f }, { -10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.080000f, -25.730000f, 28.000000f }, { -10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.220000f, -18.750000f, 33.780000f }, { -10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.080000f, -25.730000f, 28.000000f }, { -6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.970000f, -30.830000f, 28.000000f }, { -6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -10.590000f, -24.860000f, 33.780000f }, { -6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.970000f, -30.830000f, 28.000000f }, { -0.000000f, -30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { -0.000000f, -32.700000f, 28.000000f }, { -0.000000f, -30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { -6.110000f, -29.340000f, 33.780000f }, { -0.000000f, -30.970000f, 33.780000f } });
                sd.wireframe.push_back({ { -0.000000f, -32.700000f, 28.000000f }, { 6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.970000f, -30.830000f, 28.000000f }, { 6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { -0.000000f, -30.970000f, 33.780000f }, { 6.110000f, -29.340000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.970000f, -30.830000f, 28.000000f }, { 10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.080000f, -25.730000f, 28.000000f }, { 10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 6.110000f, -29.340000f, 33.780000f }, { 10.590000f, -24.860000f, 33.780000f } });
                sd.wireframe.push_back({ { 12.080000f, -25.730000f, 28.000000f }, { 12.220000f, -18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { 13.950000f, -18.750000f, 28.000000f }, { 12.220000f, -18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { 10.590000f, -24.860000f, 33.780000f }, { 12.220000f, -18.750000f, 33.780000f } });
                sd.wireframe.push_back({ { -12.220000f, -18.750000f, 33.780000f }, { -7.470000f, -18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { -12.220000f, -18.750000f, 33.780000f }, { -6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -10.590000f, -24.860000f, 33.780000f }, { -6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -7.470000f, -18.750000f, 38.130000f }, { -6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { -10.590000f, -24.860000f, 33.780000f }, { -3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.110000f, -29.340000f, 33.780000f }, { -3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.470000f, -22.490000f, 38.130000f }, { -3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -6.110000f, -29.340000f, 33.780000f }, { -0.000000f, -26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -0.000000f, -30.970000f, 33.780000f }, { -0.000000f, -26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -3.740000f, -25.220000f, 38.130000f }, { -0.000000f, -26.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -0.000000f, -30.970000f, 33.780000f }, { 3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.110000f, -29.340000f, 33.780000f }, { 3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { -0.000000f, -26.220000f, 38.130000f }, { 3.740000f, -25.220000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.110000f, -29.340000f, 33.780000f }, { 6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 10.590000f, -24.860000f, 33.780000f }, { 6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 3.740000f, -25.220000f, 38.130000f }, { 6.470000f, -22.490000f, 38.130000f } });
                sd.wireframe.push_back({ { 10.590000f, -24.860000f, 33.780000f }, { 7.470000f, -18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { 12.220000f, -18.750000f, 33.780000f }, { 7.470000f, -18.750000f, 38.130000f } });
                sd.wireframe.push_back({ { 6.470000f, -22.490000f, 38.130000f }, { 7.470000f, -18.750000f, 38.130000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Omphalion Coronation to Nave Center";
                    pd.source = { 0.000000f, -10.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 8.000000f, 1.500000f };
                    pd.directDistance = 18.000000f;
                    pd.rays.reserve(6);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.000000f;
                        rs.order = 0;
                        rs.gain = 0.056000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 18.300000f;
                        rs.order = 1;
                        rs.gain = 0.054000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -0.180000f, -0.000000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 70.340000f;
                        rs.order = 1;
                        rs.gain = 0.014000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ 34.000000f, -1.000000f, 1.650000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 70.340000f;
                        rs.order = 1;
                        rs.gain = 0.014000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ -34.000000f, -1.000000f, 1.650000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 73.000000f;
                        rs.order = 1;
                        rs.gain = 0.014000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, -37.500000f, 1.690000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 77.000000f;
                        rs.order = 1;
                        rs.gain = 0.013000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -10.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 37.500000f, 1.610000f });
                        rs.points.push_back({ 0.000000f, 8.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Altar Sanctuary to East Apse Conch";
                    pd.source = { 0.000000f, 25.000000f, 2.000000f };
                    pd.listener = { 0.000000f, -5.000000f, 1.500000f };
                    pd.directDistance = 30.000000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.000000f;
                        rs.order = 0;
                        rs.gain = 0.033000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 25.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.200000f;
                        rs.order = 1;
                        rs.gain = 0.033000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 25.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, 7.860000f, -0.000000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 55.000000f;
                        rs.order = 1;
                        rs.gain = 0.018000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 25.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, 37.500000f, 1.890000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 95.000000f;
                        rs.order = 1;
                        rs.gain = 0.010000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 25.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, -37.500000f, 1.670000f });
                        rs.points.push_back({ 0.000000f, -5.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "South Imperial Gallery Balcony";
                    pd.source = { 0.000000f, 0.000000f, 1.800000f };
                    pd.listener = { 22.000000f, 0.000000f, 14.000000f };
                    pd.directDistance = 25.160000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 25.160000f;
                        rs.order = 0;
                        rs.gain = 0.040000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 22.000000f, 0.000000f, 14.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 27.090000f;
                        rs.order = 1;
                        rs.gain = 0.037000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 2.510000f, 0.000000f, -0.000000f });
                        rs.points.push_back({ 22.000000f, 0.000000f, 14.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 47.590000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 34.000000f, 0.000000f, 10.820000f });
                        rs.points.push_back({ 22.000000f, 0.000000f, 14.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 90.820000f;
                        rs.order = 1;
                        rs.gain = 0.011000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -34.000000f, 0.000000f, 6.410000f });
                        rs.points.push_back({ 22.000000f, 0.000000f, 14.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "West Narthex Cross Perspective";
                    pd.source = { 0.000000f, 15.000000f, 2.000000f };
                    pd.listener = { 0.000000f, -32.000000f, 1.500000f };
                    pd.directDistance = 47.000000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 47.000000f;
                        rs.order = 0;
                        rs.gain = 0.021000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 15.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, -32.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 47.130000f;
                        rs.order = 1;
                        rs.gain = 0.021000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 15.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, -11.860000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, -32.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 58.000000f;
                        rs.order = 1;
                        rs.gain = 0.017000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 15.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, -37.500000f, 1.550000f });
                        rs.points.push_back({ 0.000000f, -32.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 92.000000f;
                        rs.order = 1;
                        rs.gain = 0.011000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 15.000000f, 2.000000f });
                        rs.points.push_back({ 0.000000f, 37.500000f, 1.880000f });
                        rs.points.push_back({ 0.000000f, -32.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 10: hamilton_mausoleum
            {
                SpaceData sd;
                sd.id = "hamilton_mausoleum";
                sd.title = "Hamilton Mausoleum (Scotland)";
                sd.category = "Extreme Echo Spaces";
                sd.rt60 = 15.000000f;
                sd.volume = 7500.000000f;
                sd.area = 2200.000000f;
                sd.minBound = { -8.500000f, -8.500000f, 0.000000f };
                sd.maxBound = { 8.500000f, 8.500000f, 36.000000f };
                sd.wireframe.reserve(288);
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 7.850000f, 3.250000f, 0.000000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 0.000000f }, { 6.010000f, 6.010000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 0.000000f }, { 3.250000f, 7.850000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 0.000000f }, { 0.000000f, 8.500000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 0.000000f }, { -3.250000f, 7.850000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 0.000000f }, { -6.010000f, 6.010000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 0.000000f }, { -7.850000f, 3.250000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 0.000000f }, { -8.500000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { -7.850000f, -3.250000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 0.000000f }, { -6.010000f, -6.010000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 0.000000f }, { -3.250000f, -7.850000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 0.000000f }, { -0.000000f, -8.500000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 0.000000f }, { 3.250000f, -7.850000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 0.000000f }, { 6.010000f, -6.010000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 7.850000f, -3.250000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 0.000000f }, { 7.850000f, -3.250000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 7.850000f, -3.250000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 8.500000f, 0.000000f, 14.000000f } });
                sd.wireframe.push_back({ { 7.850000f, -3.250000f, 0.000000f }, { 8.500000f, 0.000000f, 14.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 0.000000f }, { 7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 0.000000f }, { 7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 14.000000f }, { 7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 0.000000f }, { 6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 0.000000f }, { 6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 14.000000f }, { 6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 0.000000f }, { 3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 0.000000f }, { 3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 14.000000f }, { 3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 0.000000f }, { 0.000000f, 8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 0.000000f }, { 0.000000f, 8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 14.000000f }, { 0.000000f, 8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 0.000000f }, { -3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 0.000000f }, { -3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 14.000000f }, { -3.250000f, 7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 0.000000f }, { -6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 0.000000f }, { -6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 14.000000f }, { -6.010000f, 6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 0.000000f }, { -7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 0.000000f }, { -7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 14.000000f }, { -7.850000f, 3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 0.000000f }, { -8.500000f, 0.000000f, 14.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { -8.500000f, 0.000000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 14.000000f }, { -8.500000f, 0.000000f, 14.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 0.000000f }, { -7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 0.000000f }, { -7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 14.000000f }, { -7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 0.000000f }, { -6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 0.000000f }, { -6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 14.000000f }, { -6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 0.000000f }, { -3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 0.000000f }, { -3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 14.000000f }, { -3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 0.000000f }, { -0.000000f, -8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 0.000000f }, { -0.000000f, -8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 14.000000f }, { -0.000000f, -8.500000f, 14.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 0.000000f }, { 3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 0.000000f }, { 3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 14.000000f }, { 3.250000f, -7.850000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 0.000000f }, { 6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 0.000000f }, { 6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 14.000000f }, { 6.010000f, -6.010000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 0.000000f }, { 7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 7.850000f, -3.250000f, 0.000000f }, { 7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 14.000000f }, { 7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 14.000000f }, { 7.850000f, -3.250000f, 14.000000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 14.000000f }, { 8.070000f, 0.000000f, 20.600000f } });
                sd.wireframe.push_back({ { 7.850000f, -3.250000f, 14.000000f }, { 8.070000f, 0.000000f, 20.600000f } });
                sd.wireframe.push_back({ { 8.500000f, 0.000000f, 14.000000f }, { 7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 14.000000f }, { 7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 8.070000f, 0.000000f, 20.600000f }, { 7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 7.850000f, 3.250000f, 14.000000f }, { 5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 14.000000f }, { 5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 7.460000f, 3.090000f, 20.600000f }, { 5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 6.010000f, 6.010000f, 14.000000f }, { 3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 14.000000f }, { 3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 5.710000f, 5.710000f, 20.600000f }, { 3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.250000f, 7.850000f, 14.000000f }, { 0.000000f, 8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 14.000000f }, { 0.000000f, 8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.090000f, 7.460000f, 20.600000f }, { 0.000000f, 8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.500000f, 14.000000f }, { -3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 14.000000f }, { -3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.070000f, 20.600000f }, { -3.090000f, 7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.250000f, 7.850000f, 14.000000f }, { -5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 14.000000f }, { -5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.090000f, 7.460000f, 20.600000f }, { -5.710000f, 5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -6.010000f, 6.010000f, 14.000000f }, { -7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 14.000000f }, { -7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -5.710000f, 5.710000f, 20.600000f }, { -7.460000f, 3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.850000f, 3.250000f, 14.000000f }, { -8.070000f, 0.000000f, 20.600000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 14.000000f }, { -8.070000f, 0.000000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.460000f, 3.090000f, 20.600000f }, { -8.070000f, 0.000000f, 20.600000f } });
                sd.wireframe.push_back({ { -8.500000f, 0.000000f, 14.000000f }, { -7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 14.000000f }, { -7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -8.070000f, 0.000000f, 20.600000f }, { -7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.850000f, -3.250000f, 14.000000f }, { -5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 14.000000f }, { -5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -7.460000f, -3.090000f, 20.600000f }, { -5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { -6.010000f, -6.010000f, 14.000000f }, { -3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 14.000000f }, { -3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -5.710000f, -5.710000f, 20.600000f }, { -3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.250000f, -7.850000f, 14.000000f }, { -0.000000f, -8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 14.000000f }, { -0.000000f, -8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { -3.090000f, -7.460000f, 20.600000f }, { -0.000000f, -8.070000f, 20.600000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.500000f, 14.000000f }, { 3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 14.000000f }, { 3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.070000f, 20.600000f }, { 3.090000f, -7.460000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.250000f, -7.850000f, 14.000000f }, { 5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 14.000000f }, { 5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 3.090000f, -7.460000f, 20.600000f }, { 5.710000f, -5.710000f, 20.600000f } });
                sd.wireframe.push_back({ { 6.010000f, -6.010000f, 14.000000f }, { 7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 7.850000f, -3.250000f, 14.000000f }, { 7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 8.070000f, 0.000000f, 20.600000f }, { 7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 5.710000f, -5.710000f, 20.600000f }, { 7.460000f, -3.090000f, 20.600000f } });
                sd.wireframe.push_back({ { 8.070000f, 0.000000f, 20.600000f }, { 6.800000f, 0.000000f, 27.200000f } });
                sd.wireframe.push_back({ { 7.460000f, -3.090000f, 20.600000f }, { 6.800000f, 0.000000f, 27.200000f } });
                sd.wireframe.push_back({ { 8.070000f, 0.000000f, 20.600000f }, { 6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 7.460000f, 3.090000f, 20.600000f }, { 6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 6.800000f, 0.000000f, 27.200000f }, { 6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 7.460000f, 3.090000f, 20.600000f }, { 4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 5.710000f, 5.710000f, 20.600000f }, { 4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 6.280000f, 2.600000f, 27.200000f }, { 4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 5.710000f, 5.710000f, 20.600000f }, { 2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 3.090000f, 7.460000f, 20.600000f }, { 2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 4.810000f, 4.810000f, 27.200000f }, { 2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 3.090000f, 7.460000f, 20.600000f }, { 0.000000f, 6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.070000f, 20.600000f }, { 0.000000f, 6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { 2.600000f, 6.280000f, 27.200000f }, { 0.000000f, 6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { 0.000000f, 8.070000f, 20.600000f }, { -2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -3.090000f, 7.460000f, 20.600000f }, { -2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 0.000000f, 6.800000f, 27.200000f }, { -2.600000f, 6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -3.090000f, 7.460000f, 20.600000f }, { -4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -5.710000f, 5.710000f, 20.600000f }, { -4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -2.600000f, 6.280000f, 27.200000f }, { -4.810000f, 4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -5.710000f, 5.710000f, 20.600000f }, { -6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -7.460000f, 3.090000f, 20.600000f }, { -6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -4.810000f, 4.810000f, 27.200000f }, { -6.280000f, 2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -7.460000f, 3.090000f, 20.600000f }, { -6.800000f, 0.000000f, 27.200000f } });
                sd.wireframe.push_back({ { -8.070000f, 0.000000f, 20.600000f }, { -6.800000f, 0.000000f, 27.200000f } });
                sd.wireframe.push_back({ { -6.280000f, 2.600000f, 27.200000f }, { -6.800000f, 0.000000f, 27.200000f } });
                sd.wireframe.push_back({ { -8.070000f, 0.000000f, 20.600000f }, { -6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -7.460000f, -3.090000f, 20.600000f }, { -6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -6.800000f, 0.000000f, 27.200000f }, { -6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { -7.460000f, -3.090000f, 20.600000f }, { -4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -5.710000f, -5.710000f, 20.600000f }, { -4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -6.280000f, -2.600000f, 27.200000f }, { -4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { -5.710000f, -5.710000f, 20.600000f }, { -2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -3.090000f, -7.460000f, 20.600000f }, { -2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -4.810000f, -4.810000f, 27.200000f }, { -2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -3.090000f, -7.460000f, 20.600000f }, { -0.000000f, -6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.070000f, 20.600000f }, { -0.000000f, -6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { -2.600000f, -6.280000f, 27.200000f }, { -0.000000f, -6.800000f, 27.200000f } });
                sd.wireframe.push_back({ { -0.000000f, -8.070000f, 20.600000f }, { 2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 3.090000f, -7.460000f, 20.600000f }, { 2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { -0.000000f, -6.800000f, 27.200000f }, { 2.600000f, -6.280000f, 27.200000f } });
                sd.wireframe.push_back({ { 3.090000f, -7.460000f, 20.600000f }, { 4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 5.710000f, -5.710000f, 20.600000f }, { 4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 2.600000f, -6.280000f, 27.200000f }, { 4.810000f, -4.810000f, 27.200000f } });
                sd.wireframe.push_back({ { 5.710000f, -5.710000f, 20.600000f }, { 6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 7.460000f, -3.090000f, 20.600000f }, { 6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 6.800000f, 0.000000f, 27.200000f }, { 6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 4.810000f, -4.810000f, 27.200000f }, { 6.280000f, -2.600000f, 27.200000f } });
                sd.wireframe.push_back({ { 6.800000f, 0.000000f, 27.200000f }, { 4.670000f, 0.000000f, 32.700000f } });
                sd.wireframe.push_back({ { 6.280000f, -2.600000f, 27.200000f }, { 4.670000f, 0.000000f, 32.700000f } });
                sd.wireframe.push_back({ { 6.800000f, 0.000000f, 27.200000f }, { 4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 6.280000f, 2.600000f, 27.200000f }, { 4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.670000f, 0.000000f, 32.700000f }, { 4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 6.280000f, 2.600000f, 27.200000f }, { 3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.810000f, 4.810000f, 27.200000f }, { 3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.320000f, 1.790000f, 32.700000f }, { 3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.810000f, 4.810000f, 27.200000f }, { 1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 2.600000f, 6.280000f, 27.200000f }, { 1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 3.310000f, 3.310000f, 32.700000f }, { 1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 2.600000f, 6.280000f, 27.200000f }, { 0.000000f, 4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { 0.000000f, 6.800000f, 27.200000f }, { 0.000000f, 4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { 1.790000f, 4.320000f, 32.700000f }, { 0.000000f, 4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { 0.000000f, 6.800000f, 27.200000f }, { -1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -2.600000f, 6.280000f, 27.200000f }, { -1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 0.000000f, 4.670000f, 32.700000f }, { -1.790000f, 4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -2.600000f, 6.280000f, 27.200000f }, { -3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.810000f, 4.810000f, 27.200000f }, { -3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -1.790000f, 4.320000f, 32.700000f }, { -3.310000f, 3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.810000f, 4.810000f, 27.200000f }, { -4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.280000f, 2.600000f, 27.200000f }, { -4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -3.310000f, 3.310000f, 32.700000f }, { -4.320000f, 1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.280000f, 2.600000f, 27.200000f }, { -4.670000f, 0.000000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.800000f, 0.000000f, 27.200000f }, { -4.670000f, 0.000000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.320000f, 1.790000f, 32.700000f }, { -4.670000f, 0.000000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.800000f, 0.000000f, 27.200000f }, { -4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.280000f, -2.600000f, 27.200000f }, { -4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.670000f, 0.000000f, 32.700000f }, { -4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { -6.280000f, -2.600000f, 27.200000f }, { -3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.810000f, -4.810000f, 27.200000f }, { -3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.320000f, -1.790000f, 32.700000f }, { -3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { -4.810000f, -4.810000f, 27.200000f }, { -1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -2.600000f, -6.280000f, 27.200000f }, { -1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -3.310000f, -3.310000f, 32.700000f }, { -1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -2.600000f, -6.280000f, 27.200000f }, { -0.000000f, -4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { -0.000000f, -6.800000f, 27.200000f }, { -0.000000f, -4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { -1.790000f, -4.320000f, 32.700000f }, { -0.000000f, -4.670000f, 32.700000f } });
                sd.wireframe.push_back({ { -0.000000f, -6.800000f, 27.200000f }, { 1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 2.600000f, -6.280000f, 27.200000f }, { 1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { -0.000000f, -4.670000f, 32.700000f }, { 1.790000f, -4.320000f, 32.700000f } });
                sd.wireframe.push_back({ { 2.600000f, -6.280000f, 27.200000f }, { 3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.810000f, -4.810000f, 27.200000f }, { 3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 1.790000f, -4.320000f, 32.700000f }, { 3.310000f, -3.310000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.810000f, -4.810000f, 27.200000f }, { 4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 6.280000f, -2.600000f, 27.200000f }, { 4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.670000f, 0.000000f, 32.700000f }, { 4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 3.310000f, -3.310000f, 32.700000f }, { 4.320000f, -1.790000f, 32.700000f } });
                sd.wireframe.push_back({ { 4.670000f, 0.000000f, 32.700000f }, { 2.120000f, 0.000000f, 35.340000f } });
                sd.wireframe.push_back({ { 4.320000f, -1.790000f, 32.700000f }, { 2.120000f, 0.000000f, 35.340000f } });
                sd.wireframe.push_back({ { 4.670000f, 0.000000f, 32.700000f }, { 1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 4.320000f, 1.790000f, 32.700000f }, { 1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 2.120000f, 0.000000f, 35.340000f }, { 1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 4.320000f, 1.790000f, 32.700000f }, { 1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 3.310000f, 3.310000f, 32.700000f }, { 1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.960000f, 0.810000f, 35.340000f }, { 1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 3.310000f, 3.310000f, 32.700000f }, { 0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.790000f, 4.320000f, 32.700000f }, { 0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.500000f, 1.500000f, 35.340000f }, { 0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.790000f, 4.320000f, 32.700000f }, { 0.000000f, 2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { 0.000000f, 4.670000f, 32.700000f }, { 0.000000f, 2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { 0.810000f, 1.960000f, 35.340000f }, { 0.000000f, 2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { 0.000000f, 4.670000f, 32.700000f }, { -0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.790000f, 4.320000f, 32.700000f }, { -0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 0.000000f, 2.120000f, 35.340000f }, { -0.810000f, 1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.790000f, 4.320000f, 32.700000f }, { -1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -3.310000f, 3.310000f, 32.700000f }, { -1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -0.810000f, 1.960000f, 35.340000f }, { -1.500000f, 1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -3.310000f, 3.310000f, 32.700000f }, { -1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.320000f, 1.790000f, 32.700000f }, { -1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.500000f, 1.500000f, 35.340000f }, { -1.960000f, 0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.320000f, 1.790000f, 32.700000f }, { -2.120000f, 0.000000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.670000f, 0.000000f, 32.700000f }, { -2.120000f, 0.000000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.960000f, 0.810000f, 35.340000f }, { -2.120000f, 0.000000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.670000f, 0.000000f, 32.700000f }, { -1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.320000f, -1.790000f, 32.700000f }, { -1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -2.120000f, 0.000000f, 35.340000f }, { -1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { -4.320000f, -1.790000f, 32.700000f }, { -1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -3.310000f, -3.310000f, 32.700000f }, { -1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.960000f, -0.810000f, 35.340000f }, { -1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { -3.310000f, -3.310000f, 32.700000f }, { -0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.790000f, -4.320000f, 32.700000f }, { -0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.500000f, -1.500000f, 35.340000f }, { -0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -1.790000f, -4.320000f, 32.700000f }, { -0.000000f, -2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { -0.000000f, -4.670000f, 32.700000f }, { -0.000000f, -2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { -0.810000f, -1.960000f, 35.340000f }, { -0.000000f, -2.120000f, 35.340000f } });
                sd.wireframe.push_back({ { -0.000000f, -4.670000f, 32.700000f }, { 0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.790000f, -4.320000f, 32.700000f }, { 0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { -0.000000f, -2.120000f, 35.340000f }, { 0.810000f, -1.960000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.790000f, -4.320000f, 32.700000f }, { 1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 3.310000f, -3.310000f, 32.700000f }, { 1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 0.810000f, -1.960000f, 35.340000f }, { 1.500000f, -1.500000f, 35.340000f } });
                sd.wireframe.push_back({ { 3.310000f, -3.310000f, 32.700000f }, { 1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 4.320000f, -1.790000f, 32.700000f }, { 1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 2.120000f, 0.000000f, 35.340000f }, { 1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 1.500000f, -1.500000f, 35.340000f }, { 1.960000f, -0.810000f, 35.340000f } });
                sd.wireframe.push_back({ { 2.120000f, 0.000000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 1.960000f, 0.810000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 1.500000f, 1.500000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 0.810000f, 1.960000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 0.000000f, 2.120000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -0.810000f, 1.960000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -1.500000f, 1.500000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -1.960000f, 0.810000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -2.120000f, 0.000000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -1.960000f, -0.810000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -1.500000f, -1.500000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -0.810000f, -1.960000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { -0.000000f, -2.120000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 0.810000f, -1.960000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 1.500000f, -1.500000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.wireframe.push_back({ { 1.960000f, -0.810000f, 35.340000f }, { 0.000000f, 0.000000f, 36.000000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Central Bronze Door Slam Echo";
                    pd.source = { 0.000000f, -7.500000f, 1.500000f };
                    pd.listener = { 0.000000f, 0.000000f, 1.500000f };
                    pd.directDistance = 7.500000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 7.500000f;
                        rs.order = 0;
                        rs.gain = 0.133000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 8.080000f;
                        rs.order = 1;
                        rs.gain = 0.123000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, -3.750000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 8.080000f;
                        rs.order = 1;
                        rs.gain = 0.123000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, -3.750000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 9.430000f;
                        rs.order = 1;
                        rs.gain = 0.105000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 0.340000f, -8.430000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 9.430000f;
                        rs.order = 1;
                        rs.gain = 0.105000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ -0.340000f, -8.430000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.290000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 5.890000f, 6.090000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 23.290000f;
                        rs.order = 1;
                        rs.gain = 0.043000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ -5.890000f, 6.090000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.070000f;
                        rs.order = 1;
                        rs.gain = 0.041000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, -7.500000f, 1.500000f });
                        rs.points.push_back({ 2.120000f, 8.080000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Curved Perimeter Whispering Wall";
                    pd.source = { 7.500000f, 0.000000f, 1.500000f };
                    pd.listener = { -7.500000f, 0.000000f, 1.500000f };
                    pd.directDistance = 15.000000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.000000f;
                        rs.order = 0;
                        rs.gain = 0.067000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.300000f;
                        rs.order = 1;
                        rs.gain = 0.065000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 7.500000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 0.000000f, 0.000000f });
                        rs.points.push_back({ -7.500000f, 0.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Duke Sarcophagus Pedestal Floor";
                    pd.source = { 0.000000f, 0.000000f, 1.800000f };
                    pd.listener = { 4.000000f, 4.000000f, 1.500000f };
                    pd.directDistance = 5.660000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 5.660000f;
                        rs.order = 0;
                        rs.gain = 0.177000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.550000f;
                        rs.order = 1;
                        rs.gain = 0.152000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 2.180000f, 2.180000f, 0.000000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 6.550000f;
                        rs.order = 1;
                        rs.gain = 0.152000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 2.180000f, 2.180000f, 0.000000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 11.180000f;
                        rs.order = 1;
                        rs.gain = 0.089000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 5.320000f, 6.470000f, 1.580000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 11.180000f;
                        rs.order = 1;
                        rs.gain = 0.089000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 6.470000f, 5.320000f, 1.580000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 21.610000f;
                        rs.order = 1;
                        rs.gain = 0.046000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -0.420000f, -8.420000f, 1.680000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 21.610000f;
                        rs.order = 1;
                        rs.gain = 0.046000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -8.420000f, -0.420000f, 1.680000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.250000f;
                        rs.order = 1;
                        rs.gain = 0.045000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -7.160000f, -4.290000f, 1.690000f });
                        rs.points.push_back({ 4.000000f, 4.000000f, 1.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Upper Rotunda High Cornice Elevation";
                    pd.source = { 0.000000f, 0.000000f, 1.800000f };
                    pd.listener = { 0.000000f, 6.000000f, 13.500000f };
                    pd.directDistance = 13.150000f;
                    pd.rays.reserve(8);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 13.150000f;
                        rs.order = 0;
                        rs.gain = 0.076000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.960000f;
                        rs.order = 1;
                        rs.gain = 0.062000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 0.740000f, 8.350000f, 10.840000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.960000f;
                        rs.order = 1;
                        rs.gain = 0.062000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 0.740000f, 8.350000f, 10.840000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.960000f;
                        rs.order = 1;
                        rs.gain = 0.062000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -0.740000f, 8.350000f, 10.840000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.430000f;
                        rs.order = 1;
                        rs.gain = 0.060000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.710000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.430000f;
                        rs.order = 1;
                        rs.gain = 0.060000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 0.000000f, 0.710000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.840000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ 5.700000f, -6.220000f, 6.300000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 24.840000f;
                        rs.order = 1;
                        rs.gain = 0.040000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.800000f });
                        rs.points.push_back({ -5.700000f, -6.220000f, 6.300000f });
                        rs.points.push_back({ 0.000000f, 6.000000f, 13.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            // Space 11: epidaurus_amphitheatre
            {
                SpaceData sd;
                sd.id = "epidaurus_amphitheatre";
                sd.title = "Ancient Theatre of Epidaurus (Greece)";
                sd.category = "World Landmarks";
                sd.rt60 = 1.250000f;
                sd.volume = 45000.000000f;
                sd.area = 8500.000000f;
                sd.minBound = { -57.940000f, -15.010000f, 0.000000f };
                sd.maxBound = { 57.940000f, 57.660000f, 23.410000f };
                sd.wireframe.reserve(404);
                sd.wireframe.push_back({ { -9.660000f, -2.590000f, 0.000000f }, { -9.990000f, -0.460000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.990000f, -0.460000f, 0.000000f }, { -9.860000f, 1.690000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.860000f, 1.690000f, 0.000000f }, { -9.270000f, 3.760000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.270000f, 3.760000f, 0.000000f }, { -8.250000f, 5.650000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.250000f, 5.650000f, 0.000000f }, { -6.850000f, 7.290000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.850000f, 7.290000f, 0.000000f }, { -5.130000f, 8.580000f, 0.000000f } });
                sd.wireframe.push_back({ { -5.130000f, 8.580000f, 0.000000f }, { -3.180000f, 9.480000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.180000f, 9.480000f, 0.000000f }, { -1.080000f, 9.940000f, 0.000000f } });
                sd.wireframe.push_back({ { -1.080000f, 9.940000f, 0.000000f }, { 1.080000f, 9.940000f, 0.000000f } });
                sd.wireframe.push_back({ { 1.080000f, 9.940000f, 0.000000f }, { 3.180000f, 9.480000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.180000f, 9.480000f, 0.000000f }, { 5.130000f, 8.580000f, 0.000000f } });
                sd.wireframe.push_back({ { 5.130000f, 8.580000f, 0.000000f }, { 6.850000f, 7.290000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.850000f, 7.290000f, 0.000000f }, { 8.250000f, 5.650000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.250000f, 5.650000f, 0.000000f }, { 9.270000f, 3.760000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.270000f, 3.760000f, 0.000000f }, { 9.860000f, 1.690000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.860000f, 1.690000f, 0.000000f }, { 9.990000f, -0.460000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.990000f, -0.460000f, 0.000000f }, { 9.660000f, -2.590000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.660000f, -2.590000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.990000f, -0.460000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.860000f, 1.690000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -9.270000f, 3.760000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -8.250000f, 5.650000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -6.850000f, 7.290000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -5.130000f, 8.580000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -3.180000f, 9.480000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -1.080000f, 9.940000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 1.080000f, 9.940000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 3.180000f, 9.480000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 5.130000f, 8.580000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 6.850000f, 7.290000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 8.250000f, 5.650000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.270000f, 3.760000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.860000f, 1.690000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.990000f, -0.460000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { 9.660000f, -2.590000f, 0.000000f }, { 0.000000f, 0.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -13.000000f, -4.000000f, 0.000000f }, { 13.000000f, -4.000000f, 0.000000f } });
                sd.wireframe.push_back({ { -13.000000f, -4.000000f, 0.000000f }, { 13.000000f, -4.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 13.000000f, -4.000000f, 0.000000f }, { 13.000000f, -4.000000f, 5.500000f } });
                sd.wireframe.push_back({ { -13.000000f, -4.000000f, 0.000000f }, { -13.000000f, -4.000000f, 5.500000f } });
                sd.wireframe.push_back({ { 13.000000f, -4.000000f, 5.500000f }, { -13.000000f, -4.000000f, 5.500000f } });
                sd.wireframe.push_back({ { -9.660000f, -2.590000f, 0.000000f }, { -16.280000f, -4.360000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.660000f, -2.590000f, 0.000000f }, { -16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.990000f, -0.460000f, 0.000000f }, { -16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { -16.280000f, -4.360000f, 3.340000f }, { -16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.990000f, -0.460000f, 0.000000f }, { -16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.860000f, 1.690000f, 0.000000f }, { -16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { -16.840000f, -0.780000f, 3.340000f }, { -16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.860000f, 1.690000f, 0.000000f }, { -15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.270000f, 3.760000f, 0.000000f }, { -15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { -16.620000f, 2.840000f, 3.340000f }, { -15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { -9.270000f, 3.760000f, 0.000000f }, { -13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { -8.250000f, 5.650000f, 0.000000f }, { -13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { -15.620000f, 6.330000f, 3.340000f }, { -13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { -8.250000f, 5.650000f, 0.000000f }, { -11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { -6.850000f, 7.290000f, 0.000000f }, { -11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { -13.910000f, 9.530000f, 3.340000f }, { -11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { -6.850000f, 7.290000f, 0.000000f }, { -8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { -5.130000f, 8.580000f, 0.000000f }, { -8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { -11.550000f, 12.280000f, 3.340000f }, { -8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { -5.130000f, 8.580000f, 0.000000f }, { -5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { -3.180000f, 9.480000f, 0.000000f }, { -5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { -8.650000f, 14.470000f, 3.340000f }, { -5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { -3.180000f, 9.480000f, 0.000000f }, { -1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { -1.080000f, 9.940000f, 0.000000f }, { -1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { -5.360000f, 15.980000f, 3.340000f }, { -1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { -1.080000f, 9.940000f, 0.000000f }, { 1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { 1.080000f, 9.940000f, 0.000000f }, { 1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { -1.810000f, 16.760000f, 3.340000f }, { 1.810000f, 16.760000f, 3.340000f } });
                sd.wireframe.push_back({ { 1.080000f, 9.940000f, 0.000000f }, { 5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { 3.180000f, 9.480000f, 0.000000f }, { 5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { 1.810000f, 16.760000f, 3.340000f }, { 5.360000f, 15.980000f, 3.340000f } });
                sd.wireframe.push_back({ { 3.180000f, 9.480000f, 0.000000f }, { 8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { 5.130000f, 8.580000f, 0.000000f }, { 8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { 5.360000f, 15.980000f, 3.340000f }, { 8.650000f, 14.470000f, 3.340000f } });
                sd.wireframe.push_back({ { 5.130000f, 8.580000f, 0.000000f }, { 11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { 6.850000f, 7.290000f, 0.000000f }, { 11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { 8.650000f, 14.470000f, 3.340000f }, { 11.550000f, 12.280000f, 3.340000f } });
                sd.wireframe.push_back({ { 6.850000f, 7.290000f, 0.000000f }, { 13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { 8.250000f, 5.650000f, 0.000000f }, { 13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { 11.550000f, 12.280000f, 3.340000f }, { 13.910000f, 9.530000f, 3.340000f } });
                sd.wireframe.push_back({ { 8.250000f, 5.650000f, 0.000000f }, { 15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.270000f, 3.760000f, 0.000000f }, { 15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { 13.910000f, 9.530000f, 3.340000f }, { 15.620000f, 6.330000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.270000f, 3.760000f, 0.000000f }, { 16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.860000f, 1.690000f, 0.000000f }, { 16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { 15.620000f, 6.330000f, 3.340000f }, { 16.620000f, 2.840000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.860000f, 1.690000f, 0.000000f }, { 16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.990000f, -0.460000f, 0.000000f }, { 16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { 16.620000f, 2.840000f, 3.340000f }, { 16.840000f, -0.780000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.990000f, -0.460000f, 0.000000f }, { 16.280000f, -4.360000f, 3.340000f } });
                sd.wireframe.push_back({ { 9.660000f, -2.590000f, 0.000000f }, { 16.280000f, -4.360000f, 3.340000f } });
                sd.wireframe.push_back({ { 16.840000f, -0.780000f, 3.340000f }, { 16.280000f, -4.360000f, 3.340000f } });
                sd.wireframe.push_back({ { -16.280000f, -4.360000f, 3.340000f }, { -22.910000f, -6.140000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.280000f, -4.360000f, 3.340000f }, { -23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.840000f, -0.780000f, 3.340000f }, { -23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { -22.910000f, -6.140000f, 6.690000f }, { -23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.840000f, -0.780000f, 3.340000f }, { -23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.620000f, 2.840000f, 3.340000f }, { -23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { -23.690000f, -1.100000f, 6.690000f }, { -23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.620000f, 2.840000f, 3.340000f }, { -21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { -15.620000f, 6.330000f, 3.340000f }, { -21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { -23.370000f, 4.000000f, 6.690000f }, { -21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { -15.620000f, 6.330000f, 3.340000f }, { -19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { -13.910000f, 9.530000f, 3.340000f }, { -19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { -21.980000f, 8.910000f, 6.690000f }, { -19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { -13.910000f, 9.530000f, 3.340000f }, { -16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { -11.550000f, 12.280000f, 3.340000f }, { -16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { -19.560000f, 13.400000f, 6.690000f }, { -16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { -11.550000f, 12.280000f, 3.340000f }, { -12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { -8.650000f, 14.470000f, 3.340000f }, { -12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { -16.240000f, 17.280000f, 6.690000f }, { -12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { -8.650000f, 14.470000f, 3.340000f }, { -7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { -5.360000f, 15.980000f, 3.340000f }, { -7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { -12.170000f, 20.350000f, 6.690000f }, { -7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { -5.360000f, 15.980000f, 3.340000f }, { -2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { -1.810000f, 16.760000f, 3.340000f }, { -2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { -7.540000f, 22.480000f, 6.690000f }, { -2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { -1.810000f, 16.760000f, 3.340000f }, { 2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { 1.810000f, 16.760000f, 3.340000f }, { 2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { -2.550000f, 23.580000f, 6.690000f }, { 2.550000f, 23.580000f, 6.690000f } });
                sd.wireframe.push_back({ { 1.810000f, 16.760000f, 3.340000f }, { 7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { 5.360000f, 15.980000f, 3.340000f }, { 7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { 2.550000f, 23.580000f, 6.690000f }, { 7.540000f, 22.480000f, 6.690000f } });
                sd.wireframe.push_back({ { 5.360000f, 15.980000f, 3.340000f }, { 12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { 8.650000f, 14.470000f, 3.340000f }, { 12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { 7.540000f, 22.480000f, 6.690000f }, { 12.170000f, 20.350000f, 6.690000f } });
                sd.wireframe.push_back({ { 8.650000f, 14.470000f, 3.340000f }, { 16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { 11.550000f, 12.280000f, 3.340000f }, { 16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { 12.170000f, 20.350000f, 6.690000f }, { 16.240000f, 17.280000f, 6.690000f } });
                sd.wireframe.push_back({ { 11.550000f, 12.280000f, 3.340000f }, { 19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { 13.910000f, 9.530000f, 3.340000f }, { 19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.240000f, 17.280000f, 6.690000f }, { 19.560000f, 13.400000f, 6.690000f } });
                sd.wireframe.push_back({ { 13.910000f, 9.530000f, 3.340000f }, { 21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { 15.620000f, 6.330000f, 3.340000f }, { 21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { 19.560000f, 13.400000f, 6.690000f }, { 21.980000f, 8.910000f, 6.690000f } });
                sd.wireframe.push_back({ { 15.620000f, 6.330000f, 3.340000f }, { 23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.620000f, 2.840000f, 3.340000f }, { 23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { 21.980000f, 8.910000f, 6.690000f }, { 23.370000f, 4.000000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.620000f, 2.840000f, 3.340000f }, { 23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.840000f, -0.780000f, 3.340000f }, { 23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { 23.370000f, 4.000000f, 6.690000f }, { 23.690000f, -1.100000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.840000f, -0.780000f, 3.340000f }, { 22.910000f, -6.140000f, 6.690000f } });
                sd.wireframe.push_back({ { 16.280000f, -4.360000f, 3.340000f }, { 22.910000f, -6.140000f, 6.690000f } });
                sd.wireframe.push_back({ { 23.690000f, -1.100000f, 6.690000f }, { 22.910000f, -6.140000f, 6.690000f } });
                sd.wireframe.push_back({ { -22.910000f, -6.140000f, 6.690000f }, { -29.530000f, -7.910000f, 10.030000f } });
                sd.wireframe.push_back({ { -22.910000f, -6.140000f, 6.690000f }, { -30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { -23.690000f, -1.100000f, 6.690000f }, { -30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { -29.530000f, -7.910000f, 10.030000f }, { -30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { -23.690000f, -1.100000f, 6.690000f }, { -30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { -23.370000f, 4.000000f, 6.690000f }, { -30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { -30.540000f, -1.410000f, 10.030000f }, { -30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { -23.370000f, 4.000000f, 6.690000f }, { -28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { -21.980000f, 8.910000f, 6.690000f }, { -28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { -30.130000f, 5.150000f, 10.030000f }, { -28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { -21.980000f, 8.910000f, 6.690000f }, { -25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { -19.560000f, 13.400000f, 6.690000f }, { -25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { -28.330000f, 11.480000f, 10.030000f }, { -25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { -19.560000f, 13.400000f, 6.690000f }, { -20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { -16.240000f, 17.280000f, 6.690000f }, { -20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { -25.220000f, 17.280000f, 10.030000f }, { -20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { -16.240000f, 17.280000f, 6.690000f }, { -15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { -12.170000f, 20.350000f, 6.690000f }, { -15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { -20.940000f, 22.270000f, 10.030000f }, { -15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { -12.170000f, 20.350000f, 6.690000f }, { -9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { -7.540000f, 22.480000f, 6.690000f }, { -9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { -15.690000f, 26.240000f, 10.030000f }, { -9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { -7.540000f, 22.480000f, 6.690000f }, { -3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { -2.550000f, 23.580000f, 6.690000f }, { -3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { -9.720000f, 28.990000f, 10.030000f }, { -3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { -2.550000f, 23.580000f, 6.690000f }, { 3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { 2.550000f, 23.580000f, 6.690000f }, { 3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { -3.290000f, 30.390000f, 10.030000f }, { 3.290000f, 30.390000f, 10.030000f } });
                sd.wireframe.push_back({ { 2.550000f, 23.580000f, 6.690000f }, { 9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { 7.540000f, 22.480000f, 6.690000f }, { 9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { 3.290000f, 30.390000f, 10.030000f }, { 9.720000f, 28.990000f, 10.030000f } });
                sd.wireframe.push_back({ { 7.540000f, 22.480000f, 6.690000f }, { 15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { 12.170000f, 20.350000f, 6.690000f }, { 15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { 9.720000f, 28.990000f, 10.030000f }, { 15.690000f, 26.240000f, 10.030000f } });
                sd.wireframe.push_back({ { 12.170000f, 20.350000f, 6.690000f }, { 20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { 16.240000f, 17.280000f, 6.690000f }, { 20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { 15.690000f, 26.240000f, 10.030000f }, { 20.940000f, 22.270000f, 10.030000f } });
                sd.wireframe.push_back({ { 16.240000f, 17.280000f, 6.690000f }, { 25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { 19.560000f, 13.400000f, 6.690000f }, { 25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { 20.940000f, 22.270000f, 10.030000f }, { 25.220000f, 17.280000f, 10.030000f } });
                sd.wireframe.push_back({ { 19.560000f, 13.400000f, 6.690000f }, { 28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { 21.980000f, 8.910000f, 6.690000f }, { 28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { 25.220000f, 17.280000f, 10.030000f }, { 28.330000f, 11.480000f, 10.030000f } });
                sd.wireframe.push_back({ { 21.980000f, 8.910000f, 6.690000f }, { 30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { 23.370000f, 4.000000f, 6.690000f }, { 30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { 28.330000f, 11.480000f, 10.030000f }, { 30.130000f, 5.150000f, 10.030000f } });
                sd.wireframe.push_back({ { 23.370000f, 4.000000f, 6.690000f }, { 30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { 23.690000f, -1.100000f, 6.690000f }, { 30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { 30.130000f, 5.150000f, 10.030000f }, { 30.540000f, -1.410000f, 10.030000f } });
                sd.wireframe.push_back({ { 23.690000f, -1.100000f, 6.690000f }, { 29.530000f, -7.910000f, 10.030000f } });
                sd.wireframe.push_back({ { 22.910000f, -6.140000f, 6.690000f }, { 29.530000f, -7.910000f, 10.030000f } });
                sd.wireframe.push_back({ { 30.540000f, -1.410000f, 10.030000f }, { 29.530000f, -7.910000f, 10.030000f } });
                sd.wireframe.push_back({ { -29.530000f, -7.910000f, 10.030000f }, { -36.150000f, -9.690000f, 13.380000f } });
                sd.wireframe.push_back({ { -29.530000f, -7.910000f, 10.030000f }, { -37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { -30.540000f, -1.410000f, 10.030000f }, { -37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { -36.150000f, -9.690000f, 13.380000f }, { -37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { -30.540000f, -1.410000f, 10.030000f }, { -36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { -30.130000f, 5.150000f, 10.030000f }, { -36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { -37.390000f, -1.730000f, 13.380000f }, { -36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { -30.130000f, 5.150000f, 10.030000f }, { -34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { -28.330000f, 11.480000f, 10.030000f }, { -34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { -36.890000f, 6.310000f, 13.380000f }, { -34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { -28.330000f, 11.480000f, 10.030000f }, { -30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { -25.220000f, 17.280000f, 10.030000f }, { -30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { -34.690000f, 14.060000f, 13.380000f }, { -30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { -25.220000f, 17.280000f, 10.030000f }, { -25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { -20.940000f, 22.270000f, 10.030000f }, { -25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { -30.880000f, 21.150000f, 13.380000f }, { -25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { -20.940000f, 22.270000f, 10.030000f }, { -19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { -15.690000f, 26.240000f, 10.030000f }, { -19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { -25.640000f, 27.270000f, 13.380000f }, { -19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { -15.690000f, 26.240000f, 10.030000f }, { -11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { -9.720000f, 28.990000f, 10.030000f }, { -11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { -19.210000f, 32.120000f, 13.380000f }, { -11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { -9.720000f, 28.990000f, 10.030000f }, { -4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { -3.290000f, 30.390000f, 10.030000f }, { -4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { -11.890000f, 35.490000f, 13.380000f }, { -4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { -3.290000f, 30.390000f, 10.030000f }, { 4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { 3.290000f, 30.390000f, 10.030000f }, { 4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { -4.030000f, 37.210000f, 13.380000f }, { 4.030000f, 37.210000f, 13.380000f } });
                sd.wireframe.push_back({ { 3.290000f, 30.390000f, 10.030000f }, { 11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { 9.720000f, 28.990000f, 10.030000f }, { 11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { 4.030000f, 37.210000f, 13.380000f }, { 11.890000f, 35.490000f, 13.380000f } });
                sd.wireframe.push_back({ { 9.720000f, 28.990000f, 10.030000f }, { 19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { 15.690000f, 26.240000f, 10.030000f }, { 19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { 11.890000f, 35.490000f, 13.380000f }, { 19.210000f, 32.120000f, 13.380000f } });
                sd.wireframe.push_back({ { 15.690000f, 26.240000f, 10.030000f }, { 25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { 20.940000f, 22.270000f, 10.030000f }, { 25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { 19.210000f, 32.120000f, 13.380000f }, { 25.640000f, 27.270000f, 13.380000f } });
                sd.wireframe.push_back({ { 20.940000f, 22.270000f, 10.030000f }, { 30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { 25.220000f, 17.280000f, 10.030000f }, { 30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { 25.640000f, 27.270000f, 13.380000f }, { 30.880000f, 21.150000f, 13.380000f } });
                sd.wireframe.push_back({ { 25.220000f, 17.280000f, 10.030000f }, { 34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { 28.330000f, 11.480000f, 10.030000f }, { 34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { 30.880000f, 21.150000f, 13.380000f }, { 34.690000f, 14.060000f, 13.380000f } });
                sd.wireframe.push_back({ { 28.330000f, 11.480000f, 10.030000f }, { 36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { 30.130000f, 5.150000f, 10.030000f }, { 36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { 34.690000f, 14.060000f, 13.380000f }, { 36.890000f, 6.310000f, 13.380000f } });
                sd.wireframe.push_back({ { 30.130000f, 5.150000f, 10.030000f }, { 37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { 30.540000f, -1.410000f, 10.030000f }, { 37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { 36.890000f, 6.310000f, 13.380000f }, { 37.390000f, -1.730000f, 13.380000f } });
                sd.wireframe.push_back({ { 30.540000f, -1.410000f, 10.030000f }, { 36.150000f, -9.690000f, 13.380000f } });
                sd.wireframe.push_back({ { 29.530000f, -7.910000f, 10.030000f }, { 36.150000f, -9.690000f, 13.380000f } });
                sd.wireframe.push_back({ { 37.390000f, -1.730000f, 13.380000f }, { 36.150000f, -9.690000f, 13.380000f } });
                sd.wireframe.push_back({ { -36.150000f, -9.690000f, 13.380000f }, { -42.780000f, -11.460000f, 16.720000f } });
                sd.wireframe.push_back({ { -36.150000f, -9.690000f, 13.380000f }, { -44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { -37.390000f, -1.730000f, 13.380000f }, { -44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { -42.780000f, -11.460000f, 16.720000f }, { -44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { -37.390000f, -1.730000f, 13.380000f }, { -43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { -36.890000f, 6.310000f, 13.380000f }, { -43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { -44.240000f, -2.050000f, 16.720000f }, { -43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { -36.890000f, 6.310000f, 13.380000f }, { -41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { -34.690000f, 14.060000f, 13.380000f }, { -41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { -43.650000f, 7.470000f, 16.720000f }, { -41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { -34.690000f, 14.060000f, 13.380000f }, { -36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -30.880000f, 21.150000f, 13.380000f }, { -36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -41.040000f, 16.630000f, 16.720000f }, { -36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -30.880000f, 21.150000f, 13.380000f }, { -30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { -25.640000f, 27.270000f, 13.380000f }, { -30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { -36.540000f, 25.030000f, 16.720000f }, { -30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { -25.640000f, 27.270000f, 13.380000f }, { -22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { -19.210000f, 32.120000f, 13.380000f }, { -22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { -30.340000f, 32.260000f, 16.720000f }, { -22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { -19.210000f, 32.120000f, 13.380000f }, { -14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { -11.890000f, 35.490000f, 13.380000f }, { -14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { -22.730000f, 38.010000f, 16.720000f }, { -14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { -11.890000f, 35.490000f, 13.380000f }, { -4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -4.030000f, 37.210000f, 13.380000f }, { -4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -14.070000f, 41.990000f, 16.720000f }, { -4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -4.030000f, 37.210000f, 13.380000f }, { 4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { 4.030000f, 37.210000f, 13.380000f }, { 4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { -4.760000f, 44.030000f, 16.720000f }, { 4.760000f, 44.030000f, 16.720000f } });
                sd.wireframe.push_back({ { 4.030000f, 37.210000f, 13.380000f }, { 14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { 11.890000f, 35.490000f, 13.380000f }, { 14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { 4.760000f, 44.030000f, 16.720000f }, { 14.070000f, 41.990000f, 16.720000f } });
                sd.wireframe.push_back({ { 11.890000f, 35.490000f, 13.380000f }, { 22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { 19.210000f, 32.120000f, 13.380000f }, { 22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { 14.070000f, 41.990000f, 16.720000f }, { 22.730000f, 38.010000f, 16.720000f } });
                sd.wireframe.push_back({ { 19.210000f, 32.120000f, 13.380000f }, { 30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { 25.640000f, 27.270000f, 13.380000f }, { 30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { 22.730000f, 38.010000f, 16.720000f }, { 30.340000f, 32.260000f, 16.720000f } });
                sd.wireframe.push_back({ { 25.640000f, 27.270000f, 13.380000f }, { 36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { 30.880000f, 21.150000f, 13.380000f }, { 36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { 30.340000f, 32.260000f, 16.720000f }, { 36.540000f, 25.030000f, 16.720000f } });
                sd.wireframe.push_back({ { 30.880000f, 21.150000f, 13.380000f }, { 41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { 34.690000f, 14.060000f, 13.380000f }, { 41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { 36.540000f, 25.030000f, 16.720000f }, { 41.040000f, 16.630000f, 16.720000f } });
                sd.wireframe.push_back({ { 34.690000f, 14.060000f, 13.380000f }, { 43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { 36.890000f, 6.310000f, 13.380000f }, { 43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { 41.040000f, 16.630000f, 16.720000f }, { 43.650000f, 7.470000f, 16.720000f } });
                sd.wireframe.push_back({ { 36.890000f, 6.310000f, 13.380000f }, { 44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { 37.390000f, -1.730000f, 13.380000f }, { 44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { 43.650000f, 7.470000f, 16.720000f }, { 44.240000f, -2.050000f, 16.720000f } });
                sd.wireframe.push_back({ { 37.390000f, -1.730000f, 13.380000f }, { 42.780000f, -11.460000f, 16.720000f } });
                sd.wireframe.push_back({ { 36.150000f, -9.690000f, 13.380000f }, { 42.780000f, -11.460000f, 16.720000f } });
                sd.wireframe.push_back({ { 44.240000f, -2.050000f, 16.720000f }, { 42.780000f, -11.460000f, 16.720000f } });
                sd.wireframe.push_back({ { -42.780000f, -11.460000f, 16.720000f }, { -49.400000f, -13.240000f, 20.070000f } });
                sd.wireframe.push_back({ { -42.780000f, -11.460000f, 16.720000f }, { -51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { -44.240000f, -2.050000f, 16.720000f }, { -51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { -49.400000f, -13.240000f, 20.070000f }, { -51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { -44.240000f, -2.050000f, 16.720000f }, { -50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { -43.650000f, 7.470000f, 16.720000f }, { -50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { -51.090000f, -2.360000f, 20.070000f }, { -50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { -43.650000f, 7.470000f, 16.720000f }, { -47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { -41.040000f, 16.630000f, 16.720000f }, { -47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { -50.410000f, 8.620000f, 20.070000f }, { -47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { -41.040000f, 16.630000f, 16.720000f }, { -42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { -36.540000f, 25.030000f, 16.720000f }, { -42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { -47.400000f, 19.210000f, 20.070000f }, { -42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { -36.540000f, 25.030000f, 16.720000f }, { -35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { -30.340000f, 32.260000f, 16.720000f }, { -35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { -42.190000f, 28.900000f, 20.070000f }, { -35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { -30.340000f, 32.260000f, 16.720000f }, { -26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { -22.730000f, 38.010000f, 16.720000f }, { -26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { -35.030000f, 37.260000f, 20.070000f }, { -26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { -22.730000f, 38.010000f, 16.720000f }, { -16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { -14.070000f, 41.990000f, 16.720000f }, { -16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { -26.250000f, 43.890000f, 20.070000f }, { -16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { -14.070000f, 41.990000f, 16.720000f }, { -5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { -4.760000f, 44.030000f, 16.720000f }, { -5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { -16.250000f, 48.490000f, 20.070000f }, { -5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { -4.760000f, 44.030000f, 16.720000f }, { 5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { 4.760000f, 44.030000f, 16.720000f }, { 5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { -5.500000f, 50.850000f, 20.070000f }, { 5.500000f, 50.850000f, 20.070000f } });
                sd.wireframe.push_back({ { 4.760000f, 44.030000f, 16.720000f }, { 16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { 14.070000f, 41.990000f, 16.720000f }, { 16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { 5.500000f, 50.850000f, 20.070000f }, { 16.250000f, 48.490000f, 20.070000f } });
                sd.wireframe.push_back({ { 14.070000f, 41.990000f, 16.720000f }, { 26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { 22.730000f, 38.010000f, 16.720000f }, { 26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { 16.250000f, 48.490000f, 20.070000f }, { 26.250000f, 43.890000f, 20.070000f } });
                sd.wireframe.push_back({ { 22.730000f, 38.010000f, 16.720000f }, { 35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { 30.340000f, 32.260000f, 16.720000f }, { 35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { 26.250000f, 43.890000f, 20.070000f }, { 35.030000f, 37.260000f, 20.070000f } });
                sd.wireframe.push_back({ { 30.340000f, 32.260000f, 16.720000f }, { 42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { 36.540000f, 25.030000f, 16.720000f }, { 42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { 35.030000f, 37.260000f, 20.070000f }, { 42.190000f, 28.900000f, 20.070000f } });
                sd.wireframe.push_back({ { 36.540000f, 25.030000f, 16.720000f }, { 47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { 41.040000f, 16.630000f, 16.720000f }, { 47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { 42.190000f, 28.900000f, 20.070000f }, { 47.400000f, 19.210000f, 20.070000f } });
                sd.wireframe.push_back({ { 41.040000f, 16.630000f, 16.720000f }, { 50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { 43.650000f, 7.470000f, 16.720000f }, { 50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { 47.400000f, 19.210000f, 20.070000f }, { 50.410000f, 8.620000f, 20.070000f } });
                sd.wireframe.push_back({ { 43.650000f, 7.470000f, 16.720000f }, { 51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { 44.240000f, -2.050000f, 16.720000f }, { 51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { 50.410000f, 8.620000f, 20.070000f }, { 51.090000f, -2.360000f, 20.070000f } });
                sd.wireframe.push_back({ { 44.240000f, -2.050000f, 16.720000f }, { 49.400000f, -13.240000f, 20.070000f } });
                sd.wireframe.push_back({ { 42.780000f, -11.460000f, 16.720000f }, { 49.400000f, -13.240000f, 20.070000f } });
                sd.wireframe.push_back({ { 51.090000f, -2.360000f, 20.070000f }, { 49.400000f, -13.240000f, 20.070000f } });
                sd.wireframe.push_back({ { -49.400000f, -13.240000f, 20.070000f }, { -56.020000f, -15.010000f, 23.410000f } });
                sd.wireframe.push_back({ { -49.400000f, -13.240000f, 20.070000f }, { -57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { -51.090000f, -2.360000f, 20.070000f }, { -57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { -56.020000f, -15.010000f, 23.410000f }, { -57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { -51.090000f, -2.360000f, 20.070000f }, { -57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -50.410000f, 8.620000f, 20.070000f }, { -57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -57.940000f, -2.680000f, 23.410000f }, { -57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -50.410000f, 8.620000f, 20.070000f }, { -53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -47.400000f, 19.210000f, 20.070000f }, { -53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -57.170000f, 9.780000f, 23.410000f }, { -53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -47.400000f, 19.210000f, 20.070000f }, { -47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -42.190000f, 28.900000f, 20.070000f }, { -47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -53.750000f, 21.780000f, 23.410000f }, { -47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -42.190000f, 28.900000f, 20.070000f }, { -39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { -35.030000f, 37.260000f, 20.070000f }, { -39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { -47.850000f, 32.780000f, 23.410000f }, { -39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { -35.030000f, 37.260000f, 20.070000f }, { -29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -26.250000f, 43.890000f, 20.070000f }, { -29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -39.730000f, 42.260000f, 23.410000f }, { -29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { -26.250000f, 43.890000f, 20.070000f }, { -18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { -16.250000f, 48.490000f, 20.070000f }, { -18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { -29.770000f, 49.780000f, 23.410000f }, { -18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { -16.250000f, 48.490000f, 20.070000f }, { -6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { -5.500000f, 50.850000f, 20.070000f }, { -6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { -18.430000f, 54.990000f, 23.410000f }, { -6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { -5.500000f, 50.850000f, 20.070000f }, { 6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { 5.500000f, 50.850000f, 20.070000f }, { 6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { -6.240000f, 57.660000f, 23.410000f }, { 6.240000f, 57.660000f, 23.410000f } });
                sd.wireframe.push_back({ { 5.500000f, 50.850000f, 20.070000f }, { 18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { 16.250000f, 48.490000f, 20.070000f }, { 18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { 6.240000f, 57.660000f, 23.410000f }, { 18.430000f, 54.990000f, 23.410000f } });
                sd.wireframe.push_back({ { 16.250000f, 48.490000f, 20.070000f }, { 29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 26.250000f, 43.890000f, 20.070000f }, { 29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 18.430000f, 54.990000f, 23.410000f }, { 29.770000f, 49.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 26.250000f, 43.890000f, 20.070000f }, { 39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { 35.030000f, 37.260000f, 20.070000f }, { 39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { 29.770000f, 49.780000f, 23.410000f }, { 39.730000f, 42.260000f, 23.410000f } });
                sd.wireframe.push_back({ { 35.030000f, 37.260000f, 20.070000f }, { 47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 42.190000f, 28.900000f, 20.070000f }, { 47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 39.730000f, 42.260000f, 23.410000f }, { 47.850000f, 32.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 42.190000f, 28.900000f, 20.070000f }, { 53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 47.400000f, 19.210000f, 20.070000f }, { 53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 47.850000f, 32.780000f, 23.410000f }, { 53.750000f, 21.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 47.400000f, 19.210000f, 20.070000f }, { 57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 50.410000f, 8.620000f, 20.070000f }, { 57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 53.750000f, 21.780000f, 23.410000f }, { 57.170000f, 9.780000f, 23.410000f } });
                sd.wireframe.push_back({ { 50.410000f, 8.620000f, 20.070000f }, { 57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { 51.090000f, -2.360000f, 20.070000f }, { 57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { 57.170000f, 9.780000f, 23.410000f }, { 57.940000f, -2.680000f, 23.410000f } });
                sd.wireframe.push_back({ { 51.090000f, -2.360000f, 20.070000f }, { 56.020000f, -15.010000f, 23.410000f } });
                sd.wireframe.push_back({ { 49.400000f, -13.240000f, 20.070000f }, { 56.020000f, -15.010000f, 23.410000f } });
                sd.wireframe.push_back({ { 57.940000f, -2.680000f, 23.410000f }, { 56.020000f, -15.010000f, 23.410000f } });
                sd.positions.reserve(4);
                {
                    PositionData pd;
                    pd.id = 0;
                    pd.name = "Orchestra Thymele Altar Sweetspot";
                    pd.source = { 0.000000f, 0.000000f, 1.500000f };
                    pd.listener = { 0.000000f, 14.000000f, 4.500000f };
                    pd.directDistance = 14.320000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 14.320000f;
                        rs.order = 0;
                        rs.gain = 0.070000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 14.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 15.230000f;
                        rs.order = 1;
                        rs.gain = 0.062000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 3.500000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 14.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 16.020000f;
                        rs.order = 1;
                        rs.gain = 0.061000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 11.470000f, 0.750000f });
                        rs.points.push_back({ 0.000000f, 14.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 22.200000f;
                        rs.order = 1;
                        rs.gain = 0.044000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, -4.000000f, 2.050000f });
                        rs.points.push_back({ 0.000000f, 14.000000f, 4.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 1;
                    pd.name = "Mid-Cavea Diazoma Promenade Tier";
                    pd.source = { 0.000000f, 0.000000f, 1.500000f };
                    pd.listener = { 0.000000f, 28.000000f, 12.000000f };
                    pd.directDistance = 29.900000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 29.900000f;
                        rs.order = 0;
                        rs.gain = 0.033000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 30.970000f;
                        rs.order = 1;
                        rs.gain = 0.031000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 20.420000f, 5.140000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 31.080000f;
                        rs.order = 1;
                        rs.gain = 0.030000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 3.110000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 37.500000f;
                        rs.order = 1;
                        rs.gain = 0.026000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, -4.000000f, 2.670000f });
                        rs.points.push_back({ 0.000000f, 28.000000f, 12.000000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 2;
                    pd.name = "Highest Upper Cavea Rim (Tier 55)";
                    pd.source = { 0.000000f, 0.000000f, 1.500000f };
                    pd.listener = { 0.000000f, 48.000000f, 24.500000f };
                    pd.directDistance = 53.230000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 53.230000f;
                        rs.order = 0;
                        rs.gain = 0.019000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 48.000000f, 24.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 54.340000f;
                        rs.order = 1;
                        rs.gain = 0.018000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 27.490000f, 8.610000f });
                        rs.points.push_back({ 0.000000f, 48.000000f, 24.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 54.590000f;
                        rs.order = 1;
                        rs.gain = 0.017000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, 2.770000f, 0.000000f });
                        rs.points.push_back({ 0.000000f, 48.000000f, 24.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 60.540000f;
                        rs.order = 1;
                        rs.gain = 0.016000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ 0.000000f, -4.000000f, 3.140000f });
                        rs.points.push_back({ 0.000000f, 48.000000f, 24.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                {
                    PositionData pd;
                    pd.id = 3;
                    pd.name = "Lateral Wing Parodos Entry Line";
                    pd.source = { 0.000000f, 0.000000f, 1.500000f };
                    pd.listener = { -22.000000f, 16.000000f, 8.500000f };
                    pd.directDistance = 28.090000f;
                    pd.rays.reserve(4);
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.090000f;
                        rs.order = 0;
                        rs.gain = 0.036000f;
                        rs.points.reserve(2);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -22.000000f, 16.000000f, 8.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.120000f;
                        rs.order = 1;
                        rs.gain = 0.035000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -21.790000f, 15.850000f, 8.300000f });
                        rs.points.push_back({ -22.000000f, 16.000000f, 8.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 28.980000f;
                        rs.order = 1;
                        rs.gain = 0.032000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -3.300000f, 2.400000f, 0.000000f });
                        rs.points.push_back({ -22.000000f, 16.000000f, 8.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    {
                        RaySegment rs;
                        rs.distanceMeters = 33.300000f;
                        rs.order = 1;
                        rs.gain = 0.029000f;
                        rs.points.reserve(3);
                        rs.points.push_back({ 0.000000f, 0.000000f, 1.500000f });
                        rs.points.push_back({ -3.670000f, -4.000000f, 2.670000f });
                        rs.points.push_back({ -22.000000f, 16.000000f, 8.500000f });
                        pd.rays.push_back(std::move(rs));
                    }
                    sd.positions.push_back(std::move(pd));
                }
                list.push_back(std::move(sd));
            }

            return list;
        }
    };
}