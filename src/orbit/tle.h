#pragma once

#include <string>

struct TLE
{
    bool valid = true;
    std::string object_name;
    std::string tle_line_1;
    std::string tle_line_2;
};

TLE fetchCelestrakTLE(int norad);