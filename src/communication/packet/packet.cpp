#include "packet.h"

#include "packetSoapyList.h"

std::unordered_map<std::string, std::shared_ptr<Packet>> packtMap;

void registerPackets()
{
    packtMap.emplace(PacketSoapyList::getID(), std::make_shared<PacketSoapyList>());
}