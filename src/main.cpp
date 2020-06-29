#include "logger/logger.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include <iostream>
#include <algorithm>
#include "orbit/pass_manager.h"
#include "dsp/dsp_manager.h"
#include "processing/pass_processing.h"

int main(int argc, char *argv[])
{
    // Start logger first
    initLogger();

    // Nice graphics!
    logger->info("   ___   ____  _ _      __    ");
    logger->info("  / _ | / / /_(_) | /| / /_ __");
    logger->info(" / __ |/ / __/ /| |/ |/ /\\ \\ /");
    logger->info("/_/ |_/_/\\__/_/ |__/|__//_\\_\\ ");
    logger->info("                              ");

    logger->info("Starting AltiWx...");
    // Init config
    initConfig();
    // Set custom log level
    setConsoleLevel(configManager->getConfig().logLevel);
    logger->debug("Using data directory " + configManager->getConfig().dataDirectory);
    // Start scheduler
    initScheduler();

    // Create a NORAD list
    std::vector<int> norads;
    for (SatelliteConfig satConfig : configManager->getConfig().satelliteConfigs)
        norads.push_back(satConfig.norad);

    // Start TLE manager
    startTLEManager(norads);
    // Start pass manager
    initPassManager();
    // Finally, start DSP
    initDSP();

    //processPass({21576, getTLEFromNORAD(21576), time(NULL), time(NULL) + 10, 10.0f, false, true});
    //processPass({40069, getTLEFromNORAD(40069), time(NULL), time(NULL) + 20, 10.0f});

    std::cin.get();

    stopDSP();
}