#include <rtl-sdr.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <thread>
#include "modem/modem.h"

#define RTL_DEFAULT_BUFFER_LENGTH 16384
#define RTL_MAX_OVERSAMPLE 16
#define RTL_MAX_BUFFER_LENGTH (RTL_MAX_OVERSAMPLE * RTL_DEFAULT_BUFFER_LENGTH)

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

private:
    static void rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx);
    void dongleThread();

public:
    DSP(long sampleRate, long centerFrequency, int gain);
    void start();
    void stop();
    void attachModem(std::string id, std::shared_ptr<Modem> modem);
    void detachModem(std::string id);
};