#include "xmtools.h"
#include "kissfft.h"


static void halfpass(double* muls, int64 nmul, double lofreq) {
    using namespace xm;

    list<double> freqs;
    list<double> wants;
    freqs.reserve(nmul + 1);
    wants.reserve(nmul + 1);
    for (int64 ii = 0; ii<nmul*10; ii++) {
        freqs.append(ii*lofreq/(nmul*10));
        wants.append(1.0);
    }
    freqs.append(lofreq);
    wants.append(1.0);

    for (;;) {

        const int64 nfreq = freqs.size();
        matrix<double> matrix(nfreq, nmul);

        for (int64 ii = 0; ii<nfreq; ii++) {
            for (int64 jj = 0; jj<nmul; jj++) {
                matrix(ii, jj) = ::cos((2*jj + 1)*M_PI*freqs[ii]);
            }
        }
        check(nfreq >= nmul, "need more frequencies than multipliers");

        bool got = deprecated::qrsolve(matrix.data(), nfreq, nmul, wants.data(), 1);
        check(got == true, "matrix can't be singular");

        for (int64 ii = 0; ii<nmul; ii++) {
            muls[ii] = wants[ii];
        }

        const int64 fftsize = 1024;
        vector<cfloat> timedata(fftsize, 0);
        vector<cfloat> freqdata(fftsize);
        kissfft<float> fft(fftsize); 

        for (int64 ii = 0; ii<nmul; ii++) {
            timedata[2*ii + 1] = timedata[fftsize - 2*ii - 1] = .5*muls[ii];
        }
        fft.exec(freqdata.data(), timedata.data());

        int64 len = llrint(lofreq*fftsize/2);
        double worst = 0;
        for (int64 ii = 0; ii<len; ii++) {
            worst = max(worst, fabs(freqdata[ii].re - 1));
        }
        freqs.clear();
        wants.clear();
        freqs.append(lofreq);
        wants.append(1 - worst);
        for (int64 ii = 0; ii<len; ii++) {
            double lo  = freqdata[(fftsize + ii - 1)%fftsize].re;
            double mid = freqdata[ii].re;
            double hi  = freqdata[ii + 1].re;

            if (mid > lo && mid > hi) {
                double fract = quadpeak(lo, mid, hi);
                freqs.append((ii + fract)*2.0/fftsize);
                wants.append(1.0 + worst);
            }

            if (mid < lo && mid < hi) {
                double fract = quadpeak(lo, mid, hi);
                freqs.append((ii + fract)*2.0/fftsize);
                wants.append(1.0 - worst);
            }
        }

        //{{{ debugging
        {
            /*
            static FILE* timeplot;
            if (!timeplot) {
                timeplot = popen("gnuplot -persist", "w");
                fprintf(timeplot, "set grid\n");
            }
            fprintf(timeplot, "plot '-' with linespoints\n");
            const int64 taps = 4*nmul + 1;
            for (int64 ii = -taps/2; ii<=taps/2; ii++) {
                fprintf(timeplot, "%lld %lf\n", ii, timedata[(ii + fftsize)%fftsize].re);
            }
            fprintf(timeplot, "e\n");
            fflush(timeplot);
            */

            static FILE* specplot;
            if (!specplot) {
                specplot = popen("gnuplot -persist", "w");
                fprintf(specplot, "set grid\n");
                fprintf(specplot, "set yrange [-140:10]\n");
            }
            fprintf(specplot, "plot '-' with lines\n");
            for (int64 ii = 0; ii<fftsize; ii++) {
                double db = 10*::log10(mag2(.5*freqdata[ii] + .5) + 1e-20);
                fprintf(specplot, "%le %lf\n", 2.0*ii/fftsize, db);
            }
            fprintf(specplot, "e\n");
            fflush(specplot);


            static FILE* freqplot;
            if (!freqplot) {
                freqplot = popen("gnuplot -persist", "w");
                fprintf(freqplot, "set grid\n");
            }
            fprintf(freqplot, "plot '-' with lines, '-' with lines, '-' with points\n");
            for (int64 ii = 0; ii<fftsize; ii++) {
                fprintf(freqplot, "%le %lf\n", 2.0*ii/fftsize, freqdata[ii].re);
            }
            fprintf(freqplot, "e\n");
            for (int64 ii = 0; ii<fftsize; ii++) {
                fprintf(freqplot, "%le %lf\n", 2.0*ii/fftsize, freqdata[ii].im);
            }
            fprintf(freqplot, "e\n");
            check(freqs.size() == wants.size(), "matching sizes");
            for (int64 ii = 0; ii<freqs.size(); ii++) {
                fprintf(freqplot, "%le %le\n", freqs[ii], wants[ii]);
            }
            fprintf(freqplot, "e\n");
            fflush(freqplot);
            getchar();
        }
        //}}}

    }

}


int main(int argc, char* argv[]) {
    using namespace xm;

    cmdline args(argc, argv, "Build FIR with the Parks McClellan algorithm");

    int64 taps    = args.getint64("taps", 33, "numer of taps, must be odd");
    double lofreq = args.getdouble("lo", .4, "low pass from 0.0 to 1.0");
    double hifreq = args.getdouble("hi", .6, "high stop from 0.0 to 1.0");

    args.done();

    check(taps%2 == 1, "must have an odd number of taps");
    check(lofreq < hifreq, "low pass must be less than high stop");

    int64 nmul = (taps - 1)/4;
    double muls[nmul];
    halfpass(muls, nmul, lofreq);

    return 0;
}
