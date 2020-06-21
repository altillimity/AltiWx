#include "modem_iq.h"

ModemIQ::ModemIQ(long frequency, long bandwidth, std::string outputFile)
{
    // Local variables & file output
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    outIQFile = std::ofstream(outputFile, std::ios::binary);
}

void ModemIQ::stop()
{
    // Cose ofstream
    outIQFile.close();
}

void ModemIQ::process(liquid_float_complex *buffer, unsigned int &length)
{
    // Write all samples to output file
    for (i = 0; i < length; i++)
        outIQFile.write((char *)&buffer[i], sizeof(buffer[i]));
}