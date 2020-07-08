/* 
AGC from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) 
Ported to C++ & Made standalone
*/

#include "agc.h"

/* Initialize an AGC object */
Agc *agc_init()
{
    Agc *agc;

    agc = (Agc *)malloc(sizeof(*agc));
    agc->window_size = AGC_WINSIZE;
    agc->target_ampl = AGC_TARGET;
    agc->avg = AGC_TARGET;
    agc->gain = 1;
    agc->bias = 0;

    return agc;
}

/* Apply the right gain to a sample */
std::complex<float> agc_apply(Agc *self, std::complex<float> sample)
{
    float rho;

    self->bias = (self->bias * ((std::complex<float>)(AGC_BIAS_WINSIZE - 1)) + sample) / ((std::complex<float>)(AGC_BIAS_WINSIZE));
    sample -= self->bias;

    /* Update the sample magnitude average */
    rho = sqrtf(sample.real() * sample.real() + sample.imag() * sample.imag());
    self->avg = (self->avg * (self->window_size - 1) + rho) / self->window_size;

    self->gain = self->target_ampl / self->avg;
    if (self->gain > AGC_MAX_GAIN)
    {
        self->gain = AGC_MAX_GAIN;
    }
    return sample * self->gain;
}

/* Free an AGC object */
void agc_free(Agc *self)
{
    free(self);
}