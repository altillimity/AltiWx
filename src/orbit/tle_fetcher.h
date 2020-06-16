#pragma once

#include <string>
#include "tle.h"

// TLE fetcher from Celestrak
class TLEFetcher
{
private:
    int norad_m;
    bool data;
    TLE tle;

public:
    TLEFetcher(int norad);
    void fetch();
    bool containsData();
    TLE getTLE();
};