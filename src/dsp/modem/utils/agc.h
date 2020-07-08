/* 
AGC from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) 
Ported to C++ & Made standalone
*/

#pragma once

#include <complex>

/* AGC default parameters */
#define AGC_WINSIZE 1024 * 64
#define AGC_TARGET 0.25f
#define AGC_MAX_GAIN 4000
#define AGC_BIAS_WINSIZE 256 * 1024

typedef struct
{
    unsigned window_size;
    float avg;
    float gain;
    float target_ampl;
    std::complex<float> bias;
} Agc;

Agc *agc_init(void);
std::complex<float> agc_apply(Agc *agc, std::complex<float> sampl);
void agc_free(Agc *agc);