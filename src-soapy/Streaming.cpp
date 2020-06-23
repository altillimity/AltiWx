#include "SoapyAltiWx.hpp"
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Formats.hpp>
#include <SoapySDR/Time.hpp>
#include <algorithm> //min
#include <climits>   //SHRT_MAX
#include <cstring>   // memcpy

std::vector<std::string> SoapyAltiWx::getStreamFormats(const int direction, const size_t channel) const
{
    std::vector<std::string> formats;

    formats.push_back(SOAPY_SDR_CS8);
    formats.push_back(SOAPY_SDR_CS16);
    formats.push_back(SOAPY_SDR_CF32);

    return formats;
}

std::string SoapyAltiWx::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const
{
    //check that direction is SOAPY_SDR_RX
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("AltiWx is RX only, use SOAPY_SDR_RX");
    }

    fullScale = 128;
    return SOAPY_SDR_CS8;
}

SoapySDR::ArgInfoList SoapyAltiWx::getStreamArgsInfo(const int direction, const size_t channel) const
{
    //check that direction is SOAPY_SDR_RX
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("AltiWx is RX only, use SOAPY_SDR_RX");
    }
    SoapySDR::ArgInfoList streamArgs;
    return streamArgs;
}

/*******************************************************************
 * Async thread work
 ******************************************************************/

void SoapyAltiWx::rx_async_operation(void)
{
    while (1)
    {
        zmq::message_t request;
        zmqSocket.recv(request, zmq::recv_flags::none);
        rx_callback((unsigned char *)request.data(), request.size());
    }
}

void SoapyAltiWx::rx_callback(unsigned char *buf, uint32_t len)
{
    //printf("_rx_callback %d _buf_head=%d, numBuffers=%d\n", len, _buf_head, _buf_tail);

    // atomically add len to ticks but return the previous value
    unsigned long long tick = ticks.fetch_add(len);

    //overflow condition: the caller is not reading fast enough
    if (_buf_count == numBuffers)
    {
        _overflowEvent = true;
        return;
    }

    //copy into the buffer queue
    auto &buff = _buffs[_buf_tail];
    buff.tick = tick;
    buff.data.resize(len);
    std::memcpy(buff.data.data(), buf, len);

    //increment the tail pointer
    _buf_tail = (_buf_tail + 1) % numBuffers;

    //increment buffers available under lock
    //to avoid race in acquireReadBuffer wait
    {
        std::lock_guard<std::mutex> lock(_buf_mutex);
        _buf_count++;
    }

    //notify readStream()
    _buf_cond.notify_one();
}

/*******************************************************************
 * Stream API
 ******************************************************************/

