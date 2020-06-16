#pragma once

#include "modem.h"
#include <string>
#include "tinywav/tinywav.h"

// Modem recording raw baseband into a wav file
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