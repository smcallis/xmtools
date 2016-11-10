#include "xmtools.h"
#include "kissfft.h"

using namespace xm;

int main() {
    const double rate = 100e6;
    const int64 size = 327680;
    const double band = 6.1e3;
    //const double band = 25e3;
    //const double band = 100e3;
    //const double band = 20e3;

    const int64 taps = 65001;
    //const int64 taps = 51000; //25*2048 == 51200

    vector<cdouble> time(size, 0);
    vector<cdouble> freq(size, 0);
    kissfft<double> fft(size);


    for (int64 ii = 0; ii<taps; ii++) {
        double xx = ii - taps/2;
        time[ii] = sinc(xx*band/rate)*cbrt(cos(xx*M_PI/taps));
        //time[ii] = sinc(xx*band/rate)*kaiswin(.1, xx, taps);
        //time[ii] = cos(xx*M_PI/taps);
    }

    if (1) {
        FILE* gnuplot = popen("gnuplot -persist", "w");
        fprintf(gnuplot, "set grid\n");
        fprintf(gnuplot, "plot '-' w l\n");
        for (int64 ii = 0; ii<taps; ii++) {
            fprintf(gnuplot, "%.18le\n", time[ii].re);
        }
        fprintf(gnuplot, "e\n");
        fflush(gnuplot);
        //return 0;
    }

    fft.exec(freq.data(), time.data());

    FILE* gnuplot = popen("gnuplot -persist", "w");
    fprintf(gnuplot, "set grid\n");
    //fprintf(gnuplot, "set yrange [-200:100]\n");
    fprintf(gnuplot, "set yrange [-1:2]\n");
    fprintf(gnuplot, "plot '-' w l, 0, '-' w l\n");
    for (int64 ii = 0; ii<size; ii++) {
        double xx = ii*rate/size;
        //fprintf(gnuplot, "%.18le %.18le\n", xx, 10*log10(mag2(freq[ii]/freq[0]) + 1e-20));
        fprintf(gnuplot, "%.18le %.18le\n", xx, mag2(freq[ii]/freq[0]));
        if (xx > band/2 + 40e3) break;
    }
    fprintf(gnuplot, "e\n");

    fprintf(gnuplot, "%lf -60\n %lf 10\n\n", band/2, band/2);
    fprintf(gnuplot, "%lf -60\n %lf 10\n\n", band/2 + 1e3, band/2 + 1e3);
    fprintf(gnuplot, "%lf -60\n %lf 10\n\n", band/2 + 25e3, band/2 + 25e3);
    fprintf(gnuplot, "0 -50\n %.18le -50\n\n", band/2 + 40e3);
    fprintf(gnuplot, "e\n");

    fflush(gnuplot);

    return 0;
}

