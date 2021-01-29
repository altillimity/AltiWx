#include "dsp.h"
#include "logger/logger.h"

DeviceDSP::DeviceDSP(int samplerate, int frequency, int gain) : d_samplerate(samplerate), d_frequency(frequency), d_gain(gain), rtlsdr_should_run(false)
{
    _lut_32f = new std::complex<float>[65535 + 1];

    // Generate complex float conversion table
    for (unsigned int i = 0; i <= 65535; i++)
    {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
        float re = ((i & 0xff) - 127.4f) * (1.0f / 128.0f);
        float im = ((i >> 8) - 127.4f) * (1.0f / 128.0f);
#else
        float re = ((i >> 8) - 127.4f) * (1.0f / 128.0f);
        float im = ((i & 0xff) - 127.4f) * (1.0f / 128.0f);
#endif

        std::complex<float> v32f, vs32f;

        v32f.real(re);
        v32f.imag(im);
        _lut_32f[i] = v32f;
    }

    rtlsdr_read_buffer = new std::complex<float>[BUFFER_SIZE * 1000];

    logger->info("Attempting to open RTLSDR device...");
    if (rtlsdr_open(&rtlsdr_device, 0) != 0)
    {
        logger->critical("Could not open SDR device!");
    }

    setSamplerate(d_samplerate);
    setFrequency(d_frequency);
    setGain(49);
}

DeviceDSP::~DeviceDSP()
{
    delete[] rtlsdr_read_buffer;
    delete[] _lut_32f;
}

void DeviceDSP::start()
{
    logger->info("Starting SDR thread...");
    rtlsdr_reset_buffer(rtlsdr_device);
    rtlsdr_should_run = true;
    rtlsdr_thread = std::thread(&DeviceDSP::work, this);
}

void DeviceDSP::work()
{
    while (rtlsdr_should_run)
    {
        rtlsdr_mutex.lock();
        rtlsdr_read_async(rtlsdr_device, _rtlsdr_callback, this, 0, 0);
        rtlsdr_mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void DeviceDSP::callback(unsigned char *buf, uint32_t &len)
{
    // Convert to complex floats
    for (int i = 0; i < len / 2; ++i)
        rtlsdr_read_buffer[i] = _lut_32f[*((uint16_t *)&buf[2 * i])];

    modems_mutex.lock();

    for (const std::pair<std::string, std::shared_ptr<Modem>> &modem : current_modems)
        modem.second->push(rtlsdr_read_buffer, len / 2);

    modems_mutex.unlock();
}

void DeviceDSP::_rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
{
    ((DeviceDSP *)ctx)->callback(buf, len);
}

void DeviceDSP::setFrequency(int frequency)
{
    if (rtlsdr_should_run)
        rtlsdr_cancel_async(rtlsdr_device);
    rtlsdr_mutex.lock();
    d_frequency = frequency;
    while (rtlsdr_set_center_freq(rtlsdr_device, d_frequency) != 0)
    {
        logger->error("Could not set SDR frequency!");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    logger->info("Tuned SDR to " + std::to_string(rtlsdr_get_center_freq(rtlsdr_device)) + " Hz");
    rtlsdr_mutex.unlock();
}

void DeviceDSP::setSamplerate(int samplerate)
{
    rtlsdr_mutex.lock();
    d_samplerate = samplerate;
    if (rtlsdr_set_sample_rate(rtlsdr_device, d_samplerate) != 0)
    {
        logger->critical("Could not set SDR samplerate!");
    }
    logger->info("Set SDR samplerate to " + std::to_string(rtlsdr_get_sample_rate(rtlsdr_device)) + " S/s");
    rtlsdr_mutex.unlock();
}

void DeviceDSP::setGain(int gain)
{
    rtlsdr_mutex.lock();
    d_gain = gain;
    if (rtlsdr_set_tuner_gain_mode(rtlsdr_device, 1) != 0)
    {
        logger->critical("Could not set SDR gain mode!");
    }
    if (rtlsdr_set_tuner_gain(rtlsdr_device, d_gain * 10) != 0)
    {
        logger->critical("Could not set SDR gain!");
    }
    logger->info("Set SDR gain to " + std::to_string(rtlsdr_get_tuner_gain(rtlsdr_device) / 10) + " dB");
    rtlsdr_mutex.unlock();
}

void DeviceDSP::attachModem(std::string id, std::shared_ptr<Modem> modem)
{
    modems_mutex.lock();
    current_modems.emplace(std::make_pair(id, modem));
    logger->debug("Attaching and starting modem id " + id + " to DSP");
    modem->start(d_samplerate, d_frequency);
    modems_mutex.unlock();
}

void DeviceDSP::detachModem(std::string id)
{
    modems_mutex.lock();
    current_modems.erase(id);
    logger->debug("Detaching modem id " + id + " from DSP");
    modems_mutex.unlock();
}