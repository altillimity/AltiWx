#pragma once

#include "tle.h"
#include "satellite.h"
#include <ctime>
#include <predict/predict.h>

// Class including everything necessary to perform doppler correction
class DopplerCorrector
{
private:
    predict_orbital_elements_t *predict_satellite;
    predict_observer_t *predict_observer;
    predict_julian_date_t predict_current_time;
    predict_observation predict_satellite_observation;
    predict_position predict_satellite_orbit;
    long d_frequency;
    long d_doppler_frequency;

public:
    DopplerCorrector(TLE tle, SatelliteStation station);
    long correctDoppler(long &frequency);
};