#pragma once

#include <string>
#include "tle.h"
#include <ctime>

struct SatellitePass
{
    int norad;
    TLE tle;
    std::time_t aos, los;
    float elevation;
    bool northbound;
    bool southbound;
};