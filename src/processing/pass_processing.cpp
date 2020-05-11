#include "pass_processing.h"
#include "logger/logger.h"
#include "dsp/dsp_manager.h"
#include "config/config.h"
#include "dsp/modem/modem_fm.h"

void processPass(SatellitePass pass)
{
    SatelliteConfig satelliteConfig = configManager->getConfig().getSatelliteConfigFromNORAD(pass.norad);
    logger->info("AOS " + pass.tle.name);

    for (DownlinkConfig downlinkConfig : satelliteConfig.downlinkConfigs)
    {
        logger->info("Recording " + downlinkConfig.name + " downlink on " + std::to_string(downlinkConfig.frequency) + " Hz");
    }

    std::shared_ptr<ModemFM> modem = std::make_shared<ModemFM>(satelliteConfig.downlinkConfigs[0].frequency, 60000, 11025, "sat.wav");
    rtlDSP->attachModem("name", modem);
    while (time(NULL) <= pass.los)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    rtlDSP->detachModem("name");
    modem->stop();

    logger->info("LOS " + pass.tle.name);
}