#include "tle_manager.h"

#include <unordered_map>
#include <mutex>
#include "logger/logger.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include "tle_fetcher.h"

// Mutex to prevent conflicts
std::mutex tleManagerMutex;
// Map holding TLE data
std::unordered_map<int, TLE> satelliteTLEs;

// Fetch TLEs for all satellites
void updateTLEs()
{
    for (std::pair<const int, TLE> &currentNorad : satelliteTLEs)
    {
        // Fetch TLEs, update if the update was sucessful
        int norad = currentNorad.first;
        TLE &tle = currentNorad.second;

        logger->info("Fetching TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));

        TLEFetcher tleFetcher(norad);
        tleFetcher.fetch();

        if (tleFetcher.containsData())
        {
            // Mutex used to prevent getTLEFromNORAD to fetch data at the same time
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
    // Initial TLE fetching, start the scheduler task
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
    // Lock mutex again
    tleManagerMutex.lock();

    // Simply return the appropriate TLE object
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