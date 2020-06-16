#include "modem_fm.h"
#include <math.h>

ModemFM::ModemFM(long frequency, long bandwidth, int audioRate, std::string outputFile) : audioRate_m(audioRate)
{
    // Setup local variables
    kf = 1.0f;
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    demodulatorFM = freqdem_create(kf); // Demodulator
    audioResampRate = (double)audioRate_m / (double)bandwidth_m; // Audio resampling
    audioResamp = msresamp_rrrf_create(audioResampRate, 60.0f); // Audio resampler
    tinywav_open_write(&outWavFile, 1, audioRate, TW_INT16, TW_INLINE, outputFile.c_str()); // Wav file
}

void ModemFM::stop()
{
    // Just close the wav file!
    tinywav_close_write(&outWavFile);
}

void ModemFM::process(liquid_float_complex *buffer, unsigned int &length)
{
    // Buffers...
    unsigned int bufferOutSize = ceil(audioResampRate * (float)length);
    float outputBuffer[bufferOutSize];
    float demodBuffer[length];

    freqdem_demodulate_block(demodulatorFM, &buffer[0], length, &demodBuffer[0]); // FM demodulation
    msresamp_rrrf_execute(audioResamp, &demodBuffer[0], (int)length, &outputBuffer[0], &bufferOutSize); // Audio resampling
    tinywav_write_f(&outWavFile, outputBuffer, bufferOutSize); // Write to file
}