#pragma once

#include "tle_manager.h"
#include "scheduler/scheduler.h"
#include "dsp/dsp.h"

class SatellitePassManager
{
private:
    std::shared_ptr<DeviceDSP> d_dsp;
    TLEManager &d_tle_manager;
    std::shared_ptr<Bosma::Scheduler> d_scheduler;

public:
    SatellitePassManager(std::shared_ptr<DeviceDSP> dsp, TLEManager &tle_manager, std::shared_ptr<Bosma::Scheduler> scheduler);
    void schedulePasses();
};