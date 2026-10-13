#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

// sampling frequency
static const int sample_freq = 2000;

// coefficient for blur window calculation
static const double gb_factor = 6.0;

// coefficients for LPF and HPF
static const int filter_order = 2;
static const int lpf_cutoff = 20;
static const int hpf_cutoff = 500;

// coefficients for notch filter
static const int hum_freq = 60;

#endif