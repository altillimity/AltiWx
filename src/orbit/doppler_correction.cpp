#include "doppler_correction.h"
#include <math.h>

#define SPEED_OF_LIGHT_M_S (double)299792458

DopplerCorrector::DopplerCorrector(TLE tle, SatelliteStation station)
{
    predict_satellite = predict_parse_tle(tle.tle_1.c_str(), tle.tle_2.c_str());
    predict_observer = predict_create_observer("Station", station.latitude * M_PI / 180, station.longitude * M_PI / 180, station.altitude);
}

long DopplerCorrector::correctDoppler(long &frequency)
{
    predict_current_time = predict_to_julian(time(NULL));
    predict_orbit(predict_satellite, &predict_satellite_orbit, predict_current_time);
    predict_observe_orbit(predict_observer, &predict_satellite_orbit, &predict_satellite_observation);
    dopplerFrequency = (predict_satellite_observation.range_rate * 1000 / SPEED_OF_LIGHT_M_S) * frequency * (-1.0);
    return frequency + dopplerFrequency;
}