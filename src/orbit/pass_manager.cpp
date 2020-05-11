#include "pass_manager.h"
#include <vector>
#include "satellite_pass.h"
#include "config/config.h"
#include "logger/logger.h"
#include "scheduler/scheduler.h"
#include "orbit_predictor.h"
#include "tle_manager.h"
#include "processing/processing.h"

void schedulePasses()
{
    std::vector<SatellitePass> passes;

    for (SatelliteConfig &satellite : configManager->getConfig().satelliteConfigs)
    {
        logger->info("Predicting passes for " + getTLEFromNORAD(satellite.norad).name);
        OrbitPredictor predictor(satellite.norad, getTLEFromNORAD(satellite.norad), configManager->getConfig().station);
        std::vector<SatellitePass> predictedPasses = predictor.getPassesBetweenOver(time(NULL), time(NULL) + 24 * 60 * 60, 10.0f);
        passes.insert(passes.end(), predictedPasses.begin(), predictedPasses.end());
    }

    std::sort(passes.begin(), passes.end(), [](const SatellitePass &v1, const SatellitePass &v2) { return v1.aos < v2.aos; });

    for (SatellitePass pass : passes)
    {
        std::tm *timeReadable = gmtime(&pass.aos);
        logger->info("Scheduling pass of " + pass.tle.name + " at " + std::to_string(timeReadable->tm_hour) + ":" + (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min)) + " with " + std::to_string(std::round(pass.elevation * 10) / 10) + "° elevation");
        globalScheduler->in(std::chrono::system_clock::from_time_t(pass.aos), processPass, pass);
    }
}

void initPassManager()
{
    logger->info("Start pass scheduler...");
    schedulePasses();
    globalScheduler->every(std::chrono::system_clock::duration(std::chrono::seconds(24 * 60 * 60)), schedulePasses);
}