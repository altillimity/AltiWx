#include "logger/logger.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include <iostream>

int main(int argc, char *argv[])
{
    initLogger();
    logger->info("Starting AutoWx...");
    initConfig();
    initScheduler();

    std::vector<int> norads;
    for(SatelliteConfig satConfig : configManager->getConfig().satelliteConfigs)
        norads.push_back(satConfig.norad);

    startTLEManager(norads);

    logger->info("Predicting passes for " + getTLEFromNORAD(configManager->getConfig().satelliteConfigs[0].norad).name);

    OrbitPredictor predictor(configManager->getConfig().satelliteConfigs[0].norad, getTLEFromNORAD(configManager->getConfig().satelliteConfigs[0].norad), configManager->getConfig().station);

    std::vector<SatellitePass> nextPasses = predictor.getPassesBetweenOver(time(NULL), time(NULL) + 24 * 60 * 60, 10.0f);

    for (SatellitePass nextPass : nextPasses)
    {
        logger->info("Next pass of " + nextPass.tle.name);
        logger->info("AOS : " + std::to_string(nextPass.aos));
        logger->info("LOS : " + std::to_string(nextPass.los));
        logger->info("El. : " + std::to_string(std::round(nextPass.elevation * 10) / 10));
    }

    std::cin.get();
}