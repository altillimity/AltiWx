#include "scheduler.h"
#include "logger/logger.h"

std::shared_ptr<Bosma::Scheduler> globalScheduler;

void initScheduler()
{
    int thread_num = 10;
    logger->debug("Starting scheduler with " + std::to_string(thread_num) + " workers");
    globalScheduler = std::make_shared<Bosma::Scheduler>(thread_num);
}