#include "lua_logger.h"
#include "logger/logger.h"

LuaLogger::LuaLogger(std::shared_ptr<spdlog::logger> logger, std::string name) : logger_m(logger), name_m(name)
{
}

// Logger functions
void LuaLogger::debug(std::string log)
{
    logger_m->debug("[" + name_m + "] " + log);
}

void LuaLogger::info(std::string log)
{
    logger_m->info("[" + name_m + "] " + log);
}

void LuaLogger::warn(std::string log)
{
    logger_m->warn("[" + name_m + "] " + log);
}

void LuaLogger::error(std::string log)
{
    logger_m->error("[" + name_m + "] " + log);
}

void LuaLogger::critical(std::string log)
{
    logger_m->critical("[" + name_m + "] " + log);
}

void bindLogger(sol::state &lua, std::string name)
{
    // Bing the logger to provided lua instance
    lua.new_usertype<LuaLogger>("lua_logger", "debug", &LuaLogger::debug, "info", &LuaLogger::info, "warn", &LuaLogger::warn, "error", &LuaLogger::error, "critical", &LuaLogger::critical);
    lua["logger"] = std::make_shared<LuaLogger>(logger, name);
}