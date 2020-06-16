#include "lua_functions.h"
#include <filesystem>

// Test if a file exists
bool file_exists(std::string file)
{
    return std::filesystem::exists(file);
}

// Bind everything
void bindCustomLuaFunctions(sol::state &lua)
{
    lua.set_function("file_exists", &file_exists);
}