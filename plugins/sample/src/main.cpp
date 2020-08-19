#include "altiwx/plugin.h"
#include "altiwx/altiwx.h"
#include "altiwx/events/events.h"

// Say hello, packet for use in AltiWx communication
class PacketHello : public Packet
{
public:
    std::string process(nlohmann::json &)
    {
        nlohmann::json answerJson;
        answerJson["type"] = "hellorep";
        answerJson["msg"] = "Hello from Sample plugin!";
        return answerJson.dump();
    }

    static std::string getID()
    {
        return "hello";
    }
};

// Sample useless modem
class ModemUseless : public Modem
{
protected:
    void process(liquid_float_complex *buffer, unsigned int &length) {};
    std::string nothing;

public:
    static std::string getType();
    std::vector<std::string> getParameters() { return {"nothing"}; };
    void setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters)
    {
        frequency_m = frequency;
        bandwidth_m = bandwidth;
        nothing = parameters["nothing"];
    }
    void stop() {}

public:
    static std::shared_ptr<Modem> getInstance() { return std::make_shared<ModemUseless>(); }
};

std::string ModemUseless::getType()
{
    return "USELESS";
}

// A sample plugin
class Sample : public altiwx::Plugin
{
public:
    // Return the plugin ID
    std::string getID()
    {
        return "sample";
    }

    // Plugin init, to register events, and everything else
    void init(std::shared_ptr<spdlog::logger> &logger)
    {
        // Log to the world!
        logger->debug("Hi from the AltiWx sample plugin!");

        // Register an event to be called when AltiWx is finished starting up
        altiwx::eventBus->register_handler<altiwx::events::StartedEvent>([&logger](const altiwx::events::StartedEvent &evt) {
            logger->critical("This is the Sample plugin, apparently AltiWx just finished starting.");
        });

        // Register our custom communication packets
        altiwx::eventBus->register_handler<altiwx::events::RegisterPacketsEvent>([&logger](const altiwx::events::RegisterPacketsEvent &evt) {
            logger->debug("Registering Sample plugin packets");
            evt.packtMap.emplace(PacketHello::getID(), std::make_shared<PacketHello>());
        });

        // Register our custom modem
        altiwx::eventBus->register_handler<altiwx::events::RegisterModemsEvent>([&logger](const altiwx::events::RegisterModemsEvent &evt) {
            logger->debug("Registering Sample plugin modems");
            evt.modemRegistry.emplace(ModemUseless::getType(), ModemUseless::getInstance);
        });
    }
};

// Setup a loader for AltiWx to load it, macro to make it easier
PLUGIN_LOADER(Sample)