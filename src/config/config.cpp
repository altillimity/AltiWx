#include "config.h"
#include "yaml_cpp_converts.h"
#include <filesystem>
#include <fstream>
#include "logger/logger.h"
#include "orbit/tle_manager.h"

std::shared_ptr<ConfigManager> configManager;

ConfigData getDefaultConfig()
{
    // Fill config object with default values
    ConfigData config;

    config.station_name = "My Station";
    config.station = {0.0, 0.0, 0.0};

    SatelliteConfig noaa15Config = {(int)25338,
                                    (float)10.0f,
                                    (int)1,
                                    (std::vector<DownlinkConfig>){
                                        {(std::string) "APT",
                                         (long)137.620e6,
                                         (long)50e3,
                                         (bool)false,
                                         (std::string) "apt-noaa.lua",
                                         (std::string) "wav",
                                         (ModemType)ModemType::FM,
                                         11025}}};
    config.satelliteConfigs.push_back(noaa15Config);

    config.tle_update = "0 0 * * *";

    config.sdrConfig = {(long)137.500e6, (long)2.4e6, (int)0, (bool)false, "ipc:///tmp/altiwx"};

    config.dataDirectory = "data";

    config.logLevel = spdlog::level::trace;

    return config;
}

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
    // Load config file. If it doesn't work let yamlcpp explain why
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

    // Parse the file into our object
    config_m.station_name = configFile["station_name"].as<std::string>();
    config_m.station = configFile["station"].as<SatelliteStation>();
    config_m.satelliteConfigs = configFile["satellites"].as<std::vector<SatelliteConfig>>();
    config_m.tle_update = configFile["tle_update"].as<std::string>();
    config_m.sdrConfig = configFile["rtlsdr"].as<SDRConfig>();
    config_m.dataDirectory = configFile["data_directory"].as<std::string>();
    config_m.logLevel = configFile["logger_level"].as<spdlog::level::level_enum>();
}

void ConfigManager::saveConfigFile()
{
    // Write object into the file
    configFile["station_name"] = (std::string)config_m.station_name;
    configFile["station"] = (SatelliteStation)config_m.station;
    configFile["satellites"] = (std::vector<SatelliteConfig>)config_m.satelliteConfigs;
    configFile["tle_update"] = (std::string)config_m.tle_update;
    configFile["rtlsdr"] = (SDRConfig)config_m.sdrConfig;
    configFile["data_directory"] = (std::string)config_m.dataDirectory;
    configFile["logger_level"] = (spdlog::level::level_enum)config_m.logLevel;

    // Write the actual file
    std::ofstream outFile(filename_m);
    outFile << configFile << '\n';
}

SatelliteConfig ConfigData::getSatelliteConfigFromNORAD(int norad)
{
    // Return a SatelliteConfig object from a NORAD id. Otherwise return an empty instance.
    std::vector<SatelliteConfig>::iterator value = std::find_if(satelliteConfigs.begin(), satelliteConfigs.end(), [&](const SatelliteConfig &c) { return c.norad == norad; });
    if (value != satelliteConfigs.end())
        return *value;
    else
        return SatelliteConfig();
}

std::string SatelliteConfig::getName()
{
    // Return the satellite's name
    return getTLEFromNORAD(norad).name;
}