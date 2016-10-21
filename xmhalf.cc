#include "xmtools.h"
#include "kissfft.h"

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

int main() {
    using namespace xm;

    const int64 size = 8192;
    vec<cfloat> timedata(size);
    vec<cfloat> freqdata(size);
    kissfft<float> fft(size);
    zero(timedata);

    //const int64 window = 2, muls = 13;
    //const int64 window = 1, muls = 8;
    //const int64 taps = 4*muls + 3;

    //const int64 taps = 49;
    //double window[taps];
    //cheby_win(window, taps, 62.5);

    const int64 taps = 33;
    double window[taps];
    cheby_win(window, taps, 44);


    for (int64 ii = 0; ii<taps; ii++) {
        //timedata[ii] = sinc(.5*(ii - taps/2)) * firwin(window, ii - taps/2, taps - 1);
        timedata[ii] = sinc(.5*(ii - taps/2)) * window[ii];
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

