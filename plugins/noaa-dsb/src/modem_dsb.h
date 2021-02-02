#pragma once

#include "dsp/modem/modem.h"
#include <fstream>
#include <dsp/agc.h>
#include <dsp/fir_filter.h>
#include <dsp/carrier_pll_psk.h>
#include <dsp/pll_carrier_tracking.h>
#include <dsp/clock_recovery_mm.h>
#include "repack_bits_byte.h"
#include "deframer.h"

// Sample useless modem
class ModemNOAADSB : public Modem
{
protected:
    std::ofstream output_file;
    std::shared_ptr<libdsp::AgcCC> agc;
    std::shared_ptr<libdsp::PLLCarrierTracking> pll1;
    std::shared_ptr<libdsp::BPSKCarrierPLL> pll2;
    std::shared_ptr<libdsp::FIRFilterFFF> rrc;
    std::shared_ptr<libdsp::ClockRecoveryMMFF> rec;
    RepackBitsByte repacker;
    DSBDeframer deframer;

    std::vector<uint8_t> defra_buf;

    std::complex<float> *agc_buffer;
    std::complex<float> *pll1_buffer;
    float *pll2_buffer;
    float *rrc_buffer;
    float *rec_buffer;
    uint8_t *sym_buffer;
    uint8_t *byte_buffer;
    uint8_t *manchester_buffer;

    int agc_out;
    int pll1_out;
    int pll2_out;
    int rrc_out;
    int rec_out;
    int byte_out;
    int num_byte;

protected:
    void work(std::complex<float> *buffer, int length);

public:
    ModemNOAADSB(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size);
    ~ModemNOAADSB();
    void stop();

public:
    static std::string getType();
    static std::shared_ptr<Modem> getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
};