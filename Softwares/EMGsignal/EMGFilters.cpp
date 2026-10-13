#include "EMGFilters.h"

void EMGFilters::init(
                     int    sampleFreq,
                     int    notchFreq,
                     bool   bypass,
                     bool   enableNotchFilter,
                     bool   enableLowpassFilter,
                     bool   enableHighpassFilter) {
    m_sampleFreq = sampleFreq;
    m_notchFreq  = notchFreq;

    m_bypassEnabled = bypass;

    LPF.init();
    HPF.init();
    AHF.init(30.0);

    m_notchFilterEnabled    = enableNotchFilter;
    m_lowpassFilterEnabled  = enableLowpassFilter;
    m_highpassFilterEnabled = enableHighpassFilter;
}

int EMGFilters::process(int inputValue) {
    if (this->m_bypassEnabled) return inputValue;

    int output = inputValue;

    // high pass filter -> pick up signal
    if (m_highpassFilterEnabled) output = HPF.process(output);
    //low pass filter -> filter noise
    if (m_lowpassFilterEnabled) output = LPF.process(output);
    // notch filter -> filter hum
    if (m_notchFilterEnabled) output = AHF.process(output);

    return output;
}
