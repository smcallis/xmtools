#ifndef XM_SINGLETON_H_
#define XM_SINGLETON_H_ 1

#include "basics.h"
#include "complex.h"

namespace xm {

    template<class atype, class btype>
    static inline void singleton(
        complex<atype>* dst, const complex<btype>* src, int64 len,
        double cycles_lo, double cycles_hi
    ) {
        // We're going through a lot of pains here to avoid making
        // expensive sine and cosine calls in the inner loop.  The
        // naive trig recurrence is numerically unstable and incurs
        // O(N) rounding error as it runs, so we don't do that.
        // Instead, we're using a recurrence from Richard Singleton
        // that incurs O(sqrt N) error.  For large lengths, that
        // could still get too large, so we restart the recurrence
        // after every 1024 samples as necessary.  We also subtract
        // out integer cycles before converting to radians so that
        // we aren't evaluating cosines and sines of large numbers.
        double invlen = 1.0/len;
        for (int64 jj = 0; jj<len; jj += 1024) {
            int64 index_lo = jj;
            int64 index_hi = min(jj + 1024, len);
            double fract_lo = index_lo*invlen;
            double fract_hi = index_hi*invlen;
            double cyc0 = (1.0 - fract_lo)*cycles_lo + fract_lo*cycles_hi;
            double cyc1 = (1.0 - fract_hi)*cycles_lo + fract_hi*cycles_hi;

            double mid = round(.5*cyc0 + .5*cyc1);
            double rad0 = 2*M_PI*(cyc0 - mid);
            double rad1 = 2*M_PI*(cyc1 - mid);
            double dph = (rad1 - rad0)/(index_hi - index_lo);
            double cc = cos(rad0);
            double ss = sin(rad0);
            double aa = 2*sin(dph/2)*sin(dph/2);
            double bb = sin(dph);

            for (int64 ii = index_lo; ii<index_hi; ii++) {
                double re = src[ii].re;
                double im = src[ii].im;
                dst[ii].re = re*cc - im*ss;
                dst[ii].im = im*cc + re*ss;
                // Singleton's trig recurrence
                double tc = cc - (aa*cc + bb*ss);
                double ts = ss + (bb*cc - aa*ss);
                cc = tc;
                ss = ts;
            }
        }
    }
    //XXX: This is the old one - delete it...
#if 0
    static inline void singleton_tune(cfloat* dst, const cfloat* src, int64 len, double ph0, double ph1) {
        // We're going through a lot of pains here to avoid making
        // expensive sine and cosine calls in the inner loop.  The
        // naive trig recurrence is numerically unstable and incurs
        // O(N) rounding error as it runs, so we don't do that.
        // Instead, we're using a recurrence from Richard Singleton
        // that incurs O(sqrt N) error.  For large lengths, that
        // could still get too large, so we restart the recurrence
        // after every 1024 samples as necessary.  We also subtract
        // out integer cycles before converting to radians so that
        // we aren't evaluating cosines and sines of large numbers.
        double invlen = 1.0/len;
        for (int64 jj = 0; jj<len; jj += 1024) {
            int64 index_lo = jj;
            int64 index_hi = min(jj + 1024, len);
            double fract_lo = index_lo*invlen;
            double fract_hi = index_hi*invlen;
            double cyc0 = (1.0 - fract_lo)*ph0 + fract_lo*ph1;
            double cyc1 = (1.0 - fract_hi)*ph0 + fract_hi*ph1;

            double mid = round(.5*cyc0 + .5*cyc1);
            double rad0 = 2*M_PI*(cyc0 - mid);
            double rad1 = 2*M_PI*(cyc1 - mid);
            double dph = (rad1 - rad0)/(index_hi - index_lo);
            double cc = cos(rad0);
            double ss = sin(rad0);
            double aa = 2*sin(dph/2)*sin(dph/2);
            double bb = sin(dph);

            for (int64 ii = index_lo; ii<index_hi; ii++) {
                dst[ii] = cfloat(cc, ss)*src[ii];
                /*
                double re = src[ii].re;
                double im = src[ii].im;
                dst[ii].re = re*cc - im*ss;
                dst[ii].im = im*cc + re*ss;
                */
                // Singleton's trig recurrence
                double tc = cc - (aa*cc + bb*ss);
                double ts = ss + (bb*cc - aa*ss);
                cc = tc;
                ss = ts;
            }
        }
    }
#endif


}

#endif // XM_SINGLETON_H_

