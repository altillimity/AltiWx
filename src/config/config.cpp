#include "config.h"
#include "yaml_cpp_converts.h"
#include <filesystem>
#include <fstream>
#include "logger/logger.h"
#include "orbit/tle_manager.h"

std::shared_ptr<ConfigManager> configManager;

void initConfig()
{
    logger->info("Initializing config...");

    // Default filename
    std::string filename = "config.yml";

    if (std::filesystem::exists(filename))
    {
        // Load existing file
        logger->debug("Config found! Loading " + filename);
        configManager = std::make_shared<ConfigManager>(filename);
        configManager->loadConfigFile();
    }
    else
    {
        // Write defaults
        logger->debug("Config not found! Exiting!");
       exit(0);
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
    // Load config file. If it doesn't work let yamlcpp explain why
    try
    {
        configFile = YAML::LoadFile(filename_m);
    }
    catch (YAML::Exception& e)
    {
        logger->critical("Fatal error reading config! Aborting!");
        logger->critical(e.what());
        exit(1);
    }

    // Parse the file into our object
    config_m.station_name = configFile["station_name"].as<std::string>();
    config_m.station = configFile["station"].as<SatelliteStation>();
    config_m.databaseConfig = configFile["database"].as<DBConfig>();
    config_m.tle_update = configFile["tle_update"].as<std::string>();
    config_m.sdrConfigs = configFile["radios"].as<std::vector<SDRConfig>>();
    config_m.dataDirectory = configFile["data_directory"].as<std::string>();
    config_m.logLevel = configFile["logger_level"].as<spdlog::level::level_enum>();
}

void ConfigManager::saveConfigFile()
{
    // Write object into the file
    configFile["station_name"] = (std::string)config_m.station_name;
    configFile["station"] = (SatelliteStation)config_m.station;
    configFile["database"] = (DBConfig)config_m.databaseConfig;
    configFile["tle_update"] = (std::string)config_m.tle_update;
    configFile["radios"] = (std::vector<SDRConfig>)config_m.sdrConfigs;
    configFile["data_directory"] = (std::string)config_m.dataDirectory;
    configFile["logger_level"] = (spdlog::level::level_enum)config_m.logLevel;

    // Write the actual file
    std::ofstream outFile(filename_m);
    outFile << configFile << '\n';
}

std::string SatelliteConfig::getName()
{
    // Return the satellite's name
    return getTLEFromNORAD(norad).name;
}