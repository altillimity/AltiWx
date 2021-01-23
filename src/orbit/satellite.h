#pragma once

#include <ctime>

enum pass_direction_t
{
    NORTHBOUND,
    SOUTHBOUND
};

struct SatellitePass
{
    int norad;
    std::time_t aos, los;
    float elevation;
    pass_direction_t direction;
};

struct SatelliteStation
{
    double latitude;
    double longitude;
    double altitude;
};