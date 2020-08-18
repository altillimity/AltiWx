#include "altiwx/plugin.h"
#include "altiwx/altiwx.h"
#include "altiwx/events/started_event.h"

class Sample : public altiwx::Plugin
{
public:
    std::string getID()
    {
        return "sample_plugim";
    }

    void init(std::shared_ptr<spdlog::logger> &logger)
    {
        logger->debug("Hi from the AltiWx sample plugin!");

        altiwx::eventBus->register_handler<altiwx::events::StartedEvent>([&logger](const altiwx::events::StartedEvent &evt) {
            logger->critical("This is the Sample plugin, apparently AltiWx just finished started.");
        });
    }
};

PLUGIN_LOADER(Sample)