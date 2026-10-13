#ifndef COMPLEXF_H
#define COMPLEXF_H

#include <math.h>

inline bool is_equal(const double& a, const double& b) { return fabs(a - b) < 1e-5; }

struct Polar_coord {
    double r;
    double theta;

    Polar_coord(double new_r, double new_theta) {
        r = new_r;
        theta = new_theta;
    }
};

class Complexf {
    private:
        double rval;
        double ival;

    public:
        Complexf(double r = 0.0, double i = 0.0):
        rval(r),
        ival(i)
        {}

        Complexf(const Complexf& c):
        rval(0.0),
        ival(0.0)
        {
            this->rval = c.real();
            this->ival = c.imaginary();
        }

        Complexf(Polar_coord c):
        rval(c.r * cos(c.theta)),
        ival(c.r * sin(c.theta))
        {}

        double real() const { return this->rval; }
        double imaginary() const { return this->ival; }

        Complexf conjugate() const { Complexf result(this->rval, -this->ival); return result; }
        static Complexf conjugate(const Complexf& num) { Complexf result(num.real(), -num.imaginary()); return result; }

        bool is_conjugate(const Complexf& num) const { return (is_equal(this->rval, num.real()) && is_equal(this->ival, -num.imaginary())); }
        static bool is_conjugate(const Complexf& n1, const Complexf& n2)  { return (is_equal(n1.real(), n2.real()) && is_equal(n1.imaginary(), -n2.imaginary())); }
        
        Complexf operator + (const double& k) const {
            Complexf result(
                this->rval + k, 
                this->ival
            );
            return result;
        }
        Complexf operator + (const Complexf& num) const {
            Complexf result(
                this->rval + num.real(), 
                this->ival + num.imaginary()
            );
            return result;
        }

        Complexf operator - (const double& k) const {
            Complexf result(
                this->rval - k, 
                this->ival
            );
            return result;
        }
        Complexf operator - (const Complexf& num) const {
            Complexf result(
                this->rval - num.real(), 
                this->ival - num.imaginary()
            );
            return result;
        }

        Complexf operator * (const double& k) const {
            Complexf result(
                this->rval * k, 
                this->ival * k
            );
            return result;
        }
        Complexf operator * (const Complexf& num) const {
            Complexf result(
                this->rval * num.real() - this->ival * num.imaginary(), 
                this->rval * num.imaginary() + this->ival * num.real()
            );
            return result;
        }

        Complexf operator / (const double& k) const {
            Complexf result(
                this->rval / k, 
                this->ival / k
            );
            return result;
        }
        Complexf operator / (const Complexf& num) const {
            Complexf conj_num(conjugate(num));
            double s = num.real() * num.real() + num.imaginary() * num.imaginary();
        
            return (*this * conj_num) / s;
        }

        Complexf& operator += (const double& k) {
            this->rval += k;

            return *this;
        }
        Complexf& operator += (const Complexf& num) {
            this->rval += num.real();
            this->ival += num.imaginary();

            return *this;
        }

        Complexf& operator -= (const double& k) {
            this->rval -= k;

            return *this;
        }
        Complexf& operator -= (const Complexf& num) {
            this->rval -= num.real();
            this->ival -= num.imaginary();

            return *this;
        }

        Complexf& operator *= (const double& k) {
            this->rval *= k;
            this->ival *= k;

            return *this;
        }
        Complexf& operator *= (const Complexf& num) {
            double nr = this->rval*num.real() - this->ival*num.imaginary();
            double ni = this->rval*num.imaginary() + this->ival*num.real();
            this->rval = nr; this->ival = ni;

            return *this;
        }

        Complexf& operator /= (const double& k) {
            this->rval /= k;
            this->ival /= k;

            return *this;
        }
        Complexf& operator /= (const Complexf& num) {
            double s = num.real() * num.real() + num.imaginary() * num.imaginary();

            double nr = (this->rval * num.real() + this->ival * num.imaginary()) / s;
            double ni = (this->ival * num.real() - this->rval * num.imaginary()) / s;
            this->rval = nr; this->ival = ni;

            return *this;
        }

        Complexf& operator = (const Complexf& num) {
            this->rval = num.real();
            this->ival = num.imaginary();

            return *this;
        }

        bool operator == (const Complexf& num) const {
            return (is_equal(this->rval, num.real()) && is_equal(this->ival, num.imaginary()));
        }
};

inline Complexf operator + (const double& k, const Complexf& num) { return num + k; }
inline Complexf operator - (const double& k, const Complexf& num) { return Complexf(k, 0.0) - num; }
inline Complexf operator * (const double& k, const Complexf& num) { return num * k; }
inline Complexf operator / (const double& k, const Complexf& num) { return  Complexf(k, 0.0) / num; }

inline Complexf polar(const double& r, const double& theta) { 
    Polar_coord c(r, theta);
    return Complexf(c);
}

#endif