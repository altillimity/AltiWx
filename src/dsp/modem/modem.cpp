#include "modem.h"
#include "logger/logger.h"

Modem::Modem(int frequency, int samplerate, int buffer_size) : d_frequency(frequency), d_samplerate(samplerate), d_buffer_size(buffer_size)
{
    input_buffer = new std::complex<float>[d_buffer_size];
    shifted_buffer = new std::complex<float>[d_buffer_size];
    resamp_buffer = new std::complex<float>[d_buffer_size];
}

Modem::~Modem()
{
    delete[] input_buffer;
    delete[] shifted_buffer;
    delete[] resamp_buffer;
}

void Modem::start(long inputSamplerate, long inputFrequency)
{
    logger->debug("Init modem at " + std::to_string(d_frequency) + "Hz bandwidth " + std::to_string(d_samplerate) + "Hz");
    d_samplerate_in = inputSamplerate;
    d_frequency_in = inputFrequency;

    double resampler_rate = (double)d_samplerate / (double)d_samplerate_in;
    freq_resampler = msresamp_crcf_create(resampler_rate, 60.0f);
    logger->debug("Modem resampler rate " + std::to_string(resampler_rate));

    freq_shifter = nco_crcf_create(LIQUID_VCO);
    setFrequency(d_frequency);
    logger->debug("Modem frequency shift " + std::to_string(d_shift_frequency) + "Hz");

    should_run = true;
    work_thread = std::thread(&Modem::workThread, this);
}

void Modem::stop()
{
    should_run = false;
    if (work_thread.joinable())
        work_thread.join();
}

void Modem::setFrequency(long frequency)
{
    modem_mutex.lock();

    d_frequency = frequency;

    if (frequency != d_frequency_in)
    {
        d_shift_frequency = d_frequency - d_frequency_in;
        nco_crcf_set_frequency(freq_shifter, (2.0 * M_PI) * (((double)abs(d_shift_frequency)) / ((double)d_samplerate_in)));
    }
    else
    {
        d_shift_frequency = 0;
    }

    modem_mutex.unlock();
}

void Modem::push(std::complex<float> *buffer, int length)
{
    fifo.push(buffer, length);
}

void Modem::workThread()
{
    int cnt;
    unsigned int resamp_cnt;
    while (should_run)
    {
        cnt = fifo.pop(input_buffer, d_buffer_size);

        if (cnt <= 0)
            return;

        modem_mutex.lock();

        if (d_shift_frequency != 0)
        {
            if (d_shift_frequency > 0)
                nco_crcf_mix_block_down(freq_shifter, input_buffer, shifted_buffer, cnt);
            else
                nco_crcf_mix_block_up(freq_shifter, input_buffer, shifted_buffer, cnt);
        }

        msresamp_crcf_execute(freq_resampler, d_shift_frequency == 0 ? input_buffer : shifted_buffer, cnt, resamp_buffer, &resamp_cnt);

        work(resamp_buffer, resamp_cnt);

        modem_mutex.unlock();
    }
}