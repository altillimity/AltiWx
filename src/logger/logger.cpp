#include "logger.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include "web_sink.h"

// Logger and sinks. We got a console sink and file sink
std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
std::shared_ptr<spdlog::sinks::daily_file_sink_mt> file_sink;
std::shared_ptr<webSink_mt> web_sink;
std::shared_ptr<spdlog::logger> logger;

std::vector<std::string> logs;

void initLogger()
{
    // Prevent libraries or programs from showing unwanted logs
    freopen("/dev/null", "w", stderr);

    try
    {
        // Initialize everything
        console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>((std::string)LOG_FOLDER_PATH + "/logs.txt", 0, 0);
        web_sink = std::make_shared<webSink_mt>();
        logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("AltiWx", {console_sink, file_sink, web_sink}));

        // Use a custom, nicer log pattern. No color in the file
        console_sink->set_pattern("[%D - %T] %^(%L) %v%$");
        file_sink->set_pattern("[%D - %T] (%L) %v");
        web_sink->set_pattern("[%D - %T] (%L) %v");

        // Default log level
        file_sink->set_level(spdlog::level::trace);
        console_sink->set_level(spdlog::level::trace);
        web_sink->set_level(spdlog::level::debug);
        logger->set_level(spdlog::level::trace);
    }
    catch (std::exception &e)
    {
        spdlog::error(e.what());
        exit(1);
    }
}

void setConsoleLevel(spdlog::level::level_enum level)
{
    // Just change out log level
    console_sink->set_level(level);
}