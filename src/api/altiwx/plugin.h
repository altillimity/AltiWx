#include <memory>
#include "altiwx.h"
#include <spdlog/logger.h>

#define PLUGIN_LOADER(constructor)                 \
    extern "C"                                     \
    {                                              \
        altiwx::Plugin *loader()                   \
        {                                          \
            return (altiwx::Plugin *)new constructor(); \
        }                                          \
    }

namespace altiwx
{
    class Plugin
    {
    public:
        Plugin() {}
        virtual std::string getID() = 0;
        virtual void init(std::shared_ptr<spdlog::logger> &logger) = 0;
        virtual ~Plugin(){};
    };
}; // namespace altiwx