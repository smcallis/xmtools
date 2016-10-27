#include "xmtools.h"
using namespace xm;

static inline double invq(
    double yy,
    double xlo, double xmm, double xhi,
    double ylo, double ymm, double yhi
) {
    return (
        xlo*(yy - ymm)*(yy - yhi)/((ylo - ymm)*(ylo - yhi)) +
        xmm*(yy - yhi)*(yy - ylo)/((ymm - yhi)*(ymm - ylo)) +
        xhi*(yy - ylo)*(yy - ymm)/((yhi - ylo)*(yhi - ymm))
    );

}

namespace xm {

    static bool samesign(double aa, double bb) {
        return (bool)signbit(aa) == (bool)signbit(bb);
    }

    template<class callable>
    double findzero(callable func, double xlo, double xhi, double tol=0) {
        if (xhi < xlo) swap(xlo, xhi);
        double ylo = func(xlo); if (ylo == 0) return xlo;
        double yhi = func(xhi); if (yhi == 0) return xhi;
        check(!samesign(yhi, ylo), "must have opposite signs");

        while (xhi - xlo > tol) {
            double xmm = xlo + .5*(xhi - xlo);
            if (xmm == xlo || xmm == xhi) return xmm;
            double ymm = func(xmm); if (ymm == 0) return xmm;

            // XXX: This test needs some cleanup...
            if (::hypot(ymm - ylo, yhi - ymm) > .765*::fabs(yhi - ylo)) {
                fprintf(stderr, "doing bisection\n");
                // standard bisection
                if (samesign(ylo, ymm)) {
                    xlo = xmm; ylo = ymm;
                } else {
                    xhi = xmm; yhi = ymm;
                }
            } else {
                fprintf(stderr, "trying inverse quad\n");
                // inverse quadratic interpolation
                double xqq = (
                    xlo*ymm*yhi/((ylo - ymm)*(ylo - yhi)) +
                    xmm*yhi*ylo/((ymm - yhi)*(ymm - ylo)) +
                    xhi*ylo*ymm/((yhi - ylo)*(yhi - ymm))
                );

                if (isnan(xqq) || xqq == xmm) {
                    fprintf(stderr, "falling back to bisection\n");
                    // back to bisection
                    if (samesign(ylo, ymm) > 0) {
                        xlo = xmm; ylo = ymm;
                    } else {
                        xhi = xmm; yhi = ymm;
                    }
                    continue;
                }

                double yqq = func(xqq); if (yqq == 0) return xqq;

                double xx[4], yy[4];
                if (xqq < xmm) {
                    xx[0] = xlo; xx[1] = xqq; xx[2] = xmm; xx[3] = xhi;
                    yy[0] = ylo; yy[1] = yqq; yy[2] = ymm; yy[3] = yhi;
                } else {
                    xx[0] = xlo; xx[1] = xmm; xx[2] = xqq; xx[3] = xhi;
                    yy[0] = ylo; yy[1] = ymm; yy[2] = yqq; yy[3] = yhi;
                }

                double best = xhi - xlo;
                for (int ii = 0; ii<3; ii++) {
                    if (!samesign(yy[ii+0], yy[ii+1]) && xx[ii+1] - xx[ii+0] < best) {
                        xlo = xx[ii+0]; ylo = yy[ii+0];
                        xhi = xx[ii+1]; yhi = yy[ii+1];
                        best = xx[ii+1] - xx[ii+0];
                    }
                }
            }
        }

        return xhi + .5*(xhi - xlo);
    }
}


static double foo(double xx) {
    using namespace xm;
    //double yy = ::sin(xx);
    //double yy = xx*xx - 1;
    double yy = exp(xx) - 1;
    //double yy = atan(xx) - 1.5;
    //double yy = sin(1/xx);
    //double yy = sin(xx*M_PI/180) - 3*cos(xx*M_PI/180);
    {
        static int64 histlen;
        static double histy[1000];
        static double histx[1000];
        histx[histlen] = xx;
        histy[histlen] = yy;
        ++histlen;
        fprintf(stderr, "probe[%lld]: %.18lf -> %.18le \n", histlen, xx, yy);

        static FILE* gnuplot;
        if (!gnuplot) {
            gnuplot = popen("gnuplot -persist", "w");
            fprintf(gnuplot, "set grid\n");
        }
        fprintf(gnuplot, "plot '-' w lp\n");
        for (int ii = 0; ii<histlen; ii++) {
            fprintf(gnuplot, "%.18le %.18le\n", histx[ii], histy[ii]);
        }
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);
        //getchar();
    }

    return yy;
}

#include "tests.h"

