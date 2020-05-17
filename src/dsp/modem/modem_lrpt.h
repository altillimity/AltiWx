#pragma once

#include "modem.h"
#include <string>
#include <fstream>

class ModemLRPT : public Modem
{
protected:
    std::ofstream outDemodFile;
    agc_crcf agc_m;
    firinterp_crcf rrc_m;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemLRPT(long frequency, long bandwidth, std::string outputFile);
    void stop();
};