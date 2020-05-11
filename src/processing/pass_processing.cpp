#include "pass_processing.h"
#include "logger/logger.h"
#include "config/config.h"
#include "downlink_recorder.h"
#include "dsp/dsp_manager.h"

std::string generateFilepath(SatellitePass &satellitePass, SatelliteConfig &satelliteConfig, DownlinkConfig &downlinkConfig)
{
    std::tm *timeReadable = gmtime(&satellitePass.aos);

    return satelliteConfig.getName() + "_" + downlinkConfig.name + "_" + std::to_string(timeReadable->tm_year) + 
    "-" + std::to_string(timeReadable->tm_mon) + "-" + std::to_string(timeReadable->tm_mday) + 
    "---" + std::to_string(timeReadable->tm_hour) + ":" + (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min)) +
    ".wav";
}

void processPass(SatellitePass pass)
{
    SatelliteConfig satelliteConfig = configManager->getConfig().getSatelliteConfigFromNORAD(pass.norad);
    logger->info("AOS " + pass.tle.name);

    std::vector<std::string> filePaths;

    std::vector<std::shared_ptr<DownlinkRecorder>> downlinkRecoders;
    for (DownlinkConfig &downlinkConfig : satelliteConfig.downlinkConfigs)
    {
        logger->debug("Adding recorder for " + downlinkConfig.name + " downlink on " + std::to_string(downlinkConfig.frequency) + " Hz");

        std::string filePath = generateFilepath(pass, satelliteConfig, downlinkConfig);
        filePaths.push_back(filePath);
        logger->debug("Using file path " + filePath);

        std::shared_ptr<DownlinkRecorder> recorder = std::make_shared<DownlinkRecorder>(rtlDSP, downlinkConfig, satelliteConfig, filePath);
        downlinkRecoders.push_back(recorder);
    }

    for (std::shared_ptr<DownlinkRecorder> &recorder : downlinkRecoders)
        recorder->start();

    while (time(NULL) <= pass.los)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    for (std::shared_ptr<DownlinkRecorder> &recorder : downlinkRecoders)
        recorder->stop();

    logger->info("LOS " + pass.tle.name);
}