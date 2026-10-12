#ifndef FILTERS_H
#define FILTERS_H
#include "constants.h"
#include "queue.h"
#include "math.h"
#include "complex.h"

class BUTTERWORTH {
    private:
        typedef std::complex<double> cd;

        Queue<double, filter_order+1> input_state;
        Queue<double, filter_order> output_state;
        double a[filter_order + 1];
        double b[filter_order + 1];

        static void polyFromRoots(const cd* roots, int n, cd* c) {
            c[0] = cd(1.0, 0.0);
            for (int i = 1; i <= n; ++i) c[i] = cd(0.0, 0.0);

            for (int k = 0; k < n; ++k) {
                for (int i = k + 1; i >= 1; --i) c[i] -= roots[k] * c[i - 1];
            }
        }

    public:
        BUTTERWORTH() { 
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
                prod *= (cd(1.0, 0.0) - pk);
            }

            const double G = prod.real() / std::pow(2.0, filter_order);

            cd bc[filter_order + 1];
            cd ac[filter_order + 1];
            this->polyFromRoots(zeros, filter_order, bc);
            this->polyFromRoots(poles, filter_order, ac);

            for (int i = 0; i <= filter_order; ++i) {
                b[i] = G * bc[i].real();
                a[i] = ac[i].real();
            }

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

class LOW_PASS : public BUTTERWORTH {
    public:
        void init() {
            BUTTERWORTH::init(lpf_cutoff, false);
        }
};

class HIGH_PASS : public BUTTERWORTH {
    public:
        void init() {
            BUTTERWORTH::init(hpf_cutoff, false);
        }
};

class NOTCH_FILTER {
  private:
    float states[notch_order];
    float stage1_num[notch_order - 1];
    float stage2_num[notch_order - 1];
    float stage1_den[notch_order - 1];
    float stage2_den[notch_order - 1];
    float gain;

    static void designSection(double fs, double f0, double Q, float* n, float* d) {
        const double w0   = 2.0 * M_PI * f0 / fs;       // notch angle (radians/sample)
        const double beta = std::tan(w0 / (2.0 * Q));   // sets the notch width
        const double g    = 1.0 / (1.0 + beta);
        const double c    = 2.0 * std::cos(w0);
 
        n[0] = (float)g;                     // b0
        n[1] = (float)(-c * g);              // b1
        n[2] = (float)g;                     // b2
        d[0] = 1.0f;
        d[1] = (float)(-c * g);              // a1
        d[2] = (float)((1.0 - beta) * g);    // a2
    }
 
  public:
    void clear_state() {
        for (int i = 0; i < 4; i++) states[i] = 0;
    }
 
    void init(double Q = 30.0) {
    }
 
    float process(float input) {
        float output;
        float stageIn;
        float stageOut;
 
        stageOut  = stage1_num[0] * input + states[0];
        states[0] = (stage1_num[1] * input + states[1]) - stage1_den[1] * stageOut;
        states[1] = stage1_num[2] * input - stage1_den[2] * stageOut;
        stageIn   = stageOut;
        stageOut  = stage2_num[0] * stageOut + states[2];
        states[2] = (stage2_num[1] * stageIn + states[3]) - stage2_den[1] * stageOut;
        states[3] = stage2_num[2] * stageIn - stage2_den[2] * stageOut;
 
        output = gain * stageOut;
 
        return output;
    }
};


#endif
