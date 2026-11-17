#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

static const float pi_f32 = 3.14159265358979f;

// sampling frequency
static const integer sample_freq = 2000;

// coefficient for blur window calculation
static constexpr integer GBLUR_SPAN = 5;

// coefficients for LPF and HPF
static const integer filter_order = 2;
static const integer lpf_cutoff = 500;
static const integer hpf_cutoff = 20;

// coefficients for notch filter
static const integer hum_freq = 60;
static const float quality_factor = 30.0f;

// How much the processed signal is scaled
static const float amplification_factor = 2.0f;

#endif
