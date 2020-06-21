#include <SoapySDR/Device.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include "modem/modem.h"
#include "zmq.hpp"

#define BUFFER_LENGTH 1024

// Class performing all the DSP work, currently only supporting rtl-sdr
class DSP
{
private:
    std::string deviceString_m;
    liquid_float_complex sdr_buffer[BUFFER_LENGTH];
    SoapySDR::Device *device;
    SoapySDR::Stream *device_stream;
    std::mutex dongleMutex;
    std::mutex modemsMutex;
    std::unordered_map<std::string, std::shared_ptr<Modem>> activeModems;
    std::thread sdrThread_m;
    long sampleRate_m;
    long centerFrequency_m;
    int gain_m;
    bool soapy_m;
    std::string socketString;
    bool running;

    // Zmq stuff
    zmq::context_t zmqContext;
    zmq::socket_t zmqSocket;

private:
    // rtlsdr callback
    static void rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx);
    // Thread running the dongle work
    void sdrThread();

public:
    DSP(std::string deviceString, long sampleRate, long centerFrequency, int gain, bool soapy, std::string soapySocket);
    void start();
    void stop();
    // Attach a modem
    void attachModem(std::string id, std::shared_ptr<Modem> modem);
    // Detach a modem
    void detachModem(std::string id);
};