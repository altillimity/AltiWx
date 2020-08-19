#include "plugins.h"
#include <string>
#include <dlfcn.h>
#include <memory>
#include "logger/logger.h"
#include <filesystem>

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
    logger->info("Loading plugins...");

    std::vector<std::string> pluginsToLoad;

    std::filesystem::recursive_directory_iterator pluginsIterator("plugins");
    std::error_code iteratorError;
    while (pluginsIterator != std::filesystem::recursive_directory_iterator())
    {
        if (!std::filesystem::is_directory(pluginsIterator->path()))
        {
            if (pluginsIterator->path().filename().string().find(".so") != std::string::npos)
            {
                logger->debug("Found plugin " + pluginsIterator->path().relative_path().string());
                pluginsToLoad.push_back(pluginsIterator->path().relative_path().string());
            }
        }

        pluginsIterator.increment(iteratorError);
        if (iteratorError)
            logger->critical(iteratorError.message());
    }

    for (std::string plugin : pluginsToLoad)
    {
        try
        {
            plugins.push_back(loadPlugin(plugin));
        }
        catch (std::exception &e)
        {
            logger->critical(e.what());
        }
    }

    logger->info("Loaded plugins (" + std::to_string(plugins.size()) + ") :");
    for (std::shared_ptr<altiwx::Plugin> &pl : plugins)
        logger->info(" - " + pl->getID());
}