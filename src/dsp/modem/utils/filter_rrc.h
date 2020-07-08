/* 
RRC Filter from meteor_demod (https://github.com/dbdexter-dev/meteor_demod) 
Ported to C++ & Made standalone
*/

#pragma once

#include <complex>

typedef struct
{
	std::complex<float> *mem;
	unsigned fwd_count;
	unsigned stage_no;
	float *fwd_coeff;
} Filter;

Filter *filter_new(unsigned fwd_count, double *fwd_coeff);
Filter *filter_copy(const Filter *orig);

Filter *filter_rrc(unsigned order, unsigned factor, float osf, float alpha);

std::complex<float> filter_fwd(Filter *flt, std::complex<float> in);
void filter_free(Filter *flt);
