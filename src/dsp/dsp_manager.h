#pragma once

#include "dsp.h"
#include <memory>

extern std::shared_ptr<DSP> rtlDSP;

void initDSP();

void stopDSP();