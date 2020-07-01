#include "SoapyAltiWx.hpp"
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <algorithm> //min
#include <climits>   //SHRT_MAX
#include <cstring>   // memcpy
#include <iostream>

std::vector<std::string> SoapyAltiWx::getStreamFormats(const int direction, const size_t channel) const
{
    std::vector<std::string> formats;

    formats.push_back(SOAPY_SDR_CF32);

    return formats;
}

std::string SoapyAltiWx::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const
{
    //check that direction is SOAPY_SDR_RX
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("RTL-SDR is RX only, use SOAPY_SDR_RX");
    }

    fullScale = 128;
    return SOAPY_SDR_CF32;
}

SoapySDR::ArgInfoList SoapyAltiWx::getStreamArgsInfo(const int direction, const size_t channel) const
{
    //check that direction is SOAPY_SDR_RX
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("RTL-SDR is RX only, use SOAPY_SDR_RX");
    }

    SoapySDR::ArgInfoList streamArgs;

    SoapySDR::ArgInfo bufflenArg;
    bufflenArg.key = "bufflen";
    bufflenArg.value = std::to_string(0);
    bufflenArg.name = "Buffer Size";
    bufflenArg.description = "Number of bytes per buffer, multiples of 512 only.";
    bufflenArg.units = "bytes";
    bufflenArg.type = SoapySDR::ArgInfo::INT;

    return streamArgs;
}

/*******************************************************************
 * Stream API
 ******************************************************************/

SoapySDR::Stream *SoapyAltiWx::setupStream(const int direction, const std::string &format, const std::vector<size_t> &channels, const SoapySDR::Kwargs &args)
{
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("RTL-SDR is RX only, use SOAPY_SDR_RX");
    }

    //check the channel configuration
    if (channels.size() > 1 or (channels.size() > 0 and channels.at(0) != 0))
    {
        throw std::runtime_error("setupStream invalid channel selection");
    }

    //check the format
    if (format == SOAPY_SDR_CF32)
    {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CF32.");
    }
    else
    {
        throw std::runtime_error(
            "setupStream invalid format '" + format + "' -- Only CS8, CS16 and CF32 are supported by SoapyAltiWx module.");
    }

    zmqContext = zmq::context_t(1);
    zmqSocket = zmq::socket_t(zmqContext, zmq::socket_type::dealer);
    zmqSocket.connect(socket);

    return (SoapySDR::Stream *)this;
}

void SoapyAltiWx::closeStream(SoapySDR::Stream *stream)
{
    this->deactivateStream(stream, 0, 0);
    //_buffs.clear();
}

size_t SoapyAltiWx::getStreamMTU(SoapySDR::Stream *stream) const
{
    return 8192; //bufferLength / BYTES_PER_SAMPLE;
}

int SoapyAltiWx::activateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    if (flags != 0)
        return SOAPY_SDR_NOT_SUPPORTED;

    return 0;
}

int SoapyAltiWx::deactivateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs)
{
    if (flags != 0)
        return SOAPY_SDR_NOT_SUPPORTED;

    return 0;
}

int SoapyAltiWx::readStream(SoapySDR::Stream *stream, void *const *buffs, const size_t numElems, int &flags, long long &timeNs, const long timeoutUs)
{
    //std::cout << "hello3" << std::endl;
    zmq::message_t packet;
    zmqSocket.recv(packet, zmq::recv_flags::none);
    //std::cout << "hello4" << packet.size() / sizeof(std::complex<float>) << std::endl;
    //for (size_t i = 0; i < packet.size() / sizeof(std::complex<float>); i++)
    //    ((std::complex<float> *)buffs[0])[i] = ((std::complex<float> *)packet.data())[i];
    std::memcpy(buffs[0], packet.data(), packet.size());
    return packet.size() / sizeof(std::complex<float>);
}
