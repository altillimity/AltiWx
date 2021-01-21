#include "modem_iq.h"

ModemIQ::ModemIQ(int frequency, int samplerate, std::string file, int buffer_size) : Modem(frequency, samplerate, buffer_size)
{
    output_file = std::ofstream(file, std::ios::binary);
}

void ModemIQ::stop()
{
    Modem::stop();
    output_file.close();
}

void ModemIQ::work(std::complex<float> *buffer, int length)
{
    output_file.write((char *)buffer, length * sizeof(std::complex<float>));
}