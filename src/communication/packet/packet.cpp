#include "packet.h"

#include "packetSoapyList.h"
#include "api/altiwx/events/events.h"
#include "api/altiwx/altiwx.h"

std::unordered_map<std::string, std::shared_ptr<Packet>> packetMap;

// Register all packets
void registerPackets()
{
    packetMap.emplace(PacketSoapyList::getID(), std::make_shared<PacketSoapyList>());

    altiwx::eventBus->fire_event<altiwx::events::RegisterPacketsEvent>({packetMap});
}