#include "packet.h"

// List SoapySDR devices
class PacketSoapyList : public Packet
{
public:
    std::string process(nlohmann::json &);
    static std::string getID();
};