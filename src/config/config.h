#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite_station.h"

struct SDRConfig
{
    long centerFrequency;
    long sampleRate;
    int gain;
};

struct SatelliteConfig
{
    int norad;
    float min_elevation;
    int priority;
    long frequency;
};

struct ConfigData
{
    std::string station_name;
    SatelliteStation station;
    std::vector<SatelliteConfig> satelliteConfigs;
    std::string tle_update;
    SDRConfig sdrConfig;
};

void initConfig();

class ConfigManager
{
private:
    YAML::Node configFile;
    std::string filename_m;
    ConfigData config_m;

public:
    ConfigManager(std::string filename);
    ConfigManager(std::string filename, ConfigData config);
    void saveConfigFile();
    void loadConfigFile();
    ConfigData &getConfig();
};

extern std::shared_ptr<ConfigManager> configManager;