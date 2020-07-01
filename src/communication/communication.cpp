#include "communication.h"
#include "logger/logger.h"
#include "nlohmann/json.h"
#include "config/config.h"

CommunicationManager::CommunicationManager(std::string socket) : socket_m(socket)
{
    logger->info("Starting API server...");
    zmqContext = zmq::context_t(1);
    zmqSocket = zmq::socket_t(zmqContext, zmq::socket_type::rep);
    zmqSocket.bind(socket_m);
    logger->debug("Bound to " + socket_m);
    running = true;
}

void CommunicationManager::start()
{
    requestThread = std::thread(&CommunicationManager::work, this);
}

void CommunicationManager::work()
{
    while (running)
    {
        zmq::message_t packet;
        if (zmqSocket.recv(packet, zmq::recv_flags::none))
        {
            std::string content, answer;
            for (size_t i = 0; i < packet.size(); i++)
                content += ((char *)packet.data())[i];
            logger->debug(content);

            nlohmann::json jsonObject;
            try
            {
                jsonObject = nlohmann::json::parse(content);
            }
            catch (std::exception e)
            {
                zmqSocket.send(zmq::buffer("bad request"), zmq::send_flags::dontwait);
                continue;
            }

            if (jsonObject["type"] == "soapylistreq")
            {
                nlohmann::json answerJson;

                answerJson["type"] = "soapylistrep";

                for (SDRConfig sdrConfig : configManager->getConfig().sdrConfigs)
                {
                    if (sdrConfig.soapy_redirect)
                    {
                        answerJson[sdrConfig.name] = sdrConfig.soapySocket;
                    }
                }
                answer = answerJson.dump();
            }
            else
                answer = "bad request";

            logger->debug("=> " + answer);
            zmqSocket.send(zmq::buffer(answer), zmq::send_flags::dontwait);
        }
    }
}

void CommunicationManager::stop()
{
    running = false;
    logger->info("Stopping API server...");
    // Exit out of the above loop
    {
        zmq::context_t context(1);
        zmq::socket_t dissocket(context, zmq::socket_type::req);
        dissocket.connect(socket_m);
        dissocket.send(zmq::buffer("stop"), zmq::send_flags::dontwait);
        dissocket.close();
        context.close();
    }
    if (requestThread.joinable())
        requestThread.join();
    zmqSocket.close();
    zmqContext.close();
}