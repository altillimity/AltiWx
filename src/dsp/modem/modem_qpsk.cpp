#include "modem_qpsk.h"
#include <dsp/fir_gen.h>

ModemQPSK::ModemQPSK(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size) : Modem(frequency, samplerate, parameters, buffer_size),
                                                                                                                       d_agc_rate(std::stof(parameters["agc_rate"])),
                                                                                                                       d_symbolrate(std::stoi(parameters["symbolrate"])),
                                                                                                                       d_rrc_alpha(std::stof(parameters["rrc_alpha"])),
                                                                                                                       d_rrc_taps(std::stoi(parameters["rrc_taps"])),
                                                                                                                       d_loop_bw(std::stof(parameters["costas_bw"]))
{
    output_file = std::ofstream(d_parameters["file"], std::ios::binary);

    // Init DSP blocks
    agc = std::make_shared<libdsp::AgcCC>(d_agc_rate, 1.0f, 1.0f, 65536);
    rrc = std::make_shared<libdsp::FIRFilterCCF>(1, libdsp::firgen::root_raised_cosine(1, samplerate, d_symbolrate, d_rrc_alpha, d_rrc_taps));
    pll = std::make_shared<libdsp::CostasLoop>(d_loop_bw, 4);
    rec = std::make_shared<libdsp::ClockRecoveryMMCC>((float)samplerate / (float)d_symbolrate, pow(8.7e-3, 2) / 4.0, 0.5f, 8.7e-3, 0.005f);

    // Buffers
    agc_buffer = new std::complex<float>[buffer_size];
    rrc_buffer = new std::complex<float>[buffer_size];
    pll_buffer = new std::complex<float>[buffer_size];
    rec_buffer = new std::complex<float>[buffer_size];
    sym_buffer = new int8_t[buffer_size * 2];
}

ModemQPSK::~ModemQPSK()
{
    // Modem::~Modem();
    delete[] agc_buffer;
    delete[] rrc_buffer;
    delete[] pll_buffer;
    delete[] rec_buffer;
    delete[] sym_buffer;
}

void ModemQPSK::stop()
{
    Modem::stop();
    output_file.close();
}

void ModemQPSK::work(std::complex<float> *buffer, int length)
{
    agc_out = agc->work(buffer, length, agc_buffer);
    rrc_out = rrc->work(agc_buffer, agc_out, rrc_buffer);
    pll_out = pll->work(rrc_buffer, rrc_out, pll_buffer);
    rec_out = rec->work(pll_buffer, pll_out, rec_buffer);

    for (int i = 0; i < rec_out; i++)
    {
        sym_buffer[i * 2 + 0] = clamp(rec_buffer[i].real() * 100);
        sym_buffer[i * 2 + 1] = clamp(rec_buffer[i].imag() * 100);
    }

    output_file.write((char *)sym_buffer, rec_out * 2);
}

std::string ModemQPSK::getType()
{
    return "QPSK";
}

std::shared_ptr<Modem> ModemQPSK::getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size)
{
    return std::make_shared<ModemQPSK>(frequency, samplerate, parameters, buffer_size);
}