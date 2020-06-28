#pragma once

#include "dsp.h"
#include <memory>

// Main DSP object, will be more flexible later!
extern std::unordered_map<std::string, std::shared_ptr<DSP>> radioList;

// Init DSP
void initDSP();

// Stop DSP
void stopDSP();