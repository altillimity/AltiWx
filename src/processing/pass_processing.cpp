#include "pass_processing.h"
#include "logger/logger.h"
#include <filesystem>
#include "config/config.h"
#include "downlink_recorder.h"
#include "processing_script.h"

// Generate output filename / path
//std::string generateFilepath(SatellitePass &satellitePass, SatelliteConfig &satelliteConfig, DownlinkConfig &downlinkConfig, TLE &tle)
std::pair<std::string, std::string> generateFilepath(SatellitePass &satellitePass, SatelliteConfig &satelliteConfig, DownlinkConfig &downlinkConfig, TLE &tle)
{
    std::tm *timeReadable = gmtime(&satellitePass.aos);

    std::string utc_timestamp = std::to_string(timeReadable->tm_year + 1900) +                                                                               // Year yyyy
                                (timeReadable->tm_mon + 1 > 9 ? std::to_string(timeReadable->tm_mon + 1) : "0" + std::to_string(timeReadable->tm_mon + 1)) + // Month MM
                                (timeReadable->tm_mday > 9 ? std::to_string(timeReadable->tm_mday) : "0" + std::to_string(timeReadable->tm_mday)) + "T" +    // Day dd
                                (timeReadable->tm_hour > 9 ? std::to_string(timeReadable->tm_hour) : "0" + std::to_string(timeReadable->tm_hour)) +          // Hour HH
                                (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min)) +             // Minutes mm
                                (timeReadable->tm_sec > 9 ? std::to_string(timeReadable->tm_sec) : "0" + std::to_string(timeReadable->tm_sec)) + "Z";        // Seconds ss

    //std::string name = tle.object_name + "_" + downlinkConfig.name + "_" + std::to_string(timeReadable->tm_year + 1900) +
    //                   "-" + std::to_string(timeReadable->tm_mon + 1) + "-" + std::to_string(timeReadable->tm_mday) +
    //                   "--" + std::to_string(timeReadable->tm_hour) + ":" + (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min));

    std::string workdDir = configManager->getConfig().data_directory + "/" + tle.object_name + "/" + downlinkConfig.name + "/" + utc_timestamp;

    std::filesystem::create_directories(workdDir);

    //return workdDir + "/" + utc_timestamp;
    return std::make_pair(workdDir + "/" + utc_timestamp, workdDir);
}

struct ToProcess
{
    std::string filename;
    std::string workdir;
    std::string filePath;
    std::string script;
    DownlinkConfig downlink;
    long samplerate;
};

void processSatellitePass(SatellitePass satPass, std::shared_ptr<DeviceDSP> dsp, TLE tle)
{
    logger->info("AOS " + tle.object_name);

    SatelliteConfig satellite_config = configManager->getConfig().getSatelliteConfig(satPass.norad);

    // Return the sdr!
    long band = getBandForSatellite(satellite_config);
    dsp->setFrequency(band);

    // Save every recorded file
    std::vector<ToProcess> filePaths;

    // Attach all downlink recorders
    std::vector<std::shared_ptr<DownlinkRecorder>> downlink_recorders;
    for (DownlinkConfig &downlinkConfig : satellite_config.downlinkConfigs)
    {
        logger->debug("Adding recorder for " + downlinkConfig.name + " downlink on " + std::to_string(downlinkConfig.frequency) + " Hz");

        // Generate filename / path, store them and setup recorder
        std::pair<std::string, std::string> pgenFilepath = generateFilepath(satPass, satellite_config, downlinkConfig, tle);
        std::string filename = pgenFilepath.first;
        std::string workdir = pgenFilepath.second;
        std::string filepath = filename + "." + downlinkConfig.output_extension;
        logger->debug("Using file path " + filepath);

        std::shared_ptr<DownlinkRecorder> recorder = std::make_shared<DownlinkRecorder>(dsp, downlinkConfig, satellite_config, tle, filepath);
        downlink_recorders.push_back(recorder);
        filePaths.push_back({filename, workdir, filepath, downlinkConfig.post_processing_script, downlinkConfig, downlinkConfig.bandwidth});
    }

    // Start recording all downlinks
    for (std::shared_ptr<DownlinkRecorder> &recorder : downlink_recorders)
        recorder->start();

    // Wait until LOS
    while (time(NULL) <= satPass.los)
        std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop recording, the pass is over!
    for (std::shared_ptr<DownlinkRecorder> &recorder : downlink_recorders)
        recorder->stop();

    logger->info("LOS " + tle.object_name);

    logger->info("Processing data for " + tle.object_name);

    // Run processing scripts
    std::vector<std::string> finalFiles;
    for (ToProcess fileToProcess : filePaths)
    {
        logger->debug("Processing " + fileToProcess.filename + " with " + fileToProcess.script);

        // Maybe nothing has to be processed? Then skip!
        if (fileToProcess.script == "none")
        {
            logger->debug("No processing script! Skipping...");
            continue;
        }

        std::string scriptFullPath = "scripts/" + fileToProcess.script;

        // Check the script exists
        if (!std::filesystem::exists(scriptFullPath))
        {
            logger->critical("Script " + scriptFullPath + " does not exist!");
            continue;
        }

        ProcessingScript currentProcessor(satPass, satellite_config, fileToProcess.downlink, tle, fileToProcess.filePath, fileToProcess.filename, fileToProcess.workdir, scriptFullPath);
        currentProcessor.process();
    }
}