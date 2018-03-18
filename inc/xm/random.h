#ifndef XM_RANDOM_H_
#define XM_RANDOM_H_ 1

namespace xm {

    //{{{ longprng 
    struct longprng {
        inline ~longprng();
        inline longprng();
        inline longprng(const longprng& other);
        inline longprng(uint64_t seed);
        inline longprng& operator =(const longprng& other);
        inline void reseed(uint64_t seed);

        inline uint64_t uint64();
        inline double uniform();
        inline double uniform(double lo, double hi);
        inline double normal();
        inline cdouble cxnormal();

        private:
            uint64_t state[16];
            int64 index;
    };

    longprng::~longprng() {}

    longprng::longprng() {
        reseed(0);
    }

    longprng::longprng(const longprng& other) : index(other.index) {
        for (int64 ii = 0; ii<16; ii++) {
            state[ii] = other.state[ii];
        }
    }

    longprng::longprng(uint64_t seed) {
        reseed(seed);
    }

    longprng& longprng::operator =(const longprng& other) {
        index = other.index;
        for (int64 ii = 0; ii<16; ii++) {
            state[ii] = other.state[ii];
        }
        return *this;
    }

    void longprng::reseed(uint64_t seed) {
        while (seed == 0) {
            struct timeval tv;
            check(gettimeofday(&tv, 0) == 0, "gettimeofday");
            seed = (tv.tv_sec*1000000ULL) ^ tv.tv_usec;
        }
        // This is 'xorshift64*' by Sebastiano Vigna (public domain).
        // We're just using it to seed the other generator.
        for (int64 ii = 0; ii<16; ii++) {
            seed ^= seed >> 12;
            seed ^= seed << 25;
            seed ^= seed >> 27;
            state[ii] = seed * 2685821657736338717LL;
        }
        index = 0;
    }

    uint64_t longprng::uint64() {
        // This is 'xorshift1024*' by Sebastiano Vigna (public domain).  It's
        // an improved version of one from George Marsaglia, and should have a
        // period of 2^1024 - 1, which is long enough for many things.  It does
        // well on the "BigCrush" suite of tests (better than Mersenne Twister).
        uint64_t state0 = state[index];
        index = (index + 1)%16;
        uint64_t state1 = state[index];
        state1 ^= state1 << 31;
        state1 ^= state1 >> 11;
        state0 ^= state0 >> 30;
        state[index] = state0 ^ state1;
        return state[index] * 1181783497276652981LL; 
    }

    double longprng::uniform() { // [0, 1]
        const double scale = pow(2, -64);
        return scale*uint64();
    }

    double longprng::uniform(double lo, double hi) {
        const double scale = pow(2, -64);
        double value = scale*uint64();
        return lo + (hi - lo)*value;
    }

    double longprng::normal() {
        cdouble zz = cxnormal();
        return zz.re + zz.im;
    }

    cdouble longprng::cxnormal() {
        // u0 is in (0, 1], and u1 is in [0, 2pi)
        const double s0 = 5.42101086242752217e-20;
        const double u0 = s0*uint64() + s0;
        const double s1 = 3.40612158008655459e-19;
        const double u1 = s1*uint64();
        // Box-Muller transform
        const double len = ::sqrt(-::log(u0));
        return cdouble(::cos(u1)*len, ::sin(u1)*len);
    }
    //}}}


}


#endif // XM_RANDOM_H_

