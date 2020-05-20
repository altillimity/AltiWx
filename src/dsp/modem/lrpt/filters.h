/**
 * Various DSP filters. filter_new() can be used for both FIR and IIR filters.
 * If back_count == 0, the filter will be FIR, otherwise it'll be IIR. Right now
 * this is used only to build the interpolating root-raised cosine filter
 */
#ifndef METEOR_FILTERS_H
#define METEOR_FILTERS_H

#include <liquid/liquid.h>

typedef struct {
	liquid_float_complex *restrict mem;
	unsigned fwd_count;
	unsigned stage_no;
	float *restrict fwd_coeff;
} Filter;

Filter*       filter_new(unsigned fwd_count, double *fwd_coeff);
Filter*       filter_copy(const Filter *orig);

Filter*       filter_rrc(unsigned order, unsigned factor, float osf, float alpha);

liquid_float_complex filter_fwd(Filter *flt, liquid_float_complex in);
void          filter_free(Filter *flt);

#endif
