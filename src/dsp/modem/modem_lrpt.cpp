#include "modem_lrpt.h"
#include <math.h>
#include "logger/logger.h"

#define SYM_RATE 72000
#define SYM_CHUNKSIZE 1024
#define RRC_ALPHA 0.6
#define RRC_FIR_ORDER 64
#define INTERP_FACTOR 4

#include <complex.h>

ModemLRPT::ModemLRPT(long frequency, long bandwidth, std::string outputFile) : outputFile_m(outputFile)
{
    frequency_m = frequency;
    bandwidth_m = bandwidth;

    samp = new Source;

    samp->_backend = (void *)this;
    samp->count = 0;
    samp->data = NULL;
    samp->read = ModemLRPT::data_read;
    samp->close = ModemLRPT::data_close;
    samp->size = ModemLRPT::data_get_size;
    samp->done = ModemLRPT::data_get_done;

    startThread = std::thread(&ModemLRPT::startThreadFunc, this);
}

void ModemLRPT::stop()
{
    exiting = true;
    demod_join(demod);
    samp->close(samp);
    if (startThread.joinable())
        startThread.join();
    delete samp;
}

void ModemLRPT::process(liquid_float_complex *buffer, unsigned int &length)
{
    vectorMutex.lock();
    for (i = 0; i < length; i++)
        sampleBuffer.push_back(buffer[i]);
    vectorMutex.unlock();
}

int ModemLRPT::data_read(Source *self, size_t count)
{
    ModemLRPT *lrpt = (ModemLRPT *)self->_backend;

    logger->info(count);

    while (lrpt->sampleBuffer.size() < count && !lrpt->exiting)
        std::this_thread::sleep_for(std::chrono::microseconds(1));

    logger->info("test2");

    lrpt->vectorMutex.lock();

    size_t i;

    if (!self->data)
    {
        self->data = (liquid_float_complex *)malloc(count * sizeof(*self->data));
    }
    else if (self->count < count)
    {
        free(self->data);
        self->data = (liquid_float_complex *)malloc(count * sizeof(*self->data));
    }

    for (i = 0; i < count && i < lrpt->sampleBuffer.size(); i++)
        self->data[i] = lrpt->sampleBuffer[i];

    lrpt->sampleBuffer.erase(lrpt->sampleBuffer.begin(), lrpt->sampleBuffer.begin() + i);
    lrpt->vectorMutex.unlock();

    return i;
}

uint64_t ModemLRPT::data_get_done(const Source *self)
{
    return 0;
}

uint64_t ModemLRPT::data_get_size(const Source *self)
{
    return 0;
}

int ModemLRPT::data_close(Source *self)
{
    return 0;
}

void ModemLRPT::startThreadFunc()
{
    demod = demod_init(samp, INTERP_FACTOR, RRC_FIR_ORDER, RRC_ALPHA, COSTAS_BW, SYM_RATE, QPSK);
    demod_start(demod, outputFile_m.c_str());
}