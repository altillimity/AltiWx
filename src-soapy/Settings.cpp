#include "SoapyAltiWx.hpp"
#include <SoapySDR/Time.hpp>
#include <algorithm>

SoapyAltiWx::SoapyAltiWx(const SoapySDR::Kwargs &args) : deviceId(-1),
                                                         dev(nullptr),
                                                         sampleRate(2.4e6),
                                                         centerFrequency(137.5e6),
                                                         numBuffers(DEFAULT_NUM_BUFFERS),
                                                         bufferLength(DEFAULT_BUFFER_LENGTH),
                                                         iqSwap(false),
                                                         offsetMode(false),
                                                         digitalAGC(false),
                                                         ticks(false),
                                                         bufferedElems(0),
                                                         resetBuffer(false),
                                                         gainMin(0.0),
                                                         gainMax(0.0)
{
}

SoapyAltiWx::~SoapyAltiWx(void)
{
}

/*******************************************************************
 * Identification API
 ******************************************************************/

std::string SoapyAltiWx::getDriverKey(void) const
{
    return "ALTIWX";
}

std::string SoapyAltiWx::getHardwareKey(void) const
{
    return "null";
}

SoapySDR::Kwargs SoapyAltiWx::getHardwareInfo(void) const
{
    SoapySDR::Kwargs args;
    args["origin"] = "https://gitlab.altillimity.com/altimmity/altiwx";
    args["index"] = "0";
    return args;
}

/*******************************************************************
 * Channels API
 ******************************************************************/

size_t SoapyAltiWx::getNumChannels(const int dir) const
{
    return (dir == SOAPY_SDR_RX) ? 1 : 0;
}

bool SoapyAltiWx::getFullDuplex(const int direction, const size_t channel) const
{
    return false;
}

/*******************************************************************
 * Antenna API
 ******************************************************************/

std::vector<std::string> SoapyAltiWx::listAntennas(const int direction, const size_t channel) const
{
    std::vector<std::string> antennas;
    antennas.push_back("RX");
    return antennas;
}

void SoapyAltiWx::setAntenna(const int direction, const size_t channel, const std::string &name)
{
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("setAntena failed: AltiWx only supports RX");
    }
}

std::string SoapyAltiWx::getAntenna(const int direction, const size_t channel) const
{
    return "RX";
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

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

/*******************************************************************
 * Gain API
 ******************************************************************/

std::vector<std::string> SoapyAltiWx::listGains(const int direction, const size_t channel) const
{
    std::vector<std::string> results;
    return results;
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

void SoapyAltiWx::setGain(const int direction, const size_t channel, const double value)
{
}

void SoapyAltiWx::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
}

double SoapyAltiWx::getGain(const int direction, const size_t channel, const std::string &name) const
{
    return 0;
}

SoapySDR::Range SoapyAltiWx::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    return SoapySDR::Range(0, 0);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/

void SoapyAltiWx::setFrequency(
    const int direction,
    const size_t channel,
    const std::string &name,
    const double frequency,
    const SoapySDR::Kwargs &args)
{
}

double SoapyAltiWx::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    return (double)centerFrequency;
}

std::vector<std::string> SoapyAltiWx::listFrequencies(const int direction, const size_t channel) const
{
    std::vector<std::string> names;
    return names;
}

SoapySDR::RangeList SoapyAltiWx::getFrequencyRange(
    const int direction,
    const size_t channel,
    const std::string &name) const
{
    SoapySDR::RangeList results;
    results.push_back(SoapySDR::Range(centerFrequency, centerFrequency));
    return results;
}

SoapySDR::ArgInfoList SoapyAltiWx::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    SoapySDR::ArgInfoList freqArgs;
    return freqArgs;
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

void SoapyAltiWx::setSampleRate(const int direction, const size_t channel, const double rate)
{
}

double SoapyAltiWx::getSampleRate(const int direction, const size_t channel) const
{
    return sampleRate;
}

std::vector<double> SoapyAltiWx::listSampleRates(const int direction, const size_t channel) const
{
    std::vector<double> results;
    results.push_back(sampleRate);
    return results;
}

void SoapyAltiWx::setBandwidth(const int direction, const size_t channel, const double bw)
{
    SoapySDR::Device::setBandwidth(direction, channel, bw);
}

double SoapyAltiWx::getBandwidth(const int direction, const size_t channel) const
{
    return SoapySDR::Device::getBandwidth(direction, channel);
}

std::vector<double> SoapyAltiWx::listBandwidths(const int direction, const size_t channel) const
{
    std::vector<double> results;
    return results;
}

/*******************************************************************
 * Time API
 ******************************************************************/

std::vector<std::string> SoapyAltiWx::listTimeSources(void) const
{
    std::vector<std::string> results;
    results.push_back("sw_ticks");
    return results;
}

std::string SoapyAltiWx::getTimeSource(void) const
{
    return "sw_ticks";
}

bool SoapyAltiWx::hasHardwareTime(const std::string &what) const
{
    return what == "" || what == "sw_ticks";
}

long long SoapyAltiWx::getHardwareTime(const std::string &what) const
{
    return SoapySDR::ticksToTimeNs(ticks, sampleRate);
}

void SoapyAltiWx::setHardwareTime(const long long timeNs, const std::string &what)
{
    ticks = SoapySDR::timeNsToTicks(timeNs, sampleRate);
}

/*******************************************************************
 * Settings API
 ******************************************************************/

SoapySDR::ArgInfoList SoapyAltiWx::getSettingInfo(void) const
{
    SoapySDR::ArgInfoList setArgs;
    return setArgs;
}

void SoapyAltiWx::writeSetting(const std::string &key, const std::string &value)
{
}

std::string SoapyAltiWx::readSetting(const std::string &key) const
{
    return "";
}
