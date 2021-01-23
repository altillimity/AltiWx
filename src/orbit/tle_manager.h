#pragma once

#include "tle.h"
#include <vector>
#include <mutex>
#include <unordered_map>

class TLEManager
{
private:
    std::vector<int> norad_ids;
    std::unordered_map<int, TLE> tle_map;
    std::mutex tle_mutex;

public:
    TLEManager();
    ~TLEManager();
    void addNORAD(int norad);
    void updateTLEs();
    TLE getTLE(int norad);
};