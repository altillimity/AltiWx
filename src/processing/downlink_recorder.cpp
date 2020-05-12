#include "downlink_recorder.h"
#include "logger/logger.h"
#include "dsp/modem/modem_fm.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"

DownlinkRecorder::DownlinkRecorder(std::shared_ptr<DSP> dsp, DownlinkConfig &downlink, SatelliteConfig satelliteConfig, std::string fileName) : dsp_m(dsp), downlink_m(downlink), satelliteConfig_m(satelliteConfig), running(true)
{
    logger->debug("Setting up recorder for " + downlink_m.name + " downlink on " + std::to_string(downlink_m.frequency) + " Hz. Bandwidth " + std::to_string(downlink_m.bandwidth) + " Hz");

    modemID = downlink_m.name + "-" + std::to_string(downlink.frequency);

    switch (downlink_m.modemType)
    {
    case FM:
        modem = std::make_shared<ModemFM>(downlink_m.frequency, downlink_m.bandwidth, downlink_m.modem_audioSamplerate, fileName);
        break;
    default:
        logger->critical("Invalid modem type!");
    }

    if (downlink_m.dopplerCorrection)
        dopplerCorrector = std::make_shared<DopplerCorrector>(getTLEFromNORAD(satelliteConfig.norad), configManager->getConfig().station);
}

void DownlinkRecorder::start()
{
    logger->info("Recording " + downlink_m.name + " downlink on " + std::to_string(downlink_m.frequency) + " Hz. Bandwidth " + std::to_string(downlink_m.bandwidth) + " Hz");
    dsp_m->attachModem(modemID, modem);
    if (downlink_m.dopplerCorrection)
        dopplerThread = std::thread(&DownlinkRecorder::doDoppler, this);
}

void DownlinkRecorder::stop()
{
    running = false;
    if (dopplerThread.joinable())
        dopplerThread.join();
    dsp_m->detachModem(modemID);
    modem->stop();
}

void DownlinkRecorder::doDoppler()
{
    while (running)
    {
        modem->setFrequency(dopplerCorrector->correctDoppler(downlink_m.frequency));
    }
}