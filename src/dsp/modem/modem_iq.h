#pragma once

#include "modem.h"
#include <string>
#include <fstream>

class ModemIQ : public Modem
{
protected:
    std::ofstream outIQFile;
    unsigned int i;
    short finalIQ[4];
    float iqToConvert[2];

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemIQ(long frequency, long bandwidth, std::string outputFile);
    void stop();
};