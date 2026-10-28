#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

static const f32 pi_f32 = 3.14159265358979f;

// sampling frequency
static const i32 sample_freq = 2000;

// coefficient for blur window calculation
static constexpr i32 GBLUR_SPAN = 5;

// coefficients for LPF and HPF
static const i32 filter_order = 2;
static const i32 lpf_cutoff = 500;
static const i32 hpf_cutoff = 20;

// coefficients for notch filter
static const i32 hum_freq = 60;
static const f32 quality_factor = 30.0f;

// How much the processed signal is scaled
static const f32 amplification_factor = 2.0f;

#endif
