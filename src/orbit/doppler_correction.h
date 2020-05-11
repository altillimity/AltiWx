#pragma once

#include "tle.h"
#include "satellite_station.h"
#include <ctime>
#include <vector>
#include <predict/predict.h>

class DopplerCorrector
{
private:
    predict_orbital_elements_t *predict_satellite;
    predict_observer_t *predict_observer;
    predict_julian_date_t predict_current_time;
    predict_observation predict_satellite_observation;
    predict_position predict_satellite_orbit;
    long frequency_m;
    long dopplerFrequency;

public:
    DopplerCorrector(TLE tle, SatelliteStation station);
    long correctDoppler(long &frequency);
};