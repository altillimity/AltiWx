#include "modem.h"
#include "logger/logger.h"
#include "api/altiwx/altiwx.h"
#include "api/altiwx/events/events.h"

Modem::Modem(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size) : d_frequency(frequency), d_samplerate(samplerate), d_parameters(parameters), d_buffer_size(buffer_size)
{
    input_buffer = new std::complex<float>[d_buffer_size];
    shifted_buffer = new std::complex<float>[d_buffer_size];
    resamp_buffer = new std::complex<float>[d_buffer_size];
    fifo.init(1e9);
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
    logger->info("Stopping modem...");
    should_run = false;
    fifo.clearWriteStop();
    fifo.clearReadStop();
    if (work_thread.joinable())
        work_thread.join();
    logger->info("Stopped!");
}

void Modem::setFrequency(long frequency)
{
    modem_mutex.lock();

    d_frequency = frequency;

    if (frequency != d_frequency_in)
    {
        d_shift_frequency = -(d_frequency - d_frequency_in);
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
    fifo.write(buffer, length);
}

void Modem::workThread()
{
    int cnt = 0;
    unsigned int resamp_cnt;
    while (should_run)
    {
        cnt = fifo.read(input_buffer, d_buffer_size);

        if (cnt <= 0)
        {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            continue;
        }

        modem_mutex.lock();

        if (d_shift_frequency != 0)
        {
            if (d_shift_frequency > 0)
                nco_crcf_mix_block_up(freq_shifter, input_buffer, shifted_buffer, cnt);
            else
                nco_crcf_mix_block_down(freq_shifter, input_buffer, shifted_buffer, cnt);
        }

        msresamp_crcf_execute(freq_resampler, d_shift_frequency == 0 ? input_buffer : shifted_buffer, cnt, resamp_buffer, &resamp_cnt);

        work(resamp_buffer, resamp_cnt);

        modem_mutex.unlock();
    }
}

// Modem registery stuff
#include "modem_iq.h"
#include "modem_fm.h"
#include "modem_qpsk.h"

std::map<std::string, std::function<std::shared_ptr<Modem>(int, int, std::map<std::string, std::string>, int)>> modem_registry;

void registerModems()
{
    // Register internal modems
    modem_registry.emplace(ModemIQ::getType(), ModemIQ::getInstance);
    modem_registry.emplace(ModemFM::getType(), ModemFM::getInstance);
    modem_registry.emplace(ModemQPSK::getType(), ModemQPSK::getInstance);

    // Let plugins do their thing
    altiwx::eventBus->fire_event<altiwx::events::RegisterModemsEvent>({modem_registry});

    // Log them out
    logger->debug("Registered modems (" + std::to_string(modem_registry.size()) + ") : ");
    for (std::pair<const std::string, std::function<std::shared_ptr<Modem>(int, int, std::map<std::string, std::string>, int)>> &it : modem_registry)
        logger->debug(" - " + it.first);
}