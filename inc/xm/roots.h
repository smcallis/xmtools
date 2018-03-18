#ifndef XM_ROOTS_H_
#define XM_ROOTS_H_ 1

#include "basics.h"

namespace xm {

    //{{{ internal
    namespace internal {
        static inline bool diffsign(double aa, double bb) {
            return (bool)signbit(aa) != (bool)signbit(bb);
        }
    }
    //}}}

    template<class callable>
    double findzero(callable func, double xlo, double xhi, double tol=0) {
        using namespace internal;
        if (xhi < xlo) swap(xlo, xhi);
        double ylo = func(xlo); if (ylo == 0) return xlo;
        double yhi = func(xhi); if (yhi == 0) return xhi;
        check(diffsign(yhi, ylo), "must have opposite signs");

        while (xhi - xlo > tol) {
            double half = .5*(xhi - xlo);
            double xmid = xlo + half;
            if (xmid == xlo || xmid == xhi) return xmid;
            double ymid = func(xmid); if (ymid == 0) return xmid;

            // We're using a variation of Ridder's method to find an
            // interpolated point, slightly modified to avoid underflow.
            double denom = ::hypot(ymid, ::sqrt(::fabs(ylo))*::sqrt(::fabs(yhi)));
            double xexp = xmid + half*::copysign(1, ylo - yhi)*ymid/denom;
            check(xlo <= xexp && xexp <= xhi, "in bounds");
            double yexp = func(xexp); if (yexp == 0) return xexp;

            double xx[4] = { xlo, xmid, xexp, xhi };
            double yy[4] = { ylo, ymid, yexp, yhi };
            if (xexp < xmid) { swap(xx[1], xx[2]); swap(yy[1], yy[2]); }

            double best = xhi - xlo;
            for (int ii = 0; ii<3; ii++) {
                if (xx[ii+1] - xx[ii+0] < best && diffsign(yy[ii+0], yy[ii+1])) {
                    xlo = xx[ii+0]; ylo = yy[ii+0];
                    xhi = xx[ii+1]; yhi = yy[ii+1];
                    best = xx[ii+1] - xx[ii+0];
                }
            }
        }
        return xhi + .5*(xhi - xlo);
    }

    // XXX: change this to goldenopt<obj, sign>, and do goldenmin and goldenmax
    template<class objective>
    double goldenmax(objective func, double lo, double hi, double tol) {
        // This is a "Golden Section Search" to find a maximum between lo and hi
        const double phisqr = .381966011250105151795413;
        double aa = min(lo, hi);
        double dd = max(lo, hi);
        double bb = aa + (dd - aa)*phisqr;
        double cc = dd - (dd - aa)*phisqr;
        double fb = func(bb);
        double fc = func(cc);

        while (dd - aa > tol) {
            if (fb > fc) {
                dd = cc;
                cc = bb;
                fc = fb;
                bb = aa + (dd - aa)*phisqr;
                fb = func(bb);
            } else {
                aa = bb;
                bb = cc;
                fb = fc;
                cc = dd - (dd - aa)*phisqr;
                fc = func(cc);
            }
        }

        return (aa + dd)*.5;
    }

    struct quadroots {
        double lo, hi;
    };

    // Find the real roots of: a*x^2 + b*x + c (if possible).  It returns NaNs
    // in bad cases.  XXX: Numerically stable, but there is room for
    // improvement in special cases and error handling....
    static inline quadroots quadsolve(double aa, double bb, double cc) {
        double rad = sqrt(bb*bb - 4*aa*cc);
        quadroots results;
        if (bb < 0) {
            double r0 = (2*cc)/(-bb + rad);
            double r1 = (-bb + rad)/(2*aa);
            results.lo = fmin(r0, r1);
            results.hi = fmax(r0, r1);
        } else {
            double r0 = (-bb - rad)/(2*aa);
            double r1 = (2*cc)/(-bb - rad);
            results.lo = fmin(r0, r1);
            results.hi = fmax(r0, r1);
        }
        return results;
    }

    // Given 3 consecutive values, find the place where the derivative is zero
    static inline double quadpeak(double before, double center, double after) {
        double two_a = (before - 2*center + after);
        double neg_b = .5*(before - after);
        return neg_b/two_a;
    }

}

#endif // XM_ROOTS_H_
