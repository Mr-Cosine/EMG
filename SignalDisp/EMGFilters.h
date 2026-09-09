#ifndef _EMGFILTERS_H
#define _EMGFILTERS_H
#include "Filters.h"
#include "constants.h"

class EMGFilters {
  public:
  /*
  * @param sampleFreq only supports SAMPLE_FREQ_500HZ & SAMPLE_FREQ_1000HZ.
  * @param notchFreq only supports NOTCH_FREQ_50HZ & NOTCH_FREQ_60HZ.
  * @param enableNotchFilter enables the notch filter. True by default.
  * @param enableLowpassFilter enables the lowpass filter. True by default.
  * @param enableHighpassFilter enables the highpass filter. True by default.
  */
    void init(
              int   sampleFreq,
              int   notchFreq,
              bool  enableNotchFilter    = true,
              bool  enableLowpassFilter  = true,
              bool  enableHighpassFilter = true
            );

    int filter(int inputValue);

  private:
    FILTER_2nd LPF;
    FILTER_2nd HPF;
    FILTER_4th AHF;

    int   m_sampleFreq;
    int   m_notchFreq;
    bool  m_bypassEnabled;
    bool  m_notchFilterEnabled;
    bool  m_lowpassFilterEnabled;
    bool  m_highpassFilterEnabled;
};

#endif
