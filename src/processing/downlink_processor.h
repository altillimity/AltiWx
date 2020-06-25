#pragma once

#include "logger/logger.h"
#include "orbit/satellite_pass.h"
#include "config/config.h"

class DownlinkProcessor
{
private:
    SatellitePass satellitePass_m;
    SatelliteConfig satelliteConfig_m;
    DownlinkConfig downlinkConfig_m;
    std::string inputFile_m;
    std::string filename_m;
    std::string script_m;
    std::string scriptContent_m;
    std::vector<std::string> outputFiles_m;

public:
    DownlinkProcessor(SatellitePass satellitePass, SatelliteConfig satelliteConfig, DownlinkConfig downlinkConfig, std::string inputFile, std::string filename, std::string script);
    void process();
    std::vector<std::string> getOutputs();
};