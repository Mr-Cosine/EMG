#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// coefficients of transfer function of LPF
// coef[sampleFreqInd][order]
static float lpf_numerator_coef[2][3] = {{0.3913, 0.7827, 0.3913},
                                         {0.1311, 0.2622, 0.1311}};
static float lpf_denominator_coef[2][3] = {{1.0000, 0.3695, 0.1958},
                                           {1.0000, -0.7478, 0.2722}};
// coefficients of transfer function of HPF
static float hpf_numerator_coef[2][3] = {{0.8371, -1.6742, 0.8371},
                                         {0.9150, -1.8299, 0.9150}};
static float hpf_denominator_coef[2][3] = {{1.0000, -1.6475, 0.7009},
                                           {1.0000, -1.8227, 0.8372}};

// coefficients of transfer function of anti-hum filter
// coef[sampleFreqInd][order] for 50Hz
static float ahf_numerator_coef_50Hz[2][6] = {
    {0.9522, -1.5407, 0.9522, 0.8158, -0.8045, 0.0855},
    {0.5869, -1.1146, 0.5869, 1.0499, -2.0000, 1.0499}};
static float ahf_denominator_coef_50Hz[2][6] = {
    {1.0000, -1.5395, 0.9056, 1.0000, -1.1187, 0.3129},
    {1.0000, -1.8844, 0.9893, 1.0000, -1.8991, 0.9892}};
static float ahf_output_gain_coef_50Hz[2] = {1.3422, 1.4399};
// coef[sampleFreqInd][order] for 60Hz
static float ahf_numerator_coef_60Hz[2][6] = {
    {0.9528, -1.3891, 0.9528, 0.8272, -0.7225, 0.0264},
    {0.5824, -1.0810, 0.5824, 1.0736, -2.0000, 1.0736}};
static float ahf_denominator_coef_60Hz[2][6] = {
    {1.0000, -1.3880, 0.9066, 1.0000, -0.9739, 0.2371},
    {1.0000, -1.8407, 0.9894, 1.0000, -1.8584, 0.9891}};
static float ahf_output_gain_coef_60Hz[2] = {1.3430, 1.4206};

enum FILTER_TYPE { LOWPASS = 0, HIGHPASS = 1 };

#endif