SoapySDR::Stream *SoapyAltiWx::setupStream(
    const int direction,
    const std::string &format,
    const std::vector<size_t> &channels,
    const SoapySDR::Kwargs &args)
{
    if (direction != SOAPY_SDR_RX)
    {
        throw std::runtime_error("AltiWx is RX only, use SOAPY_SDR_RX");
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
        rxFormat = RTL_RX_FORMAT_FLOAT32;
    }
    else if (format == SOAPY_SDR_CS16)
    {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CS16.");
        rxFormat = RTL_RX_FORMAT_INT16;
    }
    else if (format == SOAPY_SDR_CS8)
    {
        SoapySDR_log(SOAPY_SDR_INFO, "Using format CS8.");
        rxFormat = RTL_RX_FORMAT_INT8;
    }
    else
    {
        throw std::runtime_error(
            "setupStream invalid format '" + format + "' -- Only CS8, CS16 and CF32 are supported by SoapyAltiWx module.");
    }

    if (rxFormat != RTL_RX_FORMAT_INT8 && !_lut_32f.size())
    {
        SoapySDR_logf(SOAPY_SDR_DEBUG, "Generating RTL-SDR lookup tables");
        // create lookup tables
        for (unsigned int i = 0; i <= 0xffff; i++)
        {
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
            float re = ((i & 0xff) - 127.4f) * (1.0f / 128.0f);
            float im = ((i >> 8) - 127.4f) * (1.0f / 128.0f);
#else
            float re = ((i >> 8) - 127.4f) * (1.0f / 128.0f);
            float im = ((i & 0xff) - 127.4f) * (1.0f / 128.0f);
#endif

            std::complex<float> v32f, vs32f;

            v32f.real(re);
            v32f.imag(im);
            _lut_32f.push_back(v32f);

            vs32f.real(v32f.imag());
            vs32f.imag(v32f.real());
            _lut_swap_32f.push_back(vs32f);

            std::complex<int16_t> v16i, vs16i;

            v16i.real(int16_t((float(SHRT_MAX) * re)));
            v16i.imag(int16_t((float(SHRT_MAX) * im)));
            _lut_16i.push_back(v16i);

            vs16i.real(vs16i.imag());
            vs16i.imag(vs16i.real());
            _lut_swap_16i.push_back(vs16i);
        }
    }

    bufferLength = DEFAULT_BUFFER_LENGTH;
    if (args.count("bufflen") != 0)
    {
        try
        {
            int bufferLength_in = std::stoi(args.at("bufflen"));
            if (bufferLength_in > 0)
            {
                bufferLength = bufferLength_in;
            }
        }
        catch (const std::invalid_argument &)
        {
        }
    }
    SoapySDR_logf(SOAPY_SDR_DEBUG, "RTL-SDR Using buffer length %d", bufferLength);

    numBuffers = DEFAULT_NUM_BUFFERS;
    if (args.count("buffers") != 0)
    {
        try
        {
            int numBuffers_in = std::stoi(args.at("buffers"));
            if (numBuffers_in > 0)
            {
                numBuffers = numBuffers_in;
            }
        }
        catch (const std::invalid_argument &)
        {
        }
    }
    SoapySDR_logf(SOAPY_SDR_DEBUG, "RTL-SDR Using %d buffers", numBuffers);

    asyncBuffs = 0;
    if (args.count("asyncBuffs") != 0)
    {
        try
        {
            int asyncBuffs_in = std::stoi(args.at("asyncBuffs"));
            if (asyncBuffs_in > 0)
            {
                asyncBuffs = asyncBuffs_in;
            }
        }
        catch (const std::invalid_argument &)
        {
        }
    }

    {
        for (int i = 0; i < 6; i++)
        {
            IFGain[i] = 0;
        }
    }
    //tunerGain = rtlsdr_get_tuner_gain(dev);

    //clear async fifo counts
    _buf_tail = 0;
    _buf_count = 0;
    _buf_head = 0;

    //allocate buffers
    _buffs.resize(numBuffers);
    for (auto &buff : _buffs)
        buff.data.reserve(bufferLength);
    for (auto &buff : _buffs)
        buff.data.resize(bufferLength);

    return (SoapySDR::Stream *)this;
}

void SoapyAltiWx::closeStream(SoapySDR::Stream *stream)
{
    this->deactivateStream(stream, 0, 0);
    _buffs.clear();
}

size_t SoapyAltiWx::getStreamMTU(SoapySDR::Stream *stream) const
{
    return bufferLength / BYTES_PER_SAMPLE;
}

int SoapyAltiWx::activateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    printf("connect");
    zmqSocket.connect("ipc:///tmp/altiwx");

    if (not _rx_async_thread.joinable())
    {
        _rx_async_thread = std::thread(&SoapyAltiWx::rx_async_operation, this);
    }

    return 0;
}

int SoapyAltiWx::deactivateStream(SoapySDR::Stream *stream, const int flags, const long long timeNs)
{
    _rx_async_thread.~thread();

    if (_rx_async_thread.joinable())
    {
        _rx_async_thread.join();
    }
    return 0;
}

