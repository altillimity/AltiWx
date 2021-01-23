#include "pass_manager.h"
#include "config/config.h"
#include "logger/logger.h"
#include "orbit_predictor.h"
#include "processing/pass_processing.h"

SatellitePassManager::SatellitePassManager(std::shared_ptr<DeviceDSP> dsp, TLEManager &tle_manager, std::shared_ptr<Bosma::Scheduler> scheduler) : d_dsp(dsp), d_tle_manager(tle_manager), d_scheduler(scheduler)
{
    // Initial pass prediction + scheduler task
    logger->info("Starting pass scheduler...");
    //d_scheduler->every(std::chrono::system_clock::duration(std::chrono::seconds(24 * 60 * 60)), schedulePasses);
}

// Schedule upcoming passes
void SatellitePassManager::schedulePasses()
{
    std::vector<SatellitePass> passes;

    for (SatelliteConfig &satellite : configManager->getConfig().satellite_configs)
    {
        TLE tle = d_tle_manager.getTLE(satellite.norad);
        // Predict passes fullfilling requested configuration and add store them
        logger->info("Predicting passes for " + tle.object_name);
        if (tle.object_name.find("No TLE found") != std::string::npos)
            continue;
        OrbitPredictor predictor(satellite.norad, tle, configManager->getConfig().station);
        std::vector<SatellitePass> predictedPasses = predictor.getPassesBetweenOver(time(NULL), time(NULL) + 24 * 60 * 60, satellite.min_elevation);
        passes.insert(passes.end(), predictedPasses.begin(), predictedPasses.end());
    }

    // Sort by order (AOS)
    std::sort(passes.begin(), passes.end(), [](const SatellitePass &v1, const SatellitePass &v2) { return v1.aos < v2.aos; });

    for (SatellitePass pass : passes)
    {
        TLE tle = d_tle_manager.getTLE(pass.norad);
        // Write to the console and schedule it
        std::tm *timeReadable = gmtime(&pass.aos);
        logger->info("Scheduling pass of " + tle.object_name +
                     " at " + std::to_string(timeReadable->tm_hour) + ":" +
                     (timeReadable->tm_min > 9 ? std::to_string(timeReadable->tm_min) : "0" + std::to_string(timeReadable->tm_min)) +
                     " with " + std::to_string(std::round(pass.elevation * 10) / 10) + "° elevation, " +
                     (pass.direction == NORTHBOUND ? "Northbound" : "Southbound"));
        d_scheduler->in(std::chrono::system_clock::from_time_t(pass.aos), processSatellitePass, pass, d_dsp, tle);
    }
}
