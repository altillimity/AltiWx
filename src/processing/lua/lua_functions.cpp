#include "lua_functions.h"
#include <filesystem>

bool file_exists(std::string file)
{
    return std::filesystem::exists(file);
}

void bindCustomLuaFunctions(sol::state &lua)
{
    lua.set_function("file_exists", &file_exists);
}