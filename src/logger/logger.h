#pragma once

#include <spdlog/spdlog.h>
#include <memory>

extern std::shared_ptr<spdlog::logger> logger;

void initLogger();