#ifndef EMGFILTERS_H
#define EMGFILTERS_H
#include "filter_core.h"
#include "constants.h"

template <typename dataType>
class EMGFilters {
    private:
        Lopass LPF;
        Hipass HPF;
        Notch AHF;

        bool  m_bypassEnabled;
        bool  m_notchFilterEnabled;
        bool  m_lowpassFilterEnabled;
        bool  m_highpassFilterEnabled;
        
    public:
        void init(bool bypass_all, bool enable_notch, bool enable_lopass, bool enable_hipass) {

            m_bypassEnabled = bypass_all;

            LPF.init();
            HPF.init();
            AHF.init(30.0);

            m_notchFilterEnabled    = enable_notch;
            m_lowpassFilterEnabled  = enable_lopass;
            m_highpassFilterEnabled = enable_hipass;
        }

        dataType process(dataType inputValue) {
            if (this->m_bypassEnabled) return inputValue;

            double output = (double)inputValue;

            if (m_highpassFilterEnabled) output = HPF.process(output);
            if (m_lowpassFilterEnabled) output = LPF.process(output);
            if (m_notchFilterEnabled) output = AHF.process(output);

            return (dataType)output;
        }
};

#endif
