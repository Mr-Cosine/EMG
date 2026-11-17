#ifndef FILTER_CORE_H
#define FILTER_CORE_H
#include "types.h"
#include "constants.h"
#include "queue.h"
#include "complexf.h"
#include <math.h>

class Butterworth {
    private:
        Queue<float, filter_order+1> input_state;
        Queue<float, filter_order> output_state;
        float a[filter_order + 1];
        float b[filter_order + 1];

        void _create_kernel(const Complexf* roots, integer n, float res[]) {
            Complexf c[filter_order + 1];

            c[0] = Complexf(1.0f, 0.0f);
            for (integer i = 1; i <= n; ++i) c[i] = Complexf(0.0f, 0.0f);

            for (integer k = 0; k < n; ++k) {
                for (integer i = k + 1; i >= 1; --i) c[i] -= roots[k] * c[i - 1];
            }

            for (integer i = 0; i <= filter_order; i++) {
                res[i] = c[i].real();
            }
        }

    public:
        Butterworth() {
            for (integer i = 0; i <= filter_order; ++i) { a[i] = 0.0f; b[i] = 0.0f; }
            a[0] = 1.0f;
            b[0] = 1.0f;
            this->clear_state();
        }

        void clear_state() {
            this->input_state.flush(0.0f);
            this->output_state.flush(0.0f);
        }

        void init(float cutoff, bool is_lpf) {
            for (integer i = 0; i <= filter_order; ++i) {
                a[i] = 0.0f;
                b[i] = 0.0f;
            }
            this->clear_state();

            if (cutoff <= 0.0f || cutoff >= sample_freq / 2.0f) return;

            const float K  = 2.0f * sample_freq;
            const float Wc = K * tanf(pi_f32 * cutoff / sample_freq);

            Complexf poles[filter_order];
            Complexf zeros[filter_order];

            Complexf prod(1.0f, 0.0f);

            float zero = 1.0f;
            if (is_lpf) zero = -1.0f;

            for (integer k = 1; k <= filter_order; ++k) {
                const float theta = pi_f32 * (2.0f * k + filter_order - 1.0f) / (2.0f * filter_order);
                const Complexf s  = Complexf(Polar_coord(1.0f, theta)) * Wc;
                const Complexf pk = (K + s) / (K - s);

                poles[k - 1]     = pk;
                zeros[k - 1] = Complexf(zero, 0.0f);
                prod *= (Complexf(1.0f, 0.0f) + pk * zero);
            }

            const float G = prod.real() / (float)(1 << filter_order);

            this->_create_kernel(zeros, filter_order, b);
            this->_create_kernel(poles, filter_order, a);
            for (integer i = 0; i <= filter_order; ++i) b[i] *= G;
        }

        float process(float input) {
            input_state.update(input);

            float output = 0.0f;
            for (integer i = 0; i <= filter_order; ++i)
                output += b[i] * input_state[filter_order - i];     // b_i * x[n-i]
            for (integer j = 1; j <= filter_order; ++j)
                output -= a[j] * output_state[filter_order - j];         // a_j * y[n-j]

            output_state.update(output);                          // remember y[n] for next call
            return output;
        }
};

class Lopass : public Butterworth {
    public:
        void init() {
            Butterworth::init(lpf_cutoff, true);
        }
};

class Hipass : public Butterworth {
    public:
        void init() {
            Butterworth::init(hpf_cutoff, false);
        }
};

class Notch {
    class Core {
        private:
            float a[3];
            float b[3];
            Queue<float, 2> state;

        public:
            Core():
            state(true, 0.0f)
            {
                for (integer i = 0; i < 3; i++) {
                    this->a[i] = 0.0f;
                    this->b[i] = 0.0f;
                }
            }

            void clear_state() {
                this->state.flush(0.0f);
            }

            void init(integer frequency, integer freq_multiplier, float Q) {
                const float w0   = 2.0f * pi_f32 * frequency * freq_multiplier / sample_freq;
                const float beta = tanf(w0 / (2.0f * Q));
                const float g    = 1.0f / (1.0f + beta);
                const float c    = 2.0f * cosf(w0);

                this->a[0] = 1.0f;
                this->a[1] = -c * g;
                this->a[2] = (1.0f - beta) * g;

                this->b[0] = g;
                this->b[1] = -c * g;
                this->b[2] = g;

            }

            float process(float input) {
                float output;
                output  = b[0] * input + this->state[0];
                this->state[0] = (this->b[1] * input + this->state[1]) - this->a[1] * output;
                this->state[1] = this->b[2] * input - this->a[2] * output;

                return output;
            }
    };

    private:
        Core base;
        Core second_harmonic;

    public:
        Notch() {
            this->clear_state();
        }
        void clear_state() {
            this->base.clear_state();
            this->second_harmonic.clear_state();
        }

        void init(float Q) {
            base.init(hum_freq, 1, Q);
            second_harmonic.init(hum_freq, 2, Q);
        }

        float process(float input) {
            float output = base.process(input);
            return second_harmonic.process(output);
        }
};

#endif
