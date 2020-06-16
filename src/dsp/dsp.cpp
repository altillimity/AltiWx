#include "dsp.h"
#include "logger/logger.h"
#include <complex>

DSP::DSP(long sampleRate, long centerFrequency, int gain, bool soapy, std::string soapySocket) : sampleRate_m(sampleRate), centerFrequency_m(centerFrequency), gain_m(gain), soapy_m(soapy), socketString(soapySocket)
{
}

void DSP::start()
{
    // Setup rtlsdr dongle, logging everything
    int r;
    logger->info("Opening RTLSDR device...");
    rtlsdr_open(&device, 0);

    r = rtlsdr_set_center_freq(device, centerFrequency_m);
    if (r == 0)
        logger->info("Tuned to " + std::to_string(centerFrequency_m) + " Hz");
    else
        logger->warn("Could not set center frequency to " + std::to_string(centerFrequency_m) + " Hz!");

    r = rtlsdr_set_sample_rate(device, sampleRate_m);
    if (r == 0)
        logger->info("Sampling at " + std::to_string(sampleRate_m) + " S/s");
    else
        logger->warn("Could not set sample rate to " + std::to_string(sampleRate_m) + " S/s!");

    if (gain_m == 0)
    {
        r = rtlsdr_set_tuner_gain_mode(device, 0);
        if (r == 0)
            logger->info("Tuner gain set to automatic");
        else
            logger->warn("Failed to set automatic gain!");
    }
    else
    {
        r = rtlsdr_set_tuner_gain_mode(device, 1);
        if (r != 0)
            logger->warn("Failed to enable manual tuner gain!");

        r = rtlsdr_set_tuner_gain(device, gain_m);
        if (r == 0)
            logger->info("Tuner gain set to " + std::to_string(gain_m) + " dB");
        else
            logger->warn("Failed to set gain to " + std::to_string(gain_m) + "dB!");
    }

    // If soapy output is enabled, set it up
    if (soapy_m)
    {
        logger->info("Binding soapy to " + socketString);
        zmqContext = zmq::context_t(1);
        zmqSocket = zmq::socket_t(zmqContext, zmq::socket_type::dealer);
        zmqSocket.bind(socketString);
    }

    // Setup device to begin work
    r = rtlsdr_reset_buffer(device);
    if (r != 0)
        logger->warn("Failed to reset buffers!");

    // Start working thread
    dongleThread_m = std::thread(&DSP::dongleThread, this);
}

void DSP::stop()
{
    // Remove all modems
    for (std::pair<std::string, std::shared_ptr<Modem>> currentModem : activeModems)
    {
        detachModem(currentModem.first);
        currentModem.second->stop();
    }

    // Stop rtlsdr thread
    int r = rtlsdr_cancel_async(device);
    if (r == 0)
        logger->info("Closing RTLSDR device...");
    else
        logger->warn("Failed to stop thread!");

    if (dongleThread_m.joinable())
        dongleThread_m.join();

    // Close device
    rtlsdr_close(device);
}

void DSP::dongleThread()
{
    // Read samples!
    rtlsdr_read_async(device, rtlsdrCallback, (void *)this, 0, 0);
}

void DSP::rtlsdrCallback(unsigned char *buf, uint32_t len, void *ctx)
{
    // Get source DSP object
    DSP *sourceDSP = (DSP *)ctx;
    sourceDSP->modemsMutex.lock();

    // Read buffer
    for (int i = 0; i < len; i++)
        sourceDSP->sdr_buffer[i] = ((int8_t)buf[i]) + 128;

    // Convert to complex 32-bits floats
    uint sdr_buffer_length = len / 2;
    liquid_float_complex sdr_buffer[sdr_buffer_length];

    for (int i = 0; i < sdr_buffer_length; i++)
    {
        using namespace std::complex_literals;
        std::complex<float> value = ((float)sourceDSP->sdr_buffer[2 * i]) + ((float)sourceDSP->sdr_buffer[2 * i + 1]) * 1if;
        sdr_buffer[i] = reinterpret_cast<liquid_float_complex(&)>(value);
    }

    // Feed demodulators
    for (const std::pair<std::string, std::shared_ptr<Modem>> &currentModem : sourceDSP->activeModems)
        currentModem.second->demod(sdr_buffer, sdr_buffer_length);

    // Forward to SoapySDR if that's enabled
    if (sourceDSP->soapy_m)
        sourceDSP->zmqSocket.send(zmq::message_t(buf, len), zmq::send_flags::dontwait);

    sourceDSP->modemsMutex.unlock();
}

void DSP::attachModem(std::string id, std::shared_ptr<Modem> modem)
{
    modemsMutex.lock(); // Make sure we're not using anything!
    activeModems.emplace(std::make_pair(id, modem)); // Add modem
    logger->debug("Attaching modem id " + id + " to DSP");
    modem->init(sampleRate_m, centerFrequency_m); // Init modem
    modemsMutex.unlock();
}

void DSP::detachModem(std::string id)
{
    modemsMutex.lock(); // Make sure we're not using anything!
    activeModems.erase(id); // Remove modem
    logger->debug("Detaching modem id " + id + " from DSP");
    modemsMutex.unlock();
}