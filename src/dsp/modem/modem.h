#pragma once

#include <cstdint>
#include <liquid/liquid.h>
#include <mutex>

class Modem
{
protected:
    std::mutex modemMutex;
    nco_crcf freqShifter;
    msresamp_crcf freqResampler;
    long inputFrequency_m, inputRate_m;
    long bandwidth_m;
    long frequency_m;
    long shiftFrequency;
    float freqResampRate;
    int resampI;

protected:
    virtual void process(liquid_float_complex *buffer, unsigned int &length) = 0;
    virtual void initResamp(long inputRate, long inputFrequency);

public:
    void init(long inputRate, long inputFrequency);
    void demod(int8_t *buffer, uint32_t &length);
    virtual void stop() = 0;
    void setFrequency(long frequency);
};