int SoapyAltiWx::readStream(
    SoapySDR::Stream *stream,
    void *const *buffs,
    const size_t numElems,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{

    //drop remainder buffer on reset
    if (resetBuffer and bufferedElems != 0)
    {
        bufferedElems = 0;
        this->releaseReadBuffer(stream, _currentHandle);
    }

    //this is the user's buffer for channel 0
    void *buff0 = buffs[0];

    //are elements left in the buffer? if not, do a new read.
    if (bufferedElems == 0)
    {
        int ret = this->acquireReadBuffer(stream, _currentHandle, (const void **)&_currentBuff, flags, timeNs, timeoutUs);
        if (ret < 0)
            return ret;
        bufferedElems = ret;
    }

    //otherwise just update return time to the current tick count
    else
    {
        flags |= SOAPY_SDR_HAS_TIME;
        timeNs = SoapySDR::ticksToTimeNs(bufTicks, sampleRate);
    }

    size_t returnedElems = std::min(bufferedElems, numElems);

    //convert into user's buff0
    if (rxFormat == RTL_RX_FORMAT_FLOAT32)
    {
        /*float *ftarget = (float *)buff0;
        std::complex<float> tmp;
        if (iqSwap)
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                tmp = _lut_swap_32f[*((uint16_t *)&_currentBuff[2 * i])];
                ftarget[i * 2] = tmp.real();
                ftarget[i * 2 + 1] = tmp.imag();
            }
        }
        else
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                tmp = _lut_32f[*((uint16_t *)&_currentBuff[2 * i])];
                ftarget[i * 2] = tmp.real();
                ftarget[i * 2 + 1] = tmp.imag();
            }
        }*/
        std::complex<float> *ftarget = (std::complex<float> *)buff0;
         std::complex<float> *source = (std::complex<float> *)_currentBuff;
         for(int i = 0; i < returnedElems; i++)
        ftarget[i] = source[i];
    }
    else if (rxFormat == RTL_RX_FORMAT_INT16)
    {
        int16_t *itarget = (int16_t *)buff0;
        std::complex<int16_t> tmp;
        if (iqSwap)
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                tmp = _lut_swap_16i[*((uint16_t *)&_currentBuff[2 * i])];
                itarget[i * 2] = tmp.real();
                itarget[i * 2 + 1] = tmp.imag();
            }
        }
        else
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                tmp = _lut_16i[*((uint16_t *)&_currentBuff[2 * i])];
                itarget[i * 2] = tmp.real();
                itarget[i * 2 + 1] = tmp.imag();
            }
        }
    }
    else if (rxFormat == RTL_RX_FORMAT_INT8)
    {
        int8_t *itarget = (int8_t *)buff0;
        if (iqSwap)
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                itarget[i * 2] = _currentBuff[i * 2 + 1] - 128;
                itarget[i * 2 + 1] = _currentBuff[i * 2] - 128;
            }
        }
        else
        {
            for (size_t i = 0; i < returnedElems; i++)
            {
                itarget[i * 2] = _currentBuff[i * 2] - 128;
                itarget[i * 2 + 1] = _currentBuff[i * 2 + 1] - 128;
            }
        }
    }

    //bump variables for next call into readStream
    bufferedElems -= returnedElems;
    _currentBuff += returnedElems * BYTES_PER_SAMPLE;
    bufTicks += returnedElems; //for the next call to readStream if there is a remainder

    //return number of elements written to buff0
    if (bufferedElems != 0)
        flags |= SOAPY_SDR_MORE_FRAGMENTS;
    else
        this->releaseReadBuffer(stream, _currentHandle);
    return returnedElems;
    return 0;
}

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/

size_t SoapyAltiWx::getNumDirectAccessBuffers(SoapySDR::Stream *stream)
{
    return _buffs.size();
}

int SoapyAltiWx::getDirectAccessBufferAddrs(SoapySDR::Stream *stream, const size_t handle, void **buffs)
{
    buffs[0] = (void *)_buffs[handle].data.data();
    return 0;
}

int SoapyAltiWx::acquireReadBuffer(
    SoapySDR::Stream *stream,
    size_t &handle,
    const void **buffs,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    //reset is issued by various settings
    //to drain old data out of the queue
    if (resetBuffer)
    {
        //drain all buffers from the fifo
        _buf_head = (_buf_head + _buf_count.exchange(0)) % numBuffers;
        resetBuffer = false;
        _overflowEvent = false;
    }

    //handle overflow from the rx callback thread
    if (_overflowEvent)
    {
        //drain the old buffers from the fifo
        _buf_head = (_buf_head + _buf_count.exchange(0)) % numBuffers;
        _overflowEvent = false;
        SoapySDR::log(SOAPY_SDR_SSI, "O");
        return SOAPY_SDR_OVERFLOW;
    }

    //wait for a buffer to become available
    if (_buf_count == 0)
    {
        std::unique_lock<std::mutex> lock(_buf_mutex);
        _buf_cond.wait_for(lock, std::chrono::microseconds(timeoutUs), [this] { return _buf_count != 0; });
        if (_buf_count == 0)
            return SOAPY_SDR_TIMEOUT;
    }

    //extract handle and buffer
    handle = _buf_head;
    _buf_head = (_buf_head + 1) % numBuffers;
    bufTicks = _buffs[handle].tick;
    timeNs = SoapySDR::ticksToTimeNs(_buffs[handle].tick, sampleRate);
    buffs[0] = (void *)_buffs[handle].data.data();
    flags = SOAPY_SDR_HAS_TIME;

    //return number available
    return _buffs[handle].data.size() / BYTES_PER_SAMPLE;
}

void SoapyAltiWx::releaseReadBuffer(
    SoapySDR::Stream *stream,
    const size_t handle)
{
    //TODO this wont handle out of order releases
    _buf_count--;
}
