#pragma once

#include <spdlog/spdlog.h>
#include <memory>

extern std::shared_ptr<spdlog::logger> logger;

void initLogger();

void setConsoleLevel(spdlog::level::level_enum level);