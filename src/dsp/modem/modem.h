#pragma once

#include <complex>
#include <liquid/liquid.h>
#include <dsp/pipe.h>
#include <mutex>
#include <thread>

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
    bool should_run;

protected:
    int d_buffer_size;
    int d_shift_frequency;
    int d_frequency;
    int d_samplerate;
    int d_samplerate_in;
    int d_frequency_in;

private:
    void workThread();

protected:
    virtual void work(std::complex<float> *buffer, int length) = 0;

public:
    Modem(int frequency, int samplerate, int buffer_size = 8192);
    ~Modem();
    void start(long inputSamplerate, long inputFrequency);
    void stop();
    void setFrequency(long frequency);
    void push(std::complex<float> *buffer, int length);
};