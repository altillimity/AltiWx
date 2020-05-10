#include "tle_manager.h"

#include <unordered_map>
#include <mutex>
#include "logger/logger.h"
#include "tle_fetcher.h"

std::mutex tleManagerMutex;
std::unordered_map<int, TLE> satelliteTLEs;

void updateTLEs()
{
    for (std::pair<const int, TLE> &currentNorad : satelliteTLEs)
    {
        int norad = currentNorad.first;
        TLE &tle = currentNorad.second;

        logger->info("Fetching TLE for NORAD " + std::to_string(norad));

        TLEFetcher tleFetcher(norad);
        tleFetcher.fetch();

        if (tleFetcher.containsData())
        {
            tleManagerMutex.lock();
            tle = tleFetcher.getTLE();
            tleManagerMutex.unlock();
            logger->info("Success!");
        }
        else
            logger->warn("Could not fetch TLE for NORAD " + std::to_string(norad));
    }
}

void startTLEManager(std::vector<int> &norads)
{
    logger->info("Starting TLE manager...");
    for (int norad : norads)
        satelliteTLEs.emplace(norad, TLE());
    updateTLEs();
}

TLE getTLEFromNORAD(int norad)
{
    tleManagerMutex.lock();

    TLE tle;
    if (satelliteTLEs.find(norad) != satelliteTLEs.end())
        tle = satelliteTLEs[norad];
    else
    {
        logger->debug("Invalid NORAD " + std::to_string(norad));
        tle = TLE();
    }

    tleManagerMutex.unlock();
    return tle;
}