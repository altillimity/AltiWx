/**
 * Moving average AGC, there's not much to it really
 */
#ifndef METEOR_AGC_H
#define METEOR_AGC_H

/* AGC default parameters */
#define AGC_WINSIZE 1024*64
#define AGC_TARGET 180
#define AGC_MAX_GAIN 20
#define AGC_BIAS_WINSIZE 256*1024

#include <liquid/liquid.h>

typedef struct {
	unsigned window_size;
	float avg;
	float gain;
	float target_ampl;
	liquid_float_complex bias;
} Agc;

Agc*          agc_init(void);
liquid_float_complex agc_apply(Agc *agc, liquid_float_complex sampl);
void          agc_free(Agc *agc);

#endif
