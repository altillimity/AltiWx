#include "modem_fm.h"

ModemFM::ModemFM(int frequency, int samplerate, int audioSamplerate, std::string file, int buffer_size) : d_audio_samplerate(audioSamplerate), Modem(frequency, samplerate, buffer_size)
{
    audio_buffer = new float[buffer_size];
    demod_buffer = new float[buffer_size];

    tinywav_open_write(&output_wav, 1, d_audio_samplerate, TW_INT16, TW_INLINE, file.c_str());

    float kf = 1.0f;
    fm_demodulator = freqdem_create(kf);

    double audio_resample_rate = (double)d_audio_samplerate / (double)d_samplerate;
    audio_resampler = msresamp_rrrf_create(audio_resample_rate, 60.0f);
}

ModemFM::~ModemFM()
{
    delete[] audio_buffer;
    delete[] demod_buffer;
}

void ModemFM::stop()
{
    Modem::stop();
    tinywav_close_write(&output_wav);
}

void ModemFM::work(std::complex<float> *buffer, int length)
{
    freqdem_demodulate_block(fm_demodulator, buffer, length, demod_buffer);
    msresamp_rrrf_execute(audio_resampler, demod_buffer, length, audio_buffer, &resampled_length);
    tinywav_write_f(&output_wav, audio_buffer, resampled_length);
}