#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
std::shared_ptr<spdlog::logger> logger;

void initLogger()
{

    console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/logs.txt", 0, 0);
    logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("AutoWx", {console_sink, file_sink}));

    console_sink->set_pattern("[%D - %T] %^(%L) %v%$");
    file_sink->set_pattern("[%D - %T] (%L) %v");

    file_sink->set_level(spdlog::level::trace);
    console_sink->set_level(spdlog::level::trace);

    logger->set_level(spdlog::level::trace);
}