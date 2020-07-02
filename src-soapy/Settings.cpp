#include "SoapyAltiWx.hpp"

SoapyAltiWx::SoapyAltiWx(const SoapySDR::Kwargs &args)
{
    socket = args.at("socket");
    sampleRate = std::stol(args.at("samplerate"));
}
SoapyAltiWx::~SoapyAltiWx(void)
{
}

// ID API
std::string SoapyAltiWx::getDriverKey() const
{
    return "altiwx";
}
std::string SoapyAltiWx::getHardwareKey() const
{
    return "rtlsdr";
}
SoapySDR::Kwargs SoapyAltiWx::getHardwareInfo(void) const
{
    SoapySDR::Kwargs args;

    args["origin"] = "https://github.com/pothosware/SoapyRTLSDR";
    args["index"] = std::to_string(0);

    return args;
}

// Channels API
size_t SoapyAltiWx::getNumChannels(int direction) const
{
    if (direction == SOAPY_SDR_RX)
        return 1;
    else
        return 0;
}
bool SoapyAltiWx::getFullDuplex(int direction, size_t channel) const
{
    return false;
}

// Antenna API
std::vector<std::string> SoapyAltiWx::listAntennas(int direction, size_t channel) const
{
    std::vector<std::string> antennas;
    antennas.push_back("RX");
    return antennas;
}
void SoapyAltiWx::setAntenna(int direction, size_t channel, const std::string &name)
{
}
std::string SoapyAltiWx::getAntenna(const int direction, const size_t channel) const
{
    return "RX";
}

// Gain API
std::vector<std::string> SoapyAltiWx::listGains(int direction, size_t channel) const
{
    std::vector<std::string> antennas;
    if (direction == SOAPY_SDR_RX)
        return antennas;
    else
        return antennas;
}
void SoapyAltiWx::setGain(int direction, size_t channel, double value)
{
}
SoapySDR::Range SoapyAltiWx::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    return SoapySDR::Range(0, 0);
}
double SoapyAltiWx::getGain(const int direction, const size_t channel, const std::string &name) const
{
    return 0;
}
bool SoapyAltiWx::hasGainMode(const int direction, const size_t channel) const
{
    return false;
}
void SoapyAltiWx::setGainMode(const int direction, const size_t channel, const bool automatic)
{
}
bool SoapyAltiWx::getGainMode(const int direction, const size_t channel) const
{
    return false;
}
void SoapyAltiWx::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
}

// Frequency API
void SoapyAltiWx::setFrequency(const int direction, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &args)
{
}
std::vector<std::string> SoapyAltiWx::listFrequencies(int direction, size_t channel) const
{
    std::vector<std::string> antennas;
    if (direction == SOAPY_SDR_RX)
        return antennas;
    else
        return antennas;
}
SoapySDR::RangeList SoapyAltiWx::getFrequencyRange(const int direction, const size_t channel, const std::string &name) const
{
    std::vector<SoapySDR::Range> antennas;
    antennas.push_back(SoapySDR::Range(137.5e6, 137.5e6));
    if (direction == SOAPY_SDR_RX)
        return antennas;
    else
        return antennas;
}
double SoapyAltiWx::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    return (137.5e6);
}
bool SoapyAltiWx::hasDCOffsetMode(const int direction, const size_t channel) const
{
    return false;
}
bool SoapyAltiWx::hasFrequencyCorrection(const int direction, const size_t channel) const
{
    return false;
}
void SoapyAltiWx::setFrequencyCorrection(const int direction, const size_t channel, const double value)
{
}
double SoapyAltiWx::getFrequencyCorrection(const int direction, const size_t channel) const
{
    return 0;
}
SoapySDR::ArgInfoList SoapyAltiWx::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    return SoapySDR::ArgInfoList();
}

// Samplerate API
void SoapyAltiWx::setSampleRate(int direction, size_t channel, double rate)
{
}
std::vector<double> SoapyAltiWx::listSampleRates(int direction, size_t channel) const
{
    std::vector<double> samplerates;
    samplerates.push_back(sampleRate);
    return samplerates;
}
double SoapyAltiWx::getSampleRate(const int direction, const size_t channel) const
{
    return sampleRate;
}
void SoapyAltiWx::setBandwidth(const int direction, const size_t channel, const double bw)
{
}
double SoapyAltiWx::getBandwidth(const int direction, const size_t channel) const
{
    return 0;
}
std::vector<double> SoapyAltiWx::listBandwidths(const int direction, const size_t channel) const
{
    return std::vector<double>();
}

// Time API
std::vector<std::string> SoapyAltiWx::listTimeSources(void) const
{
    return std::vector<std::string>();
}
std::string SoapyAltiWx::getTimeSource(void) const
{
    return "";
}
bool SoapyAltiWx::hasHardwareTime(const std::string &what) const
{
    return false;
}
long long SoapyAltiWx::getHardwareTime(const std::string &what) const
{
    return 0;
}
void SoapyAltiWx::setHardwareTime(const long long timeNs, const std::string &what)
{
}

// Settings API
SoapySDR::ArgInfoList SoapyAltiWx::getSettingInfo(void) const
{
    return SoapySDR::ArgInfoList();
}
void SoapyAltiWx::writeSetting(const std::string &key, const std::string &value)
{
}
std::string SoapyAltiWx::readSetting(const std::string &key) const
{
    return "";
}