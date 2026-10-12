#ifndef BLUR_H
#define BLUR_H

#include <math.h>
#include "queue.h"
#include "constants.h"
 
template <typename Number>
class gaussian_blur {
    private:
        float* kernel;
        int kernel_size;
        int blur_window_len;
 
        void _create_kernel(int new_size) {
            if (new_size < 1) new_size = 1;
 
            delete[] kernel;
            kernel_size = new_size;
            kernel = new float[kernel_size];
 
            // A single-sample kernel is just a pass-through (avoids sigma = 0).
            if (kernel_size == 1) {
                kernel[0] = 1.0f;
                return;
            }
 
            float sigma = (kernel_size - 1) / 6.0f;
            float coeff = 1.0f / (sigma * sqrtf(2.0f * (float)M_PI));
            float x0 = (kernel_size - 1) / 2.0f;
 
            float sum = 0.0f;
            for (int i = 0; i < kernel_size; i++) {
                float x = i - x0;
                kernel[i] = coeff * expf(-(x * x) / (2.0f * sigma * sigma));
                sum += kernel[i];
            }
            for (int i = 0; i < kernel_size; i++) {
                kernel[i] /= sum;
            }
        }
 
    public:
        gaussian_blur(float GBLUR_SIGMA):
            kernel(nullptr),
            kernel_size(0),
            blur_window_len(0)
        {
            _create_kernel((int)(GBLUR_SIGMA * 6) + 1);
        }
 
        gaussian_blur(const gaussian_blur&) = delete;
        gaussian_blur& operator=(const gaussian_blur&) = delete;
 
        ~gaussian_blur() {
            delete[] kernel;
        }
 
        template <typename QueueType, int N>
        QueueType blur(const Queue<QueueType, N>& data_stream) {
            blur_window_len = data_stream.size();
            if (blur_window_len == 0) return (QueueType)0;
 
            if (blur_window_len != kernel_size) _create_kernel(blur_window_len);
 
            float result = 0.0f;
            for (int i = 0; i < kernel_size; i++) {
                result += (float)data_stream[i] * kernel[i];
            }
 
            return (QueueType)result;
        }
 
        int window_len() const { return blur_window_len; }
};
 
#endif
