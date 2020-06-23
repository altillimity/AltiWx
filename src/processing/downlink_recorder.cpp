#include "downlink_recorder.h"
#include "logger/logger.h"
#include "dsp/modem/modem_fm.h"
#include "dsp/modem/modem_iq.h"
#include "dsp/modem/modem_iqwav.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"
#include "dsp/modem/modem_lrpt.h"

DownlinkRecorder::DownlinkRecorder(std::shared_ptr<DSP> dsp, DownlinkConfig &downlink, SatelliteConfig satelliteConfig, std::string fileName) : dsp_m(dsp), downlink_m(downlink), satelliteConfig_m(satelliteConfig), running(true)
{
    logger->debug("Setting up recorder for " + downlink_m.name + " downlink on " + std::to_string(downlink_m.frequency) + " Hz. Bandwidth " + std::to_string(downlink_m.bandwidth) + " Hz");

    // Generate a unique ModemID
    modemID = satelliteConfig.norad + "-" + downlink_m.name + "-" + std::to_string(downlink.frequency);

    // Init our Modem object
    switch (downlink_m.modemType)
    {
    case FM:
        modem = std::make_shared<ModemFM>(downlink_m.frequency, downlink_m.bandwidth, downlink_m.modem_audioSamplerate, fileName);
        break;
    case IQ:
        modem = std::make_shared<ModemIQ>(downlink_m.frequency, downlink_m.bandwidth, fileName);
        break;
    case IQWAV:
        modem = std::make_shared<ModemIQWav>(downlink_m.frequency, downlink_m.bandwidth, fileName);
        break;
    case LRPT:
        modem = std::make_shared<ModemLRPT>(downlink_m.frequency, downlink_m.bandwidth, fileName);
        break;
    default:
        logger->critical("Invalid modem type!");
    }

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