#pragma once

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Logger.h>
#include <SoapySDR/Types.h>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <fstream>
#include "zmq.hpp"

class SoapyAltiWx : public SoapySDR::Device
{
public:
    SoapyAltiWx(const SoapySDR::Kwargs &args);
    ~SoapyAltiWx(void);
};
