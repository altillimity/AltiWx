#pragma once

#include "logger/logger.h"
#include "orbit/satellite.h"
#include "config/config.h"
#include "orbit/tle.h"

class ProcessingScript
{
private:
    SatellitePass d_satellite_pass;
    SatelliteConfig d_satellite_config;
    DownlinkConfig d_downlink_config;
    std::string d_input_file;
    std::string d_filename;
    std::string d_workdir;
    std::string d_script;
    std::string script_content;
    TLE d_tle;

public:
    ProcessingScript(SatellitePass satellite_pass, SatelliteConfig satelliteConfig, DownlinkConfig downlinkConfig, TLE tle, std::string input_file, std::string filename, std::string workdir, std::string script);
    void process();
};