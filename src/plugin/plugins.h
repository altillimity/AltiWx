#include <map>
#include <memory>
#include "api/includes/altiwx/plugin.h"

std::shared_ptr<altiwx::Plugin> loadPlugin(std::string plugin);