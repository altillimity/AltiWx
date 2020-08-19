#include "altiwx/plugin.h"
#include "altiwx/altiwx.h"
#include "altiwx/events/events.h"
#include "modems/aprs_file_modem.h"

class APRS : public altiwx::Plugin
{
public:
    std::string getID()
    {
        return "aprs";
    }

    void init(std::shared_ptr<spdlog::logger> &logger)
    {
        altiwx::eventBus->register_handler<altiwx::events::RegisterModemsEvent>([&logger](const altiwx::events::RegisterModemsEvent &evt) {
            logger->debug("Registering APRS modems");
            evt.modemRegistry.emplace(ModemAPRSFile::getType(), ModemAPRSFile::getInstance);
        });
    }
};

PLUGIN_LOADER(APRS)