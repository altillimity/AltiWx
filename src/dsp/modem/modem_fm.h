#pragma once

#include "modem.h"
#include <string>
#include "tinywav/tinywav.h"

class ModemFM : public Modem
{
private:
    freqdem fm_demodulator;
    msresamp_rrrf audio_resampler;
    TinyWav output_wav;

private:
    float *audio_buffer;
    float *demod_buffer;
    unsigned int resampled_length;

private:
    int d_audio_samplerate;

protected:
    void work(std::complex<float> *buffer, int length);

public:
    ModemFM(int frequency, int samplerate, int audioSamplerate, std::string file, int buffer_size = 8192);
    ~ModemFM();
    void stop();
};