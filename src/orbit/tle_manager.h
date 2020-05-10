#pragma once

#include "tle.h"
#include <vector>

TLE getTLEFromNORAD(int norad);
void startTLEManager(std::vector<int> &norads);