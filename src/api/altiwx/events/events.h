#include "communication/packet/packet.h"
#include "dsp/modem/modem.h"

namespace altiwx
{
    namespace events
    {
        struct StartedEvent
        {
            // Nothing
        };

        struct RegisterPacketsEvent
        {
            std::unordered_map<std::string, std::shared_ptr<Packet>> &packtMap;
        };

        struct RegisterModemsEvent
        {
            std::unordered_map<std::string, std::function<std::shared_ptr<Modem>()>> &modemRegistry;
        };
    }; // namespace events
};     // namespace altiwx