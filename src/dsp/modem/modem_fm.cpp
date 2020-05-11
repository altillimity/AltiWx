#include "modem_fm.h"
#include <math.h>

void ModemFM::initResamp(long inputRate, long inputFrequency)
{
    float intermediate_rate = 4.0f * ((float)audioRate_m);
    freqResampRate = intermediate_rate / ((float)inputRate);
    audioResampRate = ((float)audioRate_m) / intermediate_rate;
    freqResampler = msresamp_crcf_create(freqResampRate, 60.0f);
    float bw = bandwidth_m / 10 / intermediate_rate;
    audioResamp = resamp_rrrf_create(audioResampRate, 9, bw, 60.0f, 16);
}

ModemFM::ModemFM(long frequency, long bandwidth, int audioRate, std::string outputFile) : audioRate_m(audioRate)
{
    kf = 1.0f;
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    demodulatorFM = freqdem_create(kf);
    tinywav_open_write(&outWavFile, 1, audioRate, TW_FLOAT32, TW_INLINE, outputFile.c_str());
}

void ModemFM::stop()
{
    tinywav_close_write(&outWavFile);
}

void ModemFM::process(liquid_float_complex *buffer, unsigned int &length)
{
    float t = 0.0f;
    unsigned int nz = ceil(audioResampRate * (float)length);
    float z[nz];

    unsigned int num_written = 0;
    for (i = 0, j = 0; i < length; i++, j += num_written)
    {
        freqdem_demodulate(demodulatorFM, buffer[i], &t);
        resamp_rrrf_execute(audioResamp, t, &z[j], &num_written);
    }

    float output_f[length];

    for (i = 0; i < j; i++)
        output_f[i] = (float)(kf * z[i]);

    tinywav_write_f(&outWavFile, output_f, j);
}