#include "downlink_recorder.h"
#include "logger/logger.h"
#include "dsp/modem/modem.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
DownlinkRecorder::DownlinkRecorder(std::shared_ptr<DSP> dsp, DownlinkConfig &downlink, SatelliteConfig satelliteConfig, std::string fileName) : dsp_m(dsp),
                                                                                                                                                downlink_m(downlink),
                                                                                                                                                satelliteConfig_m(satelliteConfig),
                                                                                                                                                running(true)
{
    logger->debug("Setting up recorder for " + downlink_m.name + " downlink on " + std::to_string(downlink_m.frequency) + " Hz. Bandwidth " + std::to_string(downlink_m.bandwidth) + " Hz");

    // Generate a unique ModemID
    modemID = std::to_string(satelliteConfig.norad) + "-" + downlink_m.name + "-" + std::to_string(downlink.frequency);

    // Init our Modem object
    if (modemRegistry.find(downlink_m.modemType) != modemRegistry.end())
        modem = modemRegistry[downlink_m.modemType]();
    else
        logger->critical("Invalid modem type!");

    std::unordered_map<std::string, std::string> parameters = downlink_m.modemParameters;
    parameters["output_file"] = fileName;
    modem->setParameters(downlink_m.frequency, downlink_m.bandwidth, parameters);

    // If dopper is enabled, set it up
    if (downlink_m.dopplerCorrection)
        dopplerCorrector = std::make_shared<DopplerCorrector>(getTLEFromNORAD(satelliteConfig.norad), configManager->getConfig().station);
}

void DownlinkRecorder::start()
{
    // Attach modem to DSP and start doppler if enabled
    logger->info("Recording " + downlink_m.name + " downlink on " + std::to_string(downlink_m.frequency) + " Hz. Bandwidth " + std::to_string(downlink_m.bandwidth) + " Hz");
    dsp_m->attachModem(modemID, modem);
    if (downlink_m.dopplerCorrection)
        dopplerThread = std::thread(&DownlinkRecorder::doDoppler, this);
}

void DownlinkRecorder::stop()
{
    // Detach modem and stop doppler thread
    running = false;
    if (dopplerThread.joinable())
        dopplerThread.join();
    dsp_m->detachModem(modemID);
    modem->stop();
}

void DownlinkRecorder::doDoppler()
{
    // Perform doppler correction every 100ms.
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        modem->setFrequency(dopplerCorrector->correctDoppler(downlink_m.frequency));
    }
}