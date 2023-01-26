#include "modem_dsb.h"
#include <dsp/fir_gen.h>
#include "manchester.h"

ModemNOAADSB::ModemNOAADSB(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size) : Modem(frequency, samplerate, parameters, buffer_size)
{
    output_file = std::ofstream(d_parameters["file"], std::ios::binary);

    float symbolrate = 8320;

    // Init DSP blocks
    agc = std::make_shared<libdsp::AgcCC>(1e-2f, 1.0f, 1.0f, 65536);
    pll1 = std::make_shared<libdsp::PLLCarrierTracking>(0.015, (6000.0f * 2.0f * 3.14159f) / (float)samplerate, (-6000.0f * 2.0f * 3.14159f) / (float)samplerate);
    pll2 = std::make_shared<libdsp::BPSKCarrierPLL>(0.01f, powf(0.01, 2) / 4.0f, 3.0f * M_PI * 100e3 / samplerate);
    rrc = std::make_shared<libdsp::FIRFilterFFF>(1, libdsp::firgen::root_raised_cosine(1, samplerate / 2.0f, symbolrate, 0.5, 1023));
    rec = std::make_shared<libdsp::ClockRecoveryMMFF>(((float)samplerate / (float)symbolrate) / 2.0f, powf(0.01, 2) / 4.0f, 0.5f, 0.01, 100e-6);

    // Buffers
    agc_buffer = new std::complex<float>[buffer_size];
    pll1_buffer = new std::complex<float>[buffer_size];
    pll2_buffer = new float[buffer_size];
    rrc_buffer = new float[buffer_size];
    rec_buffer = new float[buffer_size];
    sym_buffer = new uint8_t[buffer_size];
    byte_buffer = new uint8_t[buffer_size];
    manchester_buffer = new uint8_t[buffer_size];
}

ModemNOAADSB::~ModemNOAADSB()
{
    delete[] agc_buffer;
    delete[] pll1_buffer;
    delete[] pll2_buffer;
    delete[] rrc_buffer;
    delete[] rec_buffer;
    delete[] sym_buffer;
    delete[] byte_buffer;
    delete[] manchester_buffer;
}

void ModemNOAADSB::stop()
{
    output_file.close();
    Modem::stop();
}

void volk_32f_binary_slicer_8i_generic(int8_t *cVector, const float *aVector, unsigned int num_points);

#include <iostream>

void ModemNOAADSB::work(std::complex<float> *buffer, int length)
{
    agc_out = agc->work(buffer, length, agc_buffer);
    pll1_out = pll1->work(agc_buffer, agc_out, pll1_buffer);
    pll2_out = pll2->work(pll1_buffer, pll1_out, pll2_buffer);
    rrc_out = rrc->work(pll2_buffer, pll2_out, rrc_buffer);
    rec_out = rec->work(rrc_buffer, rrc_out, rec_buffer);
    volk_32f_binary_slicer_8i_generic((int8_t *)sym_buffer, rec_buffer, rec_out);
    byte_out = repacker.work(sym_buffer, rec_out, byte_buffer);

    // Ensure byte_out is always a multiple of 2
    defra_buf.insert(defra_buf.end(), &byte_buffer[0], &byte_buffer[byte_out]);
    num_byte = defra_buf.size() - defra_buf.size() % 2;

    manchesterDecoder(&defra_buf.data()[0], num_byte, manchester_buffer);

    std::vector<std::array<uint8_t, FRAME_SIZE>> frameBuffer = deframer.work(manchester_buffer, num_byte / 2);

    // Erase used up elements
    defra_buf.erase(defra_buf.begin(), defra_buf.begin() + num_byte);

    if (frameBuffer.size() > 0)
    {
        for (std::array<uint8_t, FRAME_SIZE> frm : frameBuffer)
        {
            output_file.write((char *)&frm[0], FRAME_SIZE);
        }
    }
}

std::string ModemNOAADSB::getType()
{
    return "NOAA_DSB";
}

std::shared_ptr<Modem> ModemNOAADSB::getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size)
{
    return std::make_shared<ModemNOAADSB>(frequency, samplerate, parameters, buffer_size);
}

void volk_32f_binary_slicer_8i_generic(int8_t *cVector, const float *aVector, unsigned int num_points)
{
    int8_t *cPtr = cVector;
    const float *aPtr = aVector;
    unsigned int number = 0;

    for (number = 0; number < num_points; number++)
    {
        if (*aPtr++ >= 0)
        {
            *cPtr++ = 1;
        }
        else
        {
            *cPtr++ = 0;
        }
    }
}