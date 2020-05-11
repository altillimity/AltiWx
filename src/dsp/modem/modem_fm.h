#pragma once

#include "modem.h"
#include <string>
#include "tinywav/tinywav.h"

class ModemFM : public Modem
{
protected:
    freqdem demodulatorFM;
    msresamp_rrrf audioResamp;
    int audioRate_m;
    float audioResampRate;
    TinyWav outWavFile;
    float kf;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemFM(long frequency, long bandwidth, int audioRate, std::string outputFile);
    void stop();
};