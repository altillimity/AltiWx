#pragma once

#include "modem.h"
#include <fstream>
#include <vector>
#include "utils/agc.h"
#include "utils/filter_rrc.h"
#include "utils/costas.h"
#include "utils/clock_recovery.h"

// QPSK demodulator into soft bits
class ModemQPSK : public Modem
{
protected:
    long symbolrate_m;
    std::ofstream output_filestream;
    Agc *agc;
    Filter *rrc;
    std::shared_ptr<Costas> pll;
    std::shared_ptr<ClockRecovery> clockRecovery;
    liquid_float_complex processedSample;
    std::vector<liquid_float_complex> processingBuffer, clockRecovered;
    unsigned int i;
    int8_t softSymbolBuffer[2];

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    ModemQPSK(long frequency, long bandwidth, long symbolrate, std::string outputFile);
    void stop();
};