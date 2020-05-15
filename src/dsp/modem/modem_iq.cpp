#include "modem_iq.h"
#include <math.h>
#include <volk/volk.h>

ModemIQ::ModemIQ(long frequency, long bandwidth, std::string outputFile)
{
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    outIQFile = std::ofstream(outputFile, std::ios::binary);
}

void ModemIQ::stop()
{
    outIQFile.close();
}

void ModemIQ::process(liquid_float_complex *buffer, unsigned int &length)
{
    for (i = 0; i < length; i++)
    {
        //outIQFile.write((char *)&buffer[i].imag, 4);
        //outIQFile.write((char *)&buffer[i].real, 4);
        iqToConvert[0] = buffer[i].imag;
        iqToConvert[1] = buffer[i].real;
        volk_32f_s32f_convert_16i(finalIQ, iqToConvert, 1.0f, 8);
        outIQFile.write((char *)&finalIQ[0], sizeof(iqToConvert) / 2);
    }
}