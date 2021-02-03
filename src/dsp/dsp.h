#pragma once

#include <mutex>
#include <thread>
#include <complex>
#include <rtl-sdr.h>
#include "modem/modem.h"
#include <unordered_map>

#define DSB_BUFFER_SIZE 8192

class DeviceDSP
{
private:
    int d_samplerate;
    int d_frequency;
    int d_gain;

    rtlsdr_dev *rtlsdr_device;
    std::mutex rtlsdr_mutex;
    std::thread rtlsdr_thread;
    std::atomic<bool> rtlsdr_should_run;
    std::complex<float> *_lut_32f; // Conversion table

    std::complex<float> *rtlsdr_read_buffer;

    std::unordered_map<std::string, std::shared_ptr<Modem>> current_modems;
    std::mutex modems_mutex;

private:
    void work();
    static void _rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

public:
    DeviceDSP(int samplerate, int frequency, int gain);
    ~DeviceDSP();

    void start();
    void callback(unsigned char *buf, uint32_t &len);

    void setFrequency(int frequency);
    void setSamplerate(int samplerate);
    void setGain(int gain);

    void attachModem(std::string id, std::shared_ptr<Modem> modem);
    void detachModem(std::string id);
};