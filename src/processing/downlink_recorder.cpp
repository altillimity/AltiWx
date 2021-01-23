#include "downlink_recorder.h"
#include "logger/logger.h"
#include "dsp/modem/modem.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"

DownlinkRecorder::DownlinkRecorder(std::shared_ptr<DeviceDSP> dsp, DownlinkConfig &downlink, SatelliteConfig satellite_config, TLE &tle, std::string filename) : d_dsp(dsp),
                                                                                                                                                                 d_downlink(downlink),
                                                                                                                                                                 d_satellite_config(satellite_config),
                                                                                                                                                                 running(false)
{
    logger->debug("Setting up recorder for " + d_downlink.name +
                  " downlink on " + std::to_string(d_downlink.frequency) +
                  " Hz. Bandwidth " + std::to_string(d_downlink.bandwidth) + " Hz");

    // Generate a unique ModemID
    modem_id = std::to_string(d_satellite_config.norad) + "-" + d_downlink.name + "-" + std::to_string(downlink.frequency);

    std::map<std::string, std::string> parameters = d_downlink.modem_parameters;
    parameters["file"] = filename;

    // Init our Modem object
    if (modem_registry.count(d_downlink.modem_type) > 0)
        modem = modem_registry[d_downlink.modem_type](d_downlink.frequency, d_downlink.bandwidth, parameters, 8192);
    else
        logger->critical("Invalid modem type : " + d_downlink.modem_type + "!");

    // If dopper is enabled, set it up
    if (d_downlink.doppler_correction)
        dopplerCorrector = std::make_shared<DopplerCorrector>(tle, configManager->getConfig().station);
}

void DownlinkRecorder::start()
{
    // Attach modem to DSP and start doppler if enabled
    logger->info("Recording " + d_downlink.name + " downlink on " + std::to_string(d_downlink.frequency) + " Hz. Bandwidth " + std::to_string(d_downlink.bandwidth) + " Hz");
    d_dsp->attachModem(modem_id, modem);
    if (d_downlink.doppler_correction)
        doppler_thread = std::thread(&DownlinkRecorder::doDoppler, this);
}

void DownlinkRecorder::stop()
{
    // Detach modem and stop doppler thread
    if (running)
    {
        running = false;
        if (doppler_thread.joinable())
            doppler_thread.join();
    }
    d_dsp->detachModem(modem_id);
    modem->stop();
}

void DownlinkRecorder::doDoppler()
{
    running = true;

    // Perform doppler correction every 100ms.
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        modem->setFrequency(dopplerCorrector->correctDoppler(d_downlink.frequency));
    }
}