#pragma once

#include <scheduler/bosma/Scheduler.h>
#include <memory>

extern std::shared_ptr<Bosma::Scheduler> globalScheduler;

void initScheduler();