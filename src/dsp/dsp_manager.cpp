#include "dsp_manager.h"
#include "logger/logger.h"
#include "config/config.h"

std::unordered_map<std::string, std::shared_ptr<DSP>> radioList;

void initDSP()
{
    // Log everything and start DSP
    logger->info("Starting DSP...");
    for (SDRConfig currentRadioConfig : configManager->getConfig().sdrConfigs)
    {
        logger->info("Initializing radio " + currentRadioConfig.name);
        logger->debug("Frequency  (Hz)  : " + std::to_string(currentRadioConfig.centerFrequency));
        logger->debug("Samplerate (S/s) : " + std::to_string(currentRadioConfig.sampleRate));
        logger->debug("Gain       (dB)  : " + std::to_string(currentRadioConfig.gain));
        std::shared_ptr<DSP> currentRadioDSP = std::make_shared<DSP>(currentRadioConfig);
        radioList.emplace(currentRadioConfig.name, currentRadioDSP);
        currentRadioDSP->start();
    }
}

void stopDSP()
{
    logger->info("Stopping DSP...");
    for (std::pair<std::string, std::shared_ptr<DSP>> currentRadio : radioList)
        currentRadio.second->stop();
}