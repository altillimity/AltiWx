#include "doppler_correction.h"
#include <math.h>

// Pretty self-explanatory
#define SPEED_OF_LIGHT_M_S (double)299792458

DopplerCorrector::DopplerCorrector(TLE tle, SatelliteStation station)
{
    // Setup libpredict objects
    predict_satellite = predict_parse_tle(tle.tle_line_1.c_str(), tle.tle_line_2.c_str());
    predict_observer = predict_create_observer("Station", station.latitude * M_PI / 180.0f, station.longitude * M_PI / 180.0f, station.altitude);
}

long DopplerCorrector::correctDoppler(long &frequency)
{
    // Compute current satellite position and speed, then calculate doppler shift and resulting receiving frequency
    predict_current_time = predict_to_julian(time(NULL));
    predict_orbit(predict_satellite, &predict_satellite_orbit, predict_current_time);
    predict_observe_orbit(predict_observer, &predict_satellite_orbit, &predict_satellite_observation);
    d_doppler_frequency = (predict_satellite_observation.range_rate * 1000 / SPEED_OF_LIGHT_M_S) * frequency * (-1.0);
    return frequency + d_doppler_frequency;
}