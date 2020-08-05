#include "packetSoapyList.h"
#include "config/config.h"

std::string PacketSoapyList::process(nlohmann::json& inputData)
{
    // Build answer json
    nlohmann::json answerJson;

    answerJson["type"] = "soapylistrep";

    // Find SDRs that correspond
    for (SDRConfig sdrConfig : configManager->getConfig().sdrConfigs)
    {
        if (sdrConfig.soapy_redirect)
        {
            // Add them to our object
            answerJson[sdrConfig.name] = {sdrConfig.soapySocket, sdrConfig.sampleRate};
        }
    }
    // Convert to string
    return answerJson.dump();
}

std::string PacketSoapyList::getID()
{
    return "soapylistreq";
}