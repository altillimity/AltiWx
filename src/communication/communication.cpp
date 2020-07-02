#include "communication.h"
#include "logger/logger.h"
#include "nlohmann/json.h"
#include "config/config.h"

CommunicationManager::CommunicationManager(std::string socket) : socket_m(socket)
{
    logger->info("Starting API server...");
    // Init ZMQ
    zmqContext = zmq::context_t(1);
    zmqSocket = zmq::socket_t(zmqContext, zmq::socket_type::rep);
    zmqSocket.bind(socket_m);
    logger->debug("Bound to " + socket_m);
    running = true;
}

void CommunicationManager::start()
{
    // Start the thread
    requestThread = std::thread(&CommunicationManager::work, this);
}

void CommunicationManager::work()
{
    // Loop until we exit
    while (running)
    {
        // Receive a packet
        zmq::message_t packet;
        if (zmqSocket.recv(packet, zmq::recv_flags::none))
        {
            // Convert to std::string
            std::string content, answer;
            for (size_t i = 0; i < packet.size(); i++)
                content += ((char *)packet.data())[i];
            logger->debug(content);

            // Parse as json
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

            // Check packet type
            if (jsonObject["type"] == "soapylistreq")
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
                answer = answerJson.dump();
            }
            // Otherwise, bad request
            else
                answer = "bad request";

            // Send reply
            logger->debug("=> " + answer);
            zmqSocket.send(zmq::buffer(answer), zmq::send_flags::dontwait);
        }
    }
}

void CommunicationManager::stop()
{
    // Prevent the loop from repeating
    running = false;
    logger->info("Stopping API server...");
    // Exit out of the above loop, sending a request not to stay stuck
    {
        zmq::context_t context(1);
        zmq::socket_t dissocket(context, zmq::socket_type::req);
        dissocket.connect(socket_m);
        dissocket.send(zmq::buffer("stop"), zmq::send_flags::dontwait);
        dissocket.close();
        context.close();
    }
    // Wait for it to exit
    if (requestThread.joinable())
        requestThread.join();
    // Close everything
    zmqSocket.close();
    zmqContext.close();
}