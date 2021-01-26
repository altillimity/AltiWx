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
    std::vector<SatellitePass> all_passes, passes;

    for (SatelliteConfig &satellite : configManager->getConfig().satellite_configs)
    {
        TLE tle = d_tle_manager.getTLE(satellite.norad);
        // Predict passes fullfilling requested configuration and add store them
        logger->info("Predicting passes for " + tle.object_name);
        if (tle.object_name.find("No TLE found") != std::string::npos)
            continue;
        OrbitPredictor predictor(satellite.norad, tle, configManager->getConfig().station);
        std::vector<SatellitePass> predictedPasses = predictor.getPassesBetweenOver(time(NULL), time(NULL) + 24 * 60 * 60, satellite.min_elevation);
        all_passes.insert(all_passes.end(), predictedPasses.begin(), predictedPasses.end());
    }

    // Sort by order (AOS)
    std::sort(all_passes.begin(), all_passes.end(), [](const SatellitePass &v1, const SatellitePass &v2) { return v1.aos < v2.aos; });

    // Print all possible passes
    for (SatellitePass pass : all_passes)
    {
        if (!(pass.aos < pass.los))
            continue;

        TLE tle = d_tle_manager.getTLE(pass.norad);
        // Write to the console and schedule it
        std::tm tm_aos = *gmtime(&pass.aos);
        std::tm tm_los = *gmtime(&pass.los);
        logger->debug("Possible pass of " + tle.object_name +
                      " at " + std::to_string(tm_aos.tm_hour) + ":" +
                      (tm_aos.tm_min > 9 ? std::to_string(tm_aos.tm_min) : "0" + std::to_string(tm_aos.tm_min)) +
                      " with " + std::to_string(std::round(pass.elevation * 10) / 10) + "° elevation, " +
                      (pass.direction == NORTHBOUND ? "Northbound" : "Southbound") +
                      " AOS " + std::to_string(tm_aos.tm_hour) + ":" + (tm_aos.tm_min > 9 ? std::to_string(tm_aos.tm_min) : "0" + std::to_string(tm_aos.tm_min)) +
                      " LOS " + std::to_string(tm_los.tm_hour) + ":" + (tm_los.tm_min > 9 ? std::to_string(tm_los.tm_min) : "0" + std::to_string(tm_los.tm_min)));
    }

    // Compute an actually doable pass schedule, eg, avoid trying to record on 2 different bands...
    for (int i = 0; i < all_passes.size() - 1; i++)
    {
        SatellitePass &currentPass = all_passes[i];
        SatellitePass &nextPass = all_passes[i + 1];

        SatelliteConfig currentSatellite = configManager->getConfig().getSatelliteConfig(currentPass.norad);
        SatelliteConfig nextSatellite = configManager->getConfig().getSatelliteConfig(nextPass.norad);

        long currentBand = getBandForSatellite(currentSatellite);
        long nextBand = getBandForSatellite(nextSatellite);

        // This is very basic... But should do the trick for now
        if (currentPass.los >= nextPass.aos && currentBand != nextBand)
        {
            logger->warn("PASS OVERLAP bewtween " + d_tle_manager.getTLE(currentPass.norad).object_name +
                         " and " + d_tle_manager.getTLE(nextPass.norad).object_name + " (" + std::to_string(currentBand) +
                         " and " + std::to_string(nextBand) + ")");
            if (currentSatellite.priority > nextSatellite.priority)
            {
                passes.push_back(currentPass);
                nextPass.aos = currentPass.los + 2;
            }
            else if (currentPass.elevation > nextSatellite.min_elevation)
            {
                passes.push_back(currentPass);
                nextPass.aos = currentPass.los + 2;
            }
            else
            {
                currentPass.los = nextPass.aos - 2;
                passes.push_back(currentPass);
            }
        }
        else
        {
            passes.push_back(currentPass);
        }
    }

    passes.push_back(all_passes[all_passes.size() - 1]);

    for (SatellitePass pass : passes)
    {
        if (!(pass.aos < pass.los))
            continue;

        TLE tle = d_tle_manager.getTLE(pass.norad);
        // Write to the console and schedule it
        std::tm tm_aos = *gmtime(&pass.aos);
        std::tm tm_los = *gmtime(&pass.los);
        logger->info("Scheduling pass of " + tle.object_name +
                     " at " + std::to_string(tm_aos.tm_hour) + ":" +
                     (tm_aos.tm_min > 9 ? std::to_string(tm_aos.tm_min) : "0" + std::to_string(tm_aos.tm_min)) +
                     " with " + std::to_string(std::round(pass.elevation * 10) / 10) + "° elevation, " +
                     (pass.direction == NORTHBOUND ? "Northbound" : "Southbound") +
                     " AOS " + std::to_string(tm_aos.tm_hour) + ":" + (tm_aos.tm_min > 9 ? std::to_string(tm_aos.tm_min) : "0" + std::to_string(tm_aos.tm_min)) +
                     " LOS " + std::to_string(tm_los.tm_hour) + ":" + (tm_los.tm_min > 9 ? std::to_string(tm_los.tm_min) : "0" + std::to_string(tm_los.tm_min)));
        d_scheduler->in(std::chrono::system_clock::from_time_t(pass.aos), processSatellitePass, pass, d_dsp, tle);
    }
}
