#ifndef _EMGFILTERS_H
#define _EMGFILTERS_H
#include "Filters.h"
#include "constants.h"
 
/* @brief EMGFilter provides:
*        anti-hum notch filter to filter out 50HZ or 60HZ power line noise
*        lowpass filter to filter out signals above 150HZ
*        highpass filter to filter out noise below 20HZ;
* @remark only accept 500Hz and 1000Hz sampling rate as parameter when initialize
*/

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
