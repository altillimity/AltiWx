#pragma once

#include "processing/sol/sol.hpp"
#include <spdlog/spdlog.h>

// Custom logger usable in scripts
class LuaLogger
{
private:
    std::string name_m;
    std::shared_ptr<spdlog::logger> logger_m;

public:
    LuaLogger(std::shared_ptr<spdlog::logger> logger, std::string name);
    void debug(std::string log);
    void info(std::string log);
    void warn(std::string log);
    void error(std::string log);
    void critical(std::string log);
};

void bindLogger(sol::state &lua, std::string name);