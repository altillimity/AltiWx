#include "modem_iq.h"

ModemIQ::ModemIQ(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size) : Modem(frequency, samplerate, parameters, buffer_size)
{
    output_file = std::ofstream(d_parameters["file"], std::ios::binary);
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

std::string ModemIQ::getType()
{
    return "IQ";
}

std::shared_ptr<Modem> ModemIQ::getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size)
{
    return std::make_shared<ModemIQ>(frequency, samplerate, parameters, buffer_size);
}