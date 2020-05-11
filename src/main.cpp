#include "logger/logger.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "config/config.h"
#include "scheduler/scheduler.h"
#include <iostream>
#include <algorithm>
#include "orbit/pass_manager.h"
#include <sol.hpp>
#include "dsp/dsp.h"
#include "dsp/modem/modem_fm.h"

void debug(std::string log)
{
    logger->debug("[Lua] " + log);
}

void info(std::string log)
{
    logger->info("[Lua] " + log);
}

void warn(std::string log)
{
    logger->warn("[Lua] " + log);
}

void error(std::string log)
{
    logger->error("[Lua] " + log);
}

void critical(std::string log)
{
    logger->critical("[Lua] " + log);
}

int main(int argc, char *argv[])
{
    initLogger();
    logger->info("Starting AutoWx...");
    initConfig();
    initScheduler();

    std::vector<int> norads;
    for (SatelliteConfig satConfig : configManager->getConfig().satelliteConfigs)
        norads.push_back(satConfig.norad);

    startTLEManager(norads);
    initPassManager();

    sol::state lua;
    // open some common libraries
    lua.open_libraries(sol::lib::base, sol::lib::package);
    lua.new_usertype<spdlog::logger>("spdlogger", "debug", &debug, "info", &info, "warn", &warn, "error", &error, "critical", &critical);
    lua["logger"] = logger;
    lua.script_file("script.lua");

    DSP mainDSP(1e6, 100.0 * 1e6, 0);
    mainDSP.start();
    std::shared_ptr<ModemFM> fmModem = std::make_shared<ModemFM>(100.298 * 1e6, 200000, 48000, "test.wav");
    mainDSP.attachModem("test", fmModem);

    std::cin.get();

    mainDSP.stop();
}