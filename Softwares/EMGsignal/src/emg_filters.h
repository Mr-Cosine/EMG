#ifndef EMG_FILTERS_H
#define EMG_FILTERS_H
#include "types.h"
#include "filter_core.h"
#include "blur.h"
#include "constants.h"

template <typename dataType>
class EMGFilters {
    private:
        Lopass LPF;
        Hipass HPF;
        Notch  AHF;
        Gaussian_blur<dataType, GBLUR_SPAN> GB;

        bool bypass_all;
        bool notch_enabled;
        bool lopass_enabled;
        bool hipass_enabled;
        bool blur_enabled;

    public:
        EMGFilters(bool bypass_all, bool enable_notch, bool enable_lopass, bool enable_hipass, bool enable_blur):
            GB(true),
            bypass_all(bypass_all || (!enable_notch && !enable_lopass && !enable_hipass && !enable_blur)),
            notch_enabled(enable_notch),
            lopass_enabled(enable_lopass),
            hipass_enabled(enable_hipass),
            blur_enabled(enable_blur)
        {
            // every stage is initialised so the accessors below are always usable,
            // even for stages that process() skips
            LPF.init();
            HPF.init();
            AHF.init(quality_factor);
        }

        Lopass& lpf() { return LPF; }
        Hipass& hpf() { return HPF; }
        Notch&  ahf() { return AHF; }
        Gaussian_blur<dataType, GBLUR_SPAN>& gblur() { return GB; }

        dataType process(dataType inputValue) {
            if (this->bypass_all) return inputValue;

            f32 output = (f32)inputValue;

            if (this->hipass_enabled) output = HPF.process(output);
            if (this->lopass_enabled) output = LPF.process(output);
            if (this->notch_enabled)  output = AHF.process(output);
            if (this->blur_enabled)   output = (f32)GB.blur((dataType)output);

            return (dataType)output;
        }
};

#endif
