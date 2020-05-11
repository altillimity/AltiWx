#include "modem.h"
#include <complex>

void Modem::initResamp(long inputRate, long inputFrequency)
{
    freqResampRate = (double)bandwidth_m / (double)inputRate;
    freqResampler = msresamp_crcf_create(freqResampRate, 60.0f);
}

void Modem::init(long inputRate, long inputFrequency)
{
    shiftFrequency = 0;
    if (frequency_m != inputFrequency)
    {
        freqShifter = nco_crcf_create(LIQUID_VCO);
        shiftFrequency = frequency_m - inputFrequency;
        nco_crcf_set_frequency(freqShifter, (2.0 * M_PI) * (((double)abs(shiftFrequency)) / ((double)inputRate)));
    }

    initResamp(inputRate, inputFrequency);
}

void Modem::demod(int8_t *buffer, uint32_t &length)
{
    uint sdr_buffer_length = length / 2;
    uint resamp_buffer_length = ceil(freqResampRate * (float)sdr_buffer_length);
    liquid_float_complex sdr_buffer[sdr_buffer_length];
    liquid_float_complex resamp_buffer[sdr_buffer_length];

    for (resampI = 0; resampI < sdr_buffer_length; resampI++)
    {
        using namespace std::complex_literals;
        std::complex<float> value = ((float)buffer[2 * resampI]) + ((float)buffer[2 * resampI + 1]) * 1if;
        sdr_buffer[resampI] = reinterpret_cast<liquid_float_complex(&)>(value);
    }

    if (shiftFrequency != 0)
        if (shiftFrequency > 0)
            nco_crcf_mix_block_down(freqShifter, sdr_buffer, sdr_buffer, sdr_buffer_length);
        else
            nco_crcf_mix_block_up(freqShifter, sdr_buffer, sdr_buffer, sdr_buffer_length);

    msresamp_crcf_execute(freqResampler, sdr_buffer, sdr_buffer_length, resamp_buffer, &resamp_buffer_length);

    process(resamp_buffer, resamp_buffer_length);
}