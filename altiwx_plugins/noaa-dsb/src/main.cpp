#include "altiwx/plugin.h"
#include "altiwx/altiwx.h"
#include "altiwx/events/events.h"

#include "modem_dsb.h"

// A sample plugin
class NoaaDSB : public altiwx::Plugin
{
public:
    // Return the plugin ID
    std::string getID()
    {
        return "noaa-dsb";
    }

    // Plugin init, to register events, and everything else
    void init(std::shared_ptr<spdlog::logger> &logger)
    {
        // Log to the world!
        //logger->debug("Sample AltiWx plugin loading... Thought I'd let you know!");

        // Register an event to be called when AltiWx is finished starting up
        altiwx::eventBus->register_handler<altiwx::events::StartedEvent>([&logger](const altiwx::events::StartedEvent &evt) {
            //logger->critical("This is the Sample plugin, apparently AltiWx just finished starting.");
        });

        // Register our custom modem
        altiwx::eventBus->register_handler<altiwx::events::RegisterModemsEvent>([&logger](const altiwx::events::RegisterModemsEvent &evt) {
            //logger->debug("Registering Sample plugin modems");
            evt.modemRegistry.emplace(ModemNOAADSB::getType(), ModemNOAADSB::getInstance);
        });
    }
};

// Setup a loader for AltiWx to load it, macro to make it easier
PLUGIN_LOADER(NoaaDSB)