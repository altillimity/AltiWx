#include "logger/logger.h"
#include "orbit/orbit_predictor.h"
#include "orbit/tle_manager.h"

int main(int argc, char *argv[])
{
    initLogger();
    logger->info("Starting AutoWx...");

    //logger->info("Predicting passes for METEOR-M 2");

    std::vector<int> norads;
    norads.push_back(40069);

    startTLEManager(norads);

    SatelliteStation station = {0.0, 0.0, 0.0};

    OrbitPredictor predictor(40069, getTLEFromNORAD(40069), station);
    std::vector<SatellitePass> nextPasses = predictor.getPassesBetweenOver(time(NULL), time(NULL) + 24 * 60 * 60, 10.0f);

    for (SatellitePass nextPass : nextPasses)
    {
        logger->info("Next pass of " + nextPass.tle.name);
        logger->info("AOS : " + std::to_string(nextPass.aos));
        logger->info("LOS : " + std::to_string(nextPass.los));
        logger->info("El. : " + std::to_string(std::round(nextPass.elevation * 10) / 10));
    }
}