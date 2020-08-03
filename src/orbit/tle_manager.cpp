#include "tle_manager.h"

#include <unordered_map>
#include <mutex>
#include "logger/logger.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include "tle_fetcher.h"
#include "database/database.h"

// Mutex to prevent conflicts
std::mutex tleManagerMutex;

int maxTLEAge = 100000;

// Fetch TLEs for all satellites
void updateTLEs()
{
    // Current update time
    time_t updateTime = time(NULL);
    for (const int &norad : databaseManager->getAllNORADs())
    {
        // Fetch TLEs, update if the update was sucessful
        TLE tle;
        // Check if the database-stored TLE needs update
        if (databaseManager->tleExists(norad))
        {
            std::pair<TLE, time_t> res = databaseManager->getTLE(norad);
            tle = res.first;
            if (updateTime - res.second < maxTLEAge)
                continue;
        }

        logger->info("Fetching TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));

        TLEFetcher tleFetcher(norad);
        tleFetcher.fetch();

        if (tleFetcher.containsData())
        {
            // If that one was not found, we notice the user
            if (tleFetcher.getTLE().name.find("No TLE found") != std::string::npos)
                logger->warn("Could not fetch TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));
            // Mutex used to prevent getTLEFromNORAD to fetch data at the same time
            tleManagerMutex.lock();
            tle = tleFetcher.getTLE();
            databaseManager->setTLE(norad, tle, updateTime);
            tleManagerMutex.unlock();
            logger->info("Success!");
        }
        else
            logger->warn("Could not fetch TLE for NORAD " + std::to_string(norad) + (tle.name.length() > 0 ? " - " + tle.name : ""));
    }
}

void startTLEManager()
{
    // Initial TLE fetching, start the scheduler task
    logger->info("Starting TLE manager...");
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
    if (databaseManager->tleExists(norad))
        tle = databaseManager->getTLE(norad).first;
    else
    {
        logger->debug("Invalid NORAD " + std::to_string(norad));
        tle = TLE();
    }

    tleManagerMutex.unlock();
    return tle;
}