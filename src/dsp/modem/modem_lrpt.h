#pragma once

#include "modem.h"
#include <string>
#include <fstream>
#include <vector>
#include <thread>

extern "C"
{
#include "lrpt/demod.h"
#include "lrpt/source.h"
}

class ModemLRPT : public Modem
{
protected:
    int i;
    Demod *demod;
    Source *samp;
    std::vector<liquid_float_complex> sampleBuffer;
    std::mutex vectorMutex;
    std::thread startThread;
    std::string outputFile_m;
    bool exiting = false;

protected:
    void process(liquid_float_complex *buffer, unsigned int &length);
    void startThreadFunc();
    static int data_read(Source *self, size_t count);
    static uint64_t data_get_done(const Source *self);
    static uint64_t data_get_size(const Source *self);
    static int data_close(Source *self);

public:
    ModemLRPT(long frequency, long bandwidth, std::string outputFile);
    void stop();
};