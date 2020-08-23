#include "logger/logger.h"
#include <csignal>
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "tclap/CmdLine.h"
#include "orbit/pass_manager.h"
#include "dsp/dsp_manager.h"
#include "processing/pass_processing.h"
#include "processing/downlink_processor.h"
#include "communication/communication.h"
#include "database/database.h"
#include "web/server.h"
#include "plugin/plugins.h"

// API
#include "api/altiwx/altiwx.h"
#include "api/altiwx/events/events.h"

bool shouldExit = false;

void signalHandler(int signum)
{
    logger->critical("Exiting!!!!");
    shouldExit = true;
}

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

    // Plugins!
    initPlugins();

    // Modems
    registerModems();

    // Database!
    initDatabaseManager();

    // Web GUI
    initWebServer();

    // Start scheduler
    initScheduler();

    // Start TLE manager
    startTLEManager();

    // No arguments? Run normally
    if (argc == 1)
    {
        // Start communication manager
        CommunicationManager communicationManager(ALTIWX_SOCKET_PATH);
        communicationManager.start();

        // Finally, start DSP
        initDSP();

        // Start pass manager
        initPassManager();

        altiwx::eventBus->fire_event<altiwx::events::StartedEvent>({});

        //std::thread test([=] { processPass({21576, getTLEFromNORAD(21576), time(NULL), time(NULL) + 20, 10.0f, false, true}); });
        //processPass({40069, getTLEFromNORAD(40069), time(NULL), time(NULL) + 1200, 10.0f, false, true});

        // Register our custom shutdown signal
        signal(SIGINT, signalHandler);

        // And wait
        while (!shouldExit)
            std::this_thread::sleep_for(std::chrono::seconds(1));

        // Stop communication manager
        communicationManager.stop();
        // Stop DSP
        stopDSP();

        // Stop web server
        stopWebServer();
    }
    else
    {
        TCLAP::CmdLine cmd("AltiWx script tester", ' ', "");

        // Script testing
        TCLAP::ValueArg<std::string> scriptTest("s", "script", "Script to run", true, "", "script");
        TCLAP::ValueArg<std::string> fileInput("f", "file", "File to test with", true, "", "file");
        TCLAP::ValueArg<int> noradTest("n", "norad", "NORAD", true, 0, "NORAD");
        TCLAP::ValueArg<std::string> downlinkTest("d", "downlink", "Downlink to test with", true, "", "name");

        cmd.add(scriptTest);
        cmd.add(fileInput);
        cmd.add(noradTest);
        cmd.add(downlinkTest);

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

        logger->info("Testing script " + scriptTest.getValue() + " with satellite " + getTLEFromNORAD(noradTest.getValue()).name);

        // Create the directory we'll need!
        std::filesystem::create_directory("testScript");

        // Run the script!
        DownlinkProcessor downlinkProcessor(
            {noradTest.getValue(),
             getTLEFromNORAD(noradTest.getValue()),
             time(NULL),
             time(NULL) + 20, 10.0f,
             false,
             true},
            databaseManager->getSatellite(noradTest.getValue()),
            [&]() -> DownlinkConfig {
                for (DownlinkConfig &config : databaseManager->getSatellite(noradTest.getValue()).downlinkConfigs)
                    if (config.name == downlinkTest.getValue())
                        return config;
                logger->error("Downlink " + downlinkTest.getValue() + " not found!");
                exit(1);
            }(),
            fileInput.getValue(),
            "testScript/test",
            scriptTest.getValue());
        downlinkProcessor.process();
    }
}