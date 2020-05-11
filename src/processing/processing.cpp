#include "processing.h"
#include "logger/logger.h"

void processPass(SatellitePass pass)
{
    logger->info("AOS " + pass.tle.name);
    while (time(NULL) <= pass.los)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    logger->info("LOS " + pass.tle.name);
}