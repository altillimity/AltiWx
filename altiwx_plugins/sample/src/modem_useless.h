#pragma once

#include "dsp/modem/modem.h"

// Sample useless modem
class ModemUseless : public Modem
{
protected:
    void work(std::complex<float> *buffer, int length);

public:
    ModemUseless(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size);
    void stop();

public:
    static std::string getType();
    static std::shared_ptr<Modem> getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
};