#pragma once

#include "tle.h"
#include "satellite_pass.h"
#include "satellite_station.h"
#include <ctime>
#include <vector>
#include <predict/predict.h>

class OrbitPredictor
{
private:
    int norad_m;
    TLE tle_m;
    SatelliteStation station_m;
    predict_orbital_elements_t *predict_satellite;
    predict_observer_t *predict_observer;

public:
    OrbitPredictor(int norad, TLE tle, SatelliteStation station);
    SatellitePass getNextPass(std::time_t time);
    SatellitePass getNextPassOver(std::time_t time, float elevation);
    std::vector<SatellitePass> getPassesBetweenOver(std::time_t start_time, std::time_t end_time, float elevation);
};