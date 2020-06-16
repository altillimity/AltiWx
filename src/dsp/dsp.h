#include <rtl-sdr.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include "modem/modem.h"
#include "zmq.hpp"

#define RTL_DEFAULT_BUFFER_LENGTH 16384
#define RTL_MAX_OVERSAMPLE 16
#define RTL_MAX_BUFFER_LENGTH (RTL_MAX_OVERSAMPLE * RTL_DEFAULT_BUFFER_LENGTH)

// Class performing all the DSP work, currently only supporting rtl-sdr
class DSP
{
private:
    int8_t sdr_buffer[RTL_MAX_BUFFER_LENGTH];
    rtlsdr_dev_t *device;
    std::mutex dongleMutex;
    std::mutex modemsMutex;
    std::unordered_map<std::string, std::shared_ptr<Modem>> activeModems;
    std::thread dongleThread_m;
    long sampleRate_m;
    long centerFrequency_m;
    int gain_m;
    bool soapy_m;
    std::string socketString;

    // Zmq stuff
    zmq::context_t zmqContext;
    zmq::socket_t zmqSocket;

private:
    // rtlsdr callback
    static void rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx);
    // Thread running the dongle work
    void dongleThread();

public:
    DSP(long sampleRate, long centerFrequency, int gain, bool soapy, std::string soapySocket);
    void start();
    void stop();
    // Attach a modem
    void attachModem(std::string id, std::shared_ptr<Modem> modem); 
    // Detach a modem
    void detachModem(std::string id);
};