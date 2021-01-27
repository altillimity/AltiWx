#include "sun_predictor.h"
#include <predict/predict.h>
#include <math.h>

float getSunElevationAt(SatelliteStation station, std::time_t time)
{
    predict_observer_t *predict_observer = predict_create_observer("Station", station.latitude * M_PI / 180.0f, station.longitude * M_PI / 180.0f, station.altitude);
    predict_julian_date_t predict_time = predict_to_julian(time);

    // Get next LOS & AOS
    predict_observation predict_sun;
    predict_observe_sun(predict_observer, predict_time, &predict_sun);

    return predict_sun.elevation;
}
