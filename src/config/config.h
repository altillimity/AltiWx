#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite_station.h"
#include "dsp/modem/modem_enum.h"

struct SDRConfig
{
    long centerFrequency;
    long sampleRate;
    int gain;
    bool soapy;
    std::string soapySocket;
};

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

struct SatelliteConfig
{
    int norad;
    float min_elevation;
    int priority;
    std::vector<DownlinkConfig> downlinkConfigs;
    std::string getName();
};

struct ConfigData
{
    std::string station_name;
    SatelliteStation station;
    std::vector<SatelliteConfig> satelliteConfigs;
    std::string tle_update;
    SDRConfig sdrConfig;
    SatelliteConfig getSatelliteConfigFromNORAD(int norad);
    std::string dataDirectory;
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