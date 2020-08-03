#include "modem_enum.h"

std::string modemTypeToString(ModemType type)
{
    switch (type)
    {
    case FM:
        return "FM";
    case IQ:
        return "IQ";
    case IQWAV:
        return "IQWAV";
    case QPSK:
        return "QPSK";
    default:
        return "";
    }
}