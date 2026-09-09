#include "EMGFilters.h"

void EMGFilters::init(
                     int    sampleFreq,
                     int    notchFreq,
                     bool   enableNotchFilter,
                     bool   enableLowpassFilter,
                     bool   enableHighpassFilter) {
    m_sampleFreq = sampleFreq;
    m_notchFreq  = notchFreq;
    m_bypassEnabled = true;

    // If parameter valid, use the filter
    if (((sampleFreq == 500) || (sampleFreq == 1000)) &&
        ((notchFreq == 50) || (notchFreq == 60))) {
            m_bypassEnabled = false;
    }

    LPF.init(LOWPASS, m_sampleFreq);
    HPF.init(HIGHPASS, m_sampleFreq);
    AHF.init(m_sampleFreq, m_notchFreq);

    m_notchFilterEnabled    = enableNotchFilter;
    m_lowpassFilterEnabled  = enableLowpassFilter;
    m_highpassFilterEnabled = enableHighpassFilter;
}

int EMGFilters::filter(int inputValue) {
    if (m_bypassEnabled) return inputValue;

    int output = inputValue;

    // high pass filter -> pick up signal
    if (m_highpassFilterEnabled) output = HPF.process(output);
    //low pass filter -> filter noise
    if (m_lowpassFilterEnabled) output = LPF.process(output);
    // notch filter -> filter hum
    if (m_notchFilterEnabled) output = AHF.process(output);
    else output = inputValue;

    return output;
}
