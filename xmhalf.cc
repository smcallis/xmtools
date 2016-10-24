#include "xmtools.h"
#include "kissfft.h"

//{{{ Dolph Chebyshev Window
// http://practicalcryptography.com/miscellaneous/machine-learning/implementing-dolph-chebyshev-window/
/**************************************************************************
  This function computes the chebyshev polyomial T_n(x)
  ***************************************************************************/
double cheby_poly(int n, double x){
    double res;
    if (fabs(x) <= 1) res = cos(n*acos(x));
    else              res = cosh(n*acosh(x));
    return res;
}

/***************************************************************************
   calculate a chebyshev window of size N, store coeffs in out as in Antoniou
   -out should be array of size N 
   -atten is the required sidelobe attenuation (e.g. if you want -60dB atten, use '60')
   ***************************************************************************/
void cheby_win(double *out, int N, double atten){
    int nn, i;
    double M, n, sum = 0, max=0;
    double tg = pow(10,atten/20);  /* 1/r term [2], 10^gamma [2] */
    double x0 = cosh((1.0/(N-1))*acosh(tg));
    M = (N-1)/2;
    if(N%2==0) M = M + 0.5; /* handle even length windows */
    for(nn=0; nn<(N/2+1); nn++){
        n = nn-M;
        sum = 0;
        for(i=1; i<=M; i++){
            sum += cheby_poly(N-1,x0*cos(M_PI*i/N))*cos(2.0*n*M_PI*i/N);
        }
        out[nn] = tg + 2*sum;
        out[N-nn-1] = out[nn];
        if(out[nn]>max)max=out[nn];
    }
    for(nn=0; nn<N; nn++) out[nn] /= max; /* normalise everything */
    return;
}
//}}}

namespace xm {

}

//{{{ fircost
template<int muls, int coeffs>
static double fircost(double* args) {
    using namespace xm;

    const int64 size = 10000;
    vec<cfloat> timedata(size);
    vec<cfloat> freqdata(size);
    kissfft<float> fft(size);
    zero(timedata);

    const int64 taps = 4*muls + 1;

    //double a1 = args[0];
    //double a2 = args[1];
    //double a3 = args[2];
    //double a4 = args[3];
    //double a5 = args[4];
    //double a0 = 1 - a1 - a2 - a3 - a4 - a5;
    double a0 = 1;
    for (int64 ii = 0; ii<coeffs; ii++) {
        a0 -= args[ii];
    }

    for (int64 ii = 0; ii<taps; ii++) {
        double xx = 2*M_PI*(ii - taps/2)/(taps - 1);
        //double win = a0 + a1*cos(1*xx) + a2*cos(2*xx) + a3*cos(3*xx) + a4*cos(4*xx) + a5*cos(5*xx);
        double win = a0;
        for (int64 jj = 0; jj<coeffs; jj++) {
            win += args[jj]*cos((1 + jj)*xx);
        }
        timedata[ii] = sinc(.5*(ii - taps/2)) * win;
    }

    static int64 hack;
    if (0) if (hack%500 == 0) {
        static FILE* timeplot;
        if (!timeplot) {
            timeplot = popen("gnuplot -persist", "w");
            fprintf(timeplot, "set grid\n");
            fprintf(timeplot, "set yrange [-.3:1.1]\n");
        }
        fprintf(timeplot, "plot '-' with lp\n");
        for (int64 ii = 0; ii<taps; ii++) {
            fprintf(timeplot, "%le\n", timedata[ii].re);
        }
        fprintf(timeplot, "e\n");
        fflush(timeplot);
    }

    fft.exec(freqdata.data(), timedata.data());
    if (hack%500 == 0) {
        static FILE* freqplot;
        if (!freqplot) {
            freqplot = popen("gnuplot -persist", "w");
            fprintf(freqplot, "set grid\n");
            fprintf(freqplot, "set yrange [-100:10]\n");
        }

        fprintf(freqplot, "plot '-' with l\n");
        for (int64 ii = 0; ii<size; ii++) {
            double db = 10*log10(magsqr(freqdata[ii]/2) + 1e-20);
            fprintf(freqplot, "%le %le\n", ii*(200.0/size), db);
        }
        fprintf(freqplot, "e\n");
        fflush(freqplot);
    }

    double cost = 0;
    for (int64 ii = 3000; ii<= 7000; ii++) {
        cost = max(cost, magsqr(freqdata[ii]/2));
    }
    cost = 10*log10(cost + 1e-20);

    static double best = 1000;
    best = min(cost, best);

    fprintf(stderr, "cost: %lf, best: %lf\n", cost, best);
    //if (hack%100 == 0) getchar();
    hack++;

    return cost;
}
//}}}

