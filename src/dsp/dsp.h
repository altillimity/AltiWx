#include <SoapySDR/Device.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include "modem/modem.h"
#include "libs/zmq.hpp"
#include "scheduler/bosma/ctpl_stl.h"
#include "config/config.h"

// Class performing all the DSP work, currently only supporting rtl-sdr
class DSP
{
private:
    long sampleRate_m;
    long centerFrequency_m;
    int gain_m;
    bool soapy_m;
    std::string socketString;
    std::string deviceString_m;
    int demodThreads_m;
    SDRConfig settings_m;
    liquid_float_complex sdr_buffer[DSP_BUFFER_SIZE];
    SoapySDR::Device *device;
    SoapySDR::Stream *device_stream;
    std::mutex dongleMutex;
    std::mutex modemsMutex;
    std::unordered_map<std::string, std::shared_ptr<Modem>> activeModems;
    std::thread sdrThread_m;
    bool running;

    // Modem thread pool
    std::shared_ptr<ctpl::thread_pool> modem_pool;

    // Zmq stuff
    zmq::context_t zmqContext;
    zmq::socket_t zmqSocket;

private:
    // rtlsdr callback
    static void rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx);
    // Thread running the dongle work
    void sdrThread();

public:
    DSP(SDRConfig &settings);
    void start();
    void stop();
    // Attach a modem
    void attachModem(std::string id, std::shared_ptr<Modem> modem);
    // Detach a modem
    void detachModem(std::string id);
};