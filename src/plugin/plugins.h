#include <map>
#include <memory>
#include "api/altiwx/plugin.h"

#define PLUGIN_FOLDER_PATH "altiwx_plugins"

std::shared_ptr<altiwx::Plugin> loadPlugin(std::string plugin);
void initPlugins();