int main() {
    fprintf(stderr, "test01: %lf\n", findzero(root_test01, root_range01.lo, root_range01.hi));
    fprintf(stderr, "test02: %lf\n", findzero(root_test02, root_range02.lo, root_range02.hi));
    fprintf(stderr, "test03: %lf\n", findzero(root_test03, root_range03.lo, root_range03.hi));
    fprintf(stderr, "test04: %lf\n", findzero(root_test04, root_range04.lo, root_range04.hi));
    fprintf(stderr, "test05: %lf\n", findzero(root_test05, root_range05.lo, root_range05.hi));
    fprintf(stderr, "test06: %lf\n", findzero(root_test06, root_range06.lo, root_range06.hi));
    fprintf(stderr, "test07: %lf\n", findzero(root_test07, root_range07.lo, root_range07.hi));
    fprintf(stderr, "test08: %lf\n", findzero(root_test08, root_range08.lo, root_range08.hi));
    fprintf(stderr, "test09: %lf\n", findzero(root_test09, root_range09.lo, root_range09.hi));
    fprintf(stderr, "test10: %lf\n", findzero(root_test10, root_range10.lo, root_range10.hi));
    fprintf(stderr, "test11: %lf\n", findzero(root_test11, root_range11.lo, root_range11.hi));
    fprintf(stderr, "test12: %lf\n", findzero(root_test12, root_range12.lo, root_range12.hi));
    fprintf(stderr, "test13: %lf\n", findzero(root_test13, root_range13.lo, root_range13.hi));
    fprintf(stderr, "test14: %lf\n", findzero(root_test14, root_range14.lo, root_range14.hi));
    fprintf(stderr, "test15: %lf\n", findzero(root_test15, root_range15.lo, root_range15.hi));
    fprintf(stderr, "test16: %lf\n", findzero(root_test16, root_range16.lo, root_range16.hi));
    fprintf(stderr, "test17: %lf\n", findzero(root_test17, root_range17.lo, root_range17.hi));
    fprintf(stderr, "test18: %lf\n", findzero(root_test18, root_range18.lo, root_range18.hi));
    fprintf(stderr, "test19: %lf\n", findzero(root_test19, root_range19.lo, root_range19.hi));
    /*
    */

    /*
    fprintf(
        stderr, "%.18lf -> %.18lf, %.18lf -> %.18lf\n",
        root_range06.lo, root_test06(root_range06.lo),
        root_range06.hi, root_test06(root_range06.hi)
    );
    FILE* gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "set yrange [-5:5]\n");
    fprintf(gnuplot, "set xrange [-2:2]\n");
    fprintf(gnuplot, "plot '-' with lines\n");
    for (int64 ii = -2000; ii<= 12000; ii++) {
        double xx = ii*.001 + .0005;
        double yy = root_test06(xx);
        //fprintf(stderr, "%.18le %.18le\n", xx, yy);
        fprintf(gnuplot, "%.18le %.18le\n", xx, yy);
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);

    fprintf(stderr, "test06: %lf\n", findzero(root_test06, root_range06.lo, root_range06.hi));
    return 0;
    */



    //double zero = findzero(foo, -M_PI, M_LN2);
    //fprintf(stderr, "===================\n");
    //fprintf(stderr, "final zero: %.18le\n", foo(zero));

    /*
    //findzero(foo, 3.0, 3.4);
    //findzero(foo, 71, 72);
    FILE* gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "set grid\n");

    //double xlo =  0, xmm =  1, xhi =  2;
    double xlo =  0, xmm =  10, xhi =  20;
    //double ylo = -10, ymm = 4.13, yhi = +10;
    double ylo = -10, ymm = 0, yhi = +10;
    //double ylo = -10, ymm = 0, yhi = +10;
    //double ylo = -10, ymm = 4.2, yhi = +10;
    //double ylo = -10, ymm = 5.2, yhi = +10;

    fprintf(stderr, "test: %lf\n", ::hypot(ymm - ylo, yhi -ymm)/fabs(yhi - ylo));
    //fprintf(stderr, "test %lf\n", (ymm - ylo)*(yhi - ymm)/sqr(yhi - ylo));
    if (::hypot(ymm - ylo, yhi -ymm) < .765*fabs(yhi - ylo)) {
        fprintf(stderr, "should be good\n");
    } else {
        fprintf(stderr, "could be BAD\n");
    }

    fprintf(gnuplot, "plot '-' with lines, '-' with points\n");
    for (int64 ii = -100; ii<=1100; ii++) {
        double yy = ylo + ii*.001*(yhi - ylo);
        double xx = invq(yy,
            xlo, xmm, xhi,
            ylo, ymm, yhi
        );
        fprintf(gnuplot, "%.18le %.18le\n", xx, yy);
    }
    fprintf(gnuplot, "e\n");
    fprintf(
        gnuplot, "%.18le %.18le\n%.18le %.18le\n%.18le %.18le\ne\n",
        xlo, ylo, xmm, ymm, xhi, yhi
    );
    fflush(gnuplot);
    */


    return 0;
}
