#pragma once

#include <string>

// All modem types
enum ModemType
{
    FM,
    IQ,
    IQWAV,
    QPSK
};

std::string modemTypeToString(ModemType type);