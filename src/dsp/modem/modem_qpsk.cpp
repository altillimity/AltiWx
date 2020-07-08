#include "modem_qpsk.h"
#include <math.h>
#include "logger/logger.h"

// From meteor_demod
int8_t clamp(float x)
{
    if (x < -128.0)
    {
        return -128;
    }
    if (x > 127.0)
    {
        return 127;
    }
    if (x > 0 && x < 1)
    {
        return 1;
    }
    if (x > -1 && x < 0)
    {
        return -1;
    }
    return x;
}

ModemQPSK::ModemQPSK(long frequency, long bandwidth, long symbolrate, std::string outputFile) : symbolrate_m(symbolrate)
{
    frequency_m = frequency;
    bandwidth_m = bandwidth;

    agc = agc_init();
    rrc = filter_rrc(180, 1, (float)bandwidth_m / (float)symbolrate_m, 0.5f);
    pll = std::make_shared<Costas>(0.002f, 4);
    clockRecovery = std::make_shared<ClockRecovery>(bandwidth_m, symbolrate_m);

    output_filestream = std::ofstream(outputFile, std::ios::binary);
}

void ModemQPSK::stop()
{
    output_filestream.close();
    agc_free(agc);
    filter_free(rrc);
}

void ModemQPSK::process(liquid_float_complex *buffer, unsigned int &length)
{
    // Perform AGC, Root-Raised-Cosine filtering and push in buffer
    for (i = 0; i < length; i++)
    {
        processedSample = agc_apply(agc, buffer[i]);
        processedSample = filter_fwd(rrc, processedSample);
        pll->work(&processedSample, 1);
        processingBuffer.push_back(processedSample);
    }

    // Perform clock recovery
    clockRecovered = clockRecovery->work(processingBuffer, processingBuffer.size());

    // Decode into soft symbols and write to output file
    for (liquid_float_complex &sample : clockRecovered)
    {
        softSymbolBuffer[0] = clamp(sample.real() / 2);
        softSymbolBuffer[1] = clamp(sample.imag() / 2);

        output_filestream.write((char *)&softSymbolBuffer, sizeof(softSymbolBuffer));
    }
}