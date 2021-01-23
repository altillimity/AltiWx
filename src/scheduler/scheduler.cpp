#include "scheduler.h"
#include "logger/logger.h"

std::shared_ptr<Bosma::Scheduler> global_scheduler;

void initScheduler()
{
    // Start the scheduler
    int thread_num = 10;
    logger->debug("Starting scheduler with " + std::to_string(thread_num) + " workers");
    global_scheduler = std::make_shared<Bosma::Scheduler>(thread_num);
}