#ifndef _BLUR_H
#define _BLUR_H
#include <math.h>
#include "queue.h"

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

template <typename Number>
class gaussian_blur {
    private:
        float* kernel = nullptr;
        int kernel_size = 0;


        void create_kernel() {
            delete[] kernel;
            kernel = new float[this->kernel_size];

            float sigma = (this->kernel_size - 1) / 6.0f;
            float coeff = 1.0f / (sigma * sqrt(2.0f * (float)M_PI));

            float sum = 0.0f;
            float x0 = (this->kernel_size + 1) / 2.0f;
            for (int i = 0; i < this->kernel_size; i++) {
                int x = i - x0;
                this->kernel[i] = coeff * exp(-(x * x) / (2.0f * sigma * sigma));
                sum += kernel[i];
            }
            for (int i = 0; i < kernel_size; i++) {
                this->kernel[i] /= sum;
            }
        }
    public:
        ~gaussian_blur() {
            delete[] kernel;
        }
        
        template <typename QueueType, int N>
        QueueType blur(const Queue<QueueType, N>& data_stream) {         
            int data_length = (int)data_stream.size();

            if (data_length != this->kernel_size) {
                this->kernel_size = data_length;
                create_kernel();
            }

            float result = 0.0f;
            for (int i = 0; i < this->kernel_size; i++) {
                result += (float)data_stream[i] * this->kernel[i];
            }

            return (QueueType)result;
        }
};

#endif