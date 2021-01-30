#include <map>
#include <memory>
#include "api/altiwx/plugin.h"

#define PLUGIN_FOLDER_PATH "plugins"

std::shared_ptr<altiwx::Plugin> loadPlugin(std::string plugin);
void initPlugins();