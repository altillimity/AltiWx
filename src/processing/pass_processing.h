#pragma once

#include "orbit/satellite.h"
#include "dsp/dsp.h"
#include "orbit/tle.h"

void processSatellitePass(SatellitePass satPass, std::shared_ptr<DeviceDSP> dsp, TLE tle);