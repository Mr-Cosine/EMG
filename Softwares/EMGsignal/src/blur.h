#ifndef BLUR_H
#define BLUR_H

#include <math.h>
#include "types.h"
#include "queue.h"
#include "constants.h"

template <typename dataType, int blur_span>
class Gaussian_blur {
    private:
        static constexpr i32 kernel_size = blur_span*2+1;
        f32 kernel[kernel_size];
        Queue<dataType, kernel_size> state;

        void _create_kernel() {
            if (kernel_size == 1) kernel[0] = 1.0f;
            else {
                f32 sigma = (kernel_size - 1) / (2.0f * blur_span);
                f32 coeff = 1.0f / (sigma * sqrtf(2.0f * pi_f32));
                f32 x0 = (kernel_size - 1.0f) / 2.0f;

                f32 sum = 0.0f;
                for (i32 i = 0; i < kernel_size; i++) {
                    f32 x = i - x0;
                    kernel[i] = coeff * expf(-(x * x) / (2.0f * sigma * sigma));
                    sum += kernel[i];
                }
                for (i32 i = 0; i < kernel_size; i++) {
                    kernel[i] /= sum;
                }
            }
        }

    public:
        Gaussian_blur(bool flush = true, dataType filler = dataType(0)):
            state(flush, filler)
        {
            _create_kernel();
        }

        dataType blur(dataType new_data) {
            this->state.update(new_data);

            f32 result = 0.0f;
            for (i32 i = 0; i < kernel_size; i++) {
                result += (f32)state[i] * kernel[i];
            }

            return (dataType)result;
        }

        static constexpr i32 blur_length() { return kernel_size; }
};

#endif
