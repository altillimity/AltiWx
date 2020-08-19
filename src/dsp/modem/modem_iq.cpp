#include "modem_iq.h"

std::vector<std::string> ModemIQ::getParameters()
{
    return {};
}

void ModemIQ::setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters)
{
    // Local variables & file output
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    outIQFile = std::ofstream(parameters["output_file"], std::ios::binary);
}

void ModemIQ::stop()
{
    // Close ofstream
    outIQFile.close();
}

void ModemIQ::process(liquid_float_complex *buffer, unsigned int &length)
{
    // Write all samples to output file
    outIQFile.write((char *)buffer, length * sizeof(liquid_float_complex));
}

std::string ModemIQ::getType()
{
    return "IQ";
}

std::shared_ptr<Modem> ModemIQ::getInstance()
{
    return std::make_shared<ModemIQ>();
}