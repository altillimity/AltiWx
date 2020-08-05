#include "packet.h"

class PacketSoapyList : public Packet
{
public:
    std::string process(nlohmann::json &inputData);
    static std::string getID();
};