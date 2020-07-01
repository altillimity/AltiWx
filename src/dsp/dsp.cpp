#include "dsp.h"
#include "logger/logger.h"
#include <SoapySDR/Formats.hpp>

DSP::DSP(std::string deviceString, long sampleRate, long centerFrequency, int gain, bool soapy, std::string soapySocket, int demodThreads) : sampleRate_m(sampleRate), centerFrequency_m(centerFrequency), gain_m(gain), soapy_m(soapy), socketString(soapySocket), deviceString_m(deviceString), demodThreads_m(demodThreads)
{
}

void DSP::start()
{
    // Setup SDR device, logging everything
    logger->info("Opening SDR device...");
    device = SoapySDR::Device::make(deviceString_m);
    if (device == NULL)
    {
        logger->critical("Couldn't open SDR device : " + deviceString_m);
    }

    logger->info("Device " + device->getHardwareKey() + " opened!");

    device->setFrequency(SOAPY_SDR_RX, 0, centerFrequency_m);
    logger->info("Tuned to " + std::to_string(centerFrequency_m) + " Hz");

    device->setSampleRate(SOAPY_SDR_RX, 0, sampleRate_m);
    logger->info("Sampling at " + std::to_string(sampleRate_m) + " S/s");

    if (gain_m == 0)
    {
        device->setGainMode(SOAPY_SDR_RX, 0, true);
        logger->info("Tuner gain set to automatic");
    }
    else
    {
        device->setGainMode(SOAPY_SDR_RX, 0, false);
        device->setGain(SOAPY_SDR_RX, 0, gain_m);
        logger->info("Tuner gain set to " + std::to_string(gain_m) + " dB");
    }

    // If soapy output is enabled, set it up
    if (soapy_m)
    {
        logger->info("Binding soapy to " + socketString);
        zmqContext = zmq::context_t(1);
        zmqSocket = zmq::socket_t(zmqContext, zmq::socket_type::dealer);
        zmqSocket.bind(socketString);
    }

    device->writeSetting("bias_tx", "true");

    // Setup device to begin work
    device_stream = device->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);
    device->activateStream(device_stream, 0, 0, 0);
    if (device_stream == NULL)
    {
        logger->critical("Failed to setup device!");
    }

    // Start thread pool
    modem_pool = std::make_shared<ctpl::thread_pool>(demodThreads_m);

    // Start device thread
    running = true;
    sdrThread_m = std::thread(&DSP::sdrThread, this);
}

void DSP::stop()
{
    // Remove all modems
    for (std::pair<std::string, std::shared_ptr<Modem>> currentModem : activeModems)
    {
        detachModem(currentModem.first);
        currentModem.second->stop();
    }

    // Tell the thread to stop
    running = false;

    // Stop sample thread
    if (sdrThread_m.joinable())
        sdrThread_m.join();

    // Close device
    device->deactivateStream(device_stream, 0, 0);
    device->closeStream(device_stream);
    SoapySDR::Device::unmake(device);

    zmqSocket.close();
    zmqContext.close();
}

void DSP::sdrThread()
{
    void *sdr_buffer_ptr[] = {sdr_buffer};
    int flags;
    long long time_ns;

    while (running)
    {
        // Read buffer
        uint32_t length = device->readStream(device_stream, sdr_buffer_ptr, BUFFER_LENGTH, flags, time_ns, 1e5);

        // Just in case
        if (length > BUFFER_LENGTH)
        {
            modemsMutex.unlock();
            continue;
        }

        // We're gonna work on modems, can't modify them at the same time!
        modemsMutex.lock();

        // Feed demodulators and push to thread pool
        for (const std::pair<std::string, std::shared_ptr<Modem>> &currentModem : activeModems)
            currentModem.second->modemFuture = modem_pool->push([=](int) { currentModem.second->demod(sdr_buffer, length); });

        // Wait for all of them to be done
        for (const std::pair<std::string, std::shared_ptr<Modem>> &currentModem : activeModems)
            currentModem.second->modemFuture.get();

        // Forward to SoapySDR if that's enabled
        if (soapy_m)
            zmqSocket.send(zmq::message_t(sdr_buffer, length * sizeof(liquid_float_complex)), zmq::send_flags::dontwait);

        // Modems can now be modified!
        modemsMutex.unlock();
    }
}

void DSP::attachModem(std::string id, std::shared_ptr<Modem> modem)
{
    modemsMutex.lock();                              // Make sure we're not using anything!
    activeModems.emplace(std::make_pair(id, modem)); // Add modem
    logger->debug("Attaching modem id " + id + " to DSP");
    modem->init(sampleRate_m, centerFrequency_m); // Init modem
    modemsMutex.unlock();
}

void DSP::detachModem(std::string id)
{
    modemsMutex.lock();     // Make sure we're not using anything!
    activeModems.erase(id); // Remove modem
    logger->debug("Detaching modem id " + id + " from DSP");
    modemsMutex.unlock();
}