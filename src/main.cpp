#include "logger/logger.h"

int main(int argc, char *argv[])
{
    initLogger();
    logger->info("Starting AutoWx...");
}