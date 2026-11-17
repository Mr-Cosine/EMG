#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

static const _float pi_f32 = 3.14159265358979f;

// sampling frequency
static const _int sample_freq = 4000;

// coefficient for blur window calculation
static constexpr _int GBLUR_SPAN = 5;

// coefficients for LPF and HPF
static const _int filter_order = 2;
static const _int lpf_cutoff = 500;
static const _int hpf_cutoff = 20;

// coefficients for notch filter
static const _int hum_freq = 60;
static const _float quality_factor = 30.0f;

// How much the processed signal is scaled
static const _float amplification_factor = 2.0f;

#endif
