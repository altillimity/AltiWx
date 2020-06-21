#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite_station.h"
#include "dsp/modem/modem_enum.h"
#include <spdlog/spdlog.h>

// SDR Config struct
struct SDRConfig
{
    long centerFrequency;
    long sampleRate;
    int gain;
    std::string soapyDeviceString;
    bool soapy;
    std::string soapySocket;
};

// Downlink config struct
struct DownlinkConfig
{
    std::string name;
    long frequency;
    long bandwidth;
    bool dopplerCorrection;
    std::string outputExtension;
    std::string postProcessingScript;
    ModemType modemType;
    
    long modem_audioSamplerate;
};

// Satellite config struct
struct SatelliteConfig
{
    int norad;
    float min_elevation;
    int priority;
    std::vector<DownlinkConfig> downlinkConfigs;
    std::string getName();
};

// Main config struct
struct ConfigData
{
    std::string station_name;
    SatelliteStation station;
    std::vector<SatelliteConfig> satelliteConfigs;
    std::string tle_update;
    SDRConfig sdrConfig;
    SatelliteConfig getSatelliteConfigFromNORAD(int norad);
    std::string dataDirectory;
    spdlog::level::level_enum logLevel;
};

// Initialize config
void initConfig();

// ConfigManager class
class ConfigManager
{
private:
    // Yaml object
    YAML::Node configFile;
    // Config filename
    std::string filename_m;
    // Holds our config
    ConfigData config_m;

public:
    ConfigManager(std::string filename);
    ConfigManager(std::string filename, ConfigData config);
    void saveConfigFile();
    void loadConfigFile();
    ConfigData &getConfig();
};

// Main config object
extern std::shared_ptr<ConfigManager> configManager;