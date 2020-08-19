#include "plugins.h"
#include <string>
#include <dlfcn.h>
#include <memory>
#include "logger/logger.h"

std::vector<std::shared_ptr<altiwx::Plugin>> plugins;

std::shared_ptr<altiwx::Plugin> loadPlugin(std::string plugin)
{
    logger->info("Loading plugin " + plugin + "...");

    void *dynlib = dlopen(plugin.c_str(), RTLD_LAZY);
    if (!dynlib)
        throw std::runtime_error("Error loading " + plugin + "!");

    void *create = dlsym(dynlib, "loader");
    const char *dlsym_error = dlerror();
    if (dlsym_error != NULL)
        throw std::runtime_error("Error loading symbols from plugin!");

    altiwx::Plugin *pluginObject = reinterpret_cast<altiwx::Plugin *(*)()>(create)();
    pluginObject->init(logger);
    logger->info("Plugin " + pluginObject->getID() + " loaded!");

    return std::shared_ptr<altiwx::Plugin>(pluginObject);
}

void initPlugins()
{
    try
    {
        plugins.push_back(loadPlugin("plugins/sample/libsamplePlugin.so"));
    }
    catch (std::exception &e)
    {
        logger->critical(e.what());
    }
}