//{{{ tapcost
template<int muls>
static double tapcost(double* args) {
    using namespace xm;

    const int64 size = 10000;
    const int64 taps = 4*muls + 1;

    vec<cfloat> timedata(size, 0);
    vec<cfloat> freqdata(size);
    kissfft<float> fft(size);
    
    timedata[taps/2] = 1.0;
    for (int64 ii = 0; ii<muls; ii++) {
        timedata[taps/2 + 1 + 2*ii] = args[ii];
        timedata[taps/2 - 1 - 2*ii] = args[ii];
    }
    fft.exec(freqdata.data(), timedata.data());

    static int64 hack;
    if (0) if (hack%100 == 0) {
        static FILE* timeplot;
        if (!timeplot) {
            timeplot = popen("gnuplot -persist", "w");
            fprintf(timeplot, "set grid\n");
            fprintf(timeplot, "set yrange [-.3:1.1]\n");
        }
        fprintf(timeplot, "plot '-' with lp\n");
        for (int64 ii = 0; ii<taps; ii++) {
            fprintf(timeplot, "%le\n", timedata[ii].re);
        }
        fprintf(timeplot, "e\n");
        fflush(timeplot);
    }

    if (hack++%100 == 0) {
        static FILE* freqplot;
        if (!freqplot) {
            freqplot = popen("gnuplot -persist", "w");
            fprintf(freqplot, "set grid\n");
            //fprintf(freqplot, "set yrange [-160:10]\n");
        }

        fprintf(freqplot, "plot '-' with l\n");
        for (int64 ii = 0; ii<size; ii++) {
            double db = 10*::log10(magsqr(freqdata[ii]/2) + 1e-20);
            fprintf(freqplot, "%.18le %.18le\n", ii*(200.0/size), db);
        }
        fprintf(freqplot, "e\n");
        fflush(freqplot);

        //getchar();
    }

    double cost = 0;
    for (int64 ii = 3000; ii<= 7000; ii++) {
        cost = max(cost, magsqr(freqdata[ii]/2));
    }
    cost = 10*log10(cost + 1e-20);

    static double best = 1000;
    best = min(cost, best);
    fprintf(stderr, "cost: %lf, best: %lf\n", cost, best);

    if (hack == 1) getchar();

    return cost;
}
//}}}

int main() {
    using namespace xm;

    /*
    double center[8] = {
         0.631268101946684479,
        -0.196776574978599628,
         0.103072853953527291,
        -0.059434490283196605,
         0.034284147960163137,
        -0.018879142833417378,
         0.009205561027108832,
        -0.004118682669364731,
    };
    double steps[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

    covarmin(tapcost<8>, center, steps, 8, 10000);
    for (int64 ii = 0; ii<8; ii++) fprintf(stderr, "%lld: %.18lf\n", ii, center[ii]);
    */

    const int64 muls = 24;
    const int64 taps = 4*muls + 1;
    double center[muls], steps[muls];
    for (int64 ii = 0; ii<muls; ii++) {
        double xx = 2*ii + 1;
        //center[ii] = sinc(.5*xx) * kaiswin(3.195, xx, taps - 1); // keep this, good for 12 muls
        //center[ii] = sinc(.5*xx) * kaiswin(6.8, xx, taps - 1); // good for 32 muls

        //center[ii] = sinc(.5*xx) * chebwin(-125, xx, taps - 1);
        center[ii] = sinc(.5*xx) * chebwin(-125, xx, taps - 1);
        steps[ii] = .001;
    }

    covarmin(tapcost<24>, center, steps, muls, 100000);
    for (int64 ii = 0; ii<muls; ii++) fprintf(stderr, "%lld: %.18lf\n", ii, center[ii]);








    /*
    FILE* gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "set yrange [-.1:1.1]\n");
    fprintf(gnuplot, "plot '-' with lines\n");

    for (int64 ii = -1000; ii<=1000; ii++) {
        double xx = .1*ii;
        fprintf(gnuplot, "%e %le\n", xx, chebwin(-80, xx, 100));
    }
    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    */



    /*
    // These are good
    double coeffs[6] = {
        0.559695589588624087,
        0.448414190956853254,
        -0.013406624208167874,
        0.007935591347341344,
        -0.004261624947996665,
        0.001622877263345780
    };
    double coeffs[6] = {
        1, 0, 0, 0, 0, 0
        //.5, .5, 0, 0, 0, 0
        //.5383553946707251, .4616446053292749, 0, 0, 0, 0 // albrecht 2-term
        //.42438009346095, .4973406350967378, .07827927144231873, 0, 0, 0 // albrecht 3-term
        //.35875, .48829, .14128, .01168, 0, 0 // blackman harris
    };

    const int muls = 8;
    double center[muls], steps[muls];
    for (int64 ii = 0; ii<muls; ii++) {
        steps[ii] = .1;
        const double length = muls*4 + 1 - 1;
        double xx = 2*ii + 1;
        double win = coeffs[0];
        for (int64 jj = 1; jj<6; jj++) {
            win += coeffs[jj]*cos(2*M_PI*jj*xx/length);
        }
        center[ii] = sinc(.5*xx) * win;
    }
    */






    /*
    // -57dB: 0.559695589588624087 0.448414190956853254, -0.013406624208167874, 0.007935591347341344, -0.004261624947996665, 0.001622877263345780

    double center[8] = {
        0.448414190956853254, -0.013406624208167874, 0.007935591347341344, -0.004261624947996665, 0.001622877263345780, 0, 0, 0
        //48829, .14128, .01168, 0, 0 // Blackman Harris: .
        //0.5, 0, 0, 0, 0
    };
    for (int64 ii = 0; ii<10; ii++) {
        double steps[8] = { .1, .1, .1, .1, .1, .1, .1, .1 };
        covarmin(fircost<8, 8>, center, steps, 8, 100000);
        for (int64 ii = 0; ii<8; ii++) {
            printf("    %.18lf,\n", center[ii]);
        }
        //double first = 1 - center[0] - center[1] - center[2] - center[3] - center[4];
        //printf("%.18lf %.18lf, %.18lf, %.18lf, %.18lf, %.18lf\n", first, center[0], center[1], center[2], center[3], center[4]);
    }
    */

    return 0;
}

#if 0
int main() {
    using namespace xm;

    /*
    typedef float f32x4 __attribute__ ((vector_size (16)));

    const f32x4 aa = { 1, 2, 3, 4 };
    const f32x4 bb = { 5, 6, 7, 8 };
    f32x4 cc = aa+bb;
    float dd = cc[0] + cc[1] + cc[2] + cc[3];
    fprintf(stderr, "dd: %lf\n", dd);
    */

    const int64 size = 8192;
    vec<cfloat> timedata(size);
    vec<cfloat> freqdata(size);
    kissfft<float> fft(size);
    zero(timedata);

    //const int64 window = 2, muls = 12;
    const int64 window = 1, muls = 8;
    const int64 taps = 4*muls + 3;

    // about 74dB down
    //const int64 taps = 49;
    //double window[taps];
    //cheby_win(window, taps, 62.5);

    // about 55dB down
    //const int64 taps = 33;
    //double window[taps];
    //cheby_win(window, taps, 45);

    //const int64 taps = 49;

    for (int64 ii = 0; ii<taps; ii++) {
        timedata[ii] = sinc(.5*(ii - taps/2)) * firwin(window, ii - taps/2, taps - 1);
        //timedata[ii] = sinc(.5*(ii - taps/2)) * window[ii];
        //timedata[ii] = sinc(.5*(ii - taps/2)) * pow(cos((ii - taps/2)*M_PI/(taps - 1)), 3.6);
        printf("%+3lld %+.12lf\n", ii - taps/2, timedata[ii].re);
    }

    if (0) {
        FILE* timeplot = popen("gnuplot -persist", "w");
        fprintf(timeplot, "set grid\n");
        fprintf(timeplot, "plot '-' with lp\n");
        for (int64 ii = 0; ii<taps; ii++) {
            fprintf(timeplot, "%le\n", timedata[ii].re);
        }
        fprintf(timeplot, "e\n");
        fflush(timeplot);
    }

    fft.exec(freqdata.data(), timedata.data());
    {
        FILE* freqplot = popen("gnuplot -persist", "w");
        fprintf(freqplot, "set grid\n");
        fprintf(freqplot, "plot '-' with l\n");
        for (int64 ii = 0; ii<size; ii++) {
            double db = 10*log10(magsqr(freqdata[ii]/2) + 1e-20);
            fprintf(freqplot, "%le %le\n", ii*(200.0/size), db);
        }
        fprintf(freqplot, "e\n");
        fflush(freqplot);
    }

    return 0;
}
#endif

