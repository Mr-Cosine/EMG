#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

// sampling frequency
static const int sample_freq = 2000;

// estimated process time
static const int process_time_budget = 200;

// coefficient for blur window calculation
static constexpr int GBLUR_SPAN = 3;

// coefficients for LPF and HPF
static const int filter_order = 2;
static const int lpf_cutoff = 500;
static const int hpf_cutoff = 20;

// coefficients for notch filter
static const int hum_freq = 60;

// How much the processed signal is scaled
double amplification_factor = 2.0;

#endif