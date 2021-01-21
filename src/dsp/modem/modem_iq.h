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
    ModemIQ(int frequency, int samplerate, std::string file, int buffer_size = 8192);
    void stop();
};