#include "logger/logger.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include <iostream>
#include <algorithm>
#include "tclap/CmdLine.h"
#include "orbit/pass_manager.h"
#include "dsp/dsp_manager.h"
#include "processing/pass_processing.h"
#include "processing/downlink_processor.h"
#include "communication/communication.h"

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

    // No arguments? Run normally
    if (argc == 1)
    {
        // Start pass manager
        initPassManager();
        // Finally, start DSP
        initDSP();

        // Start communication manager
        CommunicationManager communicationManager(ALTIWX_SOCKET_PATH);
        communicationManager.start();

        //std::thread test([=] { processPass({21576, getTLEFromNORAD(21576), time(NULL), time(NULL) + 20, 10.0f, false, true}); });
        //processPass({40069, getTLEFromNORAD(40069), time(NULL), time(NULL) + 20, 10.0f, false, true});

        std::cin.get();

        // Stop communication manager
        communicationManager.stop();
        // Stop DSP
        stopDSP();
    }
    else
    {
        TCLAP::CmdLine cmd("AltiWx script tester", ' ', "");

        // Script testing
        TCLAP::ValueArg<std::string> scriptTest("s", "script", "Script to run", true, "", "script");
        TCLAP::ValueArg<std::string> fileInput("f", "file", "File to test with", true, "", "file");
        TCLAP::ValueArg<int> noradTest("n", "norad", "NORAD", true, 0, "NORAD");

        cmd.add(scriptTest);
        cmd.add(fileInput);
        cmd.add(noradTest);

        // Parse
        try
        {
            cmd.parse(argc, argv);
        }
        catch (TCLAP::ArgException &e)
        {
            logger->error(e.what());
            return 0;
        }

        // Run the script!
        DownlinkProcessor downlinkProcessor({noradTest.getValue(),
                                             getTLEFromNORAD(noradTest.getValue()),
                                             time(NULL),
                                             time(NULL) + 20, 10.0f,
                                             false,
                                             true},
                                            configManager->getConfig().getSatelliteConfigFromNORAD(noradTest.getValue()),
                                            configManager->getConfig().getSatelliteConfigFromNORAD(noradTest.getValue()).downlinkConfigs[0],
                                            fileInput.getValue(),
                                            "testScript/test",
                                            scriptTest.getValue());
        downlinkProcessor.process();
    }
}