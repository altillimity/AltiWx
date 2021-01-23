#include "tle_manager.h"
#include "logger/logger.h"

TLEManager::TLEManager()
{
}

TLEManager::~TLEManager()
{
}

void TLEManager::addNORAD(int norad)
{
    tle_mutex.lock();
    norad_ids.push_back(norad);
    tle_mutex.unlock();
}

void TLEManager::updateTLEs()
{
    tle_mutex.lock();

    logger->info("Updating all TLEs...");

    for (int &norad : norad_ids)
    {
        TLE tle = fetchCelestrakTLE(norad);

        if (tle.valid)
        {
            if (tle_map.count(norad) > 0)
                tle_map[norad] = tle;
            else
                tle_map.insert({norad, tle});
        }
    }

    tle_mutex.unlock();
}

TLE TLEManager::getTLE(int norad)
{
    tle_mutex.lock();

    TLE tle = {false, "", "", ""};

    if (tle_map.count(norad) > 0)
        tle = tle_map[norad];

    tle_mutex.unlock();
    return tle;
}