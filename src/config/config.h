#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite_station.h"

struct SatelliteConfig
{
    int norad;
    float min_elevation;
    int priority;
    float frequency;
};

struct ConfigData
{
    std::string station_name;
    SatelliteStation station;
    std::vector<SatelliteConfig> satelliteConfigs;
    std::string tle_update;
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