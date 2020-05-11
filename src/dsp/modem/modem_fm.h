#pragma once

#include "modem.h"
#include <string>
#include "tinywav/tinywav.h"

class ModemFM : public Modem
{
protected:
    freqdem demodulatorFM;
    resamp_rrrf audioResamp;
    int audioRate_m;
    float audioResampRate;
    TinyWav outWavFile;
    int i, j;
    float kf;

protected:
    void initResamp(long inputRate, long inputFrequency);
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemFM(long frequency, long bandwidth, int audioRate, std::string outputFile);
    void stop();
};