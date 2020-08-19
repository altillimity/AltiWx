#pragma once

#include "modem.h"
#include <string>
#include "libs/tinywav/tinywav.h"

// Simple FM modem writing to a wav file
class ModemFM : public Modem
{
protected:
    freqdem demodulatorFM;
    msresamp_rrrf audioResamp;
    int audioRate_m;
    float audioResampRate;
    TinyWav outWavFile;
    float kf;

protected:
    // Buffers...
    unsigned int bufferOutSize;
    float *outputBuffer;
    float *demodBuffer;

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