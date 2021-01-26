#include "config.h"
#include "yaml_cpp_converts.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include "logger/logger.h"
#include "orbit/tle_manager.h"

std::shared_ptr<ConfigManager> configManager;

void initConfig()
{
    logger->info("Initializing config...");

    // Default filename
    std::string filename = CONFIG_FILE_PATH;

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
    catch (YAML::Exception &e)
    {
        logger->critical("Fatal error reading config! Aborting!");
        logger->critical(e.what());
        exit(1);
    }

    // Parse the file into our object
    config_m.station_name = configFile["station_name"].as<std::string>();
    config_m.station = configFile["station"].as<SatelliteStation>();
    config_m.tle_update_cron = configFile["tle_update"].as<std::string>();
    config_m.data_directory = configFile["data_directory"].as<std::string>();
    config_m.log_level = configFile["log_level"].as<spdlog::level::level_enum>();
    config_m.radio_config = configFile["radio"].as<RadioConfig>();
    config_m.satellite_configs = configFile["satellites"].as<std::vector<SatelliteConfig>>();
}

void ConfigManager::saveConfigFile()
{
    /*
    // Write object into the file
    configFile["station_name"] = (std::string)config_m.station_name;
    configFile["station"] = (SatelliteStation)config_m.station;
    configFile["tle_update"] = (std::string)config_m.tle_update_cron;
    configFile["data_directory"] = (std::string)config_m.data_directory;
    configFile["log_level"] = (spdlog::level::level_enum)config_m.log_level;
    configFile["radio"] = (RadioConfig)config_m.radio_config;

    configFile["max_tle_age"] = (int)config_m.max_tle_age;

    // Write the actual file
    std::ofstream outFile(filename_m);
    outFile << configFile << '\n';
    */
}

SatelliteConfig ConfigData::getSatelliteConfig(int norad)
{
    std::vector<SatelliteConfig>::iterator it = std::find_if(configManager->getConfig().satellite_configs.begin(),
                                                             configManager->getConfig().satellite_configs.end(),
                                                             [&norad](const SatelliteConfig &e) {
                                                                 return e.norad == norad;
                                                             });
    return *it;
}

template <class InputIt, class T = typename std::iterator_traits<InputIt>::value_type>
T most_common(InputIt begin, InputIt end)
{
    std::map<T, int> counts;
    for (InputIt it = begin; it != end; ++it)
    {
        if (counts.find(*it) != counts.end())
            ++counts[*it];
        else
            counts[*it] = 1;
    }
    return std::max_element(counts.begin(), counts.end(), [](const std::pair<T, int> &pair1, const std::pair<T, int> &pair2) { return pair1.second < pair2.second; })->first;
}

long getBandForDownlink(DownlinkConfig cfg)
{
    long band = 0;

    for (long &freq : configManager->getConfig().radio_config.frequencies)
    {
        long minFreq = freq - (configManager->getConfig().radio_config.samplerate / 2);
        long maxFreq = freq + (configManager->getConfig().radio_config.samplerate / 2);

        if (minFreq <= cfg.frequency && cfg.frequency <= maxFreq)
            band = freq;
    }

    return band;
}

long getBandForSatellite(SatelliteConfig cfg)
{
    std::vector<long> freqs;
    for (DownlinkConfig downlink : cfg.downlinkConfigs)
        freqs.push_back(getBandForDownlink(downlink));
    return most_common(freqs.begin(), freqs.end());
}