#pragma once

#include "config/config.h"
#include "dsp/dsp_manager.h"
#include <thread>
#include <memory>
#include "orbit/doppler_correction.h"

// Class recording a satellite downlink (single)
class DownlinkRecorder
{
private:
    DownlinkConfig downlink_m;
    SatelliteConfig satelliteConfig_m;
    std::shared_ptr<DSP> dsp_m;
    std::shared_ptr<Modem> modem;
    std::string modemID;
    bool running;
    std::thread dopplerThread;
    std::shared_ptr<DopplerCorrector> dopplerCorrector;

private:
    void doDoppler();

public:
    DownlinkRecorder(std::shared_ptr<DSP> dsp, DownlinkConfig &downlink, SatelliteConfig satelliteConfig, std::string fileName);
    void start();
    void stop();
};