#include "orbit_predictor.h"
#include <math.h>

#include "logger/logger.h"

OrbitPredictor::OrbitPredictor(int norad, TLE tle, SatelliteStation station) : norad_m(norad), tle_m(tle), station_m(station)
{
    // Init libpredict objects
    predict_satellite = predict_parse_tle(tle.tle_1.c_str(), tle.tle_2.c_str());
    predict_observer = predict_create_observer("Station", station.latitude * M_PI / 180, station.longitude * M_PI / 180, station.altitude);
}

SatellitePass OrbitPredictor::getNextPass(std::time_t time)
{
    predict_julian_date_t predict_time = predict_to_julian(time);

    // Get next LOS & AOS
    predict_observation predict_aos, predict_los;
    predict_aos = predict_next_aos(predict_observer, predict_satellite, predict_time);
    predict_los = predict_next_los(predict_observer, predict_satellite, predict_time);

    // Convert to UTC
    std::time_t aosTime = predict_from_julian(predict_aos.time);
    std::time_t losTime = predict_from_julian(predict_los.time);

    float elevation = 0.0f;
    bool northbound = false, southboud = false;

    // Compute max elevation and pass direction
    for (std::time_t current_time = aosTime; current_time < losTime; current_time++)
    {
        predict_observation predict_observation;
        predict_position predict_orbit_obj;
        predict_orbit(predict_satellite, &predict_orbit_obj, predict_to_julian(current_time));
        predict_observe_orbit(predict_observer, &predict_orbit_obj, &predict_observation);

        float current_elevation = predict_observation.elevation * 180.0 / M_PI;
        if (current_elevation > elevation)
            elevation = current_elevation;

        if (current_time == aosTime)
        {
            if (predict_orbit_obj.latitude > predict_observer->latitude)
                southboud = true;
            else
                northbound = true;
        }
    }

    // Return SatellitePass
    return {norad_m, tle_m, aosTime, losTime, elevation, northbound, southboud};
}

SatellitePass OrbitPredictor::getNextPassOver(std::time_t time, float elevation)
{
    float pass_elevation = 0.0f;
    SatellitePass pass;

    // Loop until we found a pass over asked elevation. May need failsafe in case of input over 90°?
    do
    {
        pass = getNextPass(time);
        time = pass.los + 1; // To make sure it won't get stuck on the same pass
    } while (pass_elevation <= elevation);

    return pass;
}

std::vector<SatellitePass> OrbitPredictor::getPassesBetweenOver(std::time_t start_time, std::time_t end_time, float elevation)
{
    std::vector<SatellitePass> passes;

    // Loop until we're out of the timespan
    std::time_t time = start_time;
    while (time < end_time)
    {
        // Get the next pass
        SatellitePass pass = getNextPass(time);
        if (pass.elevation >= elevation && pass.aos <= end_time) // Only add it in if the elevation criteria matches
            passes.push_back(pass);
        time = pass.los + 1;
    }

    return passes;
}