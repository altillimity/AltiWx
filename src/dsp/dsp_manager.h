#pragma once

#include "dsp.h"
#include <memory>

// Main DSP object, will be more flexible later!
extern std::shared_ptr<DSP> rtlDSP;

// Init DSP
void initDSP();

// Stop DSP
void stopDSP();