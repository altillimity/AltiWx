#pragma once
#include "libs/zmq.hpp"
#include <thread>

// Class handling external requests like Soapy devices, etc
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
    // Start listening
    void start();
    // Stop server
    void stop();
};