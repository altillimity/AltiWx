#include "dsp_manager.h"
#include "logger/logger.h"
#include "config/config.h"

std::shared_ptr<DSP> rtlDSP;

void initDSP() {
    logger->info("Starting DSP...");
    SDRConfig sdrConfig = configManager->getConfig().sdrConfig;
    logger->debug("Frequency  (Hz)  : " + std::to_string(sdrConfig.centerFrequency));
    logger->debug("Samplerate (S/s) : " + std::to_string(sdrConfig.sampleRate));
    logger->debug("Gain       (dB)  : " + std::to_string(sdrConfig.gain));
    rtlDSP = std::make_shared<DSP>(sdrConfig.sampleRate, sdrConfig.centerFrequency, sdrConfig.gain, sdrConfig.soapy, sdrConfig.soapySocket);
    rtlDSP->start();
}

void stopDSP() {
    logger->info("Stopping DSP...");
    rtlDSP->stop();
}