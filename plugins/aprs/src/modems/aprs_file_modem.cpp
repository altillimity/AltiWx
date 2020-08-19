#include "aprs_file_modem.h"
#include <math.h>
#include "logger/logger.h"

std::vector<std::string> ModemAPRSFile::getParameters()
{
    return {};
}

void ModemAPRSFile::setParameters(long frequency, long bandwidth, std::unordered_map<std::string, std::string> &parameters)
{
    // Setup local variables
    audioRate_m = 22050;
    kf = 1.0f;
    frequency_m = frequency;
    bandwidth_m = bandwidth;
    demodulatorFM = freqdem_create(kf);                            // Demodulator
    audioResampRate = (double)audioRate_m / (double)bandwidth_m;   // Audio resampling
    audioResamp = msresamp_rrrf_create(audioResampRate, 60.0f);    // Audio resampler
    ax25Decoder = std::make_shared<afsk1200_impl>(audioRate_m, 1); // AFSK1200 Decoder
    outTextFile = std::ofstream(parameters["output_file"], std::ios::binary);

    inAPRSBuffer = 0;
}

void ModemAPRSFile::stop()
{
    // Flush buffer
    processAFSK();
    // Just close the text file!
    outTextFile.close();
}

// Run the QFSk buffer through and write output
void ModemAPRSFile::processAFSK()
{
    int afskOut = ax25Decoder->work(aprsBuffer, AFSK_BUFFER_SIZE, fileOutputBuffer); // AFSK1200 Decode
    if (afskOut > 0)
    {
        logger->trace(std::string(fileOutputBuffer));
        outTextFile.write((char *)fileOutputBuffer, afskOut); // Write to file
    }
    inAPRSBuffer = 0;
    outTextFile.flush(); // Since we write at pretty wide intervals, write to disk right now
}

void ModemAPRSFile::process(liquid_float_complex *buffer, unsigned int &length)
{
    // Buffers...
    unsigned int bufferOutSize = ceil(audioResampRate * (float)length);
    float outputBuffer[bufferOutSize];
    float demodBuffer[length];

    freqdem_demodulate_block(demodulatorFM, &buffer[0], length, &demodBuffer[0]);                       // FM demodulation
    msresamp_rrrf_execute(audioResamp, &demodBuffer[0], (int)length, &outputBuffer[0], &bufferOutSize); // Audio resampling

    // Push into buffer until its full, then process and push remaining if any
    if (inAPRSBuffer + bufferOutSize >= AFSK_BUFFER_SIZE)
    {
        int missing = AFSK_BUFFER_SIZE - inAPRSBuffer;
        std::memcpy(&aprsBuffer[inAPRSBuffer], outputBuffer, missing);
        inAPRSBuffer += missing;
        processAFSK();
        if (inAPRSBuffer - missing > 0)
        {
            std::memcpy(&aprsBuffer[inAPRSBuffer], &outputBuffer[missing], (inAPRSBuffer - missing) * sizeof(float));
            inAPRSBuffer += inAPRSBuffer - missing;
        }
    }
    else
    {
        std::memcpy(&aprsBuffer[inAPRSBuffer], outputBuffer, bufferOutSize * sizeof(float));
        inAPRSBuffer += bufferOutSize;
    }
}

std::string ModemAPRSFile::getType()
{
    return "APRS_FILE";
}

std::shared_ptr<Modem> ModemAPRSFile::getInstance()
{
    return std::make_shared<ModemAPRSFile>();
}