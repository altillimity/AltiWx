#pragma once

#include "tle.h"
#include <vector>

// Get TLE from NORAD
TLE getTLEFromNORAD(int norad);
// Init TLE manager
void startTLEManager();