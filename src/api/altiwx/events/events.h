#include "dsp/modem/modem.h"

namespace altiwx
{
    namespace events
    {
        struct StartedEvent
        {
            // Nothing
        };

        struct RegisterModemsEvent
        {
            std::map<std::string, std::function<std::shared_ptr<Modem>(int, int, std::map<std::string, std::string>, int)>> &modemRegistry;
        };
    }; // namespace events
};     // namespace altiwx