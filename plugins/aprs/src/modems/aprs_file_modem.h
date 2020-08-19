#pragma once

#include "dsp/modem/modem.h"
#include <string>
#include "afsk/afsk1200_impl.h"
#include <fstream>

#define AFSK_BUFFER_SIZE 2205000

// Simple FM modem writing to a wav file
class ModemAPRSFile : public Modem
{
protected:
    std::shared_ptr<afsk1200_impl> ax25Decoder;
    freqdem demodulatorFM;
    msresamp_rrrf audioResamp;
    int audioRate_m;
    float audioResampRate;
    float kf;
    std::ofstream outTextFile;
    float aprsBuffer[AFSK_BUFFER_SIZE];
    int inAPRSBuffer;
    char fileOutputBuffer[AFSK_BUFFER_SIZE];

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);
    void processAFSK();

public:
    static std::string getType();
    std::vector<std::string> getParameters();
    void setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters);
    void stop();

public:
    static std::shared_ptr<Modem> getInstance();
};