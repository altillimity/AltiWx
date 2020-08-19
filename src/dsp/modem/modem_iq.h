#pragma once

#include "modem.h"
#include <string>
#include <fstream>

// Modem recording raw baseband
class ModemIQ : public Modem
{
protected:
    std::ofstream outIQFile;
    unsigned int i;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);

public:
    static std::string getType();
    std::vector<std::string> getParameters();
    void setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters);
    void stop();

public:
    static std::shared_ptr<Modem> getInstance();
};