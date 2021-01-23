#pragma once

#include <complex>
#include <liquid/liquid.h>
#include <dsp/pipe.h>
#include <mutex>
#include <thread>
#include <map>
#include <functional>
#include <atomic>

class Modem
{
private:
    libdsp::Pipe<std::complex<float>> fifo;
    nco_crcf freq_shifter;
    msresamp_crcf freq_resampler;
    std::complex<float> *input_buffer;
    std::complex<float> *shifted_buffer;
    std::complex<float> *resamp_buffer;

private:
    std::thread work_thread;
    std::mutex modem_mutex;
    std::atomic<bool> should_run;

protected:
    int d_buffer_size;
    int d_shift_frequency;
    int d_frequency;
    int d_samplerate;
    int d_samplerate_in;
    int d_frequency_in;
    std::map<std::string, std::string> d_parameters;

private:
    void workThread();

protected:
    virtual void work(std::complex<float> *buffer, int length) = 0;

public:
    Modem(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
    ~Modem();
    void start(long inputSamplerate, long inputFrequency);
    virtual void stop();
    void setFrequency(long frequency);
    void push(std::complex<float> *buffer, int length);

public:
    static std::string getType();
    static std::shared_ptr<Modem> getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
};

extern std::map<std::string, std::function<std::shared_ptr<Modem>(int, int, std::map<std::string, std::string>, int)>> modem_registry;

void registerModems();