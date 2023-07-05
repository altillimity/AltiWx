#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite.h"
#include <spdlog/spdlog.h>

#define CONFIG_FILE_PATH "config.yml"

// SDR Config struct
struct RadioConfig
{
    // General settings
    std::vector<long> frequencies;
    long samplerate;
    int gain;
    std::string serial_number;

    // PPM Correction
    bool ppm_enabled;
    double ppm;
};

// Downlink config struct
struct DownlinkConfig
{
    std::string name;
    long frequency;
    long bandwidth;
    bool doppler_correction;
    std::string output_extension;
    std::string post_processing_script;
    std::string modem_type;
    std::map<std::string, std::string> modem_parameters;
};

// Satellite config struct
struct SatelliteConfig
{
    int norad;
    float min_elevation;
    int priority;
    std::vector<DownlinkConfig> downlinkConfigs;
};

// Main config struct
struct ConfigData
{
    std::string station_name;
    SatelliteStation station;
    std::string tle_update_cron;
    RadioConfig radio_config;
    std::string data_directory;
    spdlog::level::level_enum log_level;
    std::vector<SatelliteConfig> satellite_configs;
    SatelliteConfig getSatelliteConfig(int norad);
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

// Utils
long getBandForDownlink(DownlinkConfig cfg);
long getBandForSatellite(SatelliteConfig cfg); // We currently assume 1 satellite = 1 band
