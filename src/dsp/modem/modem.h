#pragma once

#include <vector>
#include <cstdint>
#include <complex>
#include <liquid/liquid.h>
#include <mutex>
#include <thread>
#include <future>
#include <unordered_map>
#include <functional>

// Modem object, that can be attached onto a DSP chain to decode something in the sampled spectrum
class Modem
{
protected:
    std::mutex modemMutex;
    nco_crcf freqShifter;
    msresamp_crcf freqResampler;
    long inputFrequency_m, inputRate_m;
    long bandwidth_m;
    long frequency_m;
    long shiftFrequency;
    float freqResampRate;
    std::unordered_map<std::string, std::string> parameters_m;

protected:
    // Actual demodulation work, to be implemented by child classes
    virtual void process(liquid_float_complex *buffer, unsigned int &length) = 0;
    // Initialize main resampler or other stuff
    virtual void initResamp(long inputRate, long inputFrequency);

public:
    std::future<void> modemFuture;

public:
    // Parameter stuff
    virtual std::vector<std::string> getParameters() = 0;
    virtual void setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters);
    // Modem type
    static std::string getType();
    // Init when added into a DSP chain
    void init(long inputRate, long inputFrequency);
    // Perform resampling / shifting work
    void demod(liquid_float_complex *buffer, uint32_t length);
    // Stop before dettaching
    virtual void stop() = 0;
    // Change effective frequency, live (doppler as an example)
    void setFrequency(long frequency);

public:
    static std::shared_ptr<Modem> getInstance();
};

extern std::unordered_map<std::string, std::function<std::shared_ptr<Modem>()>> modemRegistry;

void registerModems();