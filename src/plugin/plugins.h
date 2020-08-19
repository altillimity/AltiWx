#include <map>
#include <memory>
#include "api/altiwx/plugin.h"

std::shared_ptr<altiwx::Plugin> loadPlugin(std::string plugin);
void initPlugins();