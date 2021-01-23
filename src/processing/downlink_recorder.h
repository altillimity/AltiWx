#pragma once

#include "config/config.h"
#include "dsp/dsp.h"
#include <thread>
#include <memory>
#include "orbit/doppler_correction.h"

// Class recording a satellite downlink (single)
class DownlinkRecorder
{
private:
    std::shared_ptr<DeviceDSP> d_dsp;
    DownlinkConfig d_downlink;
    SatelliteConfig d_satellite_config;
    std::atomic<bool> running;
    std::shared_ptr<Modem> modem;
    std::string modem_id;
    std::thread doppler_thread;
    std::shared_ptr<DopplerCorrector> dopplerCorrector;

private:
    void doDoppler();

public:
    DownlinkRecorder(std::shared_ptr<DeviceDSP> dsp, DownlinkConfig &downlink, SatelliteConfig satellite_config, TLE &tle, std::string filename);
    void start();
    void stop();
};