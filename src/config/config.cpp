#include "config.h"
#include "yaml_cpp_converts.h"
#include <filesystem>
#include <fstream>
#include "logger/logger.h"

std::shared_ptr<ConfigManager> configManager;

ConfigData getDefaultConfig()
{
    ConfigData config;

    config.station_name = "My Station";
    config.station = {0.0, 0.0, 0.0};

    SatelliteConfig meteorConfig = {40069, 10.0f, 1, 137.100f};
    config.satelliteConfigs.push_back(meteorConfig);

    return config;
}

void initConfig()
{
    logger->info("Initializing config...");

    std::string filename = "config.yml";

    if (std::filesystem::exists(filename))
    {
        logger->debug("Config found! Loading " + filename);
        configManager = std::make_shared<ConfigManager>(filename);
        configManager->loadConfigFile();
    }
    else
    {
        logger->debug("Config not found! Writing defaults to " + filename);
        configManager = std::make_shared<ConfigManager>(filename, getDefaultConfig());
        configManager->saveConfigFile();
    }
    logger->info("Done!");
}

ConfigManager::ConfigManager(std::string filename) : filename_m(filename)
{
}

ConfigManager::ConfigManager(std::string filename, ConfigData config) : filename_m(filename), config_m(config)
{
}

ConfigData &ConfigManager::getConfig()
{
    return config_m;
}

void ConfigManager::loadConfigFile()
{
    try
    {
        configFile = YAML::LoadFile(filename_m);
    }
    catch (YAML::Exception e)
    {
        logger->critical("Fatal error reading config! Aborting!");
        logger->critical(e.what());
        exit(1);
    }

    config_m.station_name = configFile["station_name"].as<std::string>();
    config_m.station = configFile["station"].as<SatelliteStation>();
    config_m.satelliteConfigs = configFile["satellites"].as<std::vector<SatelliteConfig>>();
}

void ConfigManager::saveConfigFile()
{
    configFile["station_name"] = (std::string)config_m.station_name;
    configFile["station"] = (SatelliteStation)config_m.station;
    configFile["satellites"] = (std::vector<SatelliteConfig>)config_m.satelliteConfigs;

    std::ofstream outFile(filename_m);
    outFile << configFile << '\n';
}