#ifndef XM_POLYPHASE_H_
#define XM_POLYPHASE_H_ 1

#include "complex.h"

namespace xm {

    struct polyphase {

        //~polyphase() = default;
        //polyphase() = default;
        //polyphase(const polyphase&) = default;
        //polyphase& operator =(const polyphase&) = default;

        inline polyphase(int window, double dwidth, int64 taps);

        inline void resample(
            cfloat* dst_ptr, int64 dst_len,
            const cfloat* src_ptr, double src_lo, double src_hi
        ) const;

        private:
            // XXX: make non-copyable, non-defaultable
            enum { COUNT = 1024 };
            int64 taps;
            vector<float> bank;

            inline void makefir(float* ptr, double fract, int window, double dwidth);
            inline cfloat interp(const cfloat* src, double where) const;
    };

    polyphase::polyphase(int window, double dwidth, int64 taps) : taps(taps), bank(taps*COUNT) {
        for (int64 ii = 0; ii<COUNT; ii++) {
            double fract = ii/(double)COUNT;
            makefir(bank.data() + ii*taps, fract, window, dwidth);
        }
    }

    void polyphase::makefir(float* ptr, double fract, int window, double dwidth) {
        double sum = 0.0;
        for (int64 jj = 0; jj<taps; jj++) {
            double xx = jj + 1 - taps/2 - fract;
            double tap = firwin(window, xx, taps)*sinc(xx*dwidth);
            ptr[jj] = tap;
            sum += tap;
        }
        double inv = 1.0/sum;
        for (int64 jj = 0; jj<taps; jj++) {
            ptr[jj] *= inv;
        }
    }

    cfloat polyphase::interp(const cfloat* src, double where) const {
        double re = 0;
        double im = 0;
        int64 fixed = llrint(where*COUNT);
        int64 index = fixed/COUNT;
        int64 which = fixed%COUNT;
        const cfloat* ptr = src + index + 1 - taps/2;
        const float* filt = bank.data() + taps*which;
        for (int64 ii = 0; ii<taps; ii++) {
            re += ptr[ii].re*filt[ii];
            im += ptr[ii].im*filt[ii];
        }
        return cfloat(re, im);
    }

    void polyphase::resample(
        cfloat* dst_ptr, int64 dst_len,
        const cfloat* src_ptr, double src_lo, double src_hi
    ) const {
        double df = 1.0/dst_len;
        for (int64 ii = 0; ii<dst_len; ii++) {
            double ff = ii*df;
            dst_ptr[ii] = interp(src_ptr, (1.0 - ff)*src_lo + ff*src_hi);
        }
    }

}

#endif // XM_POLYPHASE_H_

