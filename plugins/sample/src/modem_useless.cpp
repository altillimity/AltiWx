#include "modem_useless.h"

ModemUseless::ModemUseless(int frequency, int samplerate, std::map<std::string, std::string> &parameters, int buffer_size) : Modem(frequency, samplerate, parameters, buffer_size)
{
}

void ModemUseless::stop()
{
    Modem::stop();
}

void ModemUseless::work(std::complex<float> *buffer, int length)
{
}

std::string ModemUseless::getType()
{
    return "USELESS";
}

std::shared_ptr<Modem> ModemUseless::getInstance(int frequency, int samplerate, std::map<std::string, std::string> parameters, int buffer_size)
{
    return std::make_shared<ModemUseless>(frequency, samplerate, parameters, buffer_size);
}