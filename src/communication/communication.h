#pragma once
#include "zmq.hpp"
#include <thread>

class CommunicationManager
{
private:
    zmq::context_t zmqContext;
    zmq::socket_t zmqSocket;
    std::thread requestThread;
    void work();
    bool running;
    std::string socket_m;

public:
    CommunicationManager(std::string socket);
    void start();
    void stop();
};