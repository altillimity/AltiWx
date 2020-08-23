#include "modem_fm.h"
#include <math.h>

std::vector<std::string> ModemFM::getParameters()
{
    return {"audio_rate"};
}

void ModemFM::setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters)
{
    // Setup local variables
    audioRate_m = std::stoi(parameters["audio_rate"]);
    kf = 1.0f;
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    demodulatorFM = freqdem_create(kf);                                                                      // Demodulator
    audioResampRate = (double)audioRate_m / (double)bandwidth_m;                                             // Audio resampling
    audioResamp = msresamp_rrrf_create(audioResampRate, 60.0f);                                              // Audio resampler
    tinywav_open_write(&outWavFile, 1, audioRate_m, TW_INT16, TW_INLINE, parameters["output_file"].c_str()); // Wav file

    // Work buffers
    bufferOutSize = ceil(audioResampRate * DSP_BUFFER_SIZE);
    outputBuffer = new float[bufferOutSize];
    demodBuffer = new float[DSP_BUFFER_SIZE];
}

void ModemFM::stop()
{
    // Just close the wav file!
    tinywav_close_write(&outWavFile);

    // Delete buffers
    delete[] outputBuffer;
    delete[] demodBuffer;
}

void ModemFM::process(liquid_float_complex *buffer, unsigned int &length)
{
    freqdem_demodulate_block(demodulatorFM, &buffer[0], length, &demodBuffer[0]);                       // FM demodulation
    msresamp_rrrf_execute(audioResamp, &demodBuffer[0], (int)length, &outputBuffer[0], &bufferOutSize); // Audio resampling
    tinywav_write_f(&outWavFile, outputBuffer, bufferOutSize);                                          // Write to file
}

std::string ModemFM::getType()
{
    return "FM";
}

std::shared_ptr<Modem> ModemFM::getInstance()
{
    return std::make_shared<ModemFM>();
}