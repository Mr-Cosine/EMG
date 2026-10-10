#ifndef _FILTERS_H
#define _FILTERS_H
#include "constants.h"

// second-order filter

// 2th order butterworth lowpass filter
// cutoff frequency 150Hz

// 2th order butterworth highpass filter
// cutoff frequency 20Hz

class FILTER_2nd {
  private:
    float states[2];
    float num[3];
    float den[3];

  public:
    void init(FILTER_TYPE ftype, int sampleFreq) {
        states[0] = 0;
        states[1] = 0;
        if (ftype == LOWPASS) {

            if (sampleFreq == 500) {
                for (int i = 0; i < 3; i++) {
                    num[i] = lpf_numerator_coef[0][i];
                    den[i] = lpf_denominator_coef[0][i];
                }
            } else if (sampleFreq == 1000) {
                for (int i = 0; i < 3; i++) {
                    num[i] = lpf_numerator_coef[1][i];
                    den[i] = lpf_denominator_coef[1][i];
                }
            }
        } else if (ftype == HIGHPASS) {
            if (sampleFreq == 500) {
                for (int i = 0; i < 3; i++) {
                    num[i] = hpf_numerator_coef[0][i];
                    den[i] = hpf_denominator_coef[0][i];
                }
            } else if (sampleFreq == 1000) {
                for (int i = 0; i < 3; i++) {
                    num[i] = hpf_numerator_coef[1][i];
                    den[i] = hpf_denominator_coef[1][i];
                }
            }
        }
    }

    float process(float input) {
        float tmp = (input - den[1] * states[0] - den[2] * states[1]) / den[0];
        float output = num[0] * tmp + num[1] * states[0] + num[2] * states[1];
        // save last states
        states[1] = states[0];
        states[0] = tmp;
        return output;
    }
};

class FILTER_4th {
  private:
    float states[4];
    float num[6];
    float den[6];
    float gain;

  public:
    void init(int sampleFreq, int humFreq) {
        gain = 0;
        for (int i = 0; i < 4; i++) {
            states[i] = 0;
        }
        if (humFreq == 50) {
            if (sampleFreq == 500) {
                for (int i = 0; i < 6; i++) {
                    num[i] = ahf_numerator_coef_50Hz[0][i];
                    den[i] = ahf_denominator_coef_50Hz[0][i];
                }
                gain = ahf_output_gain_coef_50Hz[0];
            } else if (sampleFreq == 1000) {
                for (int i = 0; i < 6; i++) {
                    num[i] = ahf_numerator_coef_50Hz[1][i];
                    den[i] = ahf_denominator_coef_50Hz[1][i];
                }
                gain = ahf_output_gain_coef_50Hz[1];
            }
        } else if (humFreq == 60) {
            if (sampleFreq == 500) {
                for (int i = 0; i < 6; i++) {
                    num[i] = ahf_numerator_coef_60Hz[0][i];
                    den[i] = ahf_denominator_coef_60Hz[0][i];
                }
                gain = ahf_output_gain_coef_60Hz[0];
            } else if (sampleFreq == 1000) {
                for (int i = 0; i < 6; i++) {
                    num[i] = ahf_numerator_coef_60Hz[1][i];
                    den[i] = ahf_denominator_coef_60Hz[1][i];
                }
                gain = ahf_output_gain_coef_60Hz[1];
            }
        }
    }

    float process(float input) {
        float output;
        float stageIn;
        float stageOut;

        stageOut  = num[0] * input + states[0];
        states[0] = (num[1] * input + states[1]) - den[1] * stageOut;
        states[1] = num[2] * input - den[2] * stageOut;
        stageIn   = stageOut;
        stageOut  = num[3] * stageOut + states[2];
        states[2] = (num[4] * stageIn + states[3]) - den[4] * stageOut;
        states[3] = num[5] * stageIn - den[5] * stageOut;

        output = gain * stageOut;

        return output;
    }
};

#endif
