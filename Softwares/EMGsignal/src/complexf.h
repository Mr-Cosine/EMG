#ifndef COMPLEXF_H
#define COMPLEXF_H

#include <math.h>
#include "types.h"

inline bool is_equal(const f32& a, const f32& b) { return fabsf(a - b) < 1e-5f; }

struct Polar_coord {
    f32 r;
    f32 theta;

    Polar_coord(f32 new_r, f32 new_theta) {
        r = new_r;
        theta = new_theta;
    }
};

class Complexf {
    private:
        f32 rval;
        f32 ival;

    public:
        // Constructors
        Complexf(f32 r = 0.0f, f32 i = 0.0f):
        rval(r),
        ival(i)
        {}

        Complexf(const Complexf& c):
        rval(0.0f),
        ival(0.0f)
        {
            this->rval = c.real();
            this->ival = c.imaginary();
        }

        Complexf(Polar_coord c):
        rval(c.r * cosf(c.theta)),
        ival(c.r * sinf(c.theta))
        {}

        // Getter
        f32 real() const { return this->rval; }
        f32 imaginary() const { return this->ival; }

        // Conjugate pairs
        Complexf conjugate() const { Complexf result(this->rval, -this->ival); return result; }
        static Complexf conjugate(const Complexf& num) { Complexf result(num.real(), -num.imaginary()); return result; }

        bool is_conjugate(const Complexf& num) const { return (is_equal(this->rval, num.real()) && is_equal(this->ival, -num.imaginary())); }
        static bool is_conjugate(const Complexf& n1, const Complexf& n2)  { return (is_equal(n1.real(), n2.real()) && is_equal(n1.imaginary(), -n2.imaginary())); }
        
        // Operators
        Complexf operator + (const f32& k) const {
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

        Complexf operator - (const f32& k) const {
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

        Complexf operator * (const f32& k) const {
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

        Complexf operator / (const f32& k) const {
            Complexf result(
                this->rval / k, 
                this->ival / k
            );
            return result;
        }
        Complexf operator / (const Complexf& num) const {
            Complexf conj_num(conjugate(num));
            f32 s = num.real() * num.real() + num.imaginary() * num.imaginary();
        
            return (*this * conj_num) / s;
        }

        Complexf& operator += (const f32& k) {
            this->rval += k;

            return *this;
        }
        Complexf& operator += (const Complexf& num) {
            this->rval += num.real();
            this->ival += num.imaginary();

            return *this;
        }

        Complexf& operator -= (const f32& k) {
            this->rval -= k;

            return *this;
        }
        Complexf& operator -= (const Complexf& num) {
            this->rval -= num.real();
            this->ival -= num.imaginary();

            return *this;
        }

        Complexf& operator *= (const f32& k) {
            this->rval *= k;
            this->ival *= k;

            return *this;
        }
        Complexf& operator *= (const Complexf& num) {
            f32 nr = this->rval*num.real() - this->ival*num.imaginary();
            f32 ni = this->rval*num.imaginary() + this->ival*num.real();
            this->rval = nr; this->ival = ni;

            return *this;
        }

        Complexf& operator /= (const f32& k) {
            this->rval /= k;
            this->ival /= k;

            return *this;
        }
        Complexf& operator /= (const Complexf& num) {
            f32 s = num.real() * num.real() + num.imaginary() * num.imaginary();

            f32 nr = (this->rval * num.real() + this->ival * num.imaginary()) / s;
            f32 ni = (this->ival * num.real() - this->rval * num.imaginary()) / s;
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

inline Complexf operator + (const f32& k, const Complexf& num) { return num + k; }
inline Complexf operator - (const f32& k, const Complexf& num) { return Complexf(k, 0.0f) - num; }
inline Complexf operator * (const f32& k, const Complexf& num) { return num * k; }
inline Complexf operator / (const f32& k, const Complexf& num) { return  Complexf(k, 0.0f) / num; }

inline Complexf polar(const f32& r, const f32& theta) { 
    Polar_coord c(r, theta);
    return Complexf(c);
}

#endif