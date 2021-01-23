#pragma once

#include "modem.h"
#include <string>
#include <fstream>

class ModemIQ : public Modem
{
protected:
    std::ofstream output_file;

protected:
    void work(std::complex<float> *buffer, int length);

public:
    ModemIQ(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size);
    void stop();

public:
    static std::string getType();
    static std::shared_ptr<Modem> getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size);
};