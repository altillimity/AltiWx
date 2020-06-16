#pragma once

#include <scheduler/bosma/Scheduler.h>
#include <memory>

// Main scheduler object
extern std::shared_ptr<Bosma::Scheduler> globalScheduler;

// Init the scheduler
void initScheduler();