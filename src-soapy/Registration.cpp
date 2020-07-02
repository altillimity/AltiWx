#include "SoapyAltiWx.hpp"
#include <SoapySDR/Registry.hpp>
#include "nlohmann/json.h"

static std::vector<SoapySDR::Kwargs> findAltiWx(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

    zmq::context_t context(1);
    zmq::socket_t socket(context, zmq::socket_type::req);
    socket.connect("ipc:///tmp/altiwx");
    //socket.setsockopt(ZMQ_SNDTIMEO, 1000);
    if (!socket.send(zmq::buffer("{\"type\":\"soapylistreq\"}"), zmq::send_flags::dontwait))
        return results;
    zmq::message_t packet;
    socket.recv(packet);
    std::string content;
    for (size_t i = 0; i < packet.size(); i++)
        content += ((char *)packet.data())[i];

    nlohmann::json jsonObj = nlohmann::json::parse(content);

    if (jsonObj["type"] != "soapylistrep")
        return results;

    for (nlohmann::detail::iteration_proxy_value<nlohmann::detail::iter_impl<nlohmann::json>> device : jsonObj.items())
    {
        if (device.key() == "type")
            continue;
        if (args.count("socket") != 0)
            if (device.value()[0] != args.at("socket"))
                continue;
        SoapySDR::Kwargs devInfo;
        devInfo["label"] = "AltiWx - " + device.key();
        devInfo["product"] = device.key();
        devInfo["socket"] = device.value()[0];
        devInfo["samplerate"] = std::to_string((long)device.value()[1]);
        results.push_back(devInfo);
    }

    return results;
}

static SoapySDR::Device *makeAltiWx(const SoapySDR::Kwargs &args)
{
    return new SoapyAltiWx(args);
}

static SoapySDR::Registry registerAltiWx("altiwx", &findAltiWx, &makeAltiWx, SOAPY_SDR_ABI_VERSION);
