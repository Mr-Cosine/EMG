#ifndef BLUR_H
#define BLUR_H

#include <math.h>
#include "queue.h"
#include "constants.h"
 
template <typename dataType, int kernel_size>
class Gaussian_blur {
    private:
        double* kernel;
        Queue<dataType, kernel_size> state;
 
        void _create_kernel() {
            if (kernel != nullptr) delete[] kernel;
            kernel = new double[kernel_size];
 
            // A single-sample kernel is just a pass-through (avoids sigma = 0).
            if (kernel_size == 1) {
                kernel[0] = 1.0f;
                return;
            }
 
            double sigma = (kernel_size - 1) / gb_factor;
            double coeff = 1.0 / (sigma * sqrt(2.0 * (float)M_PI));
            double x0 = (kernel_size - 1.0) / 2.0;
 
            double sum = 0.0;
            for (int i = 0; i < kernel_size; i++) {
                double x = i - x0;
                kernel[i] = coeff * exp(-(x * x) / (2.0 * sigma * sigma));
                sum += kernel[i];
            }
            for (int i = 0; i < kernel_size; i++) {
                kernel[i] /= sum;
            }
        }
 
    public:
        Gaussian_blur(bool flush = true, dataType filler = dataType(0)):
            kernel(nullptr),
            state(flush, filler)
        {
            _create_kernel();
        }
 
        Gaussian_blur(const Gaussian_blur&) = delete;
        Gaussian_blur& operator=(const Gaussian_blur&) = delete;
 
        ~Gaussian_blur() {
            if (this->kernel != nullptr) delete[] this->kernel;
        }
 
        dataType blur(dataType new_data) {
            this->state.update(new_data);

            double result = 0.0;
            for (int i = 0; i < kernel_size; i++) {
                result += (double)state[i] * kernel[i];
            }
 
            return (dataType)result;
        }
 
        static constexpr int blur_length() { return kernel_size; }
};
 
#endif
