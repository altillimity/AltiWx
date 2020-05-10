#include "tle_manager.h"

#include <unordered_map>
#include <mutex>
#include "logger/logger.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include "tle_fetcher.h"

std::mutex tleManagerMutex;
std::unordered_map<int, TLE> satelliteTLEs;

void updateTLEs()
{
    for (std::pair<const int, TLE> &currentNorad : satelliteTLEs)
    {
        int norad = currentNorad.first;
        TLE &tle = currentNorad.second;

        logger->info("Fetching TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));

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
            logger->warn("Could not fetch TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));
    }
}

void startTLEManager(std::vector<int> &norads)
{
    logger->info("Starting TLE manager...");
    for (int &norad : norads)
        satelliteTLEs.emplace(norad, TLE());
    updateTLEs();
    std::string &cron = configManager->getConfig().tle_update;
    logger->info("TLE updates will run according to " + cron);
    globalScheduler->cron(cron, updateTLEs);
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