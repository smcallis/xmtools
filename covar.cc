#include "xmtools.h"

static double sphere(double* xx) {
    static int64 count;
    static double costs[10000];
    static double spots[10000][2];

    //double cost = hypot(50 - xx[0], 50 - xx[1]);
    double cost = fabs(50 - xx[0]) + fabs(50 - xx[1]);
    spots[count][0] = xx[0];
    spots[count][1] = xx[1];
    costs[count++] = cost;
    static double success = 1.0;
    static double best = 1e100;
    success *= .95;
    if (cost < best) {
        best = cost;
        success += .05;
    }
    fprintf(stderr, "%6lld: f(%.18lf, %.18lf) == %.18lf, ratio: %lf\n", count, xx[0], xx[1], cost, success);

    if (1) {
        static FILE* gnuplot;
        if (!gnuplot) {
            gnuplot = popen("gnuplot", "w");
            fprintf(gnuplot, "set term wxt size 800,800\n");
            fprintf(gnuplot, "set xrange [-100:100]\n");
            fprintf(gnuplot, "set yrange [-100:100]\n");
            fprintf(gnuplot, "set grid\n");
        }
        fprintf(gnuplot, "plot '-' with lp\n");
        for (int64 ii = 0; ii<count; ii++) {
            fprintf(gnuplot, "%.18le %.18le\n", spots[ii][0], spots[ii][1]);
        }
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);
    }

    if (1) {
        static FILE* gnuplot;
        if (!gnuplot) {
            gnuplot = popen("gnuplot", "w");
            fprintf(gnuplot, "set xrange [0:5000]\n");
            fprintf(gnuplot, "set yrange [-15:5]\n");
            fprintf(gnuplot, "set grid\n");
        }
        fprintf(gnuplot, "plot '-' with lines\n");
        for (int64 ii = 0; ii<count; ii++) {
            fprintf(gnuplot, "%lld %.18le\n", ii, ::log10(costs[ii]));
        }
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);
    }

    //getchar();

    return cost;
}

#if 0
static double sphere(double* xx) {
    double cost = hypot(50 - xx[0], 50 - xx[1]);
    return cost;
}
#endif

int main() {
    using namespace xm;

    double center[2] = { 0, 0 };
    double steps[2] = { 10, 10 };
    covarmin(sphere, center, steps, 2, 10000);

    return 0;
}

