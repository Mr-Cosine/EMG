#ifndef FILTERS_H
#define FILTERS_H
#include "constants.h"
#include "queue.h"
#include <cmath>
#include <complex>

class Butterworth {
    private:
        typedef std::complex<double> cd;

        Queue<double, filter_order+1> input_state;
        Queue<double, filter_order> output_state;
        double a[filter_order + 1];
        double b[filter_order + 1];

        void _create_kernel(const cd* roots, int n, double res[]) {
            cd c[filter_order + 1];

            c[0] = cd(1.0, 0.0);
            for (int i = 1; i <= n; ++i) c[i] = cd(0.0, 0.0);

            for (int k = 0; k < n; ++k) {
                for (int i = k + 1; i >= 1; --i) c[i] -= roots[k] * c[i - 1];
            }

            for (int i = 0; i <= filter_order; i++) {
                res[i] = c[i].real();
            }
        }

    public:
        Butterworth() { 
            for (int i = 0; i <= filter_order; ++i) { a[i] = 0.0; b[i] = 0.0; }
            a[0] = 1.0;
            b[0] = 1.0;
        }

        void clear_state() {
            this->input_state.flush(0.0);
            this->output_state.flush(0.0);
        }

        void init(double cutoff, bool is_lpf) {
            for (int i = 0; i <= filter_order; ++i) { 
                a[i] = 0.0; 
                b[i] = 0.0; 
            }

            if (cutoff <= 0.0 || cutoff >= sample_freq / 2.0) return;

            const double K  = 2.0 * sample_freq;
            const double Wc = K * std::tan(M_PI * cutoff / sample_freq);

            cd poles[filter_order];
            cd zeros[filter_order];

            cd prod(1.0, 0.0); 

            double zero = 1.0;
            if (is_lpf) zero = -1.0;

            for (int k = 1; k <= filter_order; ++k) {
                const double theta = M_PI * (2.0 * k + filter_order - 1.0) / (2.0 * filter_order);
                const cd s  = std::polar(1.0, theta) * Wc;
                const cd pk = (K + s) / (K - s);

                poles[k - 1]     = pk;
                zeros[k - 1] = cd(zero, 0.0);
                prod *= (cd(1.0, 0.0) + pk * zero);
            }

            const double G = prod.real() / std::pow(2.0, filter_order);

            this->_create_kernel(zeros, filter_order, b);
            this->_create_kernel(poles, filter_order, a);
            for (int i = 0; i <= filter_order; ++i) b[i] *= G;

            this->clear_state();
        }

        double process(double input) {
            input_state.update(input);

            double output = 0.0;
            for (int i = 0; i <= filter_order; ++i)
                output += b[i] * input_state[filter_order - i];     // b_i * x[n-i]
            for (int j = 1; j <= filter_order; ++j)
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
            double a[3];
            double b[3];
            Queue<double, 2> state;

        public:
            Core():
            state(double(0))            
            {
                for (int i = 0; i < 3; i++) { 
                    this->a[i] = 0.0;
                    this->b[i] = 0.0;
                }
                
                this->clear_state();
            }

            void clear_state() {
                this->state.flush(0.0);
            }

            void init(int frequency, int freq_multiplier, double Q) {
                const double w0   = 2.0 * M_PI * frequency * freq_multiplier / sample_freq;
                const double beta = std::tan(w0 / (2.0 * Q));
                const double g    = 1.0 / (1.0 + beta);
                const double c    = 2.0 * std::cos(w0);

                this->a[0] = 1.0f;
                this->a[1] = (double)(-c * g);
                this->a[2] = (double)((1.0 - beta) * g);
    
                this->b[0] = (double)g;                     
                this->b[1] = (double)(-c * g);
                this->b[2] = (double)g;

            }

            double process(double input) {
                double output;
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

        void init(double Q) {
            base.init(hum_freq, 1, Q);
            second_harmonic.init(hum_freq, 2, Q);
        }   
    
        double process(double input) {
            double output = base.process(input);
            return second_harmonic.process(output);
        }
};

#endif
