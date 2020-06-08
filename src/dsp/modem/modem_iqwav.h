#pragma once

#include "modem.h"
#include <string>
#include "tinywav/tinywav.h"

class ModemIQWav : public Modem
{
protected:
    TinyWav outWavFile;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemIQWav(long frequency, long bandwidth, std::string outputFile);
    void stop();
};