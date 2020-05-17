#include "modem_lrpt.h"
#include <math.h>

ModemLRPT::ModemLRPT(long frequency, long bandwidth, std::string outputFile)
{
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    outDemodFile = std::ofstream(outputFile, std::ios::binary);
    agc_m = agc_crcf_create();
    agc_crcf_set_bandwidth(agc_m, bandwidth);
    agc_crcf_set_gain(agc_m, 1.0f);

    unsigned int k = bandwidth / 75; // samples/symbol
    unsigned int m = 3;
    float beta = 0.3f;
    float dt = 0.6f;
    unsigned int h_len = 2 * k * m + 1;
    float hm[h_len];

    liquid_firdes_rrcos(k, m, beta, dt, hm);

    rrc_m = firinterp_crcf_create(k, hm, h_len);
}

void ModemLRPT::stop()
{
    outDemodFile.close();
}

void ModemLRPT::process(liquid_float_complex *buffer, unsigned int &length)
{
    liquid_float_complex agcBuffer[length];
    liquid_float_complex rrcBuffer[length];

    agc_crcf_execute_block(agc_m, &buffer[0], length, &agcBuffer[0]);
    firinterp_crcf_execute_block(rrc_m, &agcBuffer[0], length, &rrcBuffer[0]);
}