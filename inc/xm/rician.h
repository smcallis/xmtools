#ifndef XM_RICIAN_H_
#define XM_RICIAN_H_ 1

#include "basics.h"
#include "bessel.h"

namespace xm {

    static inline double ricepdf(double vv, double ss, double xx) {
        using namespace internal;
        check(vv >= 0, "sanity");
        check(ss > 0, "sanity");
        if (xx < 0) return 0;
        return (
            xx*exp(-(xx*xx + vv*vv) / (2 * ss*ss)) *
            besseli0(xx*vv/(ss*ss)) / (ss*ss)
        );
    }

    static inline double ricecdf(double vv, double ss, double xx) {
        check(vv >= 0, "sanity");
        check(ss > 0, "sanity");
        if (xx < 0) return 0;
        // This is magic to evaluate the Marcum Q function
        double aa = vv/ss;
        double bb = xx/ss;
        // aa and bb below should be real and non-negative
        // Should we check for this?
        aa = aa*aa/2;
        bb = bb*bb/2;
        double dd = exp(-aa);
        double hh = dd;
        double ff = bb*exp(-bb);
        double delta = ff*hh;
        double sum = delta;
        double kk = 1;
        const double eps = 1e-60;
        while (delta > eps*sum) {
            dd *= aa/kk;
            hh += dd;
            ff *= bb/(kk + 1);
            delta = ff*hh;
            sum += delta;
            kk = kk + 1;
        }
        return sum;
    }

}

#endif // XM_RICIAN_H_


