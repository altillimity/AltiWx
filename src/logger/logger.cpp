#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

// Logger and sinks. We got a console sink and file sink
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
std::shared_ptr<spdlog::logger> logger;

void initLogger()
{
    // Prevent libraries or programs from showing unwanted logs
    freopen("/dev/null", "w", stderr);

    // Initialize everything
    console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/logs.txt", 0, 0);
    logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("AltiWx", {console_sink, file_sink}));

    // Use a custom, nicer log pattern. No color in the file
    console_sink->set_pattern("[%D - %T] %^(%L) %v%$");
    file_sink->set_pattern("[%D - %T] (%L) %v");

    // Default log level
    file_sink->set_level(spdlog::level::trace);
    console_sink->set_level(spdlog::level::trace);
    logger->set_level(spdlog::level::trace);
}

void setConsoleLevel(spdlog::level::level_enum level) {
    // Just change out log level
    console_sink->set_level(level);
}