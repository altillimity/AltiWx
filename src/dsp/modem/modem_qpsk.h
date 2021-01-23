#pragma once

#include "modem.h"
#include <string>
#include <fstream>
#include <dsp/agc.h>
#include <dsp/fir_filter.h>
#include <dsp/costas_loop.h>
#include <dsp/clock_recovery_mm.h>

class ModemQPSK : public Modem
{
protected:
    std::ofstream output_file;
    std::shared_ptr<libdsp::AgcCC> agc;
    std::shared_ptr<libdsp::FIRFilterCCF> rrc;
    std::shared_ptr<libdsp::CostasLoop> pll;
    std::shared_ptr<libdsp::ClockRecoveryMMCC> rec;

    float d_agc_rate;
    int d_symbolrate;
    float d_rrc_alpha;
    int d_rrc_taps;
    float d_loop_bw;

    std::complex<float> *agc_buffer;
    std::complex<float> *rrc_buffer;
    std::complex<float> *pll_buffer;
    std::complex<float> *rec_buffer;
    int8_t *sym_buffer;

    int agc_out;
    int rrc_out;
    int pll_out;
    int rec_out;

protected:
    void work(std::complex<float> *buffer, int length);
    int8_t clamp(float x)
    {
        if (x < -128.0)
            return -127;
        if (x > 127.0)
            return 127;
        return x;
    }

public:
    ModemQPSK(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size);
    ~ModemQPSK();
    void stop();

public:
    static std::string getType();
    static std::shared_ptr<Modem> getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
};