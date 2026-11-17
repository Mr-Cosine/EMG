#ifndef BLUR_H
#define BLUR_H

#include <math.h>
#include "types.h"
#include "queue.h"
#include "constants.h"

template <typename dataType, int blur_span>
class Gaussian_blur {
    private:
        static constexpr integer kernel_size = blur_span*2+1;
        float kernel[kernel_size];
        Queue<dataType, kernel_size> state;

        void _create_kernel() {
            if (kernel_size == 1) kernel[0] = 1.0f;
            else {
                float sigma = (kernel_size - 1) / (2.0f * blur_span);
                float coeff = 1.0f / (sigma * sqrtf(2.0f * pi_f32));
                float x0 = (kernel_size - 1.0f) / 2.0f;

                float sum = 0.0f;
                for (integer i = 0; i < kernel_size; i++) {
                    float x = i - x0;
                    kernel[i] = coeff * expf(-(x * x) / (2.0f * sigma * sigma));
                    sum += kernel[i];
                }
                for (integer i = 0; i < kernel_size; i++) {
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

            float result = 0.0f;
            for (integer i = 0; i < kernel_size; i++) {
                result += (float)state[i] * kernel[i];
            }

            return (dataType)result;
        }

        static constexpr integer blur_length() { return kernel_size; }
};

#endif
