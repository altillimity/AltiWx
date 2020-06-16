#pragma once

#include "modem.h"
#include <string>
#include <fstream>

// Modem recording raw baseband
class ModemIQ : public Modem
{
protected:
    std::ofstream outIQFile;
    unsigned int i;
    int16_t imag, real;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemIQ(long frequency, long bandwidth, std::string outputFile);
    void stop();
};