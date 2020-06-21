#include "modem.h"
#include "logger/logger.h"

void Modem::initResamp(long inputRate, long inputFrequency)
{
    // Compute resampling rate, setup resampler
    freqResampRate = (double)bandwidth_m / (double)inputRate;
    freqResampler = msresamp_crcf_create(freqResampRate, 60.0f);
    logger->debug("Modem resample rate " + std::to_string(freqResampRate));
}

void Modem::init(long inputRate, long inputFrequency)
{
    // Keep DSP-provided infos
    inputFrequency_m = inputFrequency;
    inputRate_m = inputRate;

    // Init frequency shifter
    shiftFrequency = 0;
    if (frequency_m != inputFrequency_m)
    {
        freqShifter = nco_crcf_create(LIQUID_VCO);
        shiftFrequency = frequency_m - inputFrequency_m;
        if (abs(shiftFrequency) > inputFrequency_m / 2)
            logger->critical("Modem frequency shift exceeds sample rate!");
        logger->debug("Modem frequency shifting of " + std::to_string(shiftFrequency));
        nco_crcf_set_frequency(freqShifter, (2.0 * M_PI) * (((double)abs(shiftFrequency)) / ((double)inputRate_m)));
    }

    // Intialize resampler
    initResamp(inputRate, inputFrequency);
}

void Modem::demod(liquid_float_complex *buffer, uint32_t &length)
{
    modemMutex.lock(); // Lock mutex
    // Create buffers
    liquid_float_complex sdr_buffer[length];
    uint resamp_buffer_length = ceil(freqResampRate * (float)length);
    liquid_float_complex resamp_buffer[resamp_buffer_length];

    // Shift frequency to 0Hz
    if (shiftFrequency != 0)
        if (shiftFrequency > 0)
            nco_crcf_mix_block_down(freqShifter, buffer, sdr_buffer, length);
        else
            nco_crcf_mix_block_up(freqShifter, buffer, sdr_buffer, length);

    // Resample
    msresamp_crcf_execute(freqResampler, sdr_buffer, length, resamp_buffer, &resamp_buffer_length);

    // Call implemented demodulation
    process(resamp_buffer, resamp_buffer_length);
    modemMutex.unlock(); // Unlock mutex
}

void Modem::setFrequency(long frequency)
{
    modemMutex.lock(); // Lock mutex

    frequency_m = frequency;

    // Change frequency shifting settings
    if (frequency_m != inputFrequency_m)
    {
        if (!freqShifter)
            freqShifter = nco_crcf_create(LIQUID_VCO);

        shiftFrequency = frequency_m - inputFrequency_m;
        if (abs(shiftFrequency) > inputFrequency_m / 2)
            logger->critical("Modem frequency shift exceeds sample rate!");

        nco_crcf_set_frequency(freqShifter, (2.0 * M_PI) * (((double)abs(shiftFrequency)) / ((double)inputRate_m)));
    }

    modemMutex.unlock(); // Unlock mutex
}