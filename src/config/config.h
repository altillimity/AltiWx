#pragma once

#include <vector>
#include <string>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "orbit/satellite_station.h"
#include "dsp/modem/modem_enum.h"
#include <spdlog/spdlog.h>

#define ALTIWX_SOCKET_PATH "ipc:///tmp/altiwx"

// PostgreSQL Database Config struct
struct DBConfig
{
    std::string address;
    int port;
    std::string username;
    std::string database;
    std::string password;
};

// PostgreSQL Database Config struct
struct WebConfig
{
    std::string address;
    int port;
};

// SDR Config struct
struct SDRConfig
{
    std::string name;
    long centerFrequency;
    long sampleRate;
    int gain;
    std::string soapyDeviceString;
    bool soapy_redirect;
    std::string soapySocket;
    int demodThreads;

    // PPM Correction
    bool ppmEnabled;
    double ppm;
};

// Downlink config struct
struct DownlinkConfig
{
    std::string name;
    std::string radio;
    long frequency;
    long bandwidth;
    bool dopplerCorrection;
    std::string outputExtension;
    std::string postProcessingScript;
    ModemType modemType;

    // FM Modem
    long modem_audioSamplerate;

    // QPSK Modem
    long modem_symbolRate;
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
    DBConfig databaseConfig;
    WebConfig webConfig;
    std::string tle_update;
    int max_tle_age;
    std::vector<SDRConfig> sdrConfigs